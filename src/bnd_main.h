#ifndef BND_MAIN_H
#define BND_MAIN_H
// binding relay subsystem

#include "in_event.h"
#include "in_main.h"
#include "con_main.h"

#include <string>
using namespace std;

class bnd_Bind;

extern bool bnd_initialized; // whether the binding relay subsystem is ready to go or not
extern bnd_Bind bnd_standard[256]; // an array whose elements indeces correspond to a key's char value

void BND_PreInit();
void BND_Init();
void BND_Shutdown();

void BND_RegisterBind(bnd_Bind* newBind); // adds and automatically alphabetizes new binds

unsigned char BND_InputReceiver(in_Event * event); // input receiver


class bnd_Bind
{
public:
	bnd_Bind(unsigned char newKey, string newHandler, bool onMouseMove, bool onPress, bool onDrag, bool onRelease) 
		{handler = newHandler; key = newKey; nextBind = 0;
			eventImplemented[INEV_NONE] = false;
			eventImplemented[INEV_MOUSEMOVE] = onMouseMove;
			eventImplemented[INEV_PRESS] = onPress;
			eventImplemented[INEV_DRAG] = onDrag;
			eventImplemented[INEV_RELEASE] = onRelease;
		}
	bnd_Bind(unsigned char newKey, string newHandler) 
		{handler = newHandler; key = newKey; nextBind = 0;
			eventImplemented[INEV_NONE] = false;
			eventImplemented[INEV_MOUSEMOVE] = false;
			eventImplemented[INEV_PRESS] = true;
			eventImplemented[INEV_DRAG] = true;
			eventImplemented[INEV_RELEASE] = false;
		}
	bnd_Bind() {handler = ""; nextBind = 0; key = 0;}
	~bnd_Bind(){}
	


	bool eventImplemented[INEV_NUMTYPES];
	unsigned char key;
	string handler;
	bnd_Bind* nextBind;
};

#endif