
#include "CGuiSubsystem.h"
#include "CGuiWindowWidget.h"
#include "CGuiButtonWidget.h"
#include "CToken.h"
#include "CVideoSubsystem.h"
#include "CPythonSubsystem.h"
#include <boost/python.hpp>
using namespace boost::python;

CGuiWindowWidget::CGuiWindowWidget()
{
	color = CColor(1.0f, 1.0f, 1.0f);
	caption = "New Window";
	area = CRectangle(0,200,0,200);

/*	CGuiButtonWidget* button = new CGuiButtonWidget;
	button->area.rightx = area.rightx - 3;
	button->area.leftx = button->area.rightx - 10;
	button->area.topy = area.topy - 3;
	button->area.bottomy = button->area.topy - 10;
	button->caption = "X";
	button->buttonRouterEvent = TokenManager.BuildEventFromString("gui.DeleteWindowInstance()");
	button->buttonRouterEvent.m_arguments.m_argList.push_back( (long) Gui.WM.GetWindowID(this) );
	button->color = CColor(1.0f, 0.2f, 0.0f);
	widgetPanel.widgets.push_back(button);
	button->onGetFocus();*/
	dragging = false;
	dragable = true;
	visible = true;

	widgetPanel.area = area;
	widgetPanel.area.topy -= 10;
	m_trackedObject = 0;
}

CGuiWindowWidget::CGuiWindowWidget(CColor newcolor, string newcaption, CRectangle newarea)
{
	color = newcolor;
	caption = newcaption;
	area = newarea;

	/*CGuiButtonWidget* button = new CGuiButtonWidget;
	button->area.rightx = area.rightx - 2;
	button->area.leftx = button->area.rightx - 10;
	button->area.topy = area.topy - 2;
	button->area.bottomy = button->area.topy - 10;
	button->caption = "X";
	button->buttonRouterEvent = TokenManager.BuildEventFromString("gui.DeleteWindowInstance()");
	// IMPORTANT: This requires that the window have an ID already before it is instantiated
	button->buttonRouterEvent.m_arguments.m_argList.push_back( (long) Gui.WM.GetWindowID(this) );
	button->color = CColor(1.0f, 0.2f, 0.0f);
	widgetPanel.widgets.push_back(button);
	button->onGetFocus();*/
	dragging = false;
	dragable = true;
	visible = true;

	widgetPanel.area = area;
	widgetPanel.area.topy -= 10;
	m_trackedObject = 0;
}


CGuiWindowWidget::CGuiWindowWidget(const CGuiWindowWidget& rhs)
{
	copyFromOther(rhs);
}

CGuiWindowWidget::~CGuiWindowWidget()
{
}


bool CGuiWindowWidget::onMouseMove(v3d mouseDelta)
{
	if(dragging)
		Move(mouseDelta);
	return false;
}

bool CGuiWindowWidget::onMouseDown(v3d mousePosition)
{
	bool used = false;
	for(int i = 0; i < widgetPanel.widgets.size(); i++)	
		if( (widgetPanel.widgets[i]->area.leftx < mousePosition.x && widgetPanel.widgets[i]->area.rightx > mousePosition.x)
									&&
			(widgetPanel.widgets[i]->area.bottomy < mousePosition.y && widgetPanel.widgets[i]->area.topy > mousePosition.y) )
	{
		if(widgetPanel.widgets[i]->onMouseDown(mousePosition))
			used = true;
		widgetPanel.SetFocus(i);
	}
	if(!used)
		if(dragable)
			dragging = true;
	return true;
}

bool CGuiWindowWidget::onMouseUp(v3d mousePosition)
{
	bool used = false;
	for(vector<IGuiWidget*>::iterator it = widgetPanel.widgets.begin(); it != widgetPanel.widgets.end(); it++)
	if( ((*it)->area.leftx < mousePosition.x && (*it)->area.rightx > mousePosition.x)
								&&
		((*it)->area.bottomy < mousePosition.y && (*it)->area.topy > mousePosition.y) )
	{
		if((*it)->onMouseUp(mousePosition))
			used = true;
	}
	if(dragging)
		dragging = false;
	return true;
}

bool CGuiWindowWidget::onMouseEnter(v3d mousePosition)
{
	return false;
}

bool CGuiWindowWidget::onMouseLeave(v3d mousePosition)
{
	if(dragging)
		dragging = false;
	return false;
}

bool CGuiWindowWidget::onKey(CInputEvent inputEvent)
{
	if(inputEvent.inputEventType == IET_CHARACTERTYPED)
	{
		switch(inputEvent.data.characterTypeEvent.characterCode)
		{
		case IKC_TAB: // cycle through widget focus
			{
				if( ( inputEvent.data.characterTypeEvent.modifiers & IM_LSHIFT )
					||
					( inputEvent.data.characterTypeEvent.modifiers & IM_RSHIFT ) )
				{
					widgetPanel.DecrementFocus();
					return true;
				}
				else
				{
					widgetPanel.IncrementFocus();
					return true;
				}
			}
			break;
		default:
			{
				return widgetPanel.getFocusedWidget()->onKey(inputEvent);
			}
			break;
		}
	}
	return false;
}

bool CGuiWindowWidget::takesFocus()
{
	return true;
}

void CGuiWindowWidget::onGetFocus()
{
}

void CGuiWindowWidget::onLoseFocus()
{
}

void CGuiWindowWidget::Move(v3d delta)
{
	area = area + delta;
	widgetPanel.Move(delta);
}

void CGuiWindowWidget::Resize(CRectangle delta)
{
	area.leftx += delta.leftx;
	area.rightx += delta.rightx;
	area.bottomy += delta.bottomy;
	area.topy += delta.topy;
	widgetPanel.Resize(delta);
}



void CGuiWindowWidget::Render()
{
	if(visible)
	{
		if(m_trackedObject)	
			this->SetUpperLeftCorner(this->m_trackedObject->GetHudPosition());

		bool titleBar = !(caption == "");
		string titleBarCaption = caption;
		bool gradient = false;
		CColor backgroundColor = CColor(0.4f, 0.4f, 0.4f, 0.75f);
		CColor brightColor = CColor(0.6f, 0.6f, 0.6f, 0.75f);//CColor(221, 171, 0, 128);//backgroundColor + CColor(0.2f, 0.2f, 0.2f, 0.0f);
		CColor darkColor = brightColor;//backgroundColor - CColor(0.2f, 0.2f, 0.2f, 0.0f);
		CColor titleBarColor = backgroundColor;

		Video.PushProjection2d();
		
		glPushAttrib(GL_ENABLE_BIT);

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
		glEnable(GL_BLEND);
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glDisable(GL_TEXTURE_2D);

		v3d p1 = area.getLowerLeft();
		v3d p2 = area.getUpperRight();
		glBegin(GL_QUADS);		
			glColor4f( backgroundColor.getRFloat(), backgroundColor.getGFloat(), backgroundColor.getBFloat(), backgroundColor.getAFloat() );
			glVertex2f(p1.x,p1.y); //0,0

			if(gradient)
				glColor4f( darkColor.getRFloat(), darkColor.getGFloat(), darkColor.getBFloat(), darkColor.getAFloat() );
			glVertex2f(p1.x,p2.y); // 0,1

			glVertex2f(p2.x,p2.y);		// 1,1

			if(gradient)
				glColor4f( backgroundColor.getRFloat(), backgroundColor.getGFloat(), backgroundColor.getBFloat(), backgroundColor.getAFloat() );
			glVertex2f(p2.x,p1.y);	// 1,0
			glColor3f(1.0f, 1.0f, 1.0f);
		glEnd();

		v3d start = area.getLowerLeft();
		v3d end = area.getLowerRight();
		glBegin(GL_LINES);
			glColor3f( darkColor.getRFloat(), darkColor.getGFloat(), darkColor.getBFloat());
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
			glColor3f( darkColor.getRFloat(), darkColor.getGFloat(), darkColor.getBFloat());
			glVertex3f(start.x,start.y,start.z); 
			glVertex3f(end.x,end.y,end.z); 

			glColor3f( 1.0, 1.0, 1.0 );
		glEnd();


		if(titleBar)
		{
			float titleBarHeight = Video.vcout.getHeight();
			CRectangle titleBarArea(area.leftx, area.rightx, area.topy-titleBarHeight, area.topy);

			v3d start = titleBarArea.getLowerLeft();
			v3d end = titleBarArea.getLowerRight();
			glBegin(GL_LINES);
				glColor3f( darkColor.getRFloat(), darkColor.getGFloat(), darkColor.getBFloat());
				glVertex3f(start.x,start.y,start.z); 
				glVertex3f(end.x,end.y,end.z); 
				glColor3f( 1.0, 1.0, 1.0 );
			glEnd();


			Video.vcout.setPos(area.getUpperLeft() + v3d(5,-Video.vcout.getAscender()));
			Video.vcout.setColor(CColor(1.0f, 1.0f, 1.0f, 1.0f));
			Video.vcout.setBounds(titleBarArea);
			Video.vcout.clipToBounds = true;
			Video.vcout << titleBarCaption;
			Video.vcout.clipToBounds = false;
		}
		glPopAttrib();
		
		Video.PopProjection();

		widgetPanel.Render();
	}
}

void CGuiWindowWidget::PythonScriptBegin()
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	main_namespace["this"] = Gui.WM.GetWindowID(this);
}
void CGuiWindowWidget::PythonScriptEnd()
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	main_namespace["this"].del();
}
void CGuiWindowWidget::StartTracking(ITrackInHud &obj)
{
	m_trackedObject = &obj;
}
void CGuiWindowWidget::StopTracking(void)
{
	m_trackedObject = 0;
}


void CGuiWindowWidget::SetUpperLeftCorner(v3d upperLeft)
{
	v3d currentPosition = this->area.getUpperLeft();
	v3d delta = upperLeft - currentPosition;
	this->Move(delta);
}


CGuiWindowWidget CGuiWindowWidget::operator=(const CGuiWindowWidget& rhs)
{
	copyFromOther(rhs);
	return *this;
}

void CGuiWindowWidget::copyFromOther(const CGuiWindowWidget& rhs)
{
	area = rhs.area;
	name = rhs.name;

	dragging = rhs.dragging;
	dragable = rhs.dragable;

	caption = rhs.caption;
	color = rhs.color;
	widgetPanel = rhs.widgetPanel;
	visible = rhs.visible;
	m_trackedObject = rhs.m_trackedObject;
}
