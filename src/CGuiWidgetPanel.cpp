#include "CGuiWidgetPanel.h"
#include "CGuiButtonWidget.h"
#include "CGuiFieldWidget.h"
#include "CGuiProgressBarWidget.h"
#include "CGuiSliderWidget.h"
#include "CGuiViewportWidget.h"
#include "con_main.h"
#include "CPythonSubsystem.h"
#include <boost/python.hpp>
using namespace boost::python;

#include <typeinfo>

CGuiWidgetPanel::CGuiWidgetPanel()
{
	area = CRectangle(0, 100, 0, 100);
	focusIndex = 0;
}

CGuiWidgetPanel::CGuiWidgetPanel(const CGuiWidgetPanel& rhs)
{
	copyFromOther(rhs);
}


CGuiWidgetPanel::~CGuiWidgetPanel()
{
	for(vector<IGuiWidget*>::iterator it = widgets.begin(); it != widgets.end(); it++)
	{
		delete (*it);
	}
}


bool CGuiWidgetPanel::onMouseMove(v3d mouseDelta)
{
	return false;
}

bool CGuiWidgetPanel::onMouseDown(v3d mousePosition)
{
	return false;
}

bool CGuiWidgetPanel::onMouseUp(v3d mousePosition)
{
	return false;
}

bool CGuiWidgetPanel::onMouseEnter(v3d mousePosition)
{
	return false;
}

bool CGuiWidgetPanel::onMouseLeave(v3d mousePosition)
{
	return false;
}

bool CGuiWidgetPanel::onCharacter(CInputEvent inputEvent)
{
	return false;
}

void CGuiWidgetPanel::onGetFocus()
{
}

void CGuiWidgetPanel::Move(v3d delta)
{
	area = area + delta;
	for(vector<IGuiWidget*>::iterator it = widgets.begin(); it != widgets.end(); it++)
	{
		(*it)->Move(delta);
	}
}

void CGuiWidgetPanel::Resize(CRectangle delta)
{
	area.leftx += delta.leftx;
	area.rightx += delta.rightx;
	area.bottomy += delta.bottomy;
	area.topy += delta.topy;

	//(*it)->Move(delta);
	for(vector<IGuiWidget*>::iterator it = widgets.begin(); it != widgets.end(); it++)
	{
		(*it)->area.leftx += delta.leftx;
		(*it)->area.topy += delta.topy;

		if((*it)->area.rightx > area.rightx)
			(*it)->area.rightx += area.rightx - (*it)->area.rightx;
		if((*it)->area.bottomy < area.bottomy)
			(*it)->area.bottomy -= area.bottomy - (*it)->area.bottomy;

	}
}

void CGuiWidgetPanel::onLoseFocus()
{
}

void CGuiWidgetPanel::Render()
{
	for(int i = 0; i < widgets.size(); i++)
	{
		IGuiWidget* pointer = widgets[i];
		widgets[i]->Render();
	}
/*	for(vector<IGuiWidget*>::iterator it = widgets.begin(); it != widgets.end(); it++)
	{
		(*it)->Render();
	}*/
}

void CGuiWidgetPanel::PythonScriptBegin()
{
//	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
//	object main_namespace = main_module.attr("__dict__");
//	main_namespace["this"] = Gui.WM.GetWindowID();
}
void CGuiWidgetPanel::PythonScriptEnd()
{
//	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
//	object main_namespace = main_module.attr("__dict__");
//	this->name = main_namespace["name"];
//	main_namespace["name"].del();
}
IGuiWidget* CGuiWidgetPanel::GetWidgetByName(string name)
{
	for(vector<IGuiWidget*>::iterator it = widgets.begin(); it != widgets.end(); it++)
	{
		if((*it)->name == name)
			return *it;
	}
	return 0;
}

void CGuiWidgetPanel::IncrementFocus()
{
	int oldIndex = focusIndex++;
	if(focusIndex >= widgets.size())
		focusIndex = 0;

	if(oldIndex != focusIndex)
	{
		widgets[oldIndex]->onLoseFocus();
		widgets[focusIndex]->onGetFocus();
	}
}

void CGuiWidgetPanel::DecrementFocus()
{
	int oldIndex = focusIndex--;
	if(focusIndex <= 0)
		focusIndex = widgets.size()-1;

	if(oldIndex != focusIndex)
	{
		widgets[oldIndex]->onLoseFocus();
		widgets[focusIndex]->onGetFocus();
	}
}

void CGuiWidgetPanel::SetFocus(int widgetNumber)
{
	if(widgetNumber >= widgets.size())
		widgetNumber = 0;

	int oldIndex = focusIndex;
	focusIndex = widgetNumber;

	if(oldIndex != focusIndex)
	{
		widgets[oldIndex]->onLoseFocus();
		widgets[focusIndex]->onGetFocus();
	}
}

IGuiWidget* CGuiWidgetPanel::getFocusedWidget()
{
	return widgets[focusIndex];
}

CGuiWidgetPanel CGuiWidgetPanel::operator=(const CGuiWidgetPanel& rhs)
{
	copyFromOther(rhs);
	return *this;
}

void CGuiWidgetPanel::copyFromOther(const CGuiWidgetPanel& rhs)
{
	area = rhs.area;

	focusIndex = rhs.focusIndex;
	while(widgets.size())
	{
		delete widgets.back();
		widgets.pop_back();
	}
	for(int i = 0; i < rhs.widgets.size(); i++)
	{
		if(dynamic_cast<CGuiButtonWidget*>(rhs.widgets[i]))
		{
			// it's a button widget
			CGuiButtonWidget* rhsButtonWidget = static_cast<CGuiButtonWidget*>(rhs.widgets[i]);
			CGuiButtonWidget* newButtonWidget = new CGuiButtonWidget(*rhsButtonWidget);
			widgets.push_back(newButtonWidget);
		}
		else if(dynamic_cast<CGuiFieldWidget*>(rhs.widgets[i]))
		{
			// it's a button widget
			CGuiFieldWidget* rhsFieldWidget = static_cast<CGuiFieldWidget*>(rhs.widgets[i]);
			CGuiFieldWidget* newFieldWidget = new CGuiFieldWidget(*rhsFieldWidget);
			widgets.push_back(newFieldWidget);
		}
		else if(dynamic_cast<CGuiProgressBarWidget*>(rhs.widgets[i]))
		{
			// it's a button widget
			CGuiProgressBarWidget* rhsBarWidget = static_cast<CGuiProgressBarWidget*>(rhs.widgets[i]);
			CGuiProgressBarWidget* newBarWidget = new CGuiProgressBarWidget(*rhsBarWidget);
			widgets.push_back(newBarWidget);
		}
		else if(dynamic_cast<CGuiSliderWidget*>(rhs.widgets[i]))
		{
			// it's a button widget
			CGuiSliderWidget* rhsSliderWidget = static_cast<CGuiSliderWidget*>(rhs.widgets[i]);
			CGuiSliderWidget* newSliderWidget = new CGuiSliderWidget(*rhsSliderWidget);
			widgets.push_back(newSliderWidget);
		}	
		else if(dynamic_cast<CGuiViewportWidget*>(rhs.widgets[i]))
		{
			// it's a viewport widget
			CGuiViewportWidget* rhsWidget = static_cast<CGuiViewportWidget*>(rhs.widgets[i]);
			CGuiViewportWidget* newWidget = new CGuiViewportWidget(*rhsWidget);
			widgets.push_back(newWidget);
		}
	}
}

