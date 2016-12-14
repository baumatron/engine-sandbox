#include "CGuiProgressBarWidget.h"
#include "math_main.h"
#include "CVideoSubsystem.h"
#include "CPythonSubsystem.h"
#include <boost/python.hpp>
using namespace boost::python;

CGuiProgressBarWidget::CGuiProgressBarWidget()
{
	area = CRectangle(0, 100, 0, 10);
	barColor = CColor(0.0f, 0.5f, 0.0f);
	backgroundColor = CColor(0.0f, 0.0f, 0.2f);
	progress = 0.76f;
}
CGuiProgressBarWidget::CGuiProgressBarWidget(const CGuiProgressBarWidget& rhs)
{
	name = rhs.name;
	area = rhs.area;
	barColor = rhs.barColor;
	backgroundColor = rhs.backgroundColor;
	progress = rhs.progress;
}

CGuiProgressBarWidget::~CGuiProgressBarWidget()
{
}

// IGuiWidget
bool CGuiProgressBarWidget::onMouseMove(v3d mouseDelta)
{
	return false;
}
bool CGuiProgressBarWidget::onMouseDown(v3d mousePosition)
{
	return false;
}
bool CGuiProgressBarWidget::onMouseUp(v3d mousePosition)
{
	return false;
}
bool CGuiProgressBarWidget::onMouseEnter(v3d mousePosition)
{
	return false;
}
bool CGuiProgressBarWidget::onMouseLeave(v3d mousePosition)
{
	return false;
}
bool CGuiProgressBarWidget::onKey(CInputEvent inputEvent)
{
	return false;
}

bool CGuiProgressBarWidget::takesFocus()
{
	return false;
}
void CGuiProgressBarWidget::onGetFocus()
{
}
void CGuiProgressBarWidget::onLoseFocus()
{
}

void CGuiProgressBarWidget::Move(v3d delta)
{
	area = area + delta;
}
void CGuiProgressBarWidget::Resize(CRectangle delta)
{
	area.leftx += delta.leftx;
	area.rightx += delta.rightx;
	area.bottomy += delta.bottomy;
	area.topy += delta.topy;
}

void CGuiProgressBarWidget::Render()
{
	CRectangle barArea;
	barArea = area;
	//barArea.leftx += 1;
	//barArea.topy -= 1;
	//barArea.bottomy += 1;
	barArea.rightx = area.leftx + 2 + (area.rightx-area.leftx-4)*progress;

	CColor backgroundColor = CColor(0.2f, 0.2f, 0.2f, 0.75f);
	CColor barColor = CColor(0.4f, 0.4f, 0.4f, 0.75f);
	CColor brightColor = CColor(128, 128, 128, 128);//backgroundColor + CColor(0.2f, 0.2f, 0.2f, 0.0f);

	Video.PushProjection2d();
	
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable(GL_TEXTURE_2D);

	// background
	v3d p1 = area.getLowerLeft();
	v3d p2 = area.getUpperRight();
	glBegin(GL_QUADS);		
		glColor4f( backgroundColor.getRFloat(), backgroundColor.getGFloat(), backgroundColor.getBFloat(), backgroundColor.getAFloat() );
		glVertex2f(p1.x,p1.y); //0,0

		glVertex2f(p1.x,p2.y); // 0,1

		glVertex2f(p2.x,p2.y);		// 1,1

		glVertex2f(p2.x,p1.y);	// 1,0
		glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();

	// progress bar
	p1 = barArea.getLowerLeft();
	p2 = barArea.getUpperRight();
	glBegin(GL_QUADS);		
		glColor4f( barColor.getRFloat(), barColor.getGFloat(), barColor.getBFloat(), barColor.getAFloat() );
		glVertex2f(p1.x,p1.y); //0,0

		glVertex2f(p1.x,p2.y); // 0,1

		glVertex2f(p2.x,p2.y);		// 1,1

		glVertex2f(p2.x,p1.y);	// 1,0
		glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();

	v3d start = barArea.getLowerRight();
	v3d end = barArea.getUpperRight();
	glBegin(GL_LINES);
		glColor3f( brightColor.getRFloat(), brightColor.getGFloat(), brightColor.getBFloat());
		glVertex3f(start.x,start.y,start.z); 
		glVertex3f(end.x,end.y,end.z); 

		glColor3f( 1.0, 1.0, 1.0 );
	glEnd();	

// border
	start = area.getLowerLeft();
	end = area.getLowerRight();
	glBegin(GL_LINES);
		glColor3f( brightColor.getRFloat(), brightColor.getGFloat(), brightColor.getBFloat());
		glVertex3f(start.x,start.y,start.z); 
		glVertex3f(end.x,end.y,end.z); 

		start = area.getUpperLeft();
		end = area.getUpperRight();
		glColor3f( brightColor.getRFloat(), brightColor.getGFloat(), brightColor.getBFloat());
		glVertex3f(start.x,start.y,start.z); 
		glVertex3f(end.x,end.y,end.z); 

		start = area.getLowerLeft();
		end = area.getUpperLeft();
		glColor3f( brightColor.getRFloat(), brightColor.getGFloat(), brightColor.getBFloat());
		glVertex3f(start.x,start.y,start.z); 
		glVertex3f(end.x,end.y,end.z); 

		start = area.getLowerRight();
		end = area.getUpperRight();
		glColor3f( brightColor.getRFloat(), brightColor.getGFloat(), brightColor.getBFloat());
		glVertex3f(start.x,start.y,start.z); 
		glVertex3f(end.x,end.y,end.z); 

		glColor3f( 1.0, 1.0, 1.0 );
	glEnd();	

	string temp = M_ftoa(progress*100) + "%";
	Video.vcout.setPos(v3d( area.leftx + (area.rightx-area.leftx)/2, area.bottomy + (area.topy-area.bottomy)/2 - 4));
	Video.vcout.setColor(CColor(1.0f, 1.0f, 1.0f, 1.0f));
	Video.vcout.setBounds(area);
	Video.vcout.clipToBounds = true;
	Video.vcout << temp;
	Video.vcout.clipToBounds = false;

	glPopAttrib();
	Video.PopProjection();
}

void CGuiProgressBarWidget::PythonScriptBegin()
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	main_namespace["progress"] = this->progress;
}
void CGuiProgressBarWidget::PythonScriptEnd()
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	this->progress = extract<float>(main_namespace["progress"]);
	main_namespace["progress"].del();
}
// end IGuiWidget

