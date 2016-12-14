

#include "CGuiContext.h"

#include "CGuiSubsystem.h"

#include "CGuiWindowWidget.h"

#include "CInputSubsystem.h"

CGuiContext::CGuiContext()
{
	previousMouseContext.widgetOver = -1;
	previousMouseContext.windowOver = -1;
	//draggedWindow = -1;
}
CGuiContext::~CGuiContext()
{
}

void CGuiContext::RemoveWindowReference(GuiWindowID windowID)	//  notify the context manager that a window was destroyed
{
	for(deque<GuiWindowID>::iterator it = windowFocusStack.begin(); it != windowFocusStack.end(); it++)
		if(*it == windowID)
		{ // found the reference, kill it
			windowFocusStack.erase(it);
			break;
		}
}
void CGuiContext::GiveWindowFocus(GuiWindowID windowID)	//  give the window focus. first, see if it's in the map, otherwise add it to the map
{
	// first check to see if window is already in the focus stack, and if so, remove it and insert it back at the top
	RemoveWindowReference(windowID);

	if(windowFocusStack.size())
		Gui.WM.GetWindowInstance(windowFocusStack.back())->onLoseFocus();

    windowFocusStack.push_back(windowID);

	Gui.WM.GetWindowInstance(windowID)->onGetFocus();
}

bool CGuiContext::InputReceiver(const CInputEvent& event)
{
	bool mouseOverWindow(false);
	bool mouseOverWidget(false);
	MouseContext mouseContext;

	for(deque<GuiWindowID>::reverse_iterator it = windowFocusStack.rbegin(); it != windowFocusStack.rend(); it++)
	{
		bool breakout(false);
		CGuiWindowWidget* window = Gui.WM.GetWindowInstance(*it);
		if(!window)
			continue;
		if(!window->visible)
			continue;

		// set the python context
		window->PythonScriptBegin();

		// check to see if the window contains the mouse click
		switch(event.inputEventType)
		{
		case IET_MOUSEBUTTON:
			{
				if(event.data.mouseButtonEvent.mouseButtonAction == IKA_PRESS)
				{
					v3d mouse(event.data.mouseButtonEvent.mousePositionX, event.data.mouseButtonEvent.mousePositionY);
					// it was a mouse click, see if it was contained in this window
					if( (window->area.leftx < mouse.x && window->area.rightx > mouse.x)
						&&
						(window->area.bottomy < mouse.y && window->area.topy > mouse.y) )
					{
						if(it == windowFocusStack.rbegin()) // it was the window in focus that recieved the input
						{	
							if(window->onMouseDown(mouse))
								breakout = true; // break, because the input was used!
						//	else
							//	draggedWindow = *it;
						}
						else
						{
							GiveWindowFocus(*it);
							breakout = true;
						}
					}
				}
				else if(event.data.mouseButtonEvent.mouseButtonAction == IKA_RELEASE)
				{
					v3d mouse(event.data.mouseButtonEvent.mousePositionX, event.data.mouseButtonEvent.mousePositionY);
					// it was a mouse click, see if it was contained in this window
					if( (window->area.leftx < mouse.x && window->area.rightx > mouse.x)
						&&
						(window->area.bottomy < mouse.y && window->area.topy > mouse.y) )
					{
						if(it == windowFocusStack.rbegin()) // it was the window in focus that recieved the input
						{	
						//	draggedWindow = -1;
							if(window->onMouseUp(mouse))
								breakout = true; // break, because the input was used!
						}			
					}
				}
			}
			break;
		case IET_MOUSEMOVE:
			{
				v3d mouse(event.data.mouseMoveEvent.mouseX, event.data.mouseMoveEvent.mouseY);
				v3d mousedelta(event.data.mouseMoveEvent.mouseDeltaX, event.data.mouseMoveEvent.mouseDeltaY);				

				if(it == windowFocusStack.rbegin()) // only look at the top window
				{
					bool mouseMoveEvent = false;
					bool mouseAlreadyOverWidget = false;
					// see if it was contained in a window
					if(!mouseOverWindow)
					{
						if( (window->area.leftx < mouse.x-mousedelta.x && window->area.rightx > mouse.x-mousedelta.x)
							&&
							(window->area.bottomy < mouse.y-mousedelta.y && window->area.topy > mouse.y-mousedelta.y) )
						{ // check by subtracting the mouse delta to make sure windows aren't left behind when dragging
							mouseMoveEvent = true;
						}
					}
					// see if it was contained in a widget
					if(!mouseOverWidget)
					{
						vector<IGuiWidget*>& widgets(window->widgetPanel.widgets);
						for(int i = 0; i < widgets.size(); i++)
						{
							if( (widgets[i]->area.leftx < mouse.x-mousedelta.x && widgets[i]->area.rightx > mouse.x-mousedelta.x)
								&&
								(widgets[i]->area.bottomy < mouse.y-mousedelta.y && widgets[i]->area.topy > mouse.y-mousedelta.y) )
							{
								mouseAlreadyOverWidget = true;
							}
						}
					}


					if(!mouseOverWindow)
					{
						if(!mouseAlreadyOverWidget)
							if(mouseMoveEvent)
								window->onMouseMove(mousedelta);
						if( (window->area.leftx < mouse.x && window->area.rightx > mouse.x)
							&&
							(window->area.bottomy < mouse.y && window->area.topy > mouse.y) )
						{	
							// check again to see if the window is contained after any window dragging
							mouseContext.windowOver = *it;
							mouseOverWindow = true;
						}
					}
					if(!mouseOverWidget)
					{
						vector<IGuiWidget*>& widgets(window->widgetPanel.widgets);
						for(int i = 0; i < widgets.size(); i++)
						{
							if( (widgets[i]->area.leftx < mouse.x && widgets[i]->area.rightx > mouse.x)
								&&
								(widgets[i]->area.bottomy < mouse.y && widgets[i]->area.topy > mouse.y) )
							{
								mouseContext.widgetOver = i;
								mouseOverWidget = true;
								widgets[i]->onMouseMove(mousedelta);
							}
						}
					}


				}
			}
			break;
		default:
			{	
				// TODO: FIX THIS SO HIDDEN WINDOWS DON'T GET INPUT.
				// AND INPUT IS PASSED ON TO HIGHEST VISIBLE WINDOW
				if(it == windowFocusStack.rbegin()) // it was the window in focus that recieved the input
				{	
					return window->onKey(event);
				}
			}
			break;
		}
		window->PythonScriptEnd();
		if(breakout)
			break;
	}
	
	if(event.inputEventType == IET_MOUSEMOVE)
	{
		v3d mouse(event.data.mouseMoveEvent.mouseX, event.data.mouseMoveEvent.mouseY);
		v3d mousedelta(event.data.mouseMoveEvent.mouseDeltaX, event.data.mouseMoveEvent.mouseDeltaY);				

	//	CGuiWindowWidget* draggedWindowPtr = Gui.WM.GetWindowInstance(draggedWindow);
	//	if(draggedWindowPtr)
	//		draggedWindowPtr->Move(mousedelta);
		if(!mouseOverWindow)
			mouseContext.windowOver = -1;
		if(!mouseOverWidget)
			mouseContext.widgetOver = -1;

		CGuiWindowWidget* window = Gui.WM.GetWindowInstance(mouseContext.windowOver);
		CGuiWindowWidget* previousWindow = Gui.WM.GetWindowInstance(previousMouseContext.windowOver);
		IGuiWidget* widget = 0;
		IGuiWidget* previousWidget = 0;
		if(mouseContext.widgetOver != -1)
			if(window)
				widget = window->widgetPanel.widgets[mouseContext.widgetOver];
		if(previousMouseContext.widgetOver != -1)
			if(previousWindow)
				previousWidget = previousWindow->widgetPanel.widgets[previousMouseContext.widgetOver];
		
		bool mouseMoveEvent = (mousedelta.x != 0) || (mousedelta.y != 0);


		if(mouseContext.windowOver != previousMouseContext.windowOver)
		{
			if(window)
			{
				window->onMouseEnter(mouse);
				if(widget)
					widget->onMouseEnter(mouse);
		/*		if(mouseMoveEvent)
					window->onMouseMove(mousedelta);*/
			}
			if(previousWindow)
			{
				//if(mouseMoveEvent)
			//	{
				//	previousWindow->onMouseMove(mousedelta);
					// now reset everything to before this function call... to check the window properties again
			//		mouseContext = previousMouseContext;
			//	}
			//	else
			//	{
				previousWindow->onMouseLeave(mouse);
				if(previousWidget)
					previousWidget->onMouseLeave(mouse);
			//	}
			}
		}
		else
		{
			if(mouseContext.widgetOver != previousMouseContext.widgetOver)
			{
				if(widget)
					widget->onMouseEnter(mouse);
				if(previousWidget)
					previousWidget->onMouseLeave(mouse);
			}
		/*	if(mouseMoveEvent)
				if(window)
					window->onMouseMove(mousedelta);*/
		}

		previousMouseContext = mouseContext;
	}

	return false;
}

void CGuiContext::RenderWindows()
{
	for(deque<GuiWindowID>::iterator it = windowFocusStack.begin(); it != windowFocusStack.end(); it++)
	{
	//	float startTime = M_TimeFloat();
		Gui.WM.GetWindowInstance(*it)->Render();
	//	float time = M_TimeFloat() - startTime;
	//	if(time > 0.00001)
	//		ccout << "Time for window: " << time << newl;
	}
}

void CGuiContext::AddWindow(GuiWindowID window, bool giveFocus)
{
	if(giveFocus)
	{
		GiveWindowFocus(window); // inherently adds window to the focus stack
	}
	else
	{
		// stick it at the bottom
		windowFocusStack.push_front(window);
	}
}
