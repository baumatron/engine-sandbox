#include <string>
#include "in_event.h"
#include "m_misc.h"
#include "math_main.h"


using namespace std;

#define EXTERN_CON_CMD(name) \
extern con_Command con_cmd_##name

#define CON_CMD(name) \
void con_cmd_##name##_handler(con_Input& input);\
con_Command con_cmd_##name##(#name, con_cmd_##name##_handler);\
void con_cmd_##name##_handler(con_Input& input)

#define CON_CMD_REG(name) \
CON_RegisterCommand(&con_cmd_##name)

#define CON_VAR(variable, name, type) \
con_Variable con_var_##name##(#name, &##variable##, #type, con_cmd_##type##_handler);

#define CON_VAR_REG(name) \
CON_RegisterVariable(&con_var_##name)

#ifndef CON_MAIN_H
#define CON_MAIN_H

#define MAX_NUM_WORDS 30 // number of seperate words allowed in a line of input
#define MAX_NUM_CHARS 256 // max number of characters per line of input

#define NUM_TEXT_BUFFER_LINES 256
#define NUM_ENTRIES_REMEMBERED 8

class con_Input;
class con_Command;
class con_Variable;

void CON_PreInit(); // registers auxiliary functions for other subsystems, etc
void CON_Init();
void CON_Shutdown();

//void CON_Message(string text);

void CON_Exec(string command);
void CON_ExecFile(string fileName);
void CON_RegisterCommand(con_Command* newCommand);
void CON_RegisterVariable(con_Variable* newVariable);
void CON_AddInitAuxFunction(void (* function)(void));

bool CON_StringEvaluatesTrue(string rhs);

unsigned char CON_InputReceiver(in_Event* event);
void con_cmd_int_handler(con_Input& input); // handler for int variables
void con_cmd_float_handler(con_Input& input); // handler for float variables
void con_cmd_string_handler(con_Input& input); // handler for string variables

extern bool con_initialized;
extern string textBuffer[NUM_TEXT_BUFFER_LINES]; // this many lines will be saved and displayed if possible
extern unsigned short textBufferDisplayLocation;
extern con_Command con_cmd_console;
extern string inputLine;
extern short inputCursor;

class con_Cout
{
public:
	// basic types
	con_Cout& operator<<(string rhs);
	con_Cout& operator<<(const char* rhs);	
	con_Cout& operator<<(char rhs);
	con_Cout& operator<<(unsigned char rhs);
	con_Cout& operator<<(double rhs);
	con_Cout& operator<<(int rhs);
	con_Cout& operator<<(long int rhs);
	con_Cout& operator<<(unsigned long int rhs);
	con_Cout& operator<<(short int rhs);
	con_Cout& operator<<(unsigned short int rhs);
	con_Cout& operator<<(bool rhs);

private:
	void message(string text);
};
extern con_Cout ccout;

class con_Input // the info sent to the command/variable linked list: essentially the string input
{
public:
	con_Input(string input);
	~con_Input();
	con_Input(con_Input& rhs);

	string unparsedtext; // unparsed input string
	string arguments[MAX_NUM_WORDS]; // the parsed version of the input string
	short numarguments; // number of paramaters/words

	con_Input* nextstatement; // for those cases when there is more than one statement contained in the string recieved
};

class con_Command // this includes commands and variables since variables are basically commands
{
public:
	con_Command(string newName, void (* newHandler) (con_Input& consoleInput)); // constructor
	~con_Command(); // destructor

	string name;
	void (* Handler) (con_Input& consoleInput);
	con_Command* nextCommand;
};

class con_Variable
{
public:
	con_Variable(string newName, void* newData, string newType, void (* newHandler) (con_Input& consoleInput));
	~con_Variable();
	
	string name;
	string type;
	void* data; // actual data... float, int, char--assigned to a global's address
	void (* Handler) (con_Input& consoleInput);
	con_Variable* nextVariable;
};

void CON_DebugDraw();
#endif