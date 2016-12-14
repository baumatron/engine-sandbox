#include "bnd_main.h"
#include "con_main.h"
#include "in_main.h"
#include "CSoundSubsystem.h"
#include "in_event.h"
#include "con_display.h"
#include "m_misc.h"

bool bnd_initialized(false);


bnd_Bind* bindList;

bnd_Bind bnd_standard[256]; // because dynamic memory allocation is no fun
bnd_Bind bnd_console('`', "console", false, true, false, false);


void con_cmd_unbind_handler(con_Input& input)
{
	if(input.numarguments < 2)
	{
		ccout << "usage: unbind [key]" << newl;
	}
	bnd_standard[input.arguments[1][0]].handler = "";
}
con_Command con_cmd_unbind("unbind", con_cmd_unbind_handler);

void con_cmd_unbindall_handler(con_Input& input)
{
	for(int i = 0; i < 256; i++)
	{
		bnd_standard[i].handler = "";
	}
}
con_Command con_cmd_unbindall("unbindall", con_cmd_unbindall_handler);

void con_cmd_bind_handler(con_Input& input)
{
	if(input.numarguments == 2) // just bind and the key so print current command its bound to
	{
		input.arguments[1] = M_Uppercase(input.arguments[1]);
		short index = 0;	
		char* current = in_KeyNames[index].name;
		while(current) // try to find a key name that is the same
		{
			if(input.arguments[1] == current)
				break;
			else
			{
				index++;
				current = in_KeyNames[index].name;
			}
		}
		if(current == 0) // couldn't match a key name, so it must just be a character
		{
			if(input.arguments[1].length() > 1)
				return;

			ccout << "\"" << input.arguments[1] << "\" = \"" << bnd_standard[input.arguments[1][0]].handler << "\" on ";
			if( bnd_standard[input.arguments[1][0]].eventImplemented[INEV_MOUSEMOVE] )
				ccout << "[mouse move] ";
			if( bnd_standard[input.arguments[1][0]].eventImplemented[INEV_PRESS] )
				ccout << "[press] ";
			if( bnd_standard[input.arguments[1][0]].eventImplemented[INEV_DRAG] )
				ccout << "[drag] ";
			if( bnd_standard[input.arguments[1][0]].eventImplemented[INEV_RELEASE] )
				ccout << "[release] ";
			ccout << newl;
		}
		else // matched a key name
		{
			ccout << "\"" << input.arguments[1] << "\" = \"" << bnd_standard[in_KeyNames[index].key].handler << "\" on ";
			if( bnd_standard[in_KeyNames[index].key].eventImplemented[INEV_MOUSEMOVE] )
				ccout << "[mouse move] ";
			if( bnd_standard[in_KeyNames[index].key].eventImplemented[INEV_PRESS] )
				ccout << "[press] ";
			if( bnd_standard[in_KeyNames[index].key].eventImplemented[INEV_DRAG] )
				ccout << "[drag] ";
			if( bnd_standard[in_KeyNames[index].key].eventImplemented[INEV_RELEASE] )
				ccout << "[release] ";
			ccout << newl;
		}
		return;
	}
	if(input.numarguments < 2)
	{
		ccout << "usage: bind [key] [action] | [on_mouse_move] [on_press] [on_drag] [on_release]" << newl;
		return;
	}
	if(input.numarguments > 2) // more than 1 argument
	{
	//	bool quoted(false);
		input.arguments[1] = M_Uppercase(input.arguments[1]);

		short index = 0;	
		char* current = in_KeyNames[index].name;
		while(current) // try to find a key name that matches
		{
			if(input.arguments[1] == current)
				break;
			else
				index++;
				current = in_KeyNames[index].name;
		}

		if(current == 0) // couldn't match a key name, so it must just be a character
		{
			if(input.arguments[1].length() > 1)
				return;
			bnd_standard[input.arguments[1][0]].handler = input.arguments[2];
		
			if(input.numarguments > 3)
			{	
				for(int j = INEV_MOUSEMOVE; j < INEV_RELEASE+1; j++)
				{
					bnd_standard[input.arguments[1][0]].eventImplemented[j] = CON_StringEvaluatesTrue(input.arguments[2+j]);
				}
			}
		}
		else // matched a key name
		{
			bnd_standard[in_KeyNames[index].key].handler = input.arguments[2];

			if(input.numarguments > 3)
			{	
				for(int j = INEV_MOUSEMOVE; j < INEV_RELEASE+1; j++)
				{
					bnd_standard[in_KeyNames[index].key].eventImplemented[j] = CON_StringEvaluatesTrue(input.arguments[2+j]);
				}
			}
		}
	}
}
con_Command con_cmd_bind("bind", con_cmd_bind_handler);

void BND_CON_InitAux()
{
	CON_RegisterCommand(&con_cmd_unbind);
	CON_RegisterCommand(&con_cmd_unbindall);
	CON_RegisterCommand(&con_cmd_bind);
}

void BND_PreInit()
{
	CON_AddInitAuxFunction(BND_CON_InitAux);
}

void BND_Init()
{
	if(bnd_initialized) return;
	for(int i = 0; i < 256; i++)
	{
		bnd_standard[i].key = IN_GetCharacter(i);

		bnd_standard[i].eventImplemented[INEV_NONE] = false;
		bnd_standard[i].eventImplemented[INEV_MOUSEMOVE] = false;
		bnd_standard[i].eventImplemented[INEV_PRESS] = true;
		bnd_standard[i].eventImplemented[INEV_DRAG] = true;
		bnd_standard[i].eventImplemented[INEV_RELEASE] = false;
	}
	bnd_initialized = true;
	BND_RegisterBind(&bnd_console);
}

void BND_Shutdown()
{
	if(!bnd_initialized) return;
	bnd_initialized = false;

	bindList = 0;
}

void BND_RegisterBind(bnd_Bind* newBind) // adds and automatically alphabetizes new commands
{
	// case 1: list is empty
	if(!bindList) // list is empty
	{
		bindList = newBind;
		bindList->nextBind = 0;
		return;
	}
	else // add command to beginning of list
	{
		if(newBind->key < bindList->key) // case 2: is first in list
		{
			newBind->nextBind = bindList;
			bindList = newBind;
			return;
		}
		// case 3: goes somewhere else in list
		// make sure no stray pointers are left by keeping track of previous command
		else
		{
			bnd_Bind* current = bindList;
			while (current->nextBind)
			{
				if(newBind->key < current->nextBind->key) // newCommand goes before nextCommand
				{
					bnd_Bind* temp = current->nextBind;
					current->nextBind = newBind;
					newBind->nextBind = temp;
					return;
				}
				current = current->nextBind;
			}
			current->nextBind = newBind; // put at end of list
			current->nextBind->nextBind = 0;
			return;
		}
	}
	return;
}

unsigned char BND_InputReceiver(in_Event * event)
{
	if(!bnd_initialized) return 0;
	if( (event->key >= 'a') && (event->key <= 'z') ) // convert lower to upper case
		event->key-=32;
	if(bindList) // only do this if any binds exist
	{
		for(bnd_Bind* tempBind = bindList;
			tempBind;
			tempBind = tempBind->nextBind)
			// check to see if its a bind
		{
			if((event->key == tempBind->key) && (tempBind->eventImplemented[event->eventType])) // if requested command matches current
			{
				if(tempBind->handler != "")
				{
					CON_Exec(tempBind->handler);
					return 1;
				}
			}
		}		
	}
	for(int i = 0; i < 256; i++)
	{
		if((event->key == bnd_standard[i].key) && (bnd_standard[i].eventImplemented[event->eventType]))
		{
			if(bnd_standard[i].handler != "")
			{
				CON_Exec(bnd_standard[i].handler);
				return 1;
			}
		}
	}
	return 0;
}
