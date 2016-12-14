#ifndef S_MAIN_H
#define S_MAIN_H

#include "ISubsystem.h"
#include "CEventRouter.h"
#include <string>
using namespace std;

class S_SystemData;

class CSoundSubsystem: public ISubsystem
{
public:
	CSoundSubsystem();
	virtual ~CSoundSubsystem();

	virtual bool PreInitialize(); // PreInitialize is called before Initialize, only links to other subsystems should be made here
	virtual bool Initialize();
	virtual bool Shutdown();

	virtual CRouterReturnCode EventReceiver(CEvent& event);

////////////////////////
	short LoadSound(string filename, bool stream);
	void UnloadSound(short id);
	void UnloadSound(string filename);
	void StopSound(short id);
	void StopSound(string filename);
	void PlaySound(short id, float volume = 1.0f);
	void PlaySound(string filename, float volume = 1.0f, bool stream = false);

private:
	S_SystemData* p_systemData; // for all the shit that requires funky headers and shit (audiere)
	
	int idCount;
	bool initialized;
};

extern CSoundSubsystem Sound;

#endif