

#include "CVideoSubsystem.h"
#include "in_win.h"
#include "sys_win.h"
#include "sys_main.h"
#include "con_main.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <string>
using namespace std;

#define INW_INPUTBUFFERSIZE 128 // maximum pieces of dinput data per frame
#define INW_NUMDIKEYS 256 // maximum number of directinput DIK_ keys
#define INW_MAXINJECT 128 // maximum injected key events per frame

void INW_GetFocus();
void INW_KillFocus();

// directinput globals
LPDIRECTINPUT8      inw_lpdi      = NULL;    // dinput object
LPDIRECTINPUTDEVICE8  inw_lpdi_keyboard   = NULL;    // dinput keyboard
LPDIRECTINPUTDEVICE8  inw_lpdi_mouse = NULL;    // dinput mouse

///////////////////////////////////////////////////////////////////////////////

// directinput DIK_ to INKEY_ key map, filled in at init
static int inw_DIKToINKEY[INW_NUMDIKEYS];

// current and previous key states
static char keyStates[INKEY_NUMKEYS];
static char oldKeyStates[INKEY_NUMKEYS];

// time since keys were "press"ed, for key delays etc.
static float keyPressTimes[INKEY_NUMKEYS];

// mouse positions during key events for drag deltas
static int keyMouseDragPositions[INKEY_NUMKEYS][2]; // x/y

// mouse limits, set to "no limit" extremes by default
//static int mouseMinX=0x80000000, mouseMinY=0x80000000; // signed minimum
//static int mouseMaxX=0x7fffffff, mouseMaxY=0x7fffffff; // signed maximum
// current mouse positions
static int mouseX, mouseY; // within limits
static int mouseFreeX, mouseFreeY; // without limits
// note: 32 bits is sufficient for the free coordinates unless you set a
//       super-high mouse sensitivity or feel like rolling in one direction
//       for a few weeks straight with no break... uhh, i don't think so.

// mouse sensitivity multiplier
static float mouseSensitivity = 1.0;

static void (*INW_eventHandler)(in_Event *) = NULL;

// exclusive mode toggle
static int mouseExclusive = 1;

// device acquired status
static int mouseAcquired, keyboardAcquired;

// keyboard event injection buffer
static int keyInjectionBuffer[INW_MAXINJECT][2]; // key/state
static int keyInjectionBufferIndex;

//static char* NameForDIError(HRESULT err);
static HRESULT SetMouseCooperative(DWORD inFlags);
static void FlushKeyboardData();
static void InitDIKToINKEY();
static void AcquireMouse();
static void UnacquireMouse();
static void AcquireKeyboard();
static void UnacquireKeyboard();
static void CheckKeyEvents(int key, in_Event *event);

// event index
static int inw_eventIndex;

/////////////////////////////////////////////////////////////////
// private functions ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

static HRESULT SetMouseCooperative(DWORD inFlags)
{
	if (mouseExclusive)
		inFlags |= DISCL_EXCLUSIVE;
	else
		inFlags |= DISCL_NONEXCLUSIVE;
	return(inw_lpdi_mouse->SetCooperativeLevel(sysw_mainWindow, inFlags));
}

static void FlushKeyboardData()
{
	if (inw_lpdi_keyboard)
	{
		int num;
	
		// flush the dinput buffer, don't worry about return value
		num = INFINITE;
		inw_lpdi_keyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),NULL,&num,0);
	}

	// wipe our internal key data
	memset(keyStates, 0, sizeof(char)*INKEY_NUMKEYS);
	memset(oldKeyStates, 0, sizeof(char)*INKEY_NUMKEYS);
	memset(keyPressTimes, 0, sizeof(float)*INKEY_NUMKEYS);
	keyInjectionBufferIndex = 0;
}

static void InitDIKToINKEY()
{
	// since we can't guarantee that the constants for these DIKs won't
	// change, initialize the remap table here instead of statically

	// any key mapped to zero means unmapped and should be ignored
	memset(inw_DIKToINKEY, 0, INW_NUMDIKEYS*sizeof(int));

	inw_DIKToINKEY[DIK_ESCAPE] = INKEY_ESCAPE;
	inw_DIKToINKEY[DIK_1] = '1';
	inw_DIKToINKEY[DIK_2] = '2';
	inw_DIKToINKEY[DIK_3] = '3';
	inw_DIKToINKEY[DIK_4] = '4';
	inw_DIKToINKEY[DIK_5] = '5';
	inw_DIKToINKEY[DIK_6] = '6';
	inw_DIKToINKEY[DIK_7] = '7';
	inw_DIKToINKEY[DIK_8] = '8';
	inw_DIKToINKEY[DIK_9] = '9';
	inw_DIKToINKEY[DIK_0] = '0';
	inw_DIKToINKEY[DIK_MINUS] = '-';
	inw_DIKToINKEY[DIK_EQUALS] = '=';
	inw_DIKToINKEY[DIK_BACK] = INKEY_BACKSPACE;
	inw_DIKToINKEY[DIK_TAB] = INKEY_TAB;
	inw_DIKToINKEY[DIK_Q] = 'q';
	inw_DIKToINKEY[DIK_W] = 'w';
	inw_DIKToINKEY[DIK_E] = 'e';
	inw_DIKToINKEY[DIK_R] = 'r';
	inw_DIKToINKEY[DIK_T] = 't';
	inw_DIKToINKEY[DIK_Y] = 'y';
	inw_DIKToINKEY[DIK_U] = 'u';
	inw_DIKToINKEY[DIK_I] = 'i';
	inw_DIKToINKEY[DIK_O] = 'o';
	inw_DIKToINKEY[DIK_P] = 'p';
	inw_DIKToINKEY[DIK_LBRACKET] = '[';
	inw_DIKToINKEY[DIK_RBRACKET] = ']';
	inw_DIKToINKEY[DIK_RETURN] = INKEY_ENTER;
	inw_DIKToINKEY[DIK_LCONTROL] = INKEY_LEFTCTRL;
	inw_DIKToINKEY[DIK_A] = 'a';
	inw_DIKToINKEY[DIK_S] = 's';
	inw_DIKToINKEY[DIK_D] = 'd';
	inw_DIKToINKEY[DIK_F] = 'f';
	inw_DIKToINKEY[DIK_G] = 'g';
	inw_DIKToINKEY[DIK_H] = 'h';
	inw_DIKToINKEY[DIK_J] = 'j';
	inw_DIKToINKEY[DIK_K] = 'k';
	inw_DIKToINKEY[DIK_L] = 'l';
	inw_DIKToINKEY[DIK_SEMICOLON] = ';';
	inw_DIKToINKEY[DIK_APOSTROPHE] = '\'';
	inw_DIKToINKEY[DIK_GRAVE] = '`';
	inw_DIKToINKEY[DIK_LSHIFT] = INKEY_LEFTSHIFT;
	inw_DIKToINKEY[DIK_BACKSLASH] = '\\';
	inw_DIKToINKEY[DIK_Z] = 'z';
	inw_DIKToINKEY[DIK_X] = 'x';
	inw_DIKToINKEY[DIK_C] = 'c';
	inw_DIKToINKEY[DIK_V] = 'v';
	inw_DIKToINKEY[DIK_B] = 'b';
	inw_DIKToINKEY[DIK_N] = 'n';
	inw_DIKToINKEY[DIK_M] = 'm';
	inw_DIKToINKEY[DIK_COMMA] = ',';
	inw_DIKToINKEY[DIK_PERIOD] = '.';
	inw_DIKToINKEY[DIK_SLASH] = '/';
	inw_DIKToINKEY[DIK_RSHIFT] = INKEY_RIGHTSHIFT;
	inw_DIKToINKEY[DIK_MULTIPLY] = INKEY_NUMSTAR;
	inw_DIKToINKEY[DIK_LMENU] = INKEY_LEFTALT;
	inw_DIKToINKEY[DIK_SPACE] = INKEY_SPACE;
	inw_DIKToINKEY[DIK_CAPITAL] = INKEY_CAPSLOCK;
	inw_DIKToINKEY[DIK_F1] = INKEY_F1;
	inw_DIKToINKEY[DIK_F2] = INKEY_F2;
	inw_DIKToINKEY[DIK_F3] = INKEY_F3;
	inw_DIKToINKEY[DIK_F4] = INKEY_F4;
	inw_DIKToINKEY[DIK_F5] = INKEY_F5;
	inw_DIKToINKEY[DIK_F6] = INKEY_F6;
	inw_DIKToINKEY[DIK_F7] = INKEY_F7;
	inw_DIKToINKEY[DIK_F8] = INKEY_F8;
	inw_DIKToINKEY[DIK_F9] = INKEY_F9;
	inw_DIKToINKEY[DIK_F10] = INKEY_F10;
	inw_DIKToINKEY[DIK_NUMLOCK] = INKEY_NUMLOCK;
	inw_DIKToINKEY[DIK_SCROLL] = INKEY_SCROLLLOCK;
	inw_DIKToINKEY[DIK_NUMPAD7] = INKEY_NUM7;
	inw_DIKToINKEY[DIK_NUMPAD8] = INKEY_NUM8;
	inw_DIKToINKEY[DIK_NUMPAD9] = INKEY_NUM9;
	inw_DIKToINKEY[DIK_SUBTRACT] = INKEY_NUMMINUS;
	inw_DIKToINKEY[DIK_NUMPAD4] = INKEY_NUM4;
	inw_DIKToINKEY[DIK_NUMPAD5] = INKEY_NUM5;
	inw_DIKToINKEY[DIK_NUMPAD6] = INKEY_NUM6;
	inw_DIKToINKEY[DIK_ADD] = INKEY_NUMPLUS;
	inw_DIKToINKEY[DIK_NUMPAD1] = INKEY_NUM1;
	inw_DIKToINKEY[DIK_NUMPAD2] = INKEY_NUM2;
	inw_DIKToINKEY[DIK_NUMPAD3] = INKEY_NUM3;
	inw_DIKToINKEY[DIK_NUMPAD0] = INKEY_NUM0;
	inw_DIKToINKEY[DIK_DECIMAL] = INKEY_NUMPERIOD;
	inw_DIKToINKEY[DIK_F11] = INKEY_F11;
	inw_DIKToINKEY[DIK_F12] = INKEY_F12;
	inw_DIKToINKEY[DIK_NUMPADENTER] = INKEY_NUMENTER;
	inw_DIKToINKEY[DIK_RCONTROL] = INKEY_RIGHTCTRL;
	inw_DIKToINKEY[DIK_DIVIDE] = INKEY_NUMSLASH;
	inw_DIKToINKEY[DIK_SYSRQ] = INKEY_PRINTSCRN;
	inw_DIKToINKEY[DIK_RMENU] = INKEY_RIGHTALT;
	inw_DIKToINKEY[DIK_HOME] = INKEY_HOME;
	inw_DIKToINKEY[DIK_UP] = INKEY_UPARROW;
	inw_DIKToINKEY[DIK_PRIOR] = INKEY_PGUP;
	inw_DIKToINKEY[DIK_LEFT] = INKEY_LEFTARROW;
	inw_DIKToINKEY[DIK_RIGHT] = INKEY_RIGHTARROW;
	inw_DIKToINKEY[DIK_END] = INKEY_END;
	inw_DIKToINKEY[DIK_DOWN] = INKEY_DOWNARROW;
	inw_DIKToINKEY[DIK_NEXT] = INKEY_PGDN;
	inw_DIKToINKEY[DIK_INSERT] = INKEY_INS;
	inw_DIKToINKEY[DIK_DELETE] = INKEY_DEL;
	inw_DIKToINKEY[DIK_LWIN] = 0;
	inw_DIKToINKEY[DIK_RWIN] = 0;
	inw_DIKToINKEY[DIK_APPS] = 0;
}

static void AcquireMouse()
{
	if(inw_lpdi_mouse)
	{
		int i;
		mouseAcquired = 1;
		if(inw_lpdi_mouse->Acquire() == DI_OK)
			return;
		// didn't get it back, try a few more times
		for(i = 0; i < 50; i++)
		{
			if(inw_lpdi_mouse->Acquire() == DI_OK)
				return;
		}
	}
	// still don't have it, something's up
	mouseAcquired = 0;
}

static void UnacquireMouse()
{
	mouseAcquired = 0;
	if(inw_lpdi_mouse)
	{
		inw_lpdi_mouse->Unacquire();
	}
}

static void AcquireKeyboard()
{
	if(inw_lpdi_keyboard)
	{
		int i;
		keyboardAcquired = 1;
		if(inw_lpdi_keyboard->Acquire() == DI_OK)
			return;
		// didn't get it back, try a few more times
		for(i = 0; i < 50; i++)
		{
			if(inw_lpdi_keyboard->Acquire() == DI_OK)
				return;
		}
	}
	// still don't have it, something's up
	keyboardAcquired = 0;

	FlushKeyboardData();
}

static void UnacquireKeyboard()
{
	FlushKeyboardData();
	
	keyboardAcquired = 0;
	if(inw_lpdi_keyboard)
		inw_lpdi_keyboard->Unacquire();	
}

static void CheckKeyEvents(int key, in_Event *event)
{
	if (keyStates[key])
	{
		if (!oldKeyStates[key])
		{
			// press event (key was up before but down now)
			event->eventType = INEV_PRESS;
			event->mouseDeltaX = 0;
			event->mouseDeltaY = 0;
			event->pressTimeDelta = 0.0;
            event->lastPressEventDelta = sys_curTime - keyPressTimes[key];
			event->eventIndex = inw_eventIndex++;
			if (INW_eventHandler)
				INW_eventHandler(event);
			keyMouseDragPositions[key][0] = mouseFreeX;
			keyMouseDragPositions[key][1] = mouseFreeY;
			keyPressTimes[key] = sys_curTime;
		}
		else
		{
			// drag event (key is still down)
			event->eventType = INEV_DRAG;
			event->mouseDeltaX = mouseFreeX - keyMouseDragPositions[key][0];
			event->mouseDeltaY = mouseFreeY - keyMouseDragPositions[key][1];
			event->pressTimeDelta = sys_curTime - keyPressTimes[key];
            event->lastPressEventDelta = event->pressTimeDelta;
			event->eventIndex = inw_eventIndex++;
			if (INW_eventHandler)
				INW_eventHandler(event);
			keyMouseDragPositions[key][0] = mouseFreeX;
			keyMouseDragPositions[key][1] = mouseFreeY;
		}
	}
	else if (oldKeyStates[key])
	{
		// release event (key was down before but up now)
		event->eventType = INEV_RELEASE;
		event->mouseDeltaX = mouseFreeX - keyMouseDragPositions[key][0];
		event->mouseDeltaY = mouseFreeY - keyMouseDragPositions[key][1];
        event->pressTimeDelta = sys_curTime - keyPressTimes[key];
        event->lastPressEventDelta = event->pressTimeDelta;
		event->eventIndex = inw_eventIndex++;
		if (INW_eventHandler)
			INW_eventHandler(event);
	}
}

/////////////////////////////////////////////////////////////////
/////// global functions ////////////////////////////////////////
/////////////////////////////////////////////////////////////////

void INW_Init()
{
	DIPROPDWORD prop = { { sizeof(DIPROPDWORD), sizeof(DIPROPHEADER), 0,
		DIPH_DEVICE, }, INW_INPUTBUFFERSIZE };

	if(FAILED(DirectInput8Create(sysw_mainInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&inw_lpdi, NULL)))
		ccout << "DInput init error in INW_Startup()";
	
	inw_lpdi->CreateDevice(GUID_SysKeyboard, &inw_lpdi_keyboard, NULL);
	inw_lpdi_keyboard->SetDataFormat(&c_dfDIKeyboard);
	inw_lpdi_keyboard->SetCooperativeLevel(sysw_mainWindow,DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	inw_lpdi_keyboard->SetProperty(DIPROP_BUFFERSIZE, &prop.diph);

	// get it
	AcquireKeyboard();

	// set up DIK_ remap
	InitDIKToINKEY();
		
	inw_lpdi->CreateDevice(GUID_SysMouse, &inw_lpdi_mouse, NULL);
	inw_lpdi_mouse->SetDataFormat(&c_dfDIMouse);
	SetMouseCooperative(DISCL_FOREGROUND);

	AcquireMouse();
	
	INW_GetFocus();

	return;
}

void INW_Update()
{
	DIDEVICEOBJECTDATA keys[INW_INPUTBUFFERSIZE];
	DIMOUSESTATE mstate;
	int key;
	int i, num;
	int oldx, oldy;
	HRESULT err;
	in_Event event;
	int ready;
	static int keyForMouseButton[3] = { INKEY_MOUSELEFT, INKEY_MOUSERIGHT, INKEY_MOUSEMIDDLE };
	static int keyEventSent[INKEY_NUMKEYS];

	//---------------------
	// mouse
	//---------------------	

	if(inw_lpdi_mouse)
	{
		ready = 1;
		err = inw_lpdi_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID) &mstate);
		if (err != DI_OK)
		{
			if (err==DIERR_NOTACQUIRED)
			{
				// if it's not acquired, get it back and try again
				ready = 0;
				AcquireMouse();
				if ((err = inw_lpdi_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID) &mstate)) == DI_OK)
					ready = 1;
				return;
			}
			else if (err==DIERR_INPUTLOST)
			{
				// if the input is gone, all we can do is reacquire
				ready = 0;
				AcquireMouse();
			}
		}
		
		// handle events
		if (ready)
		{
			// get new mouse position status
			oldx = mouseFreeX;
			oldy = mouseFreeY;
			mouseFreeX += mstate.lX * mouseSensitivity;
			mouseFreeY += mstate.lY * mouseSensitivity;
			mouseX += mstate.lX * mouseSensitivity;
			mouseY += mstate.lY * mouseSensitivity;

			// clamp non-free mouse values to limits
/*			if (mouseX >= mouseMaxX)
				mouseX = mouseMaxX-1;
			if (mouseY >= mouseMaxY)
				mouseY = mouseMaxY-1;
			if (mouseX <= mouseMinX)
				mouseX = mouseMinX+1;
			if (mouseY <= mouseMinY)
				mouseY = mouseMinY+1;*/

			if (mouseX >= Video.settings.getSw())
				mouseX = Video.settings.getSw()-1;
			if (mouseY >= Video.settings.getSh())
				mouseY = Video.settings.getSh()-1;
			if (mouseX <= -1)
				mouseX = -1+1;
			if (mouseY <= -1)
				mouseY = -1+1;

			// rig up event data that won't change in the loop
			event.mouseX = mouseX;
			event.mouseY = mouseY;
			event.mouseFreeX = mouseFreeX;
			event.mouseFreeY = mouseFreeY;
			event.mouseDeltaX = event.mouseDeltaY = 0;
			event.frameTimeStamp = sys_curTime;
			event.pressTimeDelta = 0.0;
			event.key = 0;
			event.flags = 0; // note: key flags will be off by a frame here since the mouse goes first
			if (keyStates[INKEY_LEFTSHIFT] || keyStates[INKEY_RIGHTSHIFT])
				event.flags |= INEVF_SHIFT;
			if (keyStates[INKEY_LEFTCTRL] || keyStates[INKEY_RIGHTCTRL])
				event.flags |= INEVF_CTRL;
			if (keyStates[INKEY_LEFTALT] || keyStates[INKEY_RIGHTALT])
				event.flags |= INEVF_ALT;
			
			// call a MOUSEMOVE event first if one occured
			if ((mouseFreeX != oldx) || (mouseFreeY != oldy))
			{
				event.mouseDeltaX = mouseFreeX - oldx;
				event.mouseDeltaY = mouseFreeY - oldy;
				event.eventType = INEV_MOUSEMOVE;
				event.eventIndex = inw_eventIndex++;
				if (INW_eventHandler)
					INW_eventHandler(&event);
			}
			
			// check the mouse buttons
			for(i=0; i<3; i++)
			{
				key = keyForMouseButton[i];
				event.key = key;

				// set current key state
				keyStates[key] = mstate.rgbButtons[i] & (char)0x80;

				CheckKeyEvents(key, &event);

				// save current key state to old
				oldKeyStates[key] = keyStates[key];
			}
		}
	}
	//---------------------
	// keyboard
	//---------------------
	
	ready = 1;
	num=INW_INPUTBUFFERSIZE;
	err=inw_lpdi_keyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),keys,&num,0);
	if (err != DI_OK)
	{
		if (err==DIERR_NOTACQUIRED)
		{
			// if it's not acquired, get it back and try again
			ready = 0;
			AcquireKeyboard();
			num=INW_INPUTBUFFERSIZE;
			if ((err=inw_lpdi_keyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),keys,&num,0)) == DI_OK)
				ready = 1;
		}
		else if (err==DIERR_INPUTLOST)
		{
			// if the input is gone, all we can do is reacquire
			ready = 0;
			AcquireKeyboard();
		}
	}
	
	// handle events
	if (ready)
	{
		event.mouseX = mouseX;
		event.mouseY = mouseY;
		event.mouseFreeX = mouseFreeX;
		event.mouseFreeY = mouseFreeY;
		event.frameTimeStamp = sys_curTime;

		// run through input buffer to account for changed states
		for (i=0;i<num;i++)
		{
			key = inw_DIKToINKEY[keys[i].dwOfs]; // convert to our key table
			if (!key)
				continue; // not a mapped key, ignore it
			
			// set current key state
			keyStates[key] = (char)keys[i].dwData & 0x80L;

			if (keyStates[key] != oldKeyStates[key])
			{
				// press or release event, need to handle it now
				event.key = key;
				event.flags = 0;
				if (keyStates[INKEY_LEFTSHIFT] || keyStates[INKEY_RIGHTSHIFT])
					event.flags |= INEVF_SHIFT;
				if (keyStates[INKEY_LEFTCTRL] || keyStates[INKEY_RIGHTCTRL])
					event.flags |= INEVF_CTRL;
				if (keyStates[INKEY_LEFTALT] || keyStates[INKEY_RIGHTALT])
					event.flags |= INEVF_ALT;

				CheckKeyEvents(key, &event);

				// save current key state to old
				oldKeyStates[key] = keyStates[key];

				// make sure we don't call a drag event in this frame
				keyEventSent[key] = 1;
			}
		}

		// handle key injection buffer in the same way
		for (i=0;i<keyInjectionBufferIndex;i++)
		{
			key = keyInjectionBuffer[i][0];

			// set current key state
			keyStates[key] = keyInjectionBuffer[i][1];

			if (keyStates[key] != oldKeyStates[key])
			{
				// press or release event, need to handle it now
				event.key = key;
				event.flags = 0;
				if (keyStates[INKEY_LEFTSHIFT] || keyStates[INKEY_RIGHTSHIFT])
					event.flags |= INEVF_SHIFT;
				if (keyStates[INKEY_LEFTCTRL] || keyStates[INKEY_RIGHTCTRL])
					event.flags |= INEVF_CTRL;
				if (keyStates[INKEY_LEFTALT] || keyStates[INKEY_RIGHTALT])
					event.flags |= INEVF_ALT;

				CheckKeyEvents(key, &event);

				// save current key state to old
				oldKeyStates[key] = keyStates[key];

				// make sure we don't call a drag event in this frame
				keyEventSent[key] = 1;
			}
		}
		keyInjectionBufferIndex = 0;

		// send drag events for all unhandled keys
		event.flags = 0;
		if (keyStates[INKEY_LEFTSHIFT] || keyStates[INKEY_RIGHTSHIFT])
			event.flags |= INEVF_SHIFT;
		if (keyStates[INKEY_LEFTCTRL] || keyStates[INKEY_RIGHTCTRL])
			event.flags |= INEVF_CTRL;
		if (keyStates[INKEY_LEFTALT] || keyStates[INKEY_RIGHTALT])
			event.flags |= INEVF_ALT;
		for (i=0;i<256;i++)
		{
			if (keyEventSent[i] || !keyStates[i])
				continue; // already ran events on this key, or no drag to deal with
			event.key = i;
			
			CheckKeyEvents(i, &event);

			// save current key state to old
			oldKeyStates[i] = keyStates[i];
		}

		// clear the eventsent buffer
		memset(keyEventSent, 0, INKEY_NUMKEYS);
	}
}

void INW_Shutdown()
{
	if(inw_lpdi_keyboard)
	{
		UnacquireKeyboard();
		inw_lpdi_keyboard->Release();
		inw_lpdi_keyboard = 0;
	}
	if(inw_lpdi_mouse)
	{
		UnacquireMouse();
		inw_lpdi_mouse->Release();
		inw_lpdi_mouse = 0;
	}
	if(inw_lpdi)
	{
		inw_lpdi->Release();
		inw_lpdi = 0;
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////

void INW_GetFocus()
{
	inw_lpdi_keyboard->Acquire();
	if(inw_lpdi_mouse)
		inw_lpdi_mouse->Acquire();
}

void INW_KillFocus()
{
	inw_lpdi_keyboard->Unacquire();
	if(inw_lpdi_mouse)
		inw_lpdi_mouse->Unacquire();
}

///////////////////////////////////////////////////////////////////////////////

void INW_SetEventHandler(void (*handler) (in_Event*))
{
	INW_eventHandler = handler;
}

///////////////////////////////////////////////////////////////////////////////

void INW_ForceKey(short button, bool state)
{
	return;
}

void INW_SetMouseExclusive(bool exclusive)
{
	return;
}

/*void INW_SetMouseLimits(long x1, long y1, long x2, long y2)
{
	mouseMinX = x1;
	mouseMinY = y1;
	mouseMaxX = x2;
	mouseMaxY = y2;
	return;
}*/

void INW_SetMouseSensitivity(float factor)
{
	mouseSensitivity = factor;
	return;
}

float INW_GetMouseSensitivity()
{
	return mouseSensitivity;
}





