#include "CEventRouter.h"
#include "CToken.h"
CEventRouter EventRouter;

CEventRouter::CEventRouter():
		pVideo(0),
		pSound(0),
		pInput(0),
		pConsole(0),
		pNetwork(0),
		pResource(0)
{
	pSound = &Sound;
}


bool CEventRouter::RouteEvent(CEvent& event)
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
			if(pVideo)
			{
				event.Shift(1);
				return pVideo->EventReceiver(event).error;
			}
		}
		break;
	case CTokenManager::sound:
		{
			if(pSound)
			{
				event.Shift(1);
				return pSound->EventReceiver(event).error;
			}
		}
		break;
	case CTokenManager::input:
		{
			if(pInput)
			{
				event.Shift(1);
				return pInput->EventReceiver(event).error;
			}
		}
		break;
	case CTokenManager::console:
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
		break;
	default:
		{
			return false;
		}
		break;
	}
	return false;
}
