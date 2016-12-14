#ifndef CGUIFIELDWIDGET_H
#define CGUIFIELDWIDGET_H

#include "IGuiWidget.h"
#include "CRouterEvent.h"
#include "math_main.h"
#include "CColor.h"

class CGuiFieldWidget: public IGuiWidget
{
public:
	enum ButtonStates {up, down};

	CGuiFieldWidget();
	CGuiFieldWidget(const CGuiFieldWidget& rhs);
	virtual ~CGuiFieldWidget();

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

	CGuiFieldWidget operator=(const CGuiFieldWidget& rhs);
	CGuiFieldWidget operator<<(const string text);
	void Print(const string text);
	void Clear();

	void copyFromOther(const CGuiFieldWidget& rhs);

	bool canEdit;
	bool horizontalScroll;
	bool verticalScroll;

	// rendering properties
//	CRectangle area;
	CColor backgroundColor;
	CColor textColor;
	string contents;
	int selectionStart;
	int selectionEnd;
	int cursorPosition;
	bool isSelecting;
	bool hasFocus;
};

#endif