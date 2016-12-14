#include "CGuiSubsystem.h"

#include "CInputSubsystem.h"
#include "CTokenManager.h"
//#include "CGuiFileParser.h"

CGuiSubsystem Gui;

CGuiSubsystem::CGuiSubsystem()
{
}
CGuiSubsystem::~CGuiSubsystem()
{
}

bool CGuiSubsystem::PreInitialize()// PreInitialize is called before Initialize, only links to other subsystems should be made here
{
	return true;
}
bool CGuiSubsystem::Initialize()
{
	Input.AddInputReceiver(this);
	return true;
}
bool CGuiSubsystem::Shutdown()
{
	return true;
}

void CGuiSubsystem::Think()
{
	WM.Think();
	Context.RenderWindows();
}

CRouterReturnCode CGuiSubsystem::EventReceiver(CRouterEvent& event)
{
	unsigned int eventNumber = event.m_data[0];

	switch((CTokenManager::GuiTokens)event.GetLong())
	{
	case CTokenManager::gui_makewindowinstance:
		{
			// get the file name from the argument list
			if(!event.m_arguments.MatchesFormat(t_string))
				return CRouterReturnCode(true, false);
			string specFile;
			event.m_arguments.m_argList[0].GetValue(specFile);

			GuiWindowSpecID specID;
			specID = WM.ReadWindowSpec(specFile);
			WM.MakeWindowInstance( specID );
			return CRouterReturnCode(false, false);	
		}
		break;
	case CTokenManager::gui_deletewindowinstance:
		{
			long windowID;
			event.m_arguments.m_argList[0].GetValue(windowID);
			WM.DeleteWindowInstance( (GuiWindowID) windowID );
			//TokenManager.GetSingleTokenByLabel
			return CRouterReturnCode(false, false);			
		}
		break;
	/*case CTokenManager::sound_shutdown:
		{
			Shutdown();
		}
		break;
	case CTokenManager::sound_playsound:
		{
			if(event.m_arguments.MatchesFormat(t_string))
			{
				string filename;
				event.m_arguments.m_argList[0].GetValue(filename);
				//event.m_arguments.m_list.front().GetValue(filename);
				//event.m_arguments.m_list.pop();
				if(event.m_arguments.MatchesFormat(t_string, t_double))
				{
					float volume(1.0);
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
					float volume(1.0);
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
*/
		//	return CRouterReturnCode(false, false);	
		//}
//		break;
	}
}

bool CGuiSubsystem::InputReceiver(const CInputEvent& event)
{
	return Context.InputReceiver(event); 
}
