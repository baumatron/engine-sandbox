#ifndef CINPUTSUBSYSTEM_H
#define CINPUTSUBSYSTEM_H

//#include "math_main.h"

#include "ISubsystem.h"

class CInputSubsystem: public IThinkSubsystem
{
public:
	CInputSubsystem();
	virtual ~CInputSubsystem();
	
	virtual bool PreInitialize(); // PreInitialize is called before Initialize, only links to other subsystems should be made here
	virtual bool Initialize();
	virtual bool Shutdown();
	
	virtual void Think();

	virtual CRouterReturnCode EventReceiver(CRouterEvent& event);
	virtual bool InputReceiver(const CInputEvent& event);
	
	void AddInputReceiver(bool (*) (const CInputEvent& event));
	void AddInputReceiver(ISubsystem* subsystem);
	
	unsigned short GetShiftedCharacterCode(unsigned short unshiftedCharacterCode);	
private:
	void DispatchInputEvent(const CInputEvent& event);

	// input reciever interface.... bool ReceiveInputEvent(const CInputEvent& event);
	vector<bool (*)(const CInputEvent& event)> legacyInputReceivers;
	vector<ISubsystem*> inputReceivers;
	
	InputKeyStates oldKeyStates[IKC_LAST]; // used to detect state changes
	InputKeyStates keyStates[IKC_LAST];
	InputKeyStates oldMouseButtonStates[IMC_LAST]; // used to detect state changes of mouse input
	InputKeyStates mouseButtonStates[IMC_LAST];

	unsigned short shiftedCharacters[IKC_LAST];

	bool initialized;
};



extern CInputSubsystem Input;


#endif
