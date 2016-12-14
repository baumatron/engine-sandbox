

#include "sys_win.h"
#include "sys_main.h"
#include "con_main.h"
#include "CVideoSubsystem.h"
#include "vid_win.h"

#ifdef WIN32
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

//#include <windows.h>
//#include <windowsx.h>
#include <time.h>
#include <string>
#include <iostream>



using namespace std;


// timer
static int sys_ticks_per_sec;
static float sys_secs_per_tick;
static int sys_PerformanceTimerEnabled;
static int sys_PerformanceTimerStart;
static int sys_MMTimerStart;

clock_t startTime;

static void SetupTimeFloat()
{
	startTime = clock();
/*	if (!QueryPerformanceFrequency((LARGE_INTEGER *)&sys_ticks_per_sec))
	{ // no performance counter available
		sys_MMTimerStart = timeGetTime();
		sys_secs_per_tick = 1.0f/1000.0f;
		sys_PerformanceTimerEnabled = 0;
	}
	else
	{ // performance counter is available, use it instead of multimedia timer
		QueryPerformanceCounter((LARGE_INTEGER *)&sys_PerformanceTimerStart);
		sys_secs_per_tick = ((float)1.0)/((float)sys_ticks_per_sec);
		sys_PerformanceTimerEnabled = 1;
	}*/
}

// time passed in program in seconds
float SYSW_TimeFloat()
{
	return (float)(clock() - startTime)/CLOCKS_PER_SEC;
	//currentTime
	//return 1.0/75.0;
	/*static int time;

	if (sys_PerformanceTimerEnabled)
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&time);
		return(((float)(time-sys_PerformanceTimerStart)) * sys_secs_per_tick);
	}
	else
	{
		return(((float)(timeGetTime()-sys_MMTimerStart)) * sys_secs_per_tick);
	}*/
}

/*LRESULT CALLBACK WindowProc(HWND hwnd,
							UINT msg,
							WPARAM wparam,
							LPARAM lparam);

void SYSW_RegisterMainWindowClass(); // registers the main window class for this application
void SYSW_ShowMainWindow(); // displays the main window*/

/////////////////////////////////////
// Application entry point
int main(int argc, char *argv[])
{
	SDL_Surface *screen;
	SDL_Surface *picture;
	SDL_Event event;
	SDL_Rect pictureLocation;
	int leftPressed, rightPressed, upPressed, downPressed;
	const SDL_VideoInfo* videoinfo;

	SetupTimeFloat();
	
	SYS_Init();

	while(!quit)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event)) 
		{
			switch( event.type ) 
			{
				case SDL_QUIT:
					{
						quit = true;
						printf("Got quit event!\n");
					}
					break;
				case SDL_KEYDOWN:
					{
						switch(event.key.keysym.sym)
						{
						case '`':
							CON_Exec("console");
							break;
						case 'q':
							quit = true;
							break;
						}
						//printf("Key hit - exiting!\n");
						//quit = true;					
					}
					break;
			}
		}  

		SYS_Frame();     

	} // end while

	SYS_Shutdown();

	return 0;
}


void SYSW_Message(string text)
{
	ccout << text;
	//MessageBox(sysw_mainWindow, text.c_str(), "Message", MB_OK);
}