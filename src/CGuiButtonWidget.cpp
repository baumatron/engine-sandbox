#include "CGuiButtonWidget.h"
#include "CEventRouter.h"
#include "CVideoSubsystem.h"
#include "CPythonSubsystem.h"
#include <boost/python.hpp>
using namespace boost::python;


CGuiButtonWidget::CGuiButtonWidget()
{
	area = CRectangle(0,100,0,20);
	caption = "a button";
	color = CColor(0.0f, 0.3f, 0.65f);
	colorModifier = CColor(0.0f, 0.0f, 0.0f, 0.0f);
	buttonState = up;
	hasFocus = false;
	script = "print \"Button has no script\"";
}

CGuiButtonWidget::CGuiButtonWidget(const CGuiButtonWidget& rhs)
{
	name = rhs.name;
	area = rhs.area;
	caption = rhs.caption;
	color = rhs.color;
	colorModifier = rhs.colorModifier;
	buttonState = rhs.buttonState;
//	buttonRouterEvent = rhs.buttonRouterEvent;
	hasFocus = rhs.hasFocus;
	script = rhs.script;
}

CGuiButtonWidget::~CGuiButtonWidget()
{

}

bool CGuiButtonWidget::onMouseMove(v3d mouseDelta)
{
	return false;
}
bool CGuiButtonWidget::onMouseDown(v3d mousePosition)
{
	buttonState = down;
	return true;
}
bool CGuiButtonWidget::onMouseUp(v3d mousePosition)
{
	if(buttonState == down)
	{
		this->PythonScriptBegin();
		CON_Exec(script);//EventRouter.RouteEvent(buttonRouterEvent); // execute button event here
		this->PythonScriptEnd();
	}
	buttonState = up;
	return true;
}
bool CGuiButtonWidget::onMouseEnter(v3d mousePosition)
{
	colorModifier = CColor(0.1f, 0.1f, 0.1f, 0.0f);
	return true;
}
bool CGuiButtonWidget::onMouseLeave(v3d mousePosition)
{
	colorModifier = CColor(0.0f, 0.0f, 0.0f, 0.0f);
	buttonState = up;
	return true;
}
bool CGuiButtonWidget::onKey(CInputEvent inputEvent)
{
	if(inputEvent.inputEventType == IET_CHARACTERTYPED)
		if(inputEvent.data.characterTypeEvent.characterCode == IKC_RETURN)
		{
			onMouseDown(v3d(area.leftx, area.bottomy));
			onMouseUp(v3d(area.leftx, area.bottomy));
		}
	return true;
}

bool CGuiButtonWidget::takesFocus()
{
	return true;
}

void CGuiButtonWidget::onGetFocus()
{
	hasFocus = true;
}
void CGuiButtonWidget::onLoseFocus()
{
	hasFocus = false;
}

void CGuiButtonWidget::Move(v3d delta)
{
	area = area + delta;
}

void CGuiButtonWidget::Resize(CRectangle delta)
{
	area.leftx += delta.leftx;
	area.rightx += delta.rightx;
	area.bottomy += delta.bottomy;
	area.topy += delta.topy;
}

void CGuiButtonWidget::Render()
{
/*	if(hasFocus)
		Video.DrawButton(area, color+colorModifier-CColor(0.1f, 0.1f, 0.1f, 0.0f), caption, (buttonState == down) ? true : false);
	else
		Video.DrawButton(area, color+colorModifier, caption, (buttonState == down) ? true : false);*/


	CColor backgroundColor;
	if(buttonState == down) 
	{
		backgroundColor = CColor(0.2f, 0.2f, 0.2f, 0.75f) + colorModifier;
	}
	else
	{
		backgroundColor = CColor(0.4f, 0.4f, 0.4f, 0.75f) + colorModifier;
	}
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

	// border
	v3d	start = area.getLowerLeft();
	v3d	end = area.getLowerRight();
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

	float xcenter = area.leftx + (area.rightx-area.leftx)/2;
	float captionWidth = Video.vcout.getAdvance(caption);

	float ycenter = area.bottomy + (area.topy-area.bottomy)/2 + Video.vcout.getHeight()/2 - Video.vcout.getAscender();
	float ascDescDifference = -Video.vcout.getAscender() - Video.vcout.getDescender();

	Video.vcout.setPos(v3d( xcenter - captionWidth/2, ycenter));
	Video.vcout.setColor(CColor(0.75f, 0.75f, 0.75f));
	Video.vcout.setBounds(area);
	Video.vcout.clipToBounds = true;
	Video.vcout << caption;
	Video.vcout.clipToBounds = false;

	glPopAttrib();
	Video.PopProjection();

}

void CGuiButtonWidget::PythonScriptBegin()
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	main_namespace["name"] = this->name;
}

void CGuiButtonWidget::PythonScriptEnd()
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	this->name = extract<string>(main_namespace["name"]);
	main_namespace["name"].del();
}
