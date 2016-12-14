
#ifndef CEVENTROUTER_H
#define CEVENTROUTER_H

#include "CRouterEvent.h"
#include "ISubsystem.h"
#include <string>
//#include <vector>
using namespace std;

class CRouterReturnCode
{
public:
	CRouterReturnCode():
		error(false),
		couldntHandle(false){}
	CRouterReturnCode(bool _error, bool _couldntHandle):
		error(_error),
		couldntHandle(_couldntHandle)
		{}
	CRouterReturnCode(bool _error, bool _couldntHandle, string _cause):
		error(_error),
		couldntHandle(_couldntHandle),
		cause(_cause)
		{}

	bool error;
	bool couldntHandle;
	string cause;
};



/*
on a console function evaluation take the data arguments and put them in a list, each item with a unique routing id
when a function is routed to it's destination, it says to the router: "hey, here's an id and i know it's supposed to be
for data. give me the data for it and tell me what type it is. if it isn't data (and is a token), then you fucked up."
use the CTypeAbstractionLayer to recieve this data in the routing destination (sound.play()). This will make it easy to 
determine the type and recieve the pointer to it (or would the actual data be better?)
*/



class CEventRouter
{
public:
	CEventRouter();

	bool RouteEvent(CRouterEvent event);
};

extern CEventRouter EventRouter;

#endif
