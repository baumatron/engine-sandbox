
#include "CToken.h"
#include "CTokenManager.h"
#include "CRouterEvent.h"
#include "con_main.h"
#include "m_misc.h"
#include "CGenericType.h"
#include <stack>
using namespace std;

CTokenManager TokenManager;

CGenericType ToGenericType(CToken token)
{
	if(token.type == CToken::data)
	{
		CGenericType result(token.dataType);
		switch(token.dataType)
		{
		case t_bool:
			{
				result.SetValue( *( (bool*) token.p_data ) );
			}
			break;
		case t_long:
			{
				result.SetValue( *( (long*) token.p_data ) );
			}
			break;
		case t_double:
			{
				result.SetValue( *( (float*) token.p_data ) );
			}
			break;
		case t_string:
			{
				result.SetValue( *( (string*) token.p_data ) );
			}
			break;
		}	
		return result;
	}
	else
	{
		return CGenericType(t_long);
	}
}


CTokenManager::CTokenManager():
	  IDCount(0)
{
/*	AddToken("local", CToken::section, 0,		local);
	AddToken("video", CToken::section, local,	video);
	AddToken("sound", CToken::section, local,	sound);
	AddToken("gui",	CToken::section, local,		gui);
	AddToken("hsi", CToken::section, local, hsi);
	AddToken("input", CToken::section, local,	input);
	AddToken("console", CToken::section, local, console);
	AddToken("network", CToken::section, local, network);
	AddToken("resource", CToken::section, local, resource);

	// sound
	AddToken("Preinitialize", CToken::function, GetSingleTokenByLabel("sound", local).id, sound_preinitialize);
	AddToken("Initialize", CToken::function, GetSingleTokenByLabel("sound", local).id, sound_initialize);
	AddToken("Shutdown", CToken::function, GetSingleTokenByLabel("sound", local).id, sound_shutdown);
	AddToken("LoadSound", CToken::function, GetSingleTokenByLabel("sound", local).id, sound_loadsound);
	AddToken("UnloadSound", CToken::function, GetSingleTokenByLabel("sound", local).id, sound_unloadsound);
	AddToken("StopSound", CToken::function, GetSingleTokenByLabel("sound", local).id, sound_stopsound);
	AddToken("PlaySound", CToken::function, GetSingleTokenByLabel("sound", local).id, sound_playsound);

	// gui
	AddToken("MakeWindowInstance", CToken::function, GetSingleTokenByLabel("gui", local).id, gui_makewindowinstance);
	AddToken("DeleteWindowInstance", CToken::function, GetSingleTokenByLabel("gui", local).id, gui_deletewindowinstance);
	AddToken("ShowWindow", CToken::function, GetSingleTokenByLabel("gui", local).id, gui_showwindow);
	AddToken("HideWindow", CToken::function, GetSingleTokenByLabel("gui", local).id, gui_hidewindow);

	// hsi
	AddToken("OpenColonyPropertiesWindow", CToken::function, GetSingleTokenByLabel("hsi", local).id, hsi_opencolonypropertieswindow);*/
}



unsigned int CTokenManager::AddToken(string label, CToken::TokenTypes tokenType, unsigned int section_id, unsigned int uid, M_DataTypes dataType, void* data) // either pass a specific uid or allow one to be created
{
	CToken newToken;
	if(uid == 0)
	{
		newToken.id = ++IDCount;
		if(IDCount == 0)
		{
			ccout << "Error in AddToken: ran out of id's!\n";
		}
	}
	else
	{
		if(uid <= IDCount)
			ccout << "Error in AddToken: specified a dangerous uid (smaller than or equal to IDCount)!\n";
		newToken.id = uid;
		if(uid > IDCount)
			IDCount = uid;
	}
	newToken.section_id = section_id; // if this is 0, belongs to no section
	newToken.label = label;
	newToken.type = tokenType;
	if(tokenType == CToken::data)
	{
		newToken.dataType = dataType;
		newToken.p_data = data;
	}
	tokens.push_back(newToken);
	return newToken.id;
}

CToken CTokenManager::GetTokenByID(unsigned int id)
{
	for(vector<CToken>::iterator it = tokens.begin(); it != tokens.end(); it++)
	{
		if(it->id == id)
			return *it;
	}
	CToken dummy;
	dummy.id = 0;
	return dummy;
}

vector<CToken> CTokenManager::GetTokenBySection(unsigned int section_id)
{
	vector<CToken> results;

	for(vector<CToken>::iterator it = tokens.begin(); it != tokens.end(); it++)
	{
		if(it->section_id == section_id)
			results.push_back(*it);
	}

	return results;
}

vector<CToken> CTokenManager::GetTokenByLabel(string label, unsigned int section_id, bool beginsWith)
{
	vector<CToken> results;
	vector<CToken> sectionSet;
	vector<CToken>* searchSet;

	if(section_id)
	{
		sectionSet = GetTokenBySection(section_id);
		searchSet = &sectionSet;
	}
	else
	{
		searchSet = &tokens;
	}

	for( vector<CToken>::iterator it = searchSet->begin(); it != searchSet->end(); it++ )
	{
		if( beginsWith )
		{
			if( it->label.substr( 0, label.length() ) == label )
				results.push_back( *it );
		}
		else
		{
			if( it->label == label )
				results.push_back( *it );
		}
	}

	return results;
}
CToken CTokenManager::GetSingleTokenByLabel(string label, unsigned int section_id)
{
	vector<CToken> sectionSet;
	vector<CToken>* searchSet;

	if(section_id)
	{
		sectionSet = GetTokenBySection(section_id);
		searchSet = &sectionSet;
	}
	else
	{
		searchSet = &tokens;
	}

	for( vector<CToken>::iterator it = searchSet->begin(); it != searchSet->end(); it++ )
	{		
		if( it->label == label )
			return ( *it );
	}
	return CToken();
}


CRouterEvent CTokenManager::BuildEventFromString(string source)
{
	CRouterEvent result;
	result.m_data[0] = 0;

	string currentTokenString;
	unsigned int currentSection(TokenManager.GetSingleTokenByLabel("local").id);
	int i = 0; // int number
	int parseLevel = 0; // 0 = defined tokens, 1 = arguments
	for(unsigned int character = 0; character < source.size(); character++)
	{
		if(source[character] == ' ')
		{
		}
		else if( (source[character] == '.') || (source[character] == '(') || (source[character] == ',') || (source[character] == ')') || (source[character] == ';')) // . indicates section, ( indicates function parameter, ',' indicates new parameter item
		{
			// convert token to id
			if(parseLevel == 0)
			{
				if(currentTokenString.size()) // a token has finished being read in... do something with it!
				{
					vector<CToken> tokens = TokenManager.GetTokenByLabel(currentTokenString, currentSection);
					if(!tokens.size() && (i == 0)) // try to see if the scope can be found!
					{
						tokens = TokenManager.GetTokenByLabel(currentTokenString);
						if(tokens.size() == 1)
						{
							CToken current = tokens.front();
							stack<unsigned int> scope;
							while(current.section_id)
							{
								scope.push(current.section_id);
								current = GetTokenByID(current.section_id);
							}
							while(!scope.empty())
							{
								result.m_data[i++] = scope.top();
								scope.pop();
							}
						}
					}
					if(tokens.size() == 1) // there was only one token in the pool that matches
					{
						result.m_data[i] = tokens[0].id; // throw it in the event
							
						switch(tokens[0].type)
						{
						case CToken::section:
							{
								parseLevel = 0;
								currentSection = tokens[0].id;
							}
							break;
						case CToken::function:
							{
								parseLevel = 0;
							}
							break;
						case CToken::data:
							{
								parseLevel = 0; 
							}
							break;
						}
					}
					else if(tokens.size() > 1)
					{
						ccout << "Error building event because of ambiguity\n";
						result.m_data[0] = 0;
						return result;
					}
					else
					{
						ccout << "Error building event because of syntax error\n";
						result.m_data[0] = 0;
						return result;
					}
				}
				else
				{
					ccout << "Error building event because of syntax error\n";
					result.m_data[0] = 0;
					return result;
				}
			}
			else if(parseLevel == 1) // inside an argument list
			{
				bool hasBeenPushed(false);
				M_DataTypes dataType = M_DetermineType(currentTokenString);
				if(dataType == t_string)
				{
					if(currentTokenString[0] == '"' && currentTokenString[currentTokenString.length()-1] == '"')
					{
						// is immediate data
						currentTokenString = currentTokenString.substr(1,currentTokenString.length()-2); // get rid of quotes
					}
					else
					{
						// there were no quotes, it should be a variable name... try to evaluate it
						CToken currentToken = TokenManager.GetSingleTokenByLabel(currentTokenString);
						if(currentToken.type == CToken::data)
						{
							// all is well, translate the data using ::ToGenericType()
							result.m_arguments.m_argList.push_back( ToGenericType(currentToken) );
						}
						else
						{
							// not data, but could be any other type still... just push it on the list
							result.m_arguments.m_argList.push_back(CGenericType::FromString(currentTokenString));
						}
						hasBeenPushed = true;
					}				
				}

				if(!hasBeenPushed)
				{
					result.m_arguments.m_argList.push_back(CGenericType::FromString(currentTokenString));
				}
			}

			//////

			if( (source[character] == '(') )
				parseLevel = 1;
			else if( source[character] == ')' )
			{
				return result;
			}

			i++;
			if(i >= 32)
			{
				ccout << "Error building event because event is too long\n";
				result.m_data[0] = 0;
				return result;
				//break;
			}
			result.m_data[i] = 0;
			currentTokenString = "";
		}
		else
		{
			currentTokenString += source[character];
			
			if(source[character] == '"')
			{
				bool success(false);
				for(character++; character < source.size(); character++)
				{
					currentTokenString += source[character];
					if(source[character] == '"')
					{
						success = true;
						break;
					}
				}
				if(!success)
				{
					ccout << "Error building event because of missing quote.\n";
					result.m_data[0] = 0;
					return result;
				}
			}
			else if( parseLevel == 1 && source[character] != ',' && source[character] != ')' )
			{
				bool success(false);
				for(character++; character < source.size(); character++)
				{
					currentTokenString += source[character];
					if(source[character] == ',' || source[character] == ')')
					{
						character--;
						currentTokenString = currentTokenString.substr(0, currentTokenString.size()-1);
						success = true;
						break;
					}
				}
				if(!success)
				{
					ccout << "Error building event because of syntax.\n";
					result.m_data[0] = 0;
					return result;
				}
			}
		}

		if(character+1 == source.size())
		{
			ccout << "Error building event because of syntax error\n";
			result.m_data[0] = 0;
			return result;
		}
	}

	return result;
}

