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
//#include "cotc_defs.h"
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
unsigned long IN_AddReceiver(unsigned char (*handler)(in_Event *));
void IN_ChangeReceiverHandler(unsigned long receiver, bool (*handler)(in_Event *));
void IN_RemoveReceiver(unsigned long receiver);

// key names and shifted equivalents
char* IN_NameForKey(unsigned long key);
char* IN_NameForLCKey(unsigned long key);
unsigned long IN_KeyForName(char* name);
unsigned long IN_GetShiftedKey(unsigned long key);
unsigned long IN_GetUnshiftedKey(unsigned long key);

char IN_GetCharacter(unsigned long key);
char IN_GetShiftedCharacter(unsigned long key);


//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
// key name list
struct keyName_s
{
	char *name;
	unsigned long key;
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
