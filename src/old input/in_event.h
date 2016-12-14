// in_win sends events for all input
// in_event defines the event class and
// constants for keys and such

#ifndef IN_EVENT_H
#define IN_EVENT_H

enum inev_EventType
{
	INEV_NONE, // invalid
	INEV_MOUSEMOVE, // mouse has changed position
	INEV_PRESS, // key changed from released to pressed
	INEV_DRAG, // key is still being pressed
	INEV_RELEASE, // key changed from pressed to released

	INEV_NUMTYPES // number of types of events
};

enum
{
	// first three flags hold status of modifier keys
	INEVF_SHIFT		= 1,
	INEVF_ALT		= 2,
	INEVF_CTRL		= 4
};

// key constants
enum
{
	// --------------------
	// characters 0-127 map to their ascii characters
	// --------------------

	INKEY_NULL			= 0x00000000,
    INKEY_BACKSPACE		= 0x00000008,
    INKEY_TAB			= 0x00000009,
    INKEY_ENTER			= 0x0000000D,
    INKEY_ESCAPE		= 0x0000001B,
    INKEY_SPACE			= 0x00000020, // same as ' ', here for convenience
	// regular characters don't need constants here; use 'a' 'b' etc.

	// --------------------
	// characters 128-255 are used for extended keys
	// --------------------
    
	// modifier keys
    INKEY_LEFTSHIFT		= 0x00000080,
    INKEY_RIGHTSHIFT,
	INKEY_LEFTCTRL,
	INKEY_RIGHTCTRL,
    INKEY_LEFTALT,
    INKEY_RIGHTALT,
	// arrow keys
	INKEY_LEFTARROW,
    INKEY_RIGHTARROW,
    INKEY_UPARROW,
    INKEY_DOWNARROW,
    // function keys
	INKEY_F1,
    INKEY_F2,
    INKEY_F3,
    INKEY_F4,
    INKEY_F5,
    INKEY_F6,
    INKEY_F7,
    INKEY_F8,
    INKEY_F9,
    INKEY_F10,
    INKEY_F11,
    INKEY_F12,
	// cursor control keys
    INKEY_INS,
    INKEY_DEL,
    INKEY_HOME,
    INKEY_END,
    INKEY_PGUP,
    INKEY_PGDN,
	// numeric keypad
    INKEY_NUMSLASH,
    INKEY_NUMSTAR,
    INKEY_NUMMINUS,
    INKEY_NUMPLUS,
    INKEY_NUMENTER,
    INKEY_NUMPERIOD,
    INKEY_NUM0,
    INKEY_NUM1,
    INKEY_NUM2,
    INKEY_NUM3,
    INKEY_NUM4,
    INKEY_NUM5,
    INKEY_NUM6,
    INKEY_NUM7,
    INKEY_NUM8,
    INKEY_NUM9,
    // locks and misc keys
	INKEY_NUMLOCK,
    INKEY_CAPSLOCK,
    INKEY_SCROLLLOCK,
    INKEY_PRINTSCRN,
    INKEY_PAUSE,
	// windows keys deliberately not listed (i don't believe in
	// keyboards specially rigged for a single operating system)

	// --------------------
	// characters 256 and up used for mouse buttons etc.
	// --------------------

	INKEY_MOUSELEFT		= 0x00000100,
	INKEY_MOUSERIGHT,
	INKEY_MOUSEMIDDLE,

	// --------------------
	INKEY_NUMKEYS
};

class in_Event
{
public:
	inev_EventType eventType;  // type of event
	unsigned long key; // INKEY_ value
	unsigned long flags; // INEVF_ flag combination
	signed long mouseX, mouseY; // mouse position within set limits
	signed long mouseFreeX, mouseFreeY; // mouse position without limits
	signed long mouseDeltaX, mouseDeltaY; // mouse change since last context event
	float pressTimeDelta; //  time since key was first pressed, for key events
	unsigned long eventIndex; // index of event, incremented for each event sent
	float frameTimeStamp; // timestamp of event at frame resolution
	float lastPressEventDelta; // time since key last had a press event
};

#endif