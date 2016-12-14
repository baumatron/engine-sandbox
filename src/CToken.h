#ifndef CTOKEN_H
#define CTOKEN_H

#include "CEvent.h"
#include <string>
#include <vector>
using namespace std;

class CToken 
{
public:
	CToken():id(0), section_id(0){}

	enum TokenTypes {section, function, data};

	unsigned int id;
	unsigned int section_id;
	TokenTypes type;
	string label;
};


class CTokenManager
{
public:
	// token id of 0 is not a valid token
	enum SubsystemTokens {local = 1, video, sound, input, console, network, resource, SoundTokensStart};
	enum SoundTokens {sound_preinitialize = SoundTokensStart, sound_initialize, sound_shutdown, 
		sound_playsound, sound_stopsound, sound_loadsound, sound_unloadsound};

	CTokenManager();

	unsigned int AddToken(string label, CToken::TokenTypes tokenType = CToken::section, unsigned int section_id = 0, unsigned int uid = 0); // either pass a specific uid or allow one to be created
	CToken GetTokenByID(unsigned int id);
	vector<CToken> GetTokenBySection(unsigned int section_id);
	vector<CToken> GetTokenByLabel(string label, unsigned int section_id = 0, bool beginsWith = false);
	CToken GetSingleTokenByLabel(string label, unsigned int section_id = 0);

	CEvent BuildEventFromString(string source);

private:
	vector<CToken> tokens;
	unsigned int IDCount;
};

extern CTokenManager TokenManager;

#endif