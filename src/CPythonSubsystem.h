#ifndef CPYTHONSUBSYSTEM_H
#define CPYTHONSUBSYSTEM_H

#include "ISubsystem.h"

class CPythonSubsystem: public ISubsystem
{
public:
	bool PreInitialize(){return true;} // PreInitialize is called before Initialize, only links to other subsystems should be made here
	bool Initialize();
	bool Shutdown();

	void ExecuteString(string pythonCode);
	void ExecuteFile(string pythonFile);

	CRouterReturnCode EventReceiver(CRouterEvent& event){return CRouterReturnCode(false, true);}
	bool InputReceiver(const CInputEvent& event){return false;}

private:
};

extern CPythonSubsystem Python;

#endif