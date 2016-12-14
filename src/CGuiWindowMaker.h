#ifndef CGUIWINDOWMAKER_H
#define CGUIWINDOWMAKER_H

#include "CGuiWindowWidget.h"

#include <iostream>
#include <map>
#include <stack>
using namespace std;


typedef long GuiWindowID; // invalid window ids are negative
typedef long GuiWindowSpecID; // invalid specification ids are negative


class CGuiWM
{
public:
	CGuiWM();
	~CGuiWM();

	GuiWindowSpecID ReadWindowSpec(string filename);//  read a window definition from xml file, create a window "class" with name as key in a map<>
														// to display this window, an instance of it must be created
	
	GuiWindowID MakeWindowInstance(GuiWindowSpecID windowSpecID); // create an instance of a window class, return a GuiWindowID
	GuiWindowID WindowFromFile(string filename){ return MakeWindowInstance(ReadWindowSpec(filename)); } // create an instance of a window class, return a GuiWindowID

	void DeleteWindowInstance(GuiWindowID windowID); //removewindow(GuiWindowID) <- destroy a window instance... should notify the CGuiContext
	void QueueDeleteWindowInstance(GuiWindowID windowID);
	void Think();

	CGuiWindowWidget* GetWindowInstance(GuiWindowID windowID);
	CGuiWindowWidget& GetWindowInstanceReference(GuiWindowID windowID) { return *GetWindowInstance(windowID); }
	GuiWindowID GetWindowID(CGuiWindowWidget* window);

	bool WindowIDIsValid(GuiWindowID windowID);


private:
	GuiWindowID getNextWindowID();
	GuiWindowSpecID getNextWindowSpecID();

	GuiWindowID guiWindowIDCounter;
	GuiWindowSpecID guiWindowSpecIDCounter;

	map<GuiWindowID, CGuiWindowWidget> windows;
	map<GuiWindowSpecID, CGuiWindowWidget> windowSpecifications; // window specifications are just a window widget instance
	stack<GuiWindowID> deletionStack; // windows to be deleted next think
};

#endif