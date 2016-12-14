#ifndef CINPUTSUBSYSTEM_H
#define CINPUTSUBSYSTEM_H

#include "ISubsystem.h"
#include "CEventRouter.h"
#include "math_main.h"


enum InputKeyCodes // the first 322 of these match up with the SDLKey enum
{			/* The keyboard syms have been cleverly chosen to map to ASCII */
	IKC_UNKNOWN		= 0,
	IKC_FIRST		= 0,
	IKC_BACKSPACE		= 8,
	IKC_TAB		= 9,
	IKC_CLEAR		= 12,
	IKC_RETURN		= 13,
	IKC_PAUSE		= 19,
	IKC_ESCAPE		= 27,
	IKC_SPACE		= 32,
	IKC_EXCLAIM		= 33,
	IKC_QUOTEDBL		= 34,
	IKC_HASH		= 35,
	IKC_DOLLAR		= 36,
	IKC_AMPERSAND		= 38,
	IKC_QUOTE		= 39,
	IKC_LEFTPAREN		= 40,
	IKC_RIGHTPAREN		= 41,
	IKC_ASTERISK		= 42,
	IKC_PLUS		= 43,
	IKC_COMMA		= 44,
	IKC_MINUS		= 45,
	IKC_PERIOD		= 46,
	IKC_SLASH		= 47,
	IKC_0			= 48,
	IKC_1			= 49,
	IKC_2			= 50,
	IKC_3			= 51,
	IKC_4			= 52,
	IKC_5			= 53,
	IKC_6			= 54,
	IKC_7			= 55,
	IKC_8			= 56,
	IKC_9			= 57,
	IKC_COLON		= 58,
	IKC_SEMICOLON		= 59,
	IKC_LESS		= 60,
	IKC_EQUALS		= 61,
	IKC_GREATER		= 62,
	IKC_QUESTION		= 63,
	IKC_AT			= 64,
	/* 
	Skip uppercase letters
	*/
	IKC_LEFTBRACKET	= 91,
	IKC_BACKSLASH		= 92,
	IKC_RIGHTBRACKET	= 93,
	IKC_CARET		= 94,
	IKC_UNDERSCORE		= 95,
	IKC_BACKQUOTE		= 96,
	IKC_a			= 97,
	IKC_b			= 98,
	IKC_c			= 99,
	IKC_d			= 100,
	IKC_e			= 101,
	IKC_f			= 102,
	IKC_g			= 103,
	IKC_h			= 104,
	IKC_i			= 105,
	IKC_j			= 106,
	IKC_k			= 107,
	IKC_l			= 108,
	IKC_m			= 109,
	IKC_n			= 110,
	IKC_o			= 111,
	IKC_p			= 112,
	IKC_q			= 113,
	IKC_r			= 114,
	IKC_s			= 115,
	IKC_t			= 116,
	IKC_u			= 117,
	IKC_v			= 118,
	IKC_w			= 119,
	IKC_x			= 120,
	IKC_y			= 121,
	IKC_z			= 122,
	IKC_DELETE		= 127,
	/* End of ASCII mapped keysyms */

	/* International keyboard syms */
	IKC_WORLD_0		= 160,		/* 0xA0 */
	IKC_WORLD_1		= 161,
	IKC_WORLD_2		= 162,
	IKC_WORLD_3		= 163,
	IKC_WORLD_4		= 164,
	IKC_WORLD_5		= 165,
	IKC_WORLD_6		= 166,
	IKC_WORLD_7		= 167,
	IKC_WORLD_8		= 168,
	IKC_WORLD_9		= 169,
	IKC_WORLD_10		= 170,
	IKC_WORLD_11		= 171,
	IKC_WORLD_12		= 172,
	IKC_WORLD_13		= 173,
	IKC_WORLD_14		= 174,
	IKC_WORLD_15		= 175,
	IKC_WORLD_16		= 176,
	IKC_WORLD_17		= 177,
	IKC_WORLD_18		= 178,
	IKC_WORLD_19		= 179,
	IKC_WORLD_20		= 180,
	IKC_WORLD_21		= 181,
	IKC_WORLD_22		= 182,
	IKC_WORLD_23		= 183,
	IKC_WORLD_24		= 184,
	IKC_WORLD_25		= 185,
	IKC_WORLD_26		= 186,
	IKC_WORLD_27		= 187,
	IKC_WORLD_28		= 188,
	IKC_WORLD_29		= 189,
	IKC_WORLD_30		= 190,
	IKC_WORLD_31		= 191,
	IKC_WORLD_32		= 192,
	IKC_WORLD_33		= 193,
	IKC_WORLD_34		= 194,
	IKC_WORLD_35		= 195,
	IKC_WORLD_36		= 196,
	IKC_WORLD_37		= 197,
	IKC_WORLD_38		= 198,
	IKC_WORLD_39		= 199,
	IKC_WORLD_40		= 200,
	IKC_WORLD_41		= 201,
	IKC_WORLD_42		= 202,
	IKC_WORLD_43		= 203,
	IKC_WORLD_44		= 204,
	IKC_WORLD_45		= 205,
	IKC_WORLD_46		= 206,
	IKC_WORLD_47		= 207,
	IKC_WORLD_48		= 208,
	IKC_WORLD_49		= 209,
	IKC_WORLD_50		= 210,
	IKC_WORLD_51		= 211,
	IKC_WORLD_52		= 212,
	IKC_WORLD_53		= 213,
	IKC_WORLD_54		= 214,
	IKC_WORLD_55		= 215,
	IKC_WORLD_56		= 216,
	IKC_WORLD_57		= 217,
	IKC_WORLD_58		= 218,
	IKC_WORLD_59		= 219,
	IKC_WORLD_60		= 220,
	IKC_WORLD_61		= 221,
	IKC_WORLD_62		= 222,
	IKC_WORLD_63		= 223,
	IKC_WORLD_64		= 224,
	IKC_WORLD_65		= 225,
	IKC_WORLD_66		= 226,
	IKC_WORLD_67		= 227,
	IKC_WORLD_68		= 228,
	IKC_WORLD_69		= 229,
	IKC_WORLD_70		= 230,
	IKC_WORLD_71		= 231,
	IKC_WORLD_72		= 232,
	IKC_WORLD_73		= 233,
	IKC_WORLD_74		= 234,
	IKC_WORLD_75		= 235,
	IKC_WORLD_76		= 236,
	IKC_WORLD_77		= 237,
	IKC_WORLD_78		= 238,
	IKC_WORLD_79		= 239,
	IKC_WORLD_80		= 240,
	IKC_WORLD_81		= 241,
	IKC_WORLD_82		= 242,
	IKC_WORLD_83		= 243,
	IKC_WORLD_84		= 244,
	IKC_WORLD_85		= 245,
	IKC_WORLD_86		= 246,
	IKC_WORLD_87		= 247,
	IKC_WORLD_88		= 248,
	IKC_WORLD_89		= 249,
	IKC_WORLD_90		= 250,
	IKC_WORLD_91		= 251,
	IKC_WORLD_92		= 252,
	IKC_WORLD_93		= 253,
	IKC_WORLD_94		= 254,
	IKC_WORLD_95		= 255,		/* 0xFF */

	/* Numeric keypad */
	IKC_KP0		= 256,
	IKC_KP1		= 257,
	IKC_KP2		= 258,
	IKC_KP3		= 259,
	IKC_KP4		= 260,
	IKC_KP5		= 261,
	IKC_KP6		= 262,
	IKC_KP7		= 263,
	IKC_KP8		= 264,
	IKC_KP9		= 265,
	IKC_KP_PERIOD		= 266,
	IKC_KP_DIVIDE		= 267,
	IKC_KP_MULTIPLY	= 268,
	IKC_KP_MINUS		= 269,
	IKC_KP_PLUS		= 270,
	IKC_KP_ENTER		= 271,
	IKC_KP_EQUALS		= 272,

	/* Arrows + Home/End pad */
	IKC_UP			= 273,
	IKC_DOWN		= 274,
	IKC_RIGHT		= 275,
	IKC_LEFT		= 276,
	IKC_INSERT		= 277,
	IKC_HOME		= 278,
	IKC_END		= 279,
	IKC_PAGEUP		= 280,
	IKC_PAGEDOWN		= 281,

	/* Function keys */
	IKC_F1			= 282,
	IKC_F2			= 283,
	IKC_F3			= 284,
	IKC_F4			= 285,
	IKC_F5			= 286,
	IKC_F6			= 287,
	IKC_F7			= 288,
	IKC_F8			= 289,
	IKC_F9			= 290,
	IKC_F10		= 291,
	IKC_F11		= 292,
	IKC_F12		= 293,
	IKC_F13		= 294,
	IKC_F14		= 295,
	IKC_F15		= 296,

	/* Key state modifier keys */
	IKC_NUMLOCK		= 300,
	IKC_CAPSLOCK		= 301,
	IKC_SCROLLOCK		= 302,
	IKC_RSHIFT		= 303,
	IKC_LSHIFT		= 304,
	IKC_RCTRL		= 305,
	IKC_LCTRL		= 306,
	IKC_RALT		= 307,
	IKC_LALT		= 308,
	IKC_RMETA		= 309,
	IKC_LMETA		= 310,
	IKC_LSUPER		= 311,		/* Left "Windows" key */
	IKC_RSUPER		= 312,		/* Right "Windows" key */
	IKC_MODE		= 313,		/* "Alt Gr" key */
	IKC_COMPOSE		= 314,		/* Multi-key compose key */

	/* Miscellaneous function keys */
	IKC_HELP		= 315,
	IKC_PRINT		= 316,
	IKC_SYSREQ		= 317,
	IKC_BREAK		= 318,
	IKC_MENU		= 319,
	IKC_POWER		= 320,		/* Power Macintosh power key */
	IKC_EURO		= 321,		/* Some european keyboards */
	IKC_UNDO		= 322,		/* Atari keyboard has Undo */

	/* Add any other keys here */

	IKC_LAST		
};

enum InputEventTypes // bound events could be here... but does that make sense?
{
	IET_STATECHANGE,
	IET_CHARACTERTYPED
};

enum InputKeyStates
{
	IKS_DOWN = 0,
	IKS_UP = 1
};

enum InputKeyActions
{
	IKA_PRESS = 0,
	IKA_HOLD = 1,
	IKA_RELEASE = 2
};

class CInputEvent
{
public:
	InputEventTypes inputEventType; // was it a key state change or a character that was typed?
	// for key state changes
	InputKeyCodes keyCode; // if it was a key state change, which key was it?
	InputKeyActions keyAction; // ... and what action was it?
	// for character typing
	unsigned short characterCode; // if it was a typed character, what was the character?
};


class CInputSubsystem: public IThinkSubsystem
{
public:
	CInputSubsystem();
	virtual ~CInputSubsystem();
	
	virtual bool PreInitialize(); // PreInitialize is called before Initialize, only links to other subsystems should be made here
	virtual bool Initialize();
	virtual bool Shutdown();
	
	virtual void Think();

	virtual CRouterReturnCode EventReceiver(CRouterEvent& event);
	
	void AddInputReceiver(bool (*) (const CInputEvent& event));
	
	unsigned short GetShiftedCharacterCode(unsigned short unshiftedCharacterCode);	
private:
	void DispatchInputEvent(const CInputEvent& event);

	// input reciever interface.... bool ReceiveInputEvent(const CInputEvent& event);
	vector<bool (*)(const CInputEvent& event)> inputReceivers;
	
	InputKeyStates oldKeyStates[IKC_LAST]; // used to detect state changes
	InputKeyStates keyStates[IKC_LAST];
	unsigned short shiftedCharacters[IKC_LAST];

	bool initialized;
};



extern CInputSubsystem Input;


#endif
