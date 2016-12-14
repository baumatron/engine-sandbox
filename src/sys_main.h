#ifndef SYS_MAIN_H
#define SYS_MAIN_H

//#define DEBUG_RAY
//#include "in_event.h"

#include <string>
#include <vector>
using namespace std;

class sceneObject;

extern bool quit;
extern bool paused;

extern float sys_curTime; // time at start current frame
extern float sys_frameTime; // time in seconds between current and last frame

extern short opcodesPerSecond;

void SYS_Init();
void SYS_Frame();
void SYS_Shutdown();

void SYS_Message(string text);

//unsigned char SYS_InputReceiver(in_Event * event);

/*class EngineSettings
{
	
}*/


#endif