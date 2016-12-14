#ifndef CGUISUBSYSTEM_H
#define CGUISUBSYSTEM_H

#include "ISubsystem.h"
#include "CEventRouter.h"
#include "CInputSubsystem.h"

#include "CGuiWindowMaker.h"
#include "CGuiContext.h"
#include "CGuiFieldWidget.h"
#include "CGuiProgressBarWidget.h"
#include "CGuiSliderWidget.h"
#include "CGuiViewportWidget.h"

class CGuiSubsystem: public IThinkSubsystem
{
public:
	CGuiSubsystem();
	virtual ~CGuiSubsystem();

	virtual bool PreInitialize(); // PreInitialize is called before Initialize, only links to other subsystems should be made here
	virtual bool Initialize();
	virtual bool Shutdown();

	virtual void Think();

	virtual CRouterReturnCode EventReceiver(CRouterEvent& event);

	bool InputReceiver(const CInputEvent& event);

	CGuiWM WM;
	CGuiContext Context;
};

extern CGuiSubsystem Gui;

#endif