
#include "CGuiSubsystem.h"
#include "CGuiWindowMaker.h"
#include "CGuiButtonWidget.h"
#include "CGuiFieldWidget.h"
#include "CGuiProgressBarWidget.h"
#include "CGuiSliderWidget.h"
#include "CGuiViewportWidget.h"

//#include "CGuiFileParser.h"
#include "CXMLParser.h"
#include "CTokenManager.h"

#include <fstream>
#include <string>
using namespace std;

CGuiWM::CGuiWM()
{
	guiWindowIDCounter = 0;
	guiWindowSpecIDCounter = 0;
}
CGuiWM::~CGuiWM()
{
	windows.clear();
	windowSpecifications.clear();
}


GuiWindowSpecID CGuiWM::ReadWindowSpec(string filename)//  read a window definition from xml file, create a window "class" with name as key in a map<>
{													// to display this window, an instance of it must be created
	// TODO: read from a file and add a specification to the list, return it's new id


	ifstream fin;
	fin.open(filename.c_str());

	if(fin.fail())
	{
		return -1;
	}

	// now read the specification
	GuiWindowSpecID specID = getNextWindowSpecID();

	string window = CXMLParser::GetInstance()->GetTagData(fin, "window");

	CRectangle theArea;
	string area = CXMLParser::GetInstance()->GetTagData(window, "area");
	theArea.leftx = atof(CXMLParser::GetInstance()->GetTagData(area, "lx").c_str());
	theArea.rightx = atof(CXMLParser::GetInstance()->GetTagData(area, "rx").c_str());
	theArea.bottomy = atof(CXMLParser::GetInstance()->GetTagData(area, "by").c_str());
	theArea.topy = atof(CXMLParser::GetInstance()->GetTagData(area, "ty").c_str());
	//spec.widgetPanel.area = spec.area;
	//spec.widgetPanel.area.topy -= 10;

	CColor theColor;
	string color = CXMLParser::GetInstance()->GetTagData(window, "color");
	theColor.setR((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(color, "r").c_str()));
	theColor.setG((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(color, "g").c_str()));
	theColor.setB((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(color, "b").c_str()));
	theColor.setA((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(color, "a").c_str()));

	string theCaption;
	theCaption = CXMLParser::GetInstance()->GetTagData(window, "caption");

	bool dragable = M_atob(CXMLParser::GetInstance()->GetTagData(window, "dragable"));

	CGuiWindowWidget spec(theColor, theCaption, theArea);// = CGuiFileParser::GetInstance()->ParseWindowDefinitionFile(filename);
	spec.dragable = dragable;

	string widgets = CXMLParser::GetInstance()->GetTagData(window, "widgets");
	string widgetXML;
	int number(0);
	do
	{
		widgetXML = CXMLParser::GetInstance()->GetTagData(widgets, "widget", number++);
		string type = CXMLParser::GetInstance()->GetTagData(widgetXML, "type");
		if(type == "button")
		{
			CGuiButtonWidget* widget = new CGuiButtonWidget;
			
			widget->name = CXMLParser::GetInstance()->GetTagData(widgetXML, "name");

			string area = CXMLParser::GetInstance()->GetTagData(widgetXML, "area");
			widget->area.leftx = atof(CXMLParser::GetInstance()->GetTagData(area, "lx").c_str());
			widget->area.rightx = atof(CXMLParser::GetInstance()->GetTagData(area, "rx").c_str());
			widget->area.bottomy = atof(CXMLParser::GetInstance()->GetTagData(area, "by").c_str());
			widget->area.topy = atof(CXMLParser::GetInstance()->GetTagData(area, "ty").c_str());

			string color = CXMLParser::GetInstance()->GetTagData(widgetXML, "color");
			widget->color.setR((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(color, "r").c_str()));
			widget->color.setG((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(color, "g").c_str()));
			widget->color.setB((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(color, "b").c_str()));
			widget->color.setA((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(color, "a").c_str()));

			widget->caption = CXMLParser::GetInstance()->GetTagData(widgetXML, "caption");

			//widget->buttonRouterEvent = TokenManager.BuildEventFromString(CXMLParser::GetInstance()->GetTagData(widgetXML, "event"));
			widget->script = CXMLParser::GetInstance()->GetTagData(widgetXML, "script");
			spec.widgetPanel.widgets.push_back(widget);
		}
		else if(type == "field")
		{
			CGuiFieldWidget* widget = new CGuiFieldWidget;
			
			widget->name = CXMLParser::GetInstance()->GetTagData(widgetXML, "name");

			string area = CXMLParser::GetInstance()->GetTagData(widgetXML, "area");
			widget->area.leftx = atof(CXMLParser::GetInstance()->GetTagData(area, "lx").c_str());
			widget->area.rightx = atof(CXMLParser::GetInstance()->GetTagData(area, "rx").c_str());
			widget->area.bottomy = atof(CXMLParser::GetInstance()->GetTagData(area, "by").c_str());
			widget->area.topy = atof(CXMLParser::GetInstance()->GetTagData(area, "ty").c_str());

			string backgroundcolor = CXMLParser::GetInstance()->GetTagData(widgetXML, "backgroundcolor");
			widget->backgroundColor.setR((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(backgroundcolor, "r").c_str()));
			widget->backgroundColor.setG((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(backgroundcolor, "g").c_str()));
			widget->backgroundColor.setB((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(backgroundcolor, "b").c_str()));
			widget->backgroundColor.setA((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(backgroundcolor, "a").c_str()));

			string textcolor = CXMLParser::GetInstance()->GetTagData(widgetXML, "textcolor");
			widget->textColor.setR((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(textcolor, "r").c_str()));
			widget->textColor.setG((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(textcolor, "g").c_str()));
			widget->textColor.setB((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(textcolor, "b").c_str()));
			widget->textColor.setA((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(textcolor, "a").c_str()));


			widget->contents = CXMLParser::GetInstance()->GetTagData(widgetXML, "contents");

			spec.widgetPanel.widgets.push_back(widget);
		}
		else if(type == "progressbar")
		{
			CGuiProgressBarWidget* widget = new CGuiProgressBarWidget;

			widget->name = CXMLParser::GetInstance()->GetTagData(widgetXML, "name");

			string area = CXMLParser::GetInstance()->GetTagData(widgetXML, "area");
			widget->area.leftx = atof(CXMLParser::GetInstance()->GetTagData(area, "lx").c_str());
			widget->area.rightx = atof(CXMLParser::GetInstance()->GetTagData(area, "rx").c_str());
			widget->area.bottomy = atof(CXMLParser::GetInstance()->GetTagData(area, "by").c_str());
			widget->area.topy = atof(CXMLParser::GetInstance()->GetTagData(area, "ty").c_str());

			string backgroundcolor = CXMLParser::GetInstance()->GetTagData(widgetXML, "backgroundcolor");
			widget->backgroundColor.setR((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(backgroundcolor, "r").c_str()));
			widget->backgroundColor.setG((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(backgroundcolor, "g").c_str()));
			widget->backgroundColor.setB((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(backgroundcolor, "b").c_str()));
			widget->backgroundColor.setA((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(backgroundcolor, "a").c_str()));

			string barcolor = CXMLParser::GetInstance()->GetTagData(widgetXML, "barcolor");
			widget->barColor.setR((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(barcolor, "r").c_str()));
			widget->barColor.setG((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(barcolor, "g").c_str()));
			widget->barColor.setB((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(barcolor, "b").c_str()));
			widget->barColor.setA((unsigned char)atoi(CXMLParser::GetInstance()->GetTagData(barcolor, "a").c_str()));

			spec.widgetPanel.widgets.push_back(widget);
		}
		else if(type == "slider")
		{
			CGuiSliderWidget* widget = new CGuiSliderWidget;

			widget->name = CXMLParser::GetInstance()->GetTagData(widgetXML, "name");

			string area = CXMLParser::GetInstance()->GetTagData(widgetXML, "area");
			widget->area.leftx = atof(CXMLParser::GetInstance()->GetTagData(area, "lx").c_str());
			widget->area.rightx = atof(CXMLParser::GetInstance()->GetTagData(area, "rx").c_str());
			widget->area.bottomy = atof(CXMLParser::GetInstance()->GetTagData(area, "by").c_str());
			widget->area.topy = atof(CXMLParser::GetInstance()->GetTagData(area, "ty").c_str());

			widget->onModifyScript = CXMLParser::GetInstance()->GetTagData(widgetXML, "onmodify");

			spec.widgetPanel.widgets.push_back(widget);
		}		//else if...
		else if(type == "viewport")
		{
			CGuiViewportWidget* widget = new CGuiViewportWidget;

			widget->name = CXMLParser::GetInstance()->GetTagData(widgetXML, "name");

			string area = CXMLParser::GetInstance()->GetTagData(widgetXML, "area");
			widget->area.leftx = atof(CXMLParser::GetInstance()->GetTagData(area, "lx").c_str());
			widget->area.rightx = atof(CXMLParser::GetInstance()->GetTagData(area, "rx").c_str());
			widget->area.bottomy = atof(CXMLParser::GetInstance()->GetTagData(area, "by").c_str());
			widget->area.topy = atof(CXMLParser::GetInstance()->GetTagData(area, "ty").c_str());

			spec.widgetPanel.widgets.push_back(widget);
		}		//else if...


		if(spec.widgetPanel.widgets.size() == 1)
		{
			spec.widgetPanel.widgets[0]->onGetFocus();
		}
	}
	while(widgetXML != "");

/*
<window>

<area>
	<lx>100</lx>
	<rx>300</rx>
	<by>100</by>
	<ty>300</ty>
</area>
<color>
	<r>255</r>
	<g>255</g>
	<b>255</b>
	<a>255</a>
</color>

<caption>Read from File</caption>

<widgets>
</widgets>

</window>*/

	windowSpecifications[specID] = spec;

	return specID;
}

GuiWindowID CGuiWM::MakeWindowInstance(GuiWindowSpecID windowSpecID) // create an instance of a window class, return a GuiWindowID
{
	map<GuiWindowSpecID, CGuiWindowWidget>::const_iterator windowSpec = windowSpecifications.find(windowSpecID);

	if(windowSpec == windowSpecifications.end())
	{ // not found
		return -1; // no specification was found
	}

	GuiWindowID instanceID = getNextWindowID();

	windows[instanceID] = windowSpec->second;
	windows[instanceID].windowID = instanceID;

	Gui.Context.AddWindow(instanceID);

	return instanceID;
}

void CGuiWM::DeleteWindowInstance(GuiWindowID windowID) //removewindow(GuiWindowID) <- destroy a window instance... should notify the CGuiContext
{
	map<GuiWindowID, CGuiWindowWidget>::iterator window = windows.find(windowID);
	if(window == windows.end())
	{ // window wasn't found, return now
		return;
	}

	windows.erase(window);

	Gui.Context.RemoveWindowReference(windowID);
}
void CGuiWM::QueueDeleteWindowInstance(GuiWindowID windowID)
{
	deletionStack.push(windowID);
}
void CGuiWM::Think()
{
	while(!deletionStack.empty())
	{
		ccout << "Deleting window...\n";
		DeleteWindowInstance(deletionStack.top());
		deletionStack.pop();
	}
}

CGuiWindowWidget* CGuiWM::GetWindowInstance(GuiWindowID windowID)
{
	map<GuiWindowID, CGuiWindowWidget>::iterator window = windows.find(windowID);
	if(window != windows.end())
	{ 
		CGuiWindowWidget* result = dynamic_cast<CGuiWindowWidget*>(&(window->second));
		return result;
	}
	return 0;
}

GuiWindowID CGuiWM::GetWindowID(CGuiWindowWidget* window)
{
	for(map<GuiWindowID, CGuiWindowWidget>::iterator it = windows.begin(); it != windows.end(); it++)
	{
		if(&(it->second) == window)
			return it->first;
	}
	return -1;
}

bool CGuiWM::WindowIDIsValid(GuiWindowID windowID)
{
	if(windowID != -1)
		return true;
	else
		return false;
}


GuiWindowID CGuiWM::getNextWindowID()
{
	return guiWindowIDCounter++;
}
GuiWindowSpecID CGuiWM::getNextWindowSpecID()
{
	return guiWindowSpecIDCounter++;
}
