#include "con_display.h"
#include "CVideoSubsystem.h"
#include "vid_win.h"
#include "con_main.h"
#include "sys_main.h"
#include "sys_win.h"

extern bool cond_initialized(false);

short consoleY(0);
bool consoleDown(false);
bool consoleMoving(false);
float consoleSpeed;
short con_textWidth;
short con_textHeight;
short con_textPadding;
short bgTex;

//vidw_Font consoleFont;

void COND_PrintTextBuffer()
{
	if(!cond_initialized) return;

	static short lastBlinkTime(0);
	string temp = "]";
	lastBlinkTime = (short)SYSW_TimeFloat();
	bool drawCursor(false);
	if( (( (short)(SYSW_TimeFloat()*2) ) % 2) == 0)
		drawCursor = true;
		//temp+=inputLine+'|';
	//else
		//temp+=inputLine;
		temp+=inputLine;

	short ypos = consoleY-(con_textHeight)-con_textPadding;
	//Video.DrawTextShit(temp, con_textPadding, vid_Settings.getSh()-ypos, 0);

//	vcout.setFont("Courier");
//	vcout.setWeight(500);
//	vcout.setHeight(12);
	Video.vcout.setColor(CColor(0.75f, 0.75f, 0.75f));
	Video.vcout.setPosScaled(v3d(con_textPadding, Video.settings.getShScaled()-ypos*Video.settings.getShScaled()/Video.settings.getSh()));
	//Video.vcout << temp;
	string temp2;
	//Video.vcout << temp.substr( 0, temp.length()-inputCursor );
	temp2 = temp.substr( 0, temp.length()-inputCursor );
	if(drawCursor)
	{
		temp2 += "|";
		//Video.vcout << '|';
		/*temp = " ";
		for(int i = 0; i < inputLine.length()-inputCursor; i++)
		{
			temp += " ";
		}
		temp += '|';
		Video.vcout.setPosScaled(v3d(con_textPadding-4, Video.settings.getShScaled()-ypos*Video.settings.getShScaled()/Video.settings.getSh()));
		Video.vcout << temp;*/
	}
	//Video.vcout << temp.substr( temp.length()-inputCursor , temp.length() - inputCursor-1 );
	temp2 += temp.substr( temp.length()-inputCursor , temp.length() - temp.length()-inputCursor );
	Video.vcout << temp2;

	ypos -= con_textHeight;
	for(int i = 1+textBufferDisplayLocation; i < NUM_TEXT_BUFFER_LINES; i++)
	{
		Video.vcout.setPosScaled(v3d(con_textPadding, Video.settings.getShScaled()-ypos*Video.settings.getShScaled()/Video.settings.getSh()));
		Video.vcout << textBuffer[i];
		ypos-=con_textHeight;
		if(ypos <= -con_textHeight)
			break; // up high enough that its a waste of time
	}
}

void COND_Init()
{
	if(cond_initialized) return;

	consoleSpeed = (1000);
	con_textWidth = (12);
	con_textHeight = (12);
	con_textPadding = (5);

	bgTex = Video.VideoResourceManager.LoadImageToVideoMemory("data/images/system/conbg2.bmp");

//	consoleFont.SetFont("Gothic");

	cond_initialized = true;
}

void COND_Draw()
{
	if(!cond_initialized) return;
	if(consoleDown && consoleMoving)
	{
		short consoleDelta = (short)(consoleSpeed*sys_frameTime);
		consoleY -= consoleDelta;
		if(consoleY <=0)
		{
			consoleMoving = false;
			consoleDown = false;
			consoleY = 0;
		}
	}
	if(!consoleDown && consoleMoving)
	{
		short consoleDelta = (short)(consoleSpeed*sys_frameTime);
		consoleY += consoleDelta;
		if(consoleY >= Video.settings.getSh()/2)
		{
			consoleMoving = false;
			consoleDown = true;
			consoleY = Video.settings.getSh()/2;
		}
	}
	if(consoleDown || (!consoleDown && consoleMoving))
	{
	/*	bool lighting = Video.lighting;
		Video.lighting = false;
		Video.BlitBitmap(bgTex, v3d(0, (float)(vid_Settings.getSh()-consoleY)), v3d((float)vid_Settings.getSw(), ((float)vid_Settings.getSh())/2.0f));
		COND_PrintTextBuffer();
		Video.lighting = lighting;*/

		const short borderpadding(3);
		const short textpadding(4);

		v3d p1, p2, p3, p4;
		v3d position(0, (float)(Video.settings.getSh()-consoleY));
		v3d size(v3d((float)Video.settings.getSw(), ((float)Video.settings.getSh())/2.0f));
		p1 = v3d(position.x, position.y);
		p2 = v3d(position.x, position.y+size.y);
		p3 = v3d(position.x+size.x, position.y+size.y);
		p4 = v3d(position.x+size.x, position.y);

		Video.BlitRect(position, position+size, CColor(.35f, .35f, .35f, 0.75f), CColor(.35f, .35f, .35f, 0.75f));
	//	Video.BlitRect(v3d(position.x+textpadding-1, -8+position.y+size.y-textpadding-1), v3d(position.x+size.x-textpadding+1, position.y+size.y-textpadding+1), CColor(.5f,.5f,.5f));
		//Video.DrawTextShit(title, position.x+textpadding, -8+position.y+size.y-textpadding, size.y);

		Video.DrawLine(p1, p4, _RGB32BIT(64, 64, 64), false);
		Video.DrawLine(p2, p3, _RGB32BIT(192, 192, 192), false);
		Video.DrawLine(p1, p2, _RGB32BIT(192, 192, 192), false);
		Video.DrawLine(p4, p3, _RGB32BIT(64, 64, 64), false);
		
	/*	Video.BlitRect(p1 + v3d(borderpadding, borderpadding), p3 + v3d(-borderpadding, -borderpadding), CColor(.4f, .4f, .4f));
		Video.DrawLine(p1 + v3d(borderpadding, borderpadding), p4 + v3d(-borderpadding, borderpadding), _RGB32BIT(192,192,192), false);
		Video.DrawLine(p2 + v3d(borderpadding, -borderpadding), p3 + v3d(-borderpadding, -borderpadding), _RGB32BIT(64,64,64), false);
		Video.DrawLine(p1 + v3d(borderpadding, borderpadding), p2 + v3d(borderpadding, -borderpadding), _RGB32BIT(64,64,64), false);
		Video.DrawLine(p4 + v3d(-borderpadding, borderpadding), p3 + v3d(-borderpadding, -borderpadding), _RGB32BIT(192,192,192), false);
*/
		COND_PrintTextBuffer();

	}
//	CON_DebugDraw();
}

void COND_Shutdown()
{
//	if(!cond_initialized) return;
//	delete cond_background;
	bgTex = -1;
	cond_initialized = false;
}