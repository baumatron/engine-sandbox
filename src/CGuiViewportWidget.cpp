
#include "CGuiViewportWidget.h"
#include "CVideoSubsystem.h"
#include "CPythonSubsystem.h"
#include <boost/python.hpp>
using namespace boost::python;

CGuiViewportWidget::CGuiViewportWidget()
{
	render = 0;
	area = CRectangle(0, 100, 0, 100);
	name = "";
}
CGuiViewportWidget::CGuiViewportWidget(const CGuiViewportWidget& rhs)
{
	render = rhs.render;
	area = rhs.area;
	name = rhs.name;
}
CGuiViewportWidget::~CGuiViewportWidget()
{
}

// IGuiWidget
bool CGuiViewportWidget::onMouseMove(v3d mouseDelta)
{
	return false;
}
bool CGuiViewportWidget::onMouseDown(v3d mousePosition)
{
	return false;
}
bool CGuiViewportWidget::onMouseUp(v3d mousePosition)
{
	return false;
}
bool CGuiViewportWidget::onMouseEnter(v3d mousePosition)
{
	return false;
}
bool CGuiViewportWidget::onMouseLeave(v3d mousePosition)
{
	return false;
}
bool CGuiViewportWidget::onKey(CInputEvent inputEvent)
{
	return false;
}

bool CGuiViewportWidget::takesFocus()
{
	return true;
}
void CGuiViewportWidget::onGetFocus()
{
}
void CGuiViewportWidget::onLoseFocus()
{
}

void CGuiViewportWidget::Move(v3d delta)
{
	area = area + delta;
}

void CGuiViewportWidget::Resize(CRectangle delta)
{
	
}

void CGuiViewportWidget::Render()
{
	CColor brightColor = CColor(128, 128, 128, 128);//backgroundColor + CColor(0.2f, 0.2f, 0.2f, 0.0f);

	Video.PushProjection2d();
	
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable(GL_TEXTURE_2D);

	// border
	CRectangle border = area + CRectangle(0, 1, -1, 0);
	v3d	start = area.getLowerLeft();
	v3d	end = border.getLowerRight();
	glBegin(GL_LINES);
		glColor3f( brightColor.getRFloat(), brightColor.getGFloat(), brightColor.getBFloat());
		glVertex3f(start.x,start.y,start.z); 
		glVertex3f(end.x,end.y,end.z); 

		start = border.getUpperLeft();
		end = border.getUpperRight();
		glColor3f( brightColor.getRFloat(), brightColor.getGFloat(), brightColor.getBFloat());
		glVertex3f(start.x,start.y,start.z); 
		glVertex3f(end.x,end.y,end.z); 

		start = border.getLowerLeft();
		end = border.getUpperLeft();
		glColor3f( brightColor.getRFloat(), brightColor.getGFloat(), brightColor.getBFloat());
		glVertex3f(start.x,start.y,start.z); 
		glVertex3f(end.x,end.y,end.z); 

		start = border.getLowerRight();
		end = border.getUpperRight();
		glColor3f( brightColor.getRFloat(), brightColor.getGFloat(), brightColor.getBFloat());
		glVertex3f(start.x,start.y,start.z); 
		glVertex3f(end.x,end.y,end.z); 

		glColor3f( 1.0, 1.0, 1.0 );
	glEnd();	

	glPopAttrib();
	Video.PopProjection();



	CViewportContext context;
	context.area = area;
	context.viewportContextType = CViewportContext::independent;
	context.virtualPixelHeight = area.topy - area.bottomy;
	context.virtualPixelWidth = area.rightx - area.leftx;
	Video.PushViewportContext(context);

	Video.DrawRectangle(v3d(0,0), v3d(context.virtualPixelWidth,context.virtualPixelHeight), CColor(0,0,0));

	if(render)
		render();

	Video.PopViewportContext();
}
void CGuiViewportWidget::PythonScriptBegin()
{
//	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
//	object main_namespace = main_module.attr("__dict__");
//	main_namespace["this"] = Gui.WM.GetWindowID();
}
void CGuiViewportWidget::PythonScriptEnd()
{
//	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
//	object main_namespace = main_module.attr("__dict__");
//	this->name = main_namespace["name"];
//	main_namespace["name"].del();
}