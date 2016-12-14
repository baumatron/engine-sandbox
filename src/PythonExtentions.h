
#include "CSoundSubsystem.h"
#include "CGuiSubsystem.h"
#include "CHSISubsystem.h"
#include "PythonFunctions.h"
	/*virtual bool PreInitialize(); // PreInitialize is called before Initialize, only links to other subsystems should be made here
	virtual bool Initialize();
	virtual bool Shutdown();

	virtual CRouterReturnCode EventReceiver(CRouterEvent& event);
	virtual bool InputReceiver(const CInputEvent& event);

////////////////////////
	short LoadSound(string filename, bool stream);
	void UnloadSound(short id);
	void UnloadSound(string filename);
	void StopSound(short id);
	void StopSound(string filename);
	void PlaySound(short id, float volume = 1.0f);
	void PlaySound(string filename, float volume = 1.0f, bool stream = false);
	float GetPlaybackProgress(string filename);
	float GetPlaybackProgress(short id);
	void Seek(float percent, string filename);*/

using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Sound_PlaySoundById_Overloads, CSoundSubsystem::PlaySoundById, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Sound_PlaySoundByName_Overloads, CSoundSubsystem::PlaySoundByName, 1, 3)

CSoundSubsystem& GetSound()
{
	return Sound;
}

CGuiSubsystem& GetGui()
{
	return Gui;
}

CHSISubsystem& GetHsi()
{
	return Hsi;
}


BOOST_PYTHON_MODULE( injection )
{	
	def("SetTriangleBudget", SetTriangleBudget);
	

	class_<CSoundSubsystem>("CSoundSubsystem")
		.def( "PlaySoundById", &CSoundSubsystem::PlaySoundById, Sound_PlaySoundById_Overloads() )
		.def( "PlaySoundByName", &CSoundSubsystem::PlaySoundByName, Sound_PlaySoundByName_Overloads() )
		;
	def("GetSound", GetSound, return_value_policy<reference_existing_object>());

	class_<CGuiFieldWidget>("CGuiFieldWidget")
		.def( "Print", &CGuiFieldWidget::Print)
		.def( "Clear", &CGuiFieldWidget::Clear)
		.def_readwrite( "contents", &CGuiFieldWidget::contents )
		;
	class_<CGuiWidgetPanel>("CGuiWidgetPanel")
		.def( "GetFieldWidget", &CGuiWidgetPanel::GetFieldWidget, return_value_policy<reference_existing_object>() )
		;
	class_<CGuiWindowWidget>("CGuiWindowWidget")
		.def_readwrite( "WidgetPanel", &CGuiWindowWidget::widgetPanel )
		.def_readwrite( "caption", &CGuiWindowWidget::caption )
		;
	class_<CGuiWM>("CGuiWM")
		.def( "Open", &CGuiWM::WindowFromFile)
		.def( "Close", &CGuiWM::QueueDeleteWindowInstance)
		.def( "GetWindow", &CGuiWM::GetWindowInstanceReference, return_value_policy<reference_existing_object>() )
		;
	class_<CGuiContext>("CGuiContext")
		.def( "GiveWindowFocus", &CGuiContext::GiveWindowFocus )
		;
	class_<CGuiSubsystem>("CGuiSubsystem")
		.def_readwrite( "WM", &CGuiSubsystem::WM )
		.def_readwrite( "Context", &CGuiSubsystem::Context )
		;
	def("GetGui", GetGui, return_value_policy<reference_existing_object>());

	class_<CHSIMainDisplayContext>("CHSIMainDisplayContext")
		.def( "ZoomOut", &CHSIMainDisplayContext::ZoomOut )
		.def( "ZoomIn", &CHSIMainDisplayContext::ZoomIn )
		.def( "CycleForwardInTier", &CHSIMainDisplayContext::CycleForwardInTier )
		.def( "CycleBackwardInTier", &CHSIMainDisplayContext::CycleBackwardInTier )
		.def( "SetViewTier", &CHSIMainDisplayContext::SetViewTier )
		.def( "SetActivePlanet", &CHSIMainDisplayContext::SetActivePlanet )
		.def( "SetActiveSystem", &CHSIMainDisplayContext::SetActiveSystem )
		;
	class_<CHSISubsystem>("CHSISubsystem")
		.def_readwrite( "MainDisplayContext", &CHSISubsystem::m_mainDisplayContext )
		;
	def("GetHsi", GetHsi, return_value_policy<reference_existing_object>());

}
