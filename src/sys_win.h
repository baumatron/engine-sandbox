#ifndef SYS_WIN_H
#define SYS_WIN_H

#ifdef WIN32
#include <windows.h>
#endif

#include <string>
using namespace std;

void SYSW_Message(string message);

float SYSW_TimeFloat(); // get time passed in program in seconds


extern bool quit;
#endif