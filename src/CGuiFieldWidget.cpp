#include "CGuiFieldWidget.h"
#include "CEventRouter.h"
#include "CVideoSubsystem.h"

#include "sys_main.h"
#include "CPythonSubsystem.h"
#include <boost/python.hpp>
using namespace boost::python;

CGuiFieldWidget::CGuiFieldWidget()
{
	area = CRectangle(0,100,0,20);

	canEdit = false;
	horizontalScroll = false;
	verticalScroll = false;
	backgroundColor = CColor(1.0f, 1.0f, 1.0f);
	textColor = CColor(0.0f, 0.0f, 0.0f, 1.0f);
	contents = "a field's contents";
	selectionStart = 0;
	selectionEnd = 0;
	cursorPosition = 0;
	isSelecting = false;
	hasFocus = false;
}

CGuiFieldWidget::CGuiFieldWidget(const CGuiFieldWidget& rhs)
{
	copyFromOther(rhs);
}

CGuiFieldWidget::~CGuiFieldWidget()
{

}

bool CGuiFieldWidget::onMouseMove(v3d mouseDelta)
{
	return false;
}
bool CGuiFieldWidget::onMouseDown(v3d mousePosition)
{
	/*isSelecting = true;
	v3d localPosition = v3d(0, area.topy-area.bottomy) - (mousePosition - v3d(area.leftx, area.bottomy));
	localPosition.x = -localPosition.x;
	ccout << "local position " << localPosition <<newl;
	int row = ((int)localPosition.y) / ((int)(Video.vcout.getHeight()+0.5));
	int minI(0), maxI(0), rowCount(0);
	for(int i = 0; i < contents.size(); i++)
	{
		if(contents[i] == '\n')
			rowCount++;
		if(rowCount == row)
		{
			minI = i;
			break;
		}
	}
	rowCount = 0;
	for(int i = 0; i < contents.size(); i++)
	{
		if(contents[i] == '\n')
			rowCount++;
		if(rowCount == row+1)
		{
			maxI = i;
			break;
		}
	}
	if(maxI == 0)
		maxI = minI;
	string rowString = contents.substr(minI, maxI-minI);

	int rowPosition(maxI);
	for(int i = 1; i < maxI-minI; i++)
	{
		if(Video.vcout.getAdvance(rowString.substr(0, i)) >= localPosition.x)
		{
			rowPosition = i;
			break;
		}
	}

	selectionStart = minI + rowPosition;
	cursorPosition = selectionStart;

	ccout << "maxI " << maxI << " minI " << minI << newl;*/

	return true;
}
bool CGuiFieldWidget::onMouseUp(v3d mousePosition)
{
	/*isSelecting = false;
	v3d localPosition = mousePosition - v3d(area.leftx, area.bottomy);
	int row = ((int)localPosition.y)%((int)Video.vcout.getHeight());
	int minI(0), maxI(0), rowCount(0);
	for(int i = 0; i < contents.size(); i++)
	{
		if(contents[i] == '\n')
			rowCount++;
		if(rowCount == row)
			minI = i;
		if(rowCount == row+1)
			maxI = i;
	}
	if(maxI = 0)
		maxI = minI;
	string rowString = contents.substr(minI, maxI-minI);

	int rowPosition(maxI);
	for(int i = 1; i < maxI-minI; i++)
	{
		if(Video.vcout.getAdvance(rowString.substr(0, i)) >= localPosition.x)
		{
			rowPosition = i;
			break;
		}
	}

	selectionEnd = minI + rowPosition;

	ccout << "Selection is " << selectionStart << " to " << selectionEnd << newl;*/

	return true;
}
bool CGuiFieldWidget::onMouseEnter(v3d mousePosition)
{
	return true;
}
bool CGuiFieldWidget::onMouseLeave(v3d mousePosition)
{
	return true;
}
bool CGuiFieldWidget::onKey(CInputEvent inputEvent)
{
	if(!canEdit)
		return false;
	string beforeCursor = contents.substr(0, cursorPosition);
	//string afterCursor = contents.substr(
	string character;
	character += (char)inputEvent.data.characterTypeEvent.characterCode;
	if(inputEvent.inputEventType == IET_CHARACTERTYPED)
	{
		switch(inputEvent.data.characterTypeEvent.characterCode)
		{
		case IKC_RETURN:
			{
				contents.insert(cursorPosition, "\n");
				cursorPosition++;
			}
			break;
		default:
			{
				contents.insert(cursorPosition, character);
				cursorPosition++;
			}
			break;
		}
	}
	return true;
}

bool CGuiFieldWidget::takesFocus()
{
	return true;
}

void CGuiFieldWidget::onGetFocus()
{
	hasFocus = true;
}
void CGuiFieldWidget::onLoseFocus()
{
	hasFocus = false;
}

void CGuiFieldWidget::Move(v3d delta)
{
	area = area + delta;
}

void CGuiFieldWidget::Resize(CRectangle delta)
{
	area.leftx += delta.leftx;
	area.rightx += delta.rightx;
	area.bottomy += delta.bottomy;
	area.topy += delta.topy;
}

void CGuiFieldWidget::Render()
{
	CColor backgroundColor = CColor(0.2f, 0.2f, 0.2f, 0.75f);
	CColor brightColor = CColor(128, 128, 128, 128);//backgroundColor + CColor(0.2f, 0.2f, 0.2f, 0.0f);

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

		glVertex2f(p1.x,p2.y); // 0,1

		glVertex2f(p2.x,p2.y);		// 1,1

		glVertex2f(p2.x,p1.y);	// 1,0
		glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();


	v3d start = area.getLowerLeft();
	v3d end = area.getLowerRight();
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


	Video.vcout.setBounds(area);
	Video.vcout.clipToBounds = true;
	Video.vcout.setPos(v3d( area.leftx+1, area.topy - Video.vcout.getAscender()));
	Video.vcout.setColor(textColor);
	Video.vcout << this->contents;
	Video.vcout.clipToBounds = false;

	glPopAttrib();
	Video.PopProjection();


	// figure out cursor business...
/*	if(hasFocus)
	{
		static float timer(1.0f);
		if(timer <= 1)
		{
			if(timer <= 0.0f)
				timer = 2.0f;
			// find where the cursor should be
			int numLines(0);
			int lastNewline(0);
			v3d position(0,0);
			for(int i = 0; i < cursorPosition; i++)
				if(contents[i] == '\n')
				{
					lastNewline = i;
					numLines++;
				}
			position.y = (area.topy-area.bottomy)-(numLines*Video.vcout.getHeight()) + area.bottomy;
			if((lastNewline+1 < cursorPosition) && cursorPosition < contents.size())
				position.x = Video.vcout.getAdvance(contents.substr(lastNewline+1, cursorPosition-(lastNewline+1))) + area.leftx;
			else
				position.x = 0;
			rgba8888pixel color;
			color.components.r = 0;
			color.components.g = 0;
			color.components.b = 0;
			color.components.a = 255;
			Video.DrawLine(position-v3d(0,5), position+v3d(0,5), color);
			Video.DrawRectangle(position-v3d(0,Video.vcout.getHeight()-1), position+v3d(1,-1), CColor(0, 0, 0));
		}
		timer -= sys_frameTime;
	}*/
}

void CGuiFieldWidget::PythonScriptBegin()
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	main_namespace["contents"] = this->contents;
}
void CGuiFieldWidget::PythonScriptEnd()
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	this->contents = extract<string>(main_namespace["contents"]);
	main_namespace["contents"].del();
}
CGuiFieldWidget CGuiFieldWidget::operator=(const CGuiFieldWidget& rhs)
{
	copyFromOther(rhs);
	return *this;
}
CGuiFieldWidget CGuiFieldWidget::operator<<(const string text)
{
	this->Print(text);
	return *this;
}
void CGuiFieldWidget::Print(const string text)
{
	this->contents += text;
}
void CGuiFieldWidget::Clear()
{
	this->contents = "";
}

void CGuiFieldWidget::copyFromOther(const CGuiFieldWidget& rhs)
{
	area = rhs.area;
	name = rhs.name;

	canEdit = rhs.canEdit;
	horizontalScroll = rhs.horizontalScroll;
	verticalScroll = rhs.verticalScroll;
	
	backgroundColor = rhs.backgroundColor;
	textColor = rhs.textColor;
	contents = rhs.contents;
	selectionStart = rhs.selectionStart;
	selectionEnd = rhs.selectionEnd;
	cursorPosition = rhs.cursorPosition;
	isSelecting = rhs.isSelecting;
	hasFocus = rhs.hasFocus;
}
