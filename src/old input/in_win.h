// directx specific crap for the input system
// goes in this header file

#ifndef IN_WIN_H
#define IN_WIN_H

#include "in_event.h"

void INW_Init(); // executed to start up the in_win subsystem: returns true if successful, false if theres a problem
void INW_Update(); // executed once per frame
void INW_Shutdown(); // executed to shut down and clean up the subsystem: returns true if successful, false if theres a problem

void INW_GetFocus(); // to get control of keyboard, mouse, etc
void INW_KillFocus(); // to kill control of keyboard, mouse, etc

void INW_SetEventHandler(void (* handler) (in_Event*)); // sets event receiver/handler to the list

void INW_ForceKey(unsigned long key, bool state);
void INW_SetMouseExclusive(bool exclusive);
//void INW_SetMouseLimits(long x1, long y1, long x2, long y2);
void INW_SetMouseSensitivity(float factor);
float INW_GetMouseSensitivity();

#endif