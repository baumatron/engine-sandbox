#ifndef CGUIBUTTONWIDGET_H
#define CGUIBUTTONWIDGET_H

#include "IGuiWidget.h"
#include "CRouterEvent.h"
#include "math_main.h"
#include "CColor.h"

class CGuiButtonWidget: public IGuiWidget
{
public:
	enum ButtonStates {up, down};

	CGuiButtonWidget();
	CGuiButtonWidget(const CGuiButtonWidget& rhs);
	virtual ~CGuiButtonWidget();

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

	//CRouterEvent buttonRouterEvent;
	string script;

	// rendering properties
//	CRectangle area;
	CColor color;
	CColor colorModifier;
	string caption;
	ButtonStates buttonState;
	bool hasFocus;
};

#endif