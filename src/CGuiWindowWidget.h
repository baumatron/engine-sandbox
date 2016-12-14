#ifndef CGUIWINDOWWIDGET_H
#define CGUIWINDOWWIDGET_H

#include "IGuiWidget.h"
#include "ITrackInHud.h"
#include "CGuiWidgetPanel.h"
//#include "CGuiWM.h"
#include "math_main.h"
#include "CColor.h"

typedef long GuiWindowID; // invalid window ids are negative
typedef long GuiWindowSpecID; // invalid specification ids are negative

#include <vector>
using namespace std;

class CGuiWindowWidget: public IGuiWidget
{
public:
	CGuiWindowWidget();
	CGuiWindowWidget(const CGuiWindowWidget& rhs);
	CGuiWindowWidget(CColor newcolor, string newcaption, CRectangle newarea);
	virtual ~CGuiWindowWidget();


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

	void StartTracking(ITrackInHud &obj);
	void StopTracking(void);

	void SetUpperLeftCorner(v3d upperLeft);

	CGuiWindowWidget operator=(const CGuiWindowWidget& rhs);

	void copyFromOther(const CGuiWindowWidget& rhs);
	
//	CRectangle area;
	CColor color;
	string caption;

	bool dragging;
	bool dragable;
	bool visible;

	CGuiWidgetPanel widgetPanel;

	GuiWindowID windowID;
	ITrackInHud *m_trackedObject;
};

#endif