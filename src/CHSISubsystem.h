#ifndef CHSISUBSYSTEM_H
#define CHSISUBSYSTEM_H

#include "ISubsystem.h"
#include "CEventRouter.h"
#include "CInputEvent.h"
//#include "../win32/colony.h"
#include "CHSIGalaxy.h"
#include "CCamera.h"
#include "CHSIMainDisplayContext.h"


class CHSISubsystem: public IThinkSubsystem
{
public:
	CHSISubsystem();
	virtual ~CHSISubsystem();
	
	virtual bool PreInitialize(); // PreInitialize is called before Initialize, only links to other subsystems should be made here
	virtual bool Initialize();
	virtual bool Shutdown();
	
	virtual void Think();

	virtual CRouterReturnCode EventReceiver(CRouterEvent& event);
	virtual bool InputReceiver(const CInputEvent& event);
	
	void OpenColonyPropertiesWindow(long colonyNumber);	// prototyping

//	vector<Colony> colonies;	// prototyping
	CHSIGalaxy galaxy;

	CHSIMainDisplayContext m_mainDisplayContext;
private:
	bool mouseActive;

	bool initialized;
};


extern CHSISubsystem Hsi;


#endif
