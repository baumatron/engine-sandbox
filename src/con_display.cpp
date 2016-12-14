#include "con_display.h"
#include "CVideoSubsystem.h"
#include "vid_win.h"
#include "con_main.h"
#include "sys_main.h"
#include "sys_win.h"
#include "CGuiSubsystem.h"
#include "CGuiWindowWidget.h"
#include "CGuiFieldWidget.h"

extern bool cond_initialized(false);

bool showConsole(false);
GuiWindowID consoleWindow(-1);
GuiWindowSpecID consoleWindowSpec;

void COND_Init()
{
	if(cond_initialized) return;

	consoleWindowSpec = Gui.WM.ReadWindowSpec("data/gui/templates/console.xml");

	cond_initialized = true;
}

CGuiWindowWidget* window = 0;
CGuiFieldWidget* field = 0;

void COND_Draw()
{
	if(!cond_initialized) return;
	if(showConsole)
	{
		if(consoleWindow == -1)
		{
			consoleWindow = Gui.WM.MakeWindowInstance(consoleWindowSpec);
			if(consoleWindow != -1)
			{
				window = Gui.WM.GetWindowInstance(consoleWindow);
				if(window)
				{	
					// check the bounds of the window, so that in low resolution modes it isn't too wide to see everything
					if(window->area.rightx > Video.settings.getSw())
						window->Resize(CRectangle(0, Video.settings.getSw() - window->area.rightx, 0, 0));
					if(window->area.leftx < 0)
						window->Resize(CRectangle(-window->area.leftx, 0, 0, 0));

					field = dynamic_cast<CGuiFieldWidget*>(window->widgetPanel.GetWidgetByName("consoletext"));
					
					float x = Video.settings.getSw()/2 - (window->area.rightx-window->area.leftx)/2;
					float y = Video.settings.getSh() - (window->area.topy - window->area.bottomy) - 50;
					window->Move(v3d(x,y)-window->area.getLowerLeft());
				}
			}
		}
		else
		{
			if(window)
			{
				if(window->visible == false)
				{
					window->visible = true;
					Gui.Context.GiveWindowFocus(consoleWindow);
				}
			}
		}
	}
	else
	{
		if(consoleWindow != -1)
		{
			if(window)
			{
				window->visible = false;
			}
		}
	}

	if(window)
	{
		// update the contents of the field
		string consoleText;

		for(int i = 14; i > textBufferDisplayLocation; i--)
		{
			consoleText += textBuffer[i] + "\n";
		}
		consoleText += "]" + inputLine;

		if(field)
			field->contents = consoleText;
	}
}

void COND_Shutdown()
{
	cond_initialized = false;
}

bool COND_ConsoleIsVisible()
{
	return showConsole;
}
