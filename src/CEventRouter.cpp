#include "CEventRouter.h"
#include "CTokenManager.h"
#include "CSoundSubsystem.h"
#include "CVideoSubsystem.h"
#include "CGuiSubsystem.h"
#include "CHSISubsystem.h"

CEventRouter EventRouter;

CEventRouter::CEventRouter()
{
}


bool CEventRouter::RouteEvent(CRouterEvent event)
{
	unsigned int eventNumber = event.m_data[0];

	if(eventNumber == TokenManager.GetSingleTokenByLabel("local").id)
	{
		event.Shift(1);
		eventNumber = event.m_data[0];
	}

	switch((CTokenManager::SubsystemTokens)event.GetLong())
	{
	case CTokenManager::video:
		{
			event.Shift(1);
			return Video.EventReceiver(event).error;
		}
		break;
	case CTokenManager::sound:
		{
			event.Shift(1);
			return Sound.EventReceiver(event).error;
		}
		break;
	case CTokenManager::gui:
		{
			event.Shift(1);
			return Gui.EventReceiver(event).error;
		}
		break;
	case CTokenManager::input:
		{
			event.Shift(1);
			return Gui.EventReceiver(event).error;
		}
		break;
	case CTokenManager::hsi:
		{
			event.Shift(1);
			return Hsi.EventReceiver(event).error;
		}
		break;
/*	case CTokenManager::console:
		{
			if(pConsole)
			{
				event.Shift(1);
				return pConsole->EventReceiver(event).error;
			}
		}
		break;
	case CTokenManager::network:
		{
			if(pNetwork)
			{
				event.Shift(1);
				return pNetwork->EventReceiver(event).error;
			}
		}
		break;
	case CTokenManager::resource:
		{
			if(pResource)
			{
				event.Shift(1);
				return pResource->EventReceiver(event).error;
			}
		}
		break;*/
	default:
		{
			return false;
		}
		break;
	}
	return false;
}
