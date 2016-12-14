
#include "CHSISubsystem.h"
#include "CInputSubsystem.h"
#include "CTokenManager.h"
#include "CCalModel.h"
#include "CVideoSubsystem.h"
#include "CGuiSubsystem.h"
#include "CMilkShapeModel.h"
#include "sys_main.h"
#include "CShip.h"

#include <string>
using namespace std;
CHSISubsystem Hsi;
float*stars(0);
int numStars(0);

CHSISubsystem::CHSISubsystem():
	initialized(false),
	mouseActive(false)
{
}
CHSISubsystem::~CHSISubsystem()
{
	if(initialized)
		Shutdown();
}

bool CHSISubsystem::PreInitialize() // PreInitialize is called before Initialize, only links to other subsystems should be made here
{
	return true;
}

bool CHSISubsystem::Initialize()
{
	Input.AddInputReceiver(this);

	//galaxy.ReadFromFile("data/galaxy.xml");
	galaxy.GenerateRandomly();

	m_mainDisplayContext.Initialize();

	return true;
}
bool CHSISubsystem::Shutdown()
{
	if(stars)
	{
		delete [] stars;
		stars = 0;
	}
	return true;
}
void CHSISubsystem::Think()
{
//	model.worldMatrix *= RotationMatrix(v3d(0, 5*sys_frameTime, 0));

//	for(vector<Colony>::iterator it = colonies.begin(); it != colonies.end(); it++)
//	{
//		it->UpdatePropertiesWindow();
//	}

	galaxy.Think();
	m_mainDisplayContext.Think();
}

CRouterReturnCode CHSISubsystem::EventReceiver(CRouterEvent& event)
{
	CTokenManager::HsiTokens token = (CTokenManager::HsiTokens)event.m_data[0];

	switch(token)
	{
	case CTokenManager::hsi_opencolonypropertieswindow:
		{
			if(event.m_arguments.MatchesFormat(t_long))
			{
				long colonyNumber;
				event.m_arguments.m_argList[0].GetValue(colonyNumber);
				OpenColonyPropertiesWindow(colonyNumber);
			}
		}
		break;
	default:
		{
		}
		break;
	}

	return CRouterReturnCode(false, true, "don't take events");
}
bool CHSISubsystem::InputReceiver(const CInputEvent& event)
{
	switch(event.inputEventType)
	{
	case IET_STATECHANGE:
		{
			switch(event.data.stateChangeEvent.keyCode)
			{
			case IKC_LEFT:
				{
					if(event.data.stateChangeEvent.keyAction == IKA_PRESS)
						m_mainDisplayContext.CycleBackwardInTier();
				}
				break;
			case IKC_RIGHT:
				{
					if(event.data.stateChangeEvent.keyAction == IKA_PRESS)
						m_mainDisplayContext.CycleForwardInTier();
				}
				break;
			case IKC_UP:
				{
					if(event.data.stateChangeEvent.keyAction == IKA_PRESS)
					{
						m_mainDisplayContext.ZoomOut();
					}
				}
				break;
			case IKC_DOWN:
				{
					if(event.data.stateChangeEvent.keyAction == IKA_PRESS)
					{
						m_mainDisplayContext.ZoomIn();
					}
				}
				break;
			case 'm':
				{
					if(event.data.stateChangeEvent.keyAction == IKA_PRESS)
					{
						mouseActive = !mouseActive;
					}
				}
				break;
			case 't':
				{
					static bool transition(false);
					transition = !transition;
					if(transition)
					{
						this->m_mainDisplayContext.camera.SetTransitionType(CCamera::none);
					}
					else
					{
						this->m_mainDisplayContext.camera.SetTransitionType(CCamera::average);
					}
				}
				break;			
			}
		}
		break;
	case IET_MOUSEMOVE:
		{
			if(mouseActive)
			{
				m_mainDisplayContext.camera.Orbit(event.data.mouseMoveEvent.mouseDeltaX, event.data.mouseMoveEvent.mouseDeltaY);
			}
		}
		break;
	}
	return false;
}


void CHSISubsystem::OpenColonyPropertiesWindow(long colonyNumber)	// prototyping
{
//	if(colonies.size() > colonyNumber)
//		colonies[colonyNumber].OpenPropertiesWindow();
}

