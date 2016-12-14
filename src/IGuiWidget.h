#ifndef IGUIWIDGET_H
#define IGUIWIDGET_H

/*
	For each IGuiWidget class to work, there must be code specific to each class
	in the following places:

		GuiWindowSpecID CGuiWM::ReadWindowSpec(string filename);
		void CGuiWidgetPanel::copyFromOther(const CGuiWidgetPanel& rhs);

*/


#include "CInputSubsystem.h"
#include "math_main.h"
#include <string>
using namespace std;

class IGuiWidget
{
public:
	IGuiWidget(){}
	IGuiWidget(const IGuiWidget& rhs){area = rhs.area; name = rhs.name;}
	virtual ~IGuiWidget(){}

	virtual bool onMouseMove(v3d mouseDelta) = 0;
	virtual bool onMouseDown(v3d mousePosition) = 0;
	virtual bool onMouseUp(v3d mousePosition) = 0;
	virtual bool onMouseEnter(v3d mousePosition) = 0;
	virtual bool onMouseLeave(v3d mousePosition) = 0;
	virtual bool onKey(CInputEvent inputEvent) = 0;

	virtual bool takesFocus() = 0;
	virtual void onGetFocus() = 0;
	virtual void onLoseFocus() = 0;

	virtual void Move(v3d delta) = 0;
	virtual void Resize(CRectangle delta) = 0;

	virtual void Render() = 0;

	virtual void PythonScriptBegin() = 0;
	virtual void PythonScriptEnd() = 0;

	CRectangle area;
	string name; // used to find the widget you need when modifying and reading values from them
};

#endif