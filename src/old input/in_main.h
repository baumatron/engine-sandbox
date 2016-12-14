#ifndef __IN_MAIN_H__
#define __IN_MAIN_H__
//****************************************************************************
//**
//**    IN_MAIN.H
//**    Header - Input - Main Interface
//**
//**    Copyright (c) 1998 Chris Hargrove
//**    Notice: Usage of any code in this file is subject to the rules
//**            described in the LICENSE.TXT file included in this directory.
//**            Reading, compiling, or otherwise using this code constitutes
//**            automatic acceptance of the rules in said text file.
//**
//****************************************************************************
//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
#include "in_event.h"


//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
extern bool in_initialized;
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
// initialization and shutdown
void IN_Init();
void IN_Shutdown();

// per-frame operation
void IN_Update();

void IN_ResetSublayer();
void IN_FreeDevices();
void IN_AcquireDevices();

// receiver control
int IN_AddReceiver(int (*handler)(in_Event *));
void IN_ChangeReceiverHandler(int receiver, int (*handler)(in_Event *));
void IN_RemoveReceiver(int receiver);

// key names and shifted equivalents
char* IN_NameForKey(int key);
char* IN_NameForLCKey(int key);
int IN_KeyForName(char* name);
int IN_GetShiftedKey(int key);
int IN_GetUnshiftedKey(int key);

char IN_GetCharacter(int key);
char IN_GetShiftedCharacter(int key);


//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
// key name list
struct keyName_s
{
	char *name;
	int key;
	char *numname;
};
extern keyName_s in_KeyNames[];
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

//****************************************************************************
//**
//**    END HEADER IN_MAIN.H
//**
//****************************************************************************
#endif // __IN_MAIN_H__
