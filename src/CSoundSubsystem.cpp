#include "audiere.h"

#include "CSoundSubsystem.h"
#include "con_main.h"
#include "CEventRouter.h"
#include "CToken.h"
#include <vector>
using namespace std;
using namespace audiere;

CSoundSubsystem Sound;

struct sound_t
{
	string file;
	SoundEffectPtr soundEffect;
	OutputStreamPtr stream;
	int id;
};


class S_SystemData
{
public:
	S_SystemData():device(0){}
	~S_SystemData(){}

	AudioDevicePtr device;
	vector<sound_t> sounds;
};

/*
class CSoundSubsystem: public ISubsystem
{
public:
	CSoundSubsystem();
	virtual ~CSoundSubsystem();

	virtual bool PreInitialize(); // PreInitialize is called before Initialize, only links to other subsystems should be made here
	virtual bool Initialize();
	virtual bool Shutdown();

	virtual bool EventReceiver(CEvent& event);

////////////////////////
	short LoadSound(string filename);
	void UnloadSound(short id);
	void PlaySound(short id, float volume = 1.0f);
	void PlaySound(string filename, float volume = 1.0f);
};*/

CSoundSubsystem::CSoundSubsystem():
	idCount(0),
	initialized(false)
{
	p_systemData = new S_SystemData();
}

CSoundSubsystem::~CSoundSubsystem()
{
	if(initialized)
		Shutdown();
	if(p_systemData)
		delete p_systemData;
}

bool CSoundSubsystem::PreInitialize()
{
	return true;
}

bool CSoundSubsystem::Initialize()
{
	if(initialized)
		return false;

	idCount = 0;
	p_systemData->device = OpenDevice();
	if(!p_systemData->device)
	{
		ccout << "Error setting up sound device!\n";
		return false;
	}

	initialized = true;
	return true;
}

bool CSoundSubsystem::Shutdown()
{
	if(!initialized)
		return false;

	while(!p_systemData->sounds.empty())
	{
		p_systemData->sounds.back().soundEffect = 0;
		p_systemData->sounds.pop_back();
	}

	initialized = false;
	return true;
}

CRouterReturnCode CSoundSubsystem::EventReceiver(CEvent& event)
{
	unsigned int eventNumber = event.m_data[0];

	/*if(eventNumber == TokenManager.GetSingleTokenByLabel("play", TokenManager.GetSingleTokenByLabel("sound").id).id)
	{
		event.Shift(1);
		PlaySound(event.data[0]);
		return CRouterReturnCode(false,false);
	}*/

//preinitialize = SoundTokensStart, initialize, shutdown, play, loadsound, unloadsound

/*	short LoadSound(string filename, bool stream);
	void UnloadSound(short id);
	void UnloadSound(string filename);
	void StopSound(short id);
	void StopSound(string filename);
	void PlaySound(short id, float volume = 1.0f);
	void PlaySound(string filename, float volume = 1.0f, bool stream = false);
*/

	switch((CTokenManager::SoundTokens)event.GetLong())
	{
	case CTokenManager::sound_preinitialize:
		{
			PreInitialize();
		}
		break;
	case CTokenManager::sound_initialize:
		{
			Initialize();
		}
		break;
	case CTokenManager::sound_shutdown:
		{
			Shutdown();
		}
		break;
	case CTokenManager::sound_playsound:
		{
/*	void PlaySound(short id, float volume = 1.0f);
	void PlaySound(string filename, float volume = 1.0f, bool stream = false);*/
			if(event.m_arguments.MatchesFormat(t_string))
			{
				string filename;
				event.m_arguments.m_argList[0].GetValue(filename);
				//event.m_arguments.m_list.front().GetValue(filename);
				//event.m_arguments.m_list.pop();
				if(event.m_arguments.MatchesFormat(t_string, t_double))
				{
					double volume(1.0);
					event.m_arguments.m_argList[1].GetValue(volume);
					//event.m_arguments.m_list.front().GetValue(volume);
					//event.m_arguments.m_list.pop();
					if(event.m_arguments.MatchesFormat(t_string, t_double, t_bool))
					{
						bool stream;
						event.m_arguments.m_argList[2].GetValue(stream);
						//event.m_arguments.m_list.front().GetValue(stream);
						//event.m_arguments.m_list.pop();
						PlaySound(filename, (float)volume, stream);
					}
					else
					{
						PlaySound(filename, (float)volume);
					}
				}
				else
				{
					PlaySound(filename);
				}
			}
			else if(event.m_arguments.MatchesFormat(t_long))
			{
				long soundid;
				event.m_arguments.m_argList[0].GetValue(soundid);
				//event.m_arguments.m_list.front().GetValue(soundid);
				//event.m_arguments.m_list.pop();
				if(event.m_arguments.MatchesFormat(t_long, t_double))
				{
					double volume(1.0);
					event.m_arguments.m_argList[1].GetValue(volume);
					//event.m_arguments.m_list.front().GetValue(volume);
					//event.m_arguments.m_list.pop();
					PlaySound(soundid, volume);
				}
				else
				{
					PlaySound(soundid);
				}
			}
			else
			{
				return CRouterReturnCode(true, false, "argument error");
			}

			return CRouterReturnCode(false, false);	
		}
		break;
	case CTokenManager::sound_loadsound: 
		{
			string filename;
			bool stream(false);

			if(event.m_arguments.MatchesFormat(t_string))
			{	
				event.m_arguments.m_argList[0].GetValue(filename);
				//event.m_arguments.m_list.front().GetValue(filename);
				//event.m_arguments.m_list.pop();
				if(event.m_arguments.MatchesFormat(t_string, t_bool))
				{
					event.m_arguments.m_argList[1].GetValue(stream);
					//event.m_arguments.m_list.front().GetValue(stream);
				}
			}
			else
			{
				return CRouterReturnCode(true, false, "argument error");
			}

			LoadSound(filename, stream);

			return CRouterReturnCode();
		}
		break;
	case CTokenManager::sound_unloadsound: 
		{
/*	void UnloadSound(short id);
	void UnloadSound(string filename);*/
			if(event.m_arguments.MatchesFormat(t_long))
			{
				long id;
				event.m_arguments.m_argList[0].GetValue(id);
				//event.m_arguments.m_list.front().GetValue(id);
				UnloadSound((short)id);
			}
			else if(event.m_arguments.MatchesFormat(t_string))
			{
				string filename;
				event.m_arguments.m_argList[0].GetValue(filename);
				//event.m_arguments.m_list.front().GetValue(filename);
				UnloadSound(filename);
			}
			else
			{
				return CRouterReturnCode(true, false, "argument error");
			}
			
			return CRouterReturnCode();
		}
		break;
	case CTokenManager::sound_stopsound:
		{
			/*
			void StopSound(short id);
			void StopSound(string filename);*/
			if(event.m_arguments.MatchesFormat(t_long))
			{
				long id;
				event.m_arguments.m_argList[0].GetValue(id);
				//event.m_arguments.m_list.front().GetValue(id);
				StopSound((short)id);
			}
			else if(event.m_arguments.MatchesFormat(t_string))
			{
				string filename;
				event.m_arguments.m_argList[0].GetValue(filename);
				//event.m_arguments.m_list.front().GetValue(filename);
				StopSound(filename);
			}
			else
			{
				return CRouterReturnCode(true, false, "argument error");
			}

			return CRouterReturnCode();
		}
		break;
	}

	return CRouterReturnCode(false,true);
}

short CSoundSubsystem::LoadSound(string filename, bool stream)
{
	if(!initialized)
		return 0;

	sound_t sound;
	if(!stream)
	{
		sound.soundEffect = OpenSoundEffect(p_systemData->device, filename.c_str(), MULTIPLE);
		sound.stream = NULL;
		if(sound.soundEffect == NULL)
			return 0;
	}
	else
	{
		sound.stream = OpenSound(p_systemData->device, filename.c_str(), true);
		sound.soundEffect = NULL;
		if(sound.stream == NULL)
			return 0;
	}
	sound.file = filename;
	sound.id = ++idCount;
	p_systemData->sounds.push_back(sound);
	return sound.id;
}



void CSoundSubsystem::UnloadSound(short id)
{
	if(!initialized)
		return;

	if(id <= 0)
		return;
	for(vector<sound_t>::iterator it = p_systemData->sounds.begin(); it != p_systemData->sounds.end(); it++)
	{
		if(it->id == id)
		{
			it->soundEffect = 0;
			p_systemData->sounds.erase(it);
			break;
		}
	}
}

void CSoundSubsystem::UnloadSound(string filename)
{
	if(!initialized)
		return;

	for(vector<sound_t>::iterator it = p_systemData->sounds.begin(); it != p_systemData->sounds.end(); it++)
	{
		if(it->file == filename)
		{
			it->soundEffect = 0;
			p_systemData->sounds.erase(it);
			break;
		}
	}	
}

void CSoundSubsystem::StopSound(short id)
{
	if(!initialized)
		return;

	for(vector<sound_t>::iterator it = p_systemData->sounds.begin(); it != p_systemData->sounds.end(); it++)
	{
		if(it->id == id)
		{
			it->soundEffect->stop();
			break;
		}
	}
}

void CSoundSubsystem::StopSound(string filename)
{
	if(!initialized)
		return;

	for(vector<sound_t>::iterator it = p_systemData->sounds.begin(); it != p_systemData->sounds.end(); it++)
	{
		if(it->file == filename)
		{
			it->soundEffect->stop();
			break;
		}
	}
}

void CSoundSubsystem::PlaySound(short id, float volume)
{	
	if(!initialized)
		return;
	if(id <= 0)
		return;

	for(vector<sound_t>::iterator it = p_systemData->sounds.begin(); it != p_systemData->sounds.end(); it++)
	{
		if(it->id == id)
		{
			if(it->soundEffect)
			{
				it->soundEffect->setVolume(volume);
				it->soundEffect->play();
			}
			if(it->stream)
			{
				it->stream->setVolume(volume);
				it->stream->play();
			}
		}
	}
}

void CSoundSubsystem::PlaySound(string filename, float volume, bool stream)
{
	if(!initialized)
		return;

	int id(0);

		// look to see if the file has already been loaded
	for(vector<sound_t>::iterator it = p_systemData->sounds.begin(); it != p_systemData->sounds.end(); it++)
	{
		if(it->file == filename)
			id = it->id;
	}

		// file hasn't been loaded, load it
	if(id == 0)
	{
		id = LoadSound(filename, stream);
	}

		// play the sound
	PlaySound(id, volume);
}
