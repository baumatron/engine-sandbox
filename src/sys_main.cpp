

#include "sys_main.h"
#include "sys_win.h"
#include "con_main.h"
#include "con_display.h"
//#include "in_main.h"
#include "CInputSubsystem.h"
#include "m_misc.h"
#include "CVideoSubsystem.h"
#include "bnd_main.h"
//#include "bn_main.h"
//#include "bn_map.h"
#include "gui_main.h"
#include "CSoundSubsystem.h"
//#include "net_main.h"
#include "math_main.h"
#include "CModel.h"
#include "CMilkShapeModel.h"
#include "IModelLoader.h"
#include "CRouterEvent.h"
#include "CEventRouter.h"
#include "CToken.h"
#include "CBitmapImageLoader.h"

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
	sys_frameTime = 1.0f/75.0f;/*sys_curTime - sys_lastTime;*/
	sys_lastTime = sys_curTime;
}


static void ResetFrameTimes()
{
	sys_baseTime = 0;//M_TimeFloat();
	sys_lastTime = sys_curTime = sys_frameTime = 0.0;
}
VideoResourceID targaFileSystem, targaFileVideo;
void SYS_Init()
{
	fout_log.open("log.txt");

	ResetFrameTimes();

//	BN_PreInit();
	BND_PreInit();
	Video.PreInitialize();

	Video.Initialize(800,600,32,false);

	Sound.Initialize();

	IN_Init();

	BND_Init();

	COND_Init();

	CON_Init();

	GUI_Init();

	gui_Window* window = new gui_Window(v3d(100,100));
	window->title = "Window";
	window->dragable = true;
	gui_guiGraph.addObject(window);

//	BN_Init();

//	NET_Init();

	//net_updateThread.start();

	ccout << "Subsystems initialized.\n";

	CON_CMD_REG(pause);
	CON_RegisterCommand(&con_cmd_reset);

	CON_ExecFile("default.cfg"); // execute default config file

	Video.camera.destination = TranslationMatrix(v3d(0,100,200)) * RotationMatrix(v3d(-20, 0, 0));
	Video.camera.transition = vid_Camera::none;

	Video.settings.lighting = false;


	Sound.LoadSound("data/sounds/menkey.wav", false);
	Sound.LoadSound("data/sounds/pain_shot1.wav", false);
	CRouterEvent event;

	event = TokenManager.BuildEventFromString("local.sound.play(2)");

	if(!EventRouter.RouteEvent(event))
		ccout << "failed to route event\n";

	ccout << "loading targa image\n";
	targaFileSystem = Video.VideoResourceManager.LoadImageToSystemMemory("data/images/system/conbg.tga");
	targaFileVideo = Video.VideoResourceManager.LoadImageToVideoMemory("data/images/system/conbg.tga");
	//Video.VideoResourceManager.GetImageFromSystemMemory(targaFile);
	ccout << "memory usage (system): " << Video.VideoResourceManager.GetSystemMemoryUsage() << newl;
	ccout << "memory usage (video): " << Video.VideoResourceManager.GetVideoMemoryUsage() << newl;
	
	if(Video.VideoResourceManager.GetImageFromSystemMemory(targaFileSystem))
		BitmapImageLoader.SaveToFile(*Video.VideoResourceManager.GetImageFromSystemMemory(targaFileSystem), "output.bmp", true);
}

void SYS_Frame()
{
	UpdateFrameTimes();

	float sgtime(0), phystime(0), drawtime(0), starttime(0);
	if(!paused)
	{	
	/*	starttime = SYSW_TimeFloat();
		twoDStaticSceneGraph.update();
		Video.3dStaticSceneGraph.update();
		Video.serverSceneGraph.update();
		Video.clientSceneGraph.update();

		sgtime = SYSW_TimeFloat() - starttime;
	
		starttime = SYSW_TimeFloat();
		dJointGroupEmpty (contactgroup);

		dSpaceCollide (space, 0, nearCallback);

		dWorldStepFast1(world, 0.05, 5);
		for(int i = 0; i < boxes.size(); i++)
		{
			const dReal* pos = dBodyGetPosition(boxes[i].body);
			const dReal* rot = dBodyGetRotation(boxes[i].body);

			matrix4x4 position = TranslationMatrix(v3d(pos[0], pos[1], pos[2]));
			matrix4x4 rotation(rot[0], rot[1], rot[2], rot[3],
								rot[4], rot[5], rot[6] , rot[7],
								rot[8], rot[9], rot[10], rot[11],
								0,		0,		0,		1);
			matrix4x4 trans = position * rotation;
			boxes[i].object->transform = trans;
		}
		phystime = SYSW_TimeFloat() - starttime;*/
	}


	Video.camera.update();
	
	Input.Think();

	//IN_Update();

	GUI_Update();


//	starttime = SYSW_TimeFloat();
//	starttime = SYSW_TimeFloat();
	Video.Think();
	Video.BeginDraw();

	for(int i = 0; i < 10; i++)
	{
/*		twoDStaticSceneGraph.draw(editor, i);
		Video.3dStaticSceneGraph.draw(editor, i);
		Video.serverSceneGraph.draw(ingame, i);
		Video.clientSceneGraph.draw(ingame, i);*/
	}

/*	vcout.setColor(CColor(1.0f, 1.0f, 1.0f));
	vcout.setPos(v3d(100,20));
	vcout.setFont("Palatino Linotype");
	vcout.setHeight(100);
	vcout.setWeight(1000);
*/

/*	char text[64];
	for(char i = 32; i < 96; i++)
		text[i-32] = char(i);
	vcout << text;*/

	/*CViewport port;
	port.area.leftx = 100;
	port.area.rightx = 400;
	port.area.bottomy = 100;
	port.area.topy = 400;
	Video.SetViewport( port );*/
	
	GUI_Draw(ingame);

	//Video.DrawModel(model);

	if(paused)
	{
		Video.DrawTextShit("PAUSED", Video.settings.getSw()/2-24, /*vid_Settings.getSh()/2-4*/16, 0);
	}

	Video.DrawTextShit("FPS: " + M_ftoa(1.0f/sys_frameTime), Video.settings.getSw()-80, 16, 0);
	CImage* image = Video.VideoResourceManager.GetImageFromSystemMemory(targaFileSystem);
	/*Video.BlitBitmap(targaFileVideo, 0, v3d(400,400));
	Video.BlitBitmap(targaFileVideo, v3d(133,133), v3d(400,400));*/
	//Video.BlitBitmap(targaFile, v3d(133,133), v3d(image->GetWidth(), image->GetHeight()));
	//Video.BlitBitmap((unsigned long*)image->GetBuffer(), 0, v3d(image->GetWidth(), image->GetHeight()));
	//Video.BlitBitmap((unsigned long*)image->GetBuffer(), v3d(133,133), v3d(image->GetWidth(), image->GetHeight()));
//	ccout << M_ftoa(1.0f/sys_frameTime) << newl;
	// view drawn above here
	// draw console
	
	CViewport viewport;
	viewport.area.leftx = 50;
	viewport.area.rightx = Video.settings.getSw();
	viewport.area.bottomy = 50;
	viewport.area.topy = Video.settings.getSh();
	CRectangle area;
	area.leftx = 0;
	area.rightx = 100;
	area.bottomy = 0;
	area.topy = 20;
	static double percent(0.0f);
	Video.SetViewport( viewport );
	Video.DrawProgressBar( area, percent, CColor(1.0f, 1.0f, 1.0f, 1.0f), CColor(0.2f, 0.5f, 0.6f, 1.0f), CColor(0.35f, 0.35f, 0.35f, 1.0f) );
	Video.SetViewport();
	percent += 0.001f;
	if(percent > 1.00f)
		percent = 0.0f;
	COND_Draw();


	// anything thats always on top can be drawn below here
	Video.EndDraw();
	drawtime = SYSW_TimeFloat() - starttime;

/*	ccout << "sgtime: " << sgtime << newl
		<<	"phystime: " << phystime << newl
		<<	"drawtime: " << drawtime << newl;*/
//	ccout << "draw tdelta: " << SYSW_TimeFloat()-starttime << newl;
//	Sleep(100);
}
void SYS_Shutdown()
{
	//net_updateThread.stop();
//	NET_Shutdown();
	GUI_Shutdown();
	Video.Shutdown();
	Sound.Shutdown();
	COND_Shutdown();
	CON_Shutdown();
	BND_Shutdown();
	IN_Shutdown();
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

unsigned char SYS_InputReceiver(in_Event * event)
{
	return 0;
}


