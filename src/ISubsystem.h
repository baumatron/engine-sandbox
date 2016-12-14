

#include "CEvent.h"
#include "CEventRouter.h"

#ifndef ISUBSYSTEM_H
#define ISUBSYSTEM_H

class CRouterReturnCode;
// an interface for all subsystems

class ISubsystem
{
public:
	ISubsystem(){}
	virtual ~ISubsystem(){}

	virtual bool PreInitialize() = 0; // PreInitialize is called before Initialize, only links to other subsystems should be made here
	virtual bool Initialize() = 0;
	virtual bool Shutdown() = 0;

	virtual CRouterReturnCode EventReceiver(CEvent& event) = 0;
};

class IThinkSubsystem: public ISubsystem
{
public:
	virtual void Think() = 0;
};


#endif