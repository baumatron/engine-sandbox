#ifndef CGUIWIDGETPANEL_H
#define CGUIWIDGETPANEL_H

#include "IGuiWidget.h"
#include "CGuiFieldWidget.h"

class CGuiWidgetPanel
{
public:
	CGuiWidgetPanel();
	CGuiWidgetPanel(const CGuiWidgetPanel& rhs);
	virtual ~CGuiWidgetPanel();

	// IGuiWidget
	virtual bool onMouseMove(v3d mouseDelta);
	virtual bool onMouseDown(v3d mousePosition);
	virtual bool onMouseUp(v3d mousePosition);
	virtual bool onMouseEnter(v3d mousePosition);
	virtual bool onMouseLeave(v3d mousePosition);
	virtual bool onCharacter(CInputEvent inputEvent);

	virtual void onGetFocus();
	virtual void onLoseFocus();

	virtual void Move(v3d delta);
	virtual void Resize(CRectangle delta);

	virtual void Render();

	virtual void PythonScriptBegin();
	virtual void PythonScriptEnd();
	// end IGuiWidget

	IGuiWidget* GetWidgetByName(string name);
	CGuiFieldWidget& GetFieldWidget(string name) { return *(dynamic_cast<CGuiFieldWidget*>(GetWidgetByName(name)));}

	void IncrementFocus();
	void DecrementFocus();
	void SetFocus(int widgetNumber);
	IGuiWidget* getFocusedWidget();

	void copyFromOther(const CGuiWidgetPanel& rhs);

	CGuiWidgetPanel operator=(const CGuiWidgetPanel& rhs);

	CRectangle area; // area in which widgets will be drawn
	vector<IGuiWidget *> widgets;
	unsigned int focusIndex; // index to the widget that has focus
};


#endif