#ifndef CGUICONTEXT_H
#define CGUICONTEXT_H

#include "CGuiWindowMaker.h"

#include <deque>
using namespace std;



class CGuiContext
{
public:
	CGuiContext();
	~CGuiContext();

	void RemoveWindowReference(GuiWindowID windowID);	// windowWasDestroyed(GuiWindowID) <- notify the context manager that a window was destroyed

	void GiveWindowFocus(GuiWindowID windowID);	// setwindowfocus(GuiWindowID) <- give the window focus. first, see if it's in the map, otherwise add it to the map

	bool InputReceiver(const CInputEvent& event);

	void RenderWindows();

	void AddWindow(GuiWindowID window, bool giveFocus = true);

	deque<GuiWindowID> windowFocusStack;

	struct MouseContext
	{
		GuiWindowID windowOver; // window id, -1 means not over a window
		int widgetOver; // widget index, -1 if over none
	} previousMouseContext;

	//GuiWindowID draggedWindow;
};


#endif