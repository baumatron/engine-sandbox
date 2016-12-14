

#include "sys_main.h"
#include "sys_win.h"
#include "con_main.h"
#include "con_display.h"
#include "CInputSubsystem.h"
#include "m_misc.h"
#include "CVideoSubsystem.h"
#include "CSoundSubsystem.h"
#include "math_main.h"
#include "CCalModel.h"
#include "CModel.h"
//#include "CMilkShapeModel.h"
#include "IModelLoader.h"
#include "CRouterEvent.h"
#include "CEventRouter.h"
#include "CTokenManager.h"
#include "CBitmapImageLoader.h"
#include "CGuiButtonWidget.h"
#include "CGuiSubsystem.h"
#include "CHSISubsystem.h"
#include "CPythonSubsystem.h"
#include "CXMLParser.h"

//#include <ode\ode.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

ofstream fout_log;

bool quit(false);
bool paused(false);

static float sys_baseTime;
static float sys_lastTime;

float sys_curTime;
float sys_frameTime;

void con_cmd_pause_handler(con_Input& input)
{
	paused = !paused;
}
con_Command con_cmd_pause("pause", con_cmd_pause_handler);

void con_cmd_reset_handler(con_Input& input)
{
	CON_Shutdown();
	CON_Init();
}
con_Command con_cmd_reset("reset", con_cmd_reset_handler);

static void UpdateFrameTimes()
{
	sys_curTime = M_TimeFloat() - sys_baseTime;
	sys_frameTime = sys_curTime - sys_lastTime;
	sys_lastTime = sys_curTime;
}


static void ResetFrameTimes()
{
	sys_baseTime = 0;//M_TimeFloat();
	sys_lastTime = sys_curTime = sys_frameTime = 0.0;
}
VideoResourceID targaFileSystem, targaFileVideo;
	CGuiButtonWidget button;


void SYS_Init()
{
	fout_log.open("log.txt");
	ResetFrameTimes();

	ifstream configFile("config.xml");
	short width, height;
	bool fullscreen;
	if(configFile.fail())
	{
		ccout << "Warning: no config.xml file found in application directory.";
		width = 640;
		height = 480;
		fullscreen = false;
	}
	else
	{
		string videoConfig = CXMLParser::GetInstance()->GetTagData(configFile, "video");
		width = atoi(CXMLParser::GetInstance()->GetTagData(videoConfig, "x").c_str());
		height = atoi(CXMLParser::GetInstance()->GetTagData(videoConfig, "y").c_str());
		fullscreen = M_atob(CXMLParser::GetInstance()->GetTagData(videoConfig, "fullscreen"));
	}
	configFile.close();

	// set up subsystems
	Video.PreInitialize();
	Video.Initialize(width,height,32,fullscreen);
	Python.Initialize();
	Gui.Initialize();
	
	// open up the neat loading window and get references to it's widgets
	GuiWindowID startupWindow = Gui.WM.MakeWindowInstance(Gui.WM.ReadWindowSpec("data/gui/templates/starting.xml"));
	CGuiWindowWidget & windowRef = *Gui.WM.GetWindowInstance(startupWindow);
	windowRef.Move(v3d(Video.settings.getSw()/2 - (windowRef.area.rightx-windowRef.area.leftx)/2, Video.settings.getSh()/2 - (windowRef.area.topy-windowRef.area.bottomy)/2));
	float & progress((dynamic_cast<CGuiProgressBarWidget*>(windowRef.widgetPanel.GetWidgetByName("progress")))->progress);
	string & task((dynamic_cast<CGuiFieldWidget*>(windowRef.widgetPanel.GetWidgetByName("task")))->contents);

	// continue setting up subsystems, but now with loading bar
	progress = 0;
	task = "Console";
	Video.Think(); Video.BeginDraw(); Gui.Think(); Video.EndDraw();
	CON_Init();

	progress = 0.1;
	task = "Console Display";
	Video.Think(); Video.BeginDraw(); Gui.Think(); Video.EndDraw();
	COND_Init();

	progress = 0.2;
	task = "Input";
	Video.Think(); Video.BeginDraw(); Gui.Think(); Video.EndDraw(); 
	Input.Initialize();

	progress = 0.3;
	task = "Sound";
	Video.Think(); Video.BeginDraw(); Gui.Think(); Video.EndDraw(); 
	Sound.Initialize();

	progress = 0.4;
	task = "Hot Space Injection";
	Video.Think(); Video.BeginDraw(); Gui.Think(); Video.EndDraw(); 

	progress = 0.5;
	Video.Think(); Video.BeginDraw(); Gui.Think(); COND_Draw(); Video.EndDraw(); 

	Hsi.Initialize();
	progress = 0.8;
	Video.Think(); Video.BeginDraw(); Gui.Think(); Video.EndDraw(); 

	ccout << "Subsystems initialized.\n";

	CON_CMD_REG(pause);
	CON_RegisterCommand(&con_cmd_reset);

	CON_ExecFile("default.cfg"); // execute default config file

	progress = 1.0;
	task = "Initialization Complete";
	Video.Think(); Video.BeginDraw(); Gui.Think(); Video.EndDraw();

	Gui.WM.DeleteWindowInstance(startupWindow);

	Gui.WM.WindowFromFile("window.xml");
//	ccout << "float size: " << (int)sizeof(float) << newl;
//	ccout << "char size: " << (int)sizeof(char) << newl;
//	ccout << "vertex size: " << (int)sizeof(Vertex) << newl;
//	ccout << "vertexarrayelement size: " << (int)sizeof(VertexArrayElement) << newl;
	CModelRepository::Instance()->PrintMap();
}

void SYS_Frame()
{
	UpdateFrameTimes();

	Input.Think();

	Video.Think();
	Video.BeginDraw();

	Gui.Think();

	Hsi.Think();

	if(paused)
	{
		Video.DrawTextShit("PAUSED", Video.settings.getSw()/2-24, /*vid_Settings.getSh()/2-4*/16, 0);
	}

	static string fps;
	static float accumulator = 0;
	static float nextFpsTime = 0;
	static float nextDisplayTime = 0;
	static float numSamples = 0;
//	if(nextFpsTime < sys_curTime)
//	{
		accumulator += 1.0f/sys_frameTime;
		numSamples += 1.0;
	//	nextFpsTime = sys_curTime + 0.01;

//	}
	if(nextDisplayTime < sys_curTime)
	{
		if(numSamples)
			fps = M_ftoa(accumulator/numSamples);
		accumulator = 0;
		numSamples = 0;
		nextDisplayTime = sys_curTime + 1.0;
	}
	Video.DrawTextShit("FPS: " + fps, Video.settings.getSw()-80, 16, 0);
	Video.DrawTextShit("Triangles Rendered: " + M_ftoa(CModel::trianglesRendered), Video.settings.getSw()-160, 40, 0);
	// view drawn above here
	// draw console
	
//	Video.ResetViewportContexts();  // WHY WAS THIS HERE????? WAS IT ONLY WIDGET DRAWING TESTING?

	COND_Draw();

	// anything thats always on top can be drawn below here
	Video.EndDraw();

}
void SYS_Shutdown()
{
	Hsi.Shutdown();
	Sound.Shutdown();
	COND_Shutdown();
	CON_Shutdown();
	Input.Shutdown();
	Gui.Shutdown();
	Python.Shutdown();
	Video.Shutdown();
	fout_log.close();
}

void SYS_Message(string text)
{
	string message = "system: "+text;
	if(!(Video.IsInitialized() && cond_initialized && con_initialized))
	{
		if(fout_log.is_open())
			fout_log << text << flush;
	}
	else
	{
		ccout << message;
	}
}