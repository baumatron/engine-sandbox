#include "CInputSubsystem.h"

#ifdef WIN32
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif


#include <iostream>
using namespace std;
	
CInputSubsystem Input;

CInputSubsystem::CInputSubsystem():
initialized(false)
{
	for( int i = 0; i < IKC_LAST; i++ )
	{
		oldKeyStates[i] = IKS_UP;
		keyStates[i] = IKS_UP;
		shiftedCharacters[i] = i; // initialize these to unshifted values, followed below by the keys that actually shift
	}
	
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
}                      
                       
bool CInputSubsystem::PreInitialize() // PreInitialize is called before Initialize, only links to other subsystems should be made here
{                      
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
	                      
	// now, check for input events from sdl. 
	// use these events to update keyStates
	SDL_Event sdlevent;   
                       
	while(SDL_PollEvent(&sdlevent)) 
	{                     
		CInputEvent inputEvent;
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
					if( sdlevent.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
						shiftedCharacter = true;
				}                  
				break;             
			case SDL_KEYUP:     
				{                  
					keyStates[sdlevent.key.keysym.sym] = IKS_UP;
					if( sdlevent.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
						shiftedCharacter = true;
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
				inputEvent.keyCode = (InputKeyCodes)i;
				inputEvent.keyAction = IKA_PRESS;
				DispatchInputEvent(inputEvent);
				
				inputEvent.inputEventType = IET_CHARACTERTYPED;
				inputEvent.characterCode = characterCode;
				DispatchInputEvent(inputEvent);
			}
			else if( (oldKeyStates[i] == IKS_DOWN) && (keyStates[i] == IKS_UP) )
			{
				// key press action occurred
				inputEvent.inputEventType = IET_STATECHANGE;
				inputEvent.keyCode = (InputKeyCodes)i;
				inputEvent.keyAction = IKA_RELEASE;
				
				DispatchInputEvent(inputEvent);
			}
			else if( (oldKeyStates[i] == IKS_DOWN) && (keyStates[i] == IKS_DOWN) )
			{
				// key press action occurred
				inputEvent.inputEventType = IET_STATECHANGE;
				inputEvent.keyCode = (InputKeyCodes)i;
				inputEvent.keyAction = IKA_HOLD;
				
				DispatchInputEvent(inputEvent);
			}
			
			
		}
		
		                 
	}                     
	                      
	
	

}


CRouterReturnCode CInputSubsystem::EventReceiver(CRouterEvent& event)
{
	return CRouterReturnCode(false, true);
}

void CInputSubsystem::AddInputReceiver(bool (*receiver) (const CInputEvent& event))
{
	inputReceivers.push_back(receiver);
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
		if(inputReceivers[i](event))
			return;
	}
}

