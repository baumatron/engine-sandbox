#ifndef CTOKENMANAGER_H
#define CTOKENMANAGER_H

#include "CToken.h"

class CTokenManager
{
public:
	// token id of 0 is not a valid token
	enum SubsystemTokens {local = 1, video, sound, gui, hsi, input, console, network, resource, SoundTokensStart};
	enum SoundTokens {sound_preinitialize = SoundTokensStart, sound_initialize, sound_shutdown, 
		sound_playsound, sound_stopsound, sound_loadsound, sound_unloadsound, GuiTokensStart};
	enum GuiTokens { gui_makewindowinstance = GuiTokensStart, gui_deletewindowinstance, gui_showwindow, gui_hidewindow, HsiTokensStart };
	enum HsiTokens { hsi_opencolonypropertieswindow = HsiTokensStart };

	CTokenManager();

	unsigned int AddToken(string label, CToken::TokenTypes tokenType = CToken::section, unsigned int section_id = 0, unsigned int uid = 0, 	M_DataTypes dataType = t_long, void* data = 0); // either pass a specific uid or allow one to be created
	CToken GetTokenByID(unsigned int id);
	vector<CToken> GetTokenBySection(unsigned int section_id);
	vector<CToken> GetTokenByLabel(string label, unsigned int section_id = 0, bool beginsWith = false);
	CToken GetSingleTokenByLabel(string label, unsigned int section_id = 0);

	CRouterEvent BuildEventFromString(string source);

private:
	vector<CToken> tokens;
	unsigned int IDCount;
};

extern CTokenManager TokenManager;

#endif