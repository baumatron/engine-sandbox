//****************************************************************************
//**
//**    IN_MAIN.CPP
//**    Input - Main Interface
//**
//**    Copyright (c) 1998 Chris Hargrove
//**    Notice: Usage of any code in this file is subject to the rules
//**            described in the LICENSE.TXT file included in this directory.
//**            Reading, compiling, or otherwise using this code constitutes
//**            automatic acceptance of the rules in said text file.
//**
//****************************************************************************
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#define SYS_DEFS_INCLUDE_WIN32
#include "in_main.h"
#include "in_win.h"
#include "sys_main.h"
#include "bnd_main.h"
#include "con_main.h"
#include "CVideoSubsystem.h"
#include "m_misc.h"
#include "gui_main.h"
//#include "bn_main.h"

//============================================================================
//    IMPLEMENTATION PRIVATE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
#define IN_MAXRECEIVERS 32 // maximum number of receivers

//============================================================================
//    IMPLEMENTATION PRIVATE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE STRUCTURES / UTILITY CLASSES
//============================================================================

// Input receiver type
// Entry in a chain of callbacks that user input is fed through
// Receiver functions return true if the input has been accepted, or false
// if it should be continued down the chain.  Receivers can safely call
// the Remove function within themselves.

typedef struct in_receiver_s in_receiver_t;
struct in_receiver_s
{
	int (*handler)(in_Event *);
	in_receiver_t *prev, *next;
};

//============================================================================
//    IMPLEMENTATION REQUIRED EXTERNAL REFERENCES (AVOID)
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE DATA
//============================================================================
bool in_initialized(false);

// receiver data
static in_receiver_t in_ReceiverPool[IN_MAXRECEIVERS];
static in_receiver_t in_ReceiverChain;

// shifted key table
static int in_Shifted[256];
static int in_Unshifted[256];

char characterTable[256];
char shiftedCharacterTable[256];

// key name list
/*struct*/ keyName_s
/*{
	char *name;
	int key;
	char *numname;
}*/ in_KeyNames[] =
{
	{ "BACKSPACE", INKEY_BACKSPACE, NULL},
	{ "TAB", INKEY_TAB, NULL},
	{ "ENTER", INKEY_ENTER, NULL},
	{ "ESCAPE", INKEY_ESCAPE, NULL},
	{ "SPACE", INKEY_SPACE, NULL},

	{ "LEFTSHIFT", INKEY_LEFTSHIFT, NULL},
	{ "RIGHTSHIFT", INKEY_RIGHTSHIFT, NULL},
	{ "LEFTCTRL", INKEY_LEFTCTRL, NULL},
	{ "RIGHTCTRL", INKEY_RIGHTCTRL, NULL},
	{ "LEFTALT", INKEY_LEFTALT, NULL},
	{ "RIGHTALT", INKEY_RIGHTALT, NULL},
	
	{ "LEFTARROW", INKEY_LEFTARROW, NULL},
	{ "RIGHTARROW", INKEY_RIGHTARROW, NULL},
	{ "UPARROW", INKEY_UPARROW, NULL},
	{ "DOWNARROW", INKEY_DOWNARROW, NULL},

	{ "F1", INKEY_F1, NULL},
	{ "F2", INKEY_F2, NULL},
	{ "F3", INKEY_F3, NULL},
	{ "F4", INKEY_F4, NULL},
	{ "F5", INKEY_F5, NULL},
	{ "F6", INKEY_F6, NULL},
	{ "F7", INKEY_F7, NULL},
	{ "F8", INKEY_F8, NULL},
	{ "F9", INKEY_F9, NULL},
	{ "F10", INKEY_F10, NULL},
	{ "F11", INKEY_F11, NULL},
	{ "F12", INKEY_F12, NULL},

	{ "INS", INKEY_INS, NULL},
	{ "DEL", INKEY_DEL, NULL},
	{ "HOME", INKEY_HOME, NULL},
	{ "END", INKEY_END, NULL},
	{ "PGUP", INKEY_PGUP, NULL},
	{ "PGDN", INKEY_PGDN, NULL},

	{ "NUM/", INKEY_NUMSLASH, "/"},
	{ "NUM*", INKEY_NUMSTAR, "*"},
	{ "NUM-", INKEY_NUMMINUS, "-"},
	{ "NUM+", INKEY_NUMPLUS, "+"},
	{ "NUMENTER", INKEY_NUMENTER, NULL},
	{ "NUM.", INKEY_NUMPERIOD, "."},
	{ "NUM0", INKEY_NUM0, "0"},
	{ "NUM1", INKEY_NUM1, "1"},
	{ "NUM2", INKEY_NUM2, "2"},
	{ "NUM3", INKEY_NUM3, "3"},
	{ "NUM4", INKEY_NUM4, "4"},
	{ "NUM5", INKEY_NUM5, "5"},
	{ "NUM6", INKEY_NUM6, "6"},
	{ "NUM7", INKEY_NUM7, "7"},
	{ "NUM8", INKEY_NUM8, "8"},
	{ "NUM9", INKEY_NUM9, "9"},

	{ "NUMLOCK", INKEY_NUMLOCK, NULL},
	{ "CAPSLOCK", INKEY_CAPSLOCK, NULL},
	{ "SCROLLLOCK", INKEY_SCROLLLOCK, NULL},
	{ "PRINTSCREEN", INKEY_PRINTSCRN, NULL},
	{ "PAUSE", INKEY_PAUSE, NULL},
	
	{ "MOUSE1", INKEY_MOUSELEFT, NULL},
	{ "MOUSE2", INKEY_MOUSERIGHT, NULL},
	{ "MOUSE3", INKEY_MOUSEMIDDLE, NULL},

	// give names to a few regular keys that tend to be reserved
	{ "SEMICOLON", ';', ";"},
	{ "QUOTE", '\'', "\'"},
	{ "TILDE", '`', "`"},

	{ NULL, 0, NULL }
};

//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================
static void ReceiverDispatch(in_Event *event);


//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================

// event handler callback
static void ReceiverDispatch(in_Event *event)
{
	in_receiver_t *rec, *next;

	for (rec=in_ReceiverChain.next; rec!=&in_ReceiverChain; rec=next)
	{
		next = rec->next; // use a safe next pointer incase receiver removes self
		if (rec->handler(event))
			return;
	}
}

//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================

// initialization
void IN_Init()
{
	if(in_initialized) return;
	int i;
	
	INW_Init(); // init directinput
	INW_SetEventHandler(ReceiverDispatch); // set event handler to dispatcher
	
	// we're doing fullscreen stuff now, so set the mouse to exclusive mode
	INW_SetMouseExclusive(1);

	if(!Video.IsInitialized())
		ccout << "IN_Init() error: screen dimensions unknown. Video.Init() first." << newl;
	//INW_SetMouseLimits(-1,-1,Video.sw, Video.sh);
	INW_SetMouseSensitivity(1.0f);
	// link receiver head to itself
	in_ReceiverChain.next = in_ReceiverChain.prev = &in_ReceiverChain;

	// set up shifted key table
	// keys with shift entries of zero don't have shifted versions
	for (i=0;i<26;i++)
		in_Shifted[i+'a'] = i+'A';
	in_Shifted['0'] = ')';
	in_Shifted['1'] = '!';
	in_Shifted['2'] = '@';
	in_Shifted['3'] = '#';
	in_Shifted['4'] = '$';
	in_Shifted['5'] = '%';
	in_Shifted['6'] = '^';
	in_Shifted['7'] = '&';
	in_Shifted['8'] = '*';
	in_Shifted['9'] = '(';
	in_Shifted['-'] = '_';
	in_Shifted['='] = '+';
	in_Shifted['['] = '{';
	in_Shifted[']'] = '}';
	in_Shifted['\\'] = '|';
	in_Shifted[';'] = ':';
	in_Shifted['\''] = '"';
	in_Shifted[','] = '<';
	in_Shifted['.'] = '>';
	in_Shifted['/'] = '?';
	in_Shifted['`'] = '~';

	for(i = 0; i < 26; i++)
		in_Unshifted[i+'a'] = i+'a';
	in_Unshifted['0'] = '0';
	in_Unshifted['1'] = '1';
	in_Unshifted['2'] = '2';
	in_Unshifted['3'] = '3';
	in_Unshifted['4'] = '4';
	in_Unshifted['5'] = '5';
	in_Unshifted['6'] = '6';
	in_Unshifted['7'] = '7';
	in_Unshifted['8'] = '8';
	in_Unshifted['9'] = '9';
	in_Unshifted['-'] = '-';
	in_Unshifted['='] = '=';
	in_Unshifted['['] = '[';
	in_Unshifted[']'] = ']';
	in_Unshifted['\\'] = '\\';
	in_Unshifted[';'] = ';';
	in_Unshifted['\''] = '\'';
	in_Unshifted[','] = ',';
	in_Unshifted['.'] = '.';
	in_Unshifted['/'] = '/';
	in_Unshifted['`'] = '`';

	for(i = 0; i < 256; i++)
	{
		characterTable[i] = i;
	}

	for(i = 0; i < 256; i++)
	{
		shiftedCharacterTable[i] = i;
	}
	for (i=0;i<26;i++)
		shiftedCharacterTable[i+'a'] = i+'A';
	shiftedCharacterTable['0'] = ')';
	shiftedCharacterTable['1'] = '!';
	shiftedCharacterTable['2'] = '@';
	shiftedCharacterTable['3'] = '#';
	shiftedCharacterTable['4'] = '$';
	shiftedCharacterTable['5'] = '%';
	shiftedCharacterTable['6'] = '^';
	shiftedCharacterTable['7'] = '&';
	shiftedCharacterTable['8'] = '*';
	shiftedCharacterTable['9'] = '(';
	shiftedCharacterTable['-'] = '_';
	shiftedCharacterTable['='] = '+';
	shiftedCharacterTable['['] = '{';
	shiftedCharacterTable[']'] = '}';
	shiftedCharacterTable['\\'] = '|';
	shiftedCharacterTable[';'] = ':';
	shiftedCharacterTable['\''] = '"';
	shiftedCharacterTable[','] = '<';
	shiftedCharacterTable['.'] = '>';
	shiftedCharacterTable['/'] = '?';
	shiftedCharacterTable['`'] = '~';

	IN_AddReceiver(BND_InputReceiver);
	IN_AddReceiver(GUI_InputReceiver);
	IN_AddReceiver(CON_InputReceiver);
	IN_AddReceiver(SYS_InputReceiver); // 1st receiver

	in_initialized = true;
}

char IN_GetCharacter(int key)
{
	return characterTable[key];
}

char IN_GetShiftedCharacter(int key)
{
	return shiftedCharacterTable[key];
}

// shutdown
void IN_Shutdown()
{
	if(!in_initialized) return;
	in_initialized = false;


	for (int i=0;i<IN_MAXRECEIVERS;i++) // clean up some garbage
	{
		in_ReceiverPool[i].handler = 0;
		in_ReceiverPool[i].next = 0;
		in_ReceiverPool[i].prev = 0;
	}
	INW_Shutdown(); // shut down directinput
}

// per-frame operation
void IN_Update()
{
	if(!in_initialized) return;
	INW_Update(); // run directinput frame
}
void IN_FreeDevices()
{
	INW_KillFocus();
}
void IN_AcquireDevices()
{
	INW_GetFocus();
}

void IN_ResetSublayer()
{
	INW_Shutdown();
	INW_Init();
}

// allocate a new receiver on the receiver stack
// returns 0 if the allocation failed, or a handle to the receiver
int IN_AddReceiver(int (*handler)(in_Event *))
{
	int i;
	in_receiver_t *rec;

	for (i=0;i<IN_MAXRECEIVERS;i++)
	{
		rec = &in_ReceiverPool[i];
		if (!rec->handler)
		{
			// receiver is available
			rec->handler = handler;
			// add it to the list
			rec->prev = &in_ReceiverChain;
			rec->next = in_ReceiverChain.next;
			rec->prev->next = rec;
			rec->next->prev = rec;
			return((int)(rec - in_ReceiverPool) + 1);
		}
	}
	return(0);
}

// change the handler of an existing receiver
void IN_ChangeReceiverHandler(int receiver, int (*handler)(in_Event *))
{
	in_receiver_t *rec;

	if ((!receiver) || (receiver > IN_MAXRECEIVERS))
		return; // trying to free an out-of-range receiver
	rec = &in_ReceiverPool[receiver - 1];
	if (!rec->handler)
		return; // trying to free an already free receiver

	rec->handler = handler;
}

// take a receiver out of the stack and free it, takes handle as parameter
void IN_RemoveReceiver(int receiver)
{
	in_receiver_t *rec;

	if ((!receiver) || (receiver > IN_MAXRECEIVERS))
		return; // trying to free an out-of-range receiver
	rec = &in_ReceiverPool[receiver - 1];
	if (!rec->handler)
		return; // trying to free an already free receiver
	
	rec->handler = NULL;
	// unlink from the list
	rec->prev->next = rec->next;
	rec->next->prev = rec->prev;
}

// return the string name for a key
char* IN_NameForKey(int key)
{
	int i;
	static char tempStr[2] = {0,0};

	if ((!key) || (key >= INKEY_NUMKEYS))
		return(NULL); // out of range
	for (i=0; in_KeyNames[i].name; i++)
	{
		if (in_KeyNames[i].key == key)
		{
			return(in_KeyNames[i].name);
		}
	}
	// not an extended key, use regular characters
	if ((key < ' ') || (key > 127))
		return(NULL); // out of range
	if ((key >= 'a') && (key <= 'z'))
		key = IN_GetShiftedKey(key); // use uppercase for letters
	tempStr[0] = key;
	return(tempStr);
}

char* IN_NameForLCKey(int key)
{
	int i;
	static char tempStr[2] = {0,0};

	if ((!key) || (key >= INKEY_NUMKEYS))
		return(NULL); // out of range
	for (i=0; in_KeyNames[i].name; i++)
	{
		if (in_KeyNames[i].key == key)
		{
			return(in_KeyNames[i].name);
		}
	}
	// not an extended key, use regular characters
	if ((key < ' ') || (key > 127))
		return(NULL); // out of range
	if ((key >= 'a') && (key <= 'z'))
		key = IN_GetUnshiftedKey(key); // use lowercase for letters
	tempStr[0] = key;
	return(tempStr);
}


// return a key matching the string name
int IN_KeyForName(char* name)
{
	int i;
	char *str;

	for (i=0;i<INKEY_NUMKEYS;i++)
	{
		if ((str = IN_NameForKey(i)) && (!stricmp(name, str)))
			return(i);
	}
	return(0);
}

// returns a version of a key as if SHIFT were held down
// only works with regular characters
int IN_GetShiftedKey(int key)
{
	if (in_Shifted[key])
		return(in_Shifted[key]);
	return(key);
}

int IN_GetUnshiftedKey(int key)
{
	if(in_Unshifted[key])
		return(in_Unshifted[key]);
	return(key);
}
//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================

//****************************************************************************
//**
//**    END MODULE IN_MAIN.CPP
//**
//****************************************************************************

