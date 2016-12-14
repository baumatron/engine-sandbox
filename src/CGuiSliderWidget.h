#ifndef CGUISLIDERWIDGET_H
#define CGUISLIDERWIDGET_H

#include "IGuiWidget.h"
#include "CColor.h"

class CGuiSliderWidget: public IGuiWidget
{
public:
	CGuiSliderWidget();
	CGuiSliderWidget(const CGuiSliderWidget& rhs);
	~CGuiSliderWidget();

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

	string onModifyScript; // python script

	bool dragging;
	float percent; // from 0.0-1.0
private:
	void onModify();
};

#endif