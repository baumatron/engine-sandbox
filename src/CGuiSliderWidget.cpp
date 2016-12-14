#include "CGuiSliderWidget.h"
#include "math_main.h"
#include "CVideoSubsystem.h"
#include "CPythonSubsystem.h"
#include <boost/python.hpp>
using namespace boost::python;

CGuiSliderWidget::CGuiSliderWidget()
{
	area = CRectangle(0, 100, 0, 10);
	percent = 0.76f;
	dragging = false;
}
CGuiSliderWidget::CGuiSliderWidget(const CGuiSliderWidget& rhs)
{
	name = rhs.name;
	area = rhs.area;
	percent = rhs.percent;
	dragging = rhs.dragging;
	onModifyScript = rhs.onModifyScript;
}

CGuiSliderWidget::~CGuiSliderWidget()
{
}

// IGuiWidget
bool CGuiSliderWidget::onMouseMove(v3d mouseDelta)
{
	if(dragging)
	{
		//float x = mousePosition.x - area.leftx;
		percent += mouseDelta.x/(area.rightx - area.leftx);
		this->onModify();
	}
	return true;
}
bool CGuiSliderWidget::onMouseDown(v3d mousePosition)
{
	// first determine the local x location
	float x = mousePosition.x - area.leftx;
	percent = x/(area.rightx - area.leftx);
	dragging = true;
	this->onModify();
	return true;
}
bool CGuiSliderWidget::onMouseUp(v3d mousePosition)
{
	dragging = false;
	return true;
}
bool CGuiSliderWidget::onMouseEnter(v3d mousePosition)
{
	return false;
}
bool CGuiSliderWidget::onMouseLeave(v3d mousePosition)
{
	dragging = false;
	return false;
}
bool CGuiSliderWidget::onKey(CInputEvent inputEvent)
{
	if(inputEvent.inputEventType == IET_CHARACTERTYPED)
	{
		if(inputEvent.data.characterTypeEvent.characterCode == IKC_RIGHT)
		{
			percent += 0.01f;
			if(percent > 1.0f)
				percent = 1.0f;
			this->onModify();
			return true;
		}
		else if(inputEvent.data.characterTypeEvent.characterCode == IKC_LEFT)
		{
			percent -= 0.01f;
			if(percent < 0.0f)
				percent = 0.0f;
			this->onModify();
			return true;
		}
	}
	return false;
}

bool CGuiSliderWidget::takesFocus()
{
	return false;
}
void CGuiSliderWidget::onGetFocus()
{
}
void CGuiSliderWidget::onLoseFocus()
{
}

void CGuiSliderWidget::Move(v3d delta)
{
	area = area + delta;
}
void CGuiSliderWidget::Resize(CRectangle delta)
{
	area.leftx += delta.leftx;
	area.rightx += delta.rightx;
	area.bottomy += delta.bottomy;
	area.topy += delta.topy;
}

void CGuiSliderWidget::Render()
{
	Video.DrawSlider(area, percent);
//	Video.DrawProgressBar(area, progress, barColor, backgroundColor);
}

void CGuiSliderWidget::PythonScriptBegin()
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	main_namespace["percent"] = this->percent;
}
void CGuiSliderWidget::PythonScriptEnd()
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	this->percent = extract<float>(main_namespace["percent"]);
	if(this->percent < 0.0f)
		this->percent = 0.0f;
	if(this->percent > 1.0f)
		this->percent = 1.0f;
	main_namespace["percent"].del();
}

void CGuiSliderWidget::onModify()
{
	this->PythonScriptBegin();
	CON_Exec(onModifyScript);
	this->PythonScriptEnd();
}

// end IGuiWidget

