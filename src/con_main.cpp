#include "con_main.h"
#include "sys_main.h"
#include "in_main.h"
#include "in_win.h"
#include "vid_win.h"
#include "CVideoSubsystem.h"
#include "con_display.h"
#include "bnd_main.h"
#include "m_misc.h"
#include "CEventRouter.h"
#include "CToken.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

using namespace std;

extern ofstream fout_log;

bool con_initialized(false);

con_Command* commandList = 0; // head of linked list
con_Variable* variableList = 0; // linked list of variables
FunctionNode* conInitAuxList = 0; // list of auxiliary initialization functions

string textBuffer[NUM_TEXT_BUFFER_LINES];
unsigned short textBufferDisplayLocation(0);

string prevEntryBuffer[NUM_ENTRIES_REMEMBERED];
string nextEntryBuffer[NUM_ENTRIES_REMEMBERED];

string inputLine; // current line of input; is probably temporary
short inputCursor(0);

void CON_Message(string text);

void scrollTextBuffer();
void scrollPrevEntryBuffer();
void scrollNextEntryBuffer();
void useEntryBufferPrev();
void useEntryBufferNext();

unsigned long con_inputReceiver;
con_Cout ccout;

////////////////////////////////////////////////
// class implementations
////////////////////////////////////////////////

con_Cout& con_Cout::operator << (string rhs)
{
	message(rhs);
	return *this;
}

con_Cout& con_Cout::operator << (const char* rhs)
{
	message(string(rhs));
	return *this;
}	
con_Cout& con_Cout::operator << (char rhs)
{
	char cstring[2] = {rhs, '\0'};
	message(cstring);
	return *this;
}
con_Cout& con_Cout::operator << (unsigned char rhs)
{
	char cstring[2] = {rhs, '\0'};
	message(cstring);
	return *this;
}
con_Cout& con_Cout::operator << (double rhs)
{
	message(M_ftoa(rhs));
	return *this;
}
con_Cout& con_Cout::operator << (int rhs)
{
	message(M_itoa(rhs));
	return *this;
}
con_Cout& con_Cout::operator << (long int rhs)
{
	message(M_itoa(rhs));
	return *this;
}
con_Cout& con_Cout::operator << (unsigned long int rhs)
{
	message(M_itoa(rhs));
	return *this;
}
con_Cout& con_Cout::operator << (short int rhs)
{
	message(M_itoa(rhs));
	return *this;
}
con_Cout& con_Cout::operator << (unsigned short int rhs)
{
	message(M_itoa(rhs));
	return *this;
}
con_Cout& con_Cout::operator << (bool rhs)
{
	message(M_btoa(rhs));
	return *this;
}



void con_Cout::message(string text)
{
	cout << text;
	if(!con_initialized) 
	{
		SYS_Message(text);
		return;
	}
	fout_log << text;
	for(unsigned int i = 0; i < text.length(); i++)
	{
		if(text[i] == '\n')
		{
			scrollTextBuffer();
		}
		else
			textBuffer[0] += text[i];
	}
}


con_Input::con_Input(string text)
{
	nextstatement = 0;
	unsigned short character(0); // character counter
	int argument(0); // word counter
	bool lastCharWasWhitespace(true); // ignore extra whitespace
	unparsedtext = text;

	for(character = 0; character < text.length(); character++) // for every character in the recieved text
	{
		if( text[character] == '"') // make anything inside quotes one argument
		{
			character++; // move past the double quote
			if( character >= text.length() ) // oops, bad input, better not crash because of it
				return;

			while( text[character] != '"' ) // continue until end quote
			{
				if( (text[character] == '\n') || (character >= text.length()) ) // oops, bad input, better not crash because of it
					return;
				
				arguments[argument] += text[character]; // add char to current argument

				character++;
			}
	//taken care of by for statement when continues		character++; // move past the end double quote

			numarguments = argument+1;
			argument++;
			continue;
		}

		if( (text[character] == ' ') | (text[character] == '\t')) // if current character is a space or tab, move on to next word
		{
			if(!lastCharWasWhitespace) // last char wasn't whitespace
			{
				argument++; // advance to next argument
			}
			lastCharWasWhitespace = true;
			continue; // don't let the whitespace be added to argument
		}
		if( text[character] == '\n' ) // if theres a new line, stop parsing
		{
			return;
		}

		if( (text[character] == ';') ) // means there's a new statement coming
		{
			string nextInput; // the rest of the line not including before the semicolon
			
			for(++character; character < text.length(); character++) // take the rest of the line and build a string
			{
				if(text[character] == '\n')
					break;
				nextInput += text[character];
			}
			nextstatement = new con_Input(nextInput); // feed the new line to another con_Input object
			break;
		}
		else // current index contains a character that should be appended to the argument
		{
			arguments[argument] += text[character]; // add the character to the current word
			lastCharWasWhitespace = false;
		}

		numarguments = argument+1; // store the number of words for this line of input
		if(argument >= MAX_NUM_WORDS) return;
	}
}

con_Input::~con_Input()
{
	if(nextstatement)
	{
		delete nextstatement;
		nextstatement = 0;
	}
}



con_Command::con_Command(string newName, void (* newHandler) (con_Input& consoleInput))
:
nextCommand(0),
name(newName),
Handler(newHandler)
{}

con_Command::~con_Command()
{
	// nothing to clean up
}

con_Variable::con_Variable(string newName, void* newData, string newType, void (* newHandler) (con_Input& consoleInput))
:
nextVariable(0),
name(newName),
type(newType),
Handler(newHandler), 
data(newData) 
{}

con_Variable::~con_Variable()
{
	// nothing to clean up
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//// command specific handler functions and declarations
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

// commands class instances prefixed with con_cmd_
// handlers prefixed with con_cmd_ and suffixed with _handler and have (con_Input& input) as argument
// variables prefixed with con_var_
/*
void con_cmd_ _handler(con_Input& input)
{
}
con_Command con_cmd_ ("", con_cmd_ _handler);
*/

/*CON_CMD(camtarget)
{
	if(input.numarguments < 2)
	{
		ccout << "usage: camtarget [target]" << newl;
	}
        else if(input.numarguments == 2)
	{
		Video.camera.targetUid = aliases.getUid(input.arguments[1]);
	}
        else
        {
            Video.camera.targetType = vid_Camera::location;
            Video.camera.destination = TranslationMatrix(v3d(atoi(input.arguments[1].c_str()),atoi(input.arguments[2].c_str())));
        }
}*/

void con_cmd_listkeys_handler(con_Input& input)
{
	short index = 0;	
	char* current = in_KeyNames[index].name;
	ccout << "Non alpha-numeric key names:" << newl;
	while(current) // try to find a key name that matches
	{
		ccout << current << "  ";
		if(index%4 == 0)
			ccout << newl;
		index++;
		current = in_KeyNames[index].name;
	}
	ccout << newl;
}
con_Command con_cmd_listkeys("listkeys", con_cmd_listkeys_handler);


void con_cmd_vid_res_handler(con_Input& input)
{
	if(input.numarguments < 3)
	{
		ccout << "current display resolution: " << Video.settings.getSw() << 'x' << Video.settings.getSh() << 'x' << Video.settings.getBpp();
		if(Video.settings.getFullscreen())
			ccout << " fullscreen" << newl;
		else
			ccout << " windowed" << newl;

		ccout << "usage: vid_res [x_resolution] [y_resolution] [fullscreen]" << newl;
	}
	else
	{
		if(Video.IsInitialized())
		{
			Video.SetResolution(atoi(input.arguments[1].c_str()), atoi(input.arguments[2].c_str()), 32, M_atob(input.arguments[3].c_str()) );
			if(consoleDown)
				consoleY = Video.settings.getSh()/2;
			ccout << "Video resolution changed." << newl;
		}
	}
}
con_Command con_cmd_vid_res("vid_res", con_cmd_vid_res_handler);


void con_cmd_console_handler(con_Input& input)
{
	if(consoleDown && !consoleMoving)
	{
		consoleMoving = true;
	}
	else
	{ 
		if(!consoleMoving && !consoleDown)
		{
			consoleMoving = true;
		}
	}
}
con_Command con_cmd_console("console", con_cmd_console_handler);

void con_cmd_quit_handler(con_Input& input) // quit the program
{
	ccout << "Shutting down...\n";
	quit = true;
	return;
}
con_Command con_cmd_quit("quit", con_cmd_quit_handler);
con_Command con_cmd_exit("exit", con_cmd_quit_handler); // alias

void con_cmd_echo_handler(con_Input& input) // echo a message to the console
{
	string echo;
	for(int i = 1; i < input.numarguments; i++) // skip "echo"
	{
		echo += input.arguments[i];
		echo += ' ';
	}
	ccout << echo << newl;
	return;
}
con_Command con_cmd_echo("echo", con_cmd_echo_handler);


void con_cmd_listcmds_handler(con_Input& input) // list all registered commands
{
	con_Command* currentCommand = commandList;
	short index = 0;
	while(currentCommand)
	{
		ccout << currentCommand->name << newl;
		currentCommand = currentCommand->nextCommand;
	}
}
con_Command con_cmd_listcmds("listcmds", con_cmd_listcmds_handler);


void con_cmd_listvars_handler(con_Input& input) // list all registered variables
{
	con_Variable* currentVariable = variableList;
	short index = 0;
	while(currentVariable)
	{
		ccout << currentVariable->name << newl;
		currentVariable = currentVariable->nextVariable;
	} 
}
con_Command con_cmd_listvars("listvars", con_cmd_listvars_handler);


void con_cmd_exec_handler(con_Input& input) // execute commands from a text file
{
	if(input.numarguments < 2)
		ccout << "exec [file name]\n";
	else
		CON_ExecFile(input.arguments[1].c_str());
}
con_Command con_cmd_exec("exec", con_cmd_exec_handler);
//////////////////////////////////////////////////////
// integrated handler functions for variables
//////////////////////////////////////////////////////

void con_cmd_int_handler(con_Input& input) // handler for int variables
{
	if(variableList) // only do this if any variables exist
	{
		con_Variable* tempVariable = variableList;
		while(1) // check to find the right variable
		{
			if( input.arguments[0] == tempVariable->name ) // if requested variable matches current
			{
				// tempVariable is pointing to the variable that we want to mess with
				if(input.numarguments <= 1) // the only argument is the variable name
				{
					// print out the value of the variable
					// TODO: make this convert to a string then print later...
					int* temp = (int*)(tempVariable->data);
					ccout << *temp << newl;//cout << *temp << endl;
				}
				else // more than 1 argument
				{
					// assign a value to the variable
					int* temp = (int*)(tempVariable->data);
					*temp = (int)atoi(input.arguments[1].c_str());
				}
				return; // return because the input was taken care of
			}
			if(tempVariable->nextVariable)
				tempVariable = tempVariable->nextVariable; // make pointer point to next variable
			else break;
		}
	}
}

void con_cmd_float_handler(con_Input& input) // handler for float variables
{
	if(variableList) // only do this if any variables exist
	{
		con_Variable* tempVariable = variableList;
		while(1) // check to find the right variable
		{
			if( input.arguments[0] == tempVariable->name) // if requested variable matches current
			{
				// tempVariable is pointing to the variable that we want to mess with
				if(input.numarguments <= 1) // the only argument is the variable name
				{
					// print out the value of the variable
					// TODO: make this convert to a string then print later...
					float* temp = (float*)(tempVariable->data);
					ccout << *temp << newl;
				}
				else // more than 1 argument
				{
					// assign a value to the variable
					float* temp = (float*)(tempVariable->data);
					*temp = (float)atof(input.arguments[1].c_str());
				}
				return; // return because the input was taken care of
			}
			if(tempVariable->nextVariable)
				tempVariable = tempVariable->nextVariable; // make pointer point to next variable
			else break;
		}
	}
}

// usage: [variableName] [data]
//        [variableName] ["more than one word"]
//
// note:  [variableName] [more than one word] will assign "more" to the variable
void con_cmd_string_handler(con_Input& input) // handler for string variables--uses std::string
{
	if(variableList) // only do this if any variables exist
	{
		con_Variable* tempVariable = variableList;
		while(1) // check to find the right variable
		{
			if(input.arguments[0] == tempVariable->name) // if requested variable matches current
			{
				// tempVariable is pointing to the variable that we want to mess with
				if(input.numarguments <= 1) // the only argument is the variable name
				{
					// print out the value of the variable
					// TODO: make this convert to a string then print later...
					string* temp = (string*)(tempVariable->data);
					ccout << *temp << newl;
				}
				else // more than 1 argument
				{
					string* temp = (string*)(tempVariable->data); // temp will now point to data of the console variable
					*temp = ""; // clear the string

					if(input.arguments[1][0] == '"') // right hand side is quoted... take everything in the quotes
						// this is messy but it works
					{
						unsigned int i; // used to keep track of current character

						/* eat whitespace */
						i = input.arguments[0].length(); // set position to just after arg[0]
						while( (input.unparsedtext[i] == ' ') | (input.unparsedtext[i] == '\t') ) // eat whitespace between variable name and quote
							i++;
						i++;	// now i is at the first '"' so it needs to be skipped
						/* end eat whitespace */

						/* copy data */
						for(; i < input.unparsedtext.length(); i++) // continue from after the quote until 
																	// the end quote is reached or the command
																	// line ends
						{
							// now for the meat
							if(input.unparsedtext[i] == '"') // found the end quote so the rest can be ignored
								break;
							else
								*temp += input.unparsedtext[i]; // add the character to the new string
						}
						/* end copy data */
					}
					else // no quote found: simply put arg[1] in for its new value											
					{
						*temp = input.arguments[1];
					}
				}
				return; // return because the input was taken care of
			}
			if(tempVariable->nextVariable)
				tempVariable = tempVariable->nextVariable; // make pointer point to next variable
			else break;
		}
	}
}

////////////////////////////////////////////////////////
// variables
////////////////////////////////////////////////////////

/*string name;
con_Variable con_var_name("name", &name, "string", con_cmd_string_handler);*/

////////////////////////////////////////////////
// subsystem interface implementations
////////////////////////////////////////////////

void CON_Exec(string command)
{
	if(!con_initialized) return;
	if(command.length() < 1)
		return;
	bool wasCommand(false); // set true if it was a command
	bool wasVariable(false); // set true if it was a command
	con_Input input(command.c_str()); // parses string and counts number of arguments
	con_Input* currentStatement = &input;
	for(; currentStatement; currentStatement = currentStatement->nextstatement)
	{
		if(commandList) // only do this if any commands exist
		{
			for(con_Command* tempCommand = commandList;
				tempCommand;
				tempCommand = tempCommand->nextCommand)
				// check to see if its a command
			{
				//	currentStatement->arguments[0].compare(
				if(currentStatement->arguments[0] == tempCommand->name) // if requested command matches current
				{
					tempCommand->Handler(*currentStatement);
					wasCommand = true;
					break; // statement was taken care of
				}
		
			}
		}
		if(variableList) // only do this if any variables exist
		{
			for(con_Variable* tempVariable = variableList;
				tempVariable;
				tempVariable = tempVariable->nextVariable)
			{
				if(currentStatement->arguments[0] == tempVariable->name) // if requested variable matches current
				{
					tempVariable->Handler(*currentStatement);
					wasVariable = true;
					break; // statement was taken care of
				}
			}
		}
		if((!(wasVariable | wasCommand)) /*&& (currentStatement->commandLine != "")*/) 
		{
			CRouterEvent event = TokenManager.BuildEventFromString( command );
			if( EventRouter.RouteEvent( event ) )
				ccout << "Unrecognized command or variable: "<< command << newl;
		}
	}
}

// ignores whitespace for the most part when executing files
void CON_ExecFile(string fileName)
{
	if(!con_initialized) return;
	ifstream fin;
	fin.open(fileName.c_str());
	if(fin.is_open()) // if the file was successfully opened
	{
		ccout << "Executing " << fileName << newl;
		char buffer[MAX_NUM_CHARS];
		while(!fin.eof())
		{
			fin.getline(buffer, MAX_NUM_CHARS); // get everything until next \n
			if(strcmp("\0", buffer)) // if the line is empty then ignore it
				CON_Exec(buffer);
		}
		fin.close();
	}
	else
	{
		ccout << "Error opening file: " << fileName << newl;
	}
}

void CON_RegisterCommand(con_Command* newCommand) // adds and automatically alphabetizes new commands
{
	// case 1: list is empty
	if(!commandList) // list is empty
	{
		commandList = newCommand;
		commandList->nextCommand = 0;
		return;
	}
	else // add command to beginning of list
	{
		if(newCommand->name < commandList->name) // case 2: is first in list
		{
			newCommand->nextCommand = commandList;
			commandList = newCommand;
			return;
		}
		// case 3: goes somewhere else in list
		// make sure no stray pointers are left by keeping track of previous command
		else
		{
			con_Command* current = commandList;
			while (current->nextCommand)
			{
				if(newCommand->name < current->nextCommand->name) // newCommand goes before nextCommand
				{
					con_Command* temp = current->nextCommand;
					current->nextCommand = newCommand;
					newCommand->nextCommand = temp;
					return;
				}
				current = current->nextCommand;
			}
			current->nextCommand = newCommand; // put at end of list
			current->nextCommand->nextCommand = 0;
			return;
		}
	}
	return;
}

void CON_RegisterVariable(con_Variable* newVariable)
{
	// case 1: list is empty
	if(!variableList) // list is empty
	{
		variableList = newVariable;
		variableList->nextVariable = 0;
		return;
	}
	else // add command to beginning of list
	{
		if(newVariable->name < variableList->name) // case 2: is first in list
		{
			newVariable->nextVariable = variableList;
			variableList = newVariable;
			return;
		}
		// case 3: goes somewhere else in list
		// make sure no stray pointers are left by keeping track of previous command
		else
		{
			con_Variable* current = variableList;
			while (current->nextVariable)
			{
				if(newVariable->name < current->nextVariable->name) // newCommand goes before nextCommand
				{
					con_Variable* temp = current->nextVariable;
					current->nextVariable = newVariable;
					newVariable->nextVariable = temp;
					return;
				}
				current = current->nextVariable;
			}
			current->nextVariable = newVariable; // put at end of list
			current->nextVariable->nextVariable = 0;
			return;
		}
	}
	return;
}

void CON_AddInitAuxFunction(void (* function)(void))
{
	// case 1: list is empty
	if(!conInitAuxList)
	{
		conInitAuxList = new FunctionNode;
		conInitAuxList->handler = function;
		conInitAuxList->next = 0;
	}
	else
	{
		FunctionNode* index;
		for(index = conInitAuxList; index->next; index = index->next);
		index->next = new FunctionNode;
		index->next->handler = function;
		index->next->next = 0;		
	}
}

void CON_Init()
{
	if(con_initialized) return;
	//CON_CMD_REG(camtarget);
	CON_RegisterCommand(&con_cmd_quit);
	CON_RegisterCommand(&con_cmd_exit);
	CON_RegisterCommand(&con_cmd_echo);
	CON_RegisterCommand(&con_cmd_listcmds);
	CON_RegisterCommand(&con_cmd_listvars);
	CON_RegisterCommand(&con_cmd_exec);
	CON_RegisterCommand(&con_cmd_console);
	CON_RegisterCommand(&con_cmd_vid_res);
	CON_RegisterCommand(&con_cmd_listkeys);

	if(conInitAuxList)
	{
		for(FunctionNode* i = conInitAuxList; i; i = i->next)
			i->handler();
	}

	Input.AddInputReceiver(CON_InputReceiver);
	
	con_initialized = true;
}

void CON_Shutdown()
{
	if(!con_initialized) return;
	con_initialized = false;

	variableList = 0;
	commandList = 0;
}

//unsigned char CON_InputReceiver(in_Event* event)
bool CON_InputReceiver(const CInputEvent& event)
{
	if(!con_initialized) return false;
	
	if(event.inputEventType == IET_STATECHANGE)
	{
		if(event.keyAction == IKA_PRESS)
		{
			switch(event.keyCode)
			{
			case '`':
				CON_Exec("console");
				break;
			}
		}
	}
	
	if(!consoleDown && !consoleMoving)
		return false;
	//if(event->eventType == INEV_MOUSEMOVE)
	//	return false;
/*	static float keyHoldStart(0);
	static unsigned char lastKey(0);
	bool shifted(false);


	if(event->eventType == INEV_PRESS) // key was pressed
	{
		lastKey = event->key;
		keyHoldStart = sys_curTime;
	}
	if(event->eventType == INEV_DRAG)
	{
		if(lastKey != event->key)
			return 1;
		if(sys_curTime - keyHoldStart < .3)
			return 1;
	}
	if(event->eventType == INEV_RELEASE)
	{
		keyHoldStart = 0;
		lastKey = 0;
		return 1;
	}
	if(event->flags & INEVF_SHIFT)
		shifted = true;*/
	if(event.inputEventType == IET_CHARACTERTYPED)
		switch(event.characterCode) // not really unicode for now... ascii!!!
		{
			case IKC_RETURN:
				if(inputLine != "")
				{
					if(textBuffer[0] != "")
						ccout << newl;
					ccout << ']' << inputLine << newl;
					CON_Exec(inputLine);

					scrollPrevEntryBuffer();
					prevEntryBuffer[0] = inputLine;

					inputLine = "";
					inputCursor = 0;
				}
				return true;
				break;
			case IKC_BACKSPACE:
				if( (inputLine.length() > 0))
				{
					//inputLine = "hello";
					//inputLine.erase(0, 2);
					if(inputCursor < inputLine.length())
						inputLine.erase(((signed)inputLine.length())-1-inputCursor, 1);
					if(inputCursor >= inputLine.length())
						inputCursor = inputLine.length();
				}
				return true;
				break;
			case IKC_DELETE:
				{
					if(inputCursor < inputLine.length()+1 && inputCursor > 0)
					{
						inputLine.erase(((signed)inputLine.length())-inputCursor, 1);
						inputCursor--;
					}
					if(inputCursor < 0)
						inputCursor = 0;
				}
				return true;
				break;
			case IKC_UP:
				useEntryBufferPrev();
				if(inputLine == "")
					useEntryBufferNext(); // don't scroll if it's empty
				return true;
				break;
			case IKC_DOWN:
				useEntryBufferNext();
				if(inputLine == "")
					useEntryBufferPrev(); // don't scroll if it's empty
				return true;
				break;
			case IKC_LEFT:
				{
					if(inputCursor < inputLine.size())
						inputCursor++;
					return true;
				}
				break;
			case IKC_RIGHT:
				{
					if(inputCursor > 0)
						inputCursor--;
					return true;
				}
				break;
			case IKC_PAGEUP:
				textBufferDisplayLocation++;
				if(textBufferDisplayLocation > NUM_TEXT_BUFFER_LINES-2)
					textBufferDisplayLocation = NUM_TEXT_BUFFER_LINES-2; // 2 because the location 0 is used for the input line(?) and it's one more than the max index
				return true;
				break;
			case IKC_PAGEDOWN:
				if(textBufferDisplayLocation != 0)
					textBufferDisplayLocation--;
				return true;
				break;

			default:
				/*if(event->key == INKEY_TAB)
					if(event->flags & INEVF_ALT)
						return true;*/

				if(/*IN_GetCharacter(event->key) == '`'*/ event.characterCode == '`')
				{
					return false;
					break;
				}
				if(event.characterCode < 128)
				{
					string character;
					character += (char)event.characterCode;
					inputLine.insert(((signed)inputLine.length())-inputCursor, character);
					return true;
					/*if(shifted)
					{
						//inputLine += IN_GetShiftedCharacter(event->key);
						string character;
						character += IN_GetShiftedCharacter(event->key);
						inputLine.insert(((signed)inputLine.length())-inputCursor, character);

						return true;
						break;
					}	
					else
					{
					//	inputLine += IN_GetCharacter(event->key);
						string character;
						character += IN_GetCharacter(event->key);
						inputLine.insert(((signed)inputLine.length())-inputCursor, character);
						return true;
						break;
					}*/
				} // if event->key < 128
				break;
		} // switch event->key
//	}	
	return false; 
}


void scrollTextBuffer()
{
	if(!con_initialized) return;
	for(int i = NUM_TEXT_BUFFER_LINES-1; i > 0; i--) // i > 0: dont copy -1 to 0
	{
		textBuffer[i] = textBuffer[i-1];
	}
	textBuffer[0] = "";
}

void scrollPrevEntryBuffer()
{
	if(!con_initialized) return;
	for(int i = NUM_ENTRIES_REMEMBERED-1; i > 0; i--)
	{
		prevEntryBuffer[i] = prevEntryBuffer[i-1];
	}
	prevEntryBuffer[0] = "";
}

void scrollNextEntryBuffer()
{
	if(!con_initialized) return;
	for(int i = NUM_ENTRIES_REMEMBERED-1; i > 0; i--)
	{
		nextEntryBuffer[i] = nextEntryBuffer[i-1];
	}
	nextEntryBuffer[0] = "";
}

void useEntryBufferPrev()
{
	if(!con_initialized) return;
	scrollNextEntryBuffer();
	nextEntryBuffer[0] = inputLine;
	inputLine = prevEntryBuffer[0];
	for(int i = 0; i < NUM_ENTRIES_REMEMBERED-1; i++)
	{
		prevEntryBuffer[i] = prevEntryBuffer[i+1];
	}
	prevEntryBuffer[NUM_ENTRIES_REMEMBERED-1] = "";
}
void useEntryBufferNext()
{
	if(!con_initialized) return;
	scrollPrevEntryBuffer();
	prevEntryBuffer[0] = inputLine;
	inputLine = nextEntryBuffer[0];
	for(int i = 0; i < NUM_ENTRIES_REMEMBERED-1; i++)
	{
		nextEntryBuffer[i] = nextEntryBuffer[i+1];
	}
	nextEntryBuffer[NUM_ENTRIES_REMEMBERED-1] = "";
}

bool CON_StringEvaluatesTrue(string rhs)
{
	bool value(false);
	if(rhs[0] == 't')
		value = true;
	if(rhs[0] == '1')
		value = true;
	if(rhs[0] == 'T')
		value = true;
	if(rhs[0] == 'y')
		value = true;
	if(rhs[0] == 'Y')
		value = true;
	return value;
}


void CON_DebugDraw()
{
	con_Command* tempCommand = commandList;
	short index(0);
	Video.DrawTextShit("Commands:", Video.settings.getSw()/2, Video.settings.getSh()-8, 0);
	index++;
	while(tempCommand)
	{
		Video.DrawTextShit(tempCommand->name, Video.settings.getSw()/2, Video.settings.getSh()-8-index*8, 0);
		index++;
		tempCommand = tempCommand->nextCommand;
	}
	con_Variable* tempVariable = variableList;
	index = 0;
	Video.DrawTextShit("Variables:", Video.settings.getSw()/2+Video.settings.getSw()/4, Video.settings.getSh()-8, 0);
	index++;
	while(tempVariable)
	{
		Video.DrawTextShit(tempVariable->name, Video.settings.getSw()/2+Video.settings.getSw()/4, Video.settings.getSh()-8-index*8, 0);
		index++;
		tempVariable = tempVariable->nextVariable;
	}

}
