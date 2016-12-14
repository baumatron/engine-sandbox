


#ifndef ISUBSYSTEM_H
#define ISUBSYSTEM_H

#include "CRouterEvent.h"
#include "CEventRouter.h"
#include "CInputEvent.h"

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

	virtual CRouterReturnCode EventReceiver(CRouterEvent& event) = 0;
	virtual bool InputReceiver(const CInputEvent& event) = 0;
};

class IThinkSubsystem: public ISubsystem
{
public:
	virtual void Think() = 0;
};


#endif
