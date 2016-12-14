#ifndef CON_DISPLAY_H
#define CON_DISPLAY_H

extern bool cond_initialized;
extern short consoleY;
extern bool consoleDown;
extern bool consoleMoving;

void COND_PrintTextBuffer();

void COND_Init();
void COND_Draw();
void COND_Shutdown();

#endif