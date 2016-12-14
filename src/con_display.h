#ifndef CON_DISPLAY_H
#define CON_DISPLAY_H

extern bool cond_initialized;
extern bool showConsole;

void COND_Init();
void COND_Draw();
void COND_Shutdown();
bool COND_ConsoleIsVisible();

#endif