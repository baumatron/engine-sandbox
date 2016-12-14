#include "CVideoSubsystem.h"

#include "CInputSubsystem.h"
#include "math_main.h"

#ifdef WIN32
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif


#include <iostream>
using namespace std;
	
CInputSubsystem Input;

v3d mousePosition;
v3d oldMousePosition;

CInputSubsystem::CInputSubsystem():
initialized(false)
{
	for( int i = 0; i < IKC_LAST; i++ )
	{
		oldKeyStates[i] = IKS_UP;
		keyStates[i] = IKS_UP;
		shiftedCharacters[i] = i; // initialize these to unshifted values, followed below by the keys that actually shift
	}
	for( int i = 0; i < IMC_LAST; i++ )
	{
		oldMouseButtonStates[i] = IKS_UP;
		mouseButtonStates[i] = IKS_UP;
	}
	mousePosition = v3d(0,0);
	oldMousePosition = v3d(0,0);
	
	// set up the rest of the table
	for ( int i = 0; i < 26; i++ )
		shiftedCharacters[ i + 'a' ] = i + 'A';
	shiftedCharacters['0'] = ')';
	shiftedCharacters['1'] = '!';
	shiftedCharacters['2'] = '@';
	shiftedCharacters['3'] = '#';
	shiftedCharacters['4'] = '$';
	shiftedCharacters['5'] = '%';
	shiftedCharacters['6'] = '^';
	shiftedCharacters['7'] = '&';
	shiftedCharacters['8'] = '*';
	shiftedCharacters['9'] = '(';
	shiftedCharacters['-'] = '_';
	shiftedCharacters['='] = '+';
	shiftedCharacters['['] = '{';
	shiftedCharacters[']'] = '}';
	shiftedCharacters['\\'] = '|';
	shiftedCharacters[';'] = ':';
	shiftedCharacters['\''] = '"';
	shiftedCharacters[','] = '<';
	shiftedCharacters['.'] = '>';
	shiftedCharacters['/'] = '?';
	shiftedCharacters['`'] = '~';	
}                      
                       
                       
CInputSubsystem::~CInputSubsystem()
{          
	if(initialized)
		Shutdown();
}                      
                       
bool CInputSubsystem::PreInitialize() // PreInitialize is called before Initialize, only links to other subsystems should be made here
{         
	return true;
}                      
                       
bool CInputSubsystem::Initialize()
{                      
	initialized = true;   
	                      
	return true;          
}                      
                       
bool CInputSubsystem::Shutdown()
{                      
	initialized = false;  
	                      
	return true;          
}                      
                       
void CInputSubsystem::Think()
{                      
	// copy the keyStates to oldKeyStates
	for(int i = 0; i < IKC_LAST; i++)
	{                     
		oldKeyStates[i] = keyStates[i];
	}                     
	for(int i = 0; i < IMC_LAST; i++)
	{                     
		oldMouseButtonStates[i] = mouseButtonStates[i];
	}  	                      
	oldMousePosition = mousePosition;

	// now, check for input events from sdl. 
	// use these events to update keyStates
	SDL_Event sdlevent;   
                       
	if(SDL_PollEvent(&sdlevent)) 
	{                     
		CInputEvent inputEvent;
		unsigned short mods = 0;
		bool shiftedCharacter(false);
		switch( sdlevent.type ) 
		{                    
		// perhaps any events should be checked in another place... the system?
		// from the system, input related events could be forwarded here. other events could be forwarded to where they belong
			case SDL_QUIT:      
				{                  
					CON_Exec("quit"); 
					cout << "Got quit event!\n";
				}                  
				break;             
			case SDL_KEYDOWN:   
				{                  
					keyStates[sdlevent.key.keysym.sym] = IKS_DOWN;
					mods = (unsigned short)sdlevent.key.keysym.mod;
					if( sdlevent.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
						shiftedCharacter = true;
				}                  
				break;             
			case SDL_KEYUP:     
				{                  
					keyStates[sdlevent.key.keysym.sym] = IKS_UP;
					mods = (unsigned short)sdlevent.key.keysym.mod;
					if( sdlevent.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
						shiftedCharacter = true;
				}                  
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
					mouseButtonStates[sdlevent.button.button] = IKS_DOWN;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				{
					mouseButtonStates[sdlevent.button.button] = IKS_UP;
				}
				break;
			case SDL_MOUSEMOTION:
				{
					mousePosition.x = sdlevent.motion.x;
					mousePosition.y = Video.settings.getSh()-sdlevent.motion.y;
				}
				break;
			default:          
				break;             
		}   
		
		
		// now check to see what event should be generated and dispatch them
	                      
		for(int i = 0; i < IKC_LAST; i++)
		{                     
			CInputEvent inputEvent;
			
			unsigned short characterCode;
			if(shiftedCharacter)
				characterCode = GetShiftedCharacterCode(i);
			else
				characterCode = i;
			
			// check for state changes
			// key held is considered a state change, because it is an interesting event
			if( (oldKeyStates[i] == IKS_UP) && (keyStates[i] == IKS_DOWN) )
			{
				// key press action occurred
				inputEvent.inputEventType = IET_STATECHANGE;
				inputEvent.data.stateChangeEvent.keyCode = (InputKeyCodes)i;
				inputEvent.data.stateChangeEvent.keyAction = IKA_PRESS;
				inputEvent.data.stateChangeEvent.modifiers = mods;
				DispatchInputEvent(inputEvent);
				
				inputEvent.inputEventType = IET_CHARACTERTYPED;
				inputEvent.data.characterTypeEvent.characterCode = characterCode;
				inputEvent.data.characterTypeEvent.modifiers = mods;
				DispatchInputEvent(inputEvent);
			}
			else if( (oldKeyStates[i] == IKS_DOWN) && (keyStates[i] == IKS_UP) )
			{
				// key release action occurred
				inputEvent.inputEventType = IET_STATECHANGE;
				inputEvent.data.stateChangeEvent.keyCode = (InputKeyCodes)i;
				inputEvent.data.stateChangeEvent.keyAction = IKA_RELEASE;
				inputEvent.data.stateChangeEvent.modifiers = mods;
				
				DispatchInputEvent(inputEvent);
			}
			else if( (oldKeyStates[i] == IKS_DOWN) && (keyStates[i] == IKS_DOWN) )
			{
				// key hold action occurred
				inputEvent.inputEventType = IET_STATECHANGE;
				inputEvent.data.stateChangeEvent.keyCode = (InputKeyCodes)i;
				inputEvent.data.stateChangeEvent.keyAction = IKA_HOLD;
				inputEvent.data.stateChangeEvent.modifiers = mods;
				
				DispatchInputEvent(inputEvent);
			}
		}  

		// look at the mouse now
		for(int i = 0; i < IMC_LAST; i++)
		{                     
			CInputEvent inputEvent;
			
			// check for state changes
			// key held is considered a state change, because it is an interesting event
			if( (oldMouseButtonStates[i] == IKS_UP) && (mouseButtonStates[i] == IKS_DOWN) )
			{
				// button press action occurred
				inputEvent.inputEventType = IET_MOUSEBUTTON;
				inputEvent.data.mouseButtonEvent.mouseButtonCode = (InputMouseButtonCodes)i;
				inputEvent.data.mouseButtonEvent.mouseButtonAction = IKA_PRESS;
				inputEvent.data.mouseButtonEvent.mousePositionX = mousePosition.x;
				inputEvent.data.mouseButtonEvent.mousePositionY = mousePosition.y;
	
				DispatchInputEvent(inputEvent);
			}
			else if( (oldMouseButtonStates[i] == IKS_DOWN) && (mouseButtonStates[i] == IKS_UP) )
			{
				// button release action occurred
				inputEvent.inputEventType = IET_MOUSEBUTTON;
				inputEvent.data.mouseButtonEvent.mouseButtonCode = (InputMouseButtonCodes)i;
				inputEvent.data.mouseButtonEvent.mouseButtonAction = IKA_RELEASE;
				inputEvent.data.mouseButtonEvent.mousePositionX = mousePosition.x;
				inputEvent.data.mouseButtonEvent.mousePositionY = mousePosition.y;
		
				DispatchInputEvent(inputEvent);
			}
			else if( (oldMouseButtonStates[i] == IKS_DOWN) && (mouseButtonStates[i] == IKS_DOWN) )
			{
				// button hold action occurred
				inputEvent.inputEventType = IET_MOUSEBUTTON;
				inputEvent.data.mouseButtonEvent.mouseButtonCode = (InputMouseButtonCodes)i;
				inputEvent.data.mouseButtonEvent.mouseButtonAction = IKA_HOLD;
				inputEvent.data.mouseButtonEvent.mousePositionX = mousePosition.x;
				inputEvent.data.mouseButtonEvent.mousePositionY = mousePosition.y;
		
				DispatchInputEvent(inputEvent);
			}
		}       

		if( oldMousePosition != mousePosition )
		{
				// mouse move action occurred
				inputEvent.inputEventType = IET_MOUSEMOVE;
				inputEvent.data.mouseMoveEvent.mouseDeltaX = mousePosition.x - oldMousePosition.x;
				inputEvent.data.mouseMoveEvent.mouseDeltaY = mousePosition.y - oldMousePosition.y;
				inputEvent.data.mouseMoveEvent.mouseX = mousePosition.x;
				inputEvent.data.mouseMoveEvent.mouseY = mousePosition.y;
				DispatchInputEvent(inputEvent);
		}
	}                     
	                      
	
	

}


CRouterReturnCode CInputSubsystem::EventReceiver(CRouterEvent& event)
{
	return CRouterReturnCode(false, true);
}

bool CInputSubsystem::InputReceiver(const CInputEvent& event)
{
	return false;
}


void CInputSubsystem::AddInputReceiver(bool (*receiver) (const CInputEvent& event))
{
	legacyInputReceivers.push_back(receiver);
}
void CInputSubsystem::AddInputReceiver(ISubsystem* subsystem)
{
	inputReceivers.push_back(subsystem);
}

unsigned short CInputSubsystem::GetShiftedCharacterCode(unsigned short unshiftedCharacterCode)
{
	if( unshiftedCharacterCode < IKC_LAST )
		return shiftedCharacters[unshiftedCharacterCode];
	else
	{
		ccout << "unshiftedCharacterCode was too big!\n";
		return IKC_LAST;
	}
}

void CInputSubsystem::DispatchInputEvent(const CInputEvent& event)
{
	// go through each receiver, starting with the last one that was added
	for(int i = inputReceivers.size()-1; i >=0; i--)
	{
		if(inputReceivers[i]->InputReceiver(event))
			return;
	}
	for(int i = legacyInputReceivers.size()-1; i >=0; i--)
	{
		if(legacyInputReceivers[i](event))
			return;
	}
}

