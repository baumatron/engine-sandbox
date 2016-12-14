#ifndef CGUIPROGRESSBARWIDGET_H
#define CGUIPROGRESSBARWIDGET_H

#include "IGuiWidget.h"
#include "CColor.h"

class CGuiProgressBarWidget: public IGuiWidget
{
public:
	CGuiProgressBarWidget();
	CGuiProgressBarWidget(const CGuiProgressBarWidget& rhs);
	~CGuiProgressBarWidget();

	// IGuiWidget
	virtual bool onMouseMove(v3d mouseDelta);
	virtual bool onMouseDown(v3d mousePosition);
	virtual bool onMouseUp(v3d mousePosition);
	virtual bool onMouseEnter(v3d mousePosition);
	virtual bool onMouseLeave(v3d mousePosition);
	virtual bool onKey(CInputEvent inputEvent);

	virtual bool takesFocus();
	virtual void onGetFocus();
	virtual void onLoseFocus();

	virtual void Move(v3d delta);
	virtual void Resize(CRectangle delta);

	virtual void Render();

	virtual void PythonScriptBegin();
	virtual void PythonScriptEnd();
	// end IGuiWidget

	float progress;
	CColor barColor; 
	CColor backgroundColor;
};

#endif