#ifndef GUI_MAIN_H
#define GUI_MAIN_H

#include "CVideoSubsystem.h"

#include <queue>
using namespace std;

class gui_GUIObject;

void GUI_Init();
void GUI_Update();
void GUI_Draw(DrawingModes mode);
void GUI_Shutdown();
void GUI_AddInitAuxFunction(void (* function)(void));

//void GUI_AddObjectToGUI(gui_GUIObject* newObject);
void GUI_UpdateGUI(gui_GUIObject* pObject);
void GUI_DrawGUI(gui_GUIObject* pObject, DrawingModes mode);
//void GUI_ClearGUI(gui_GUIObject* pObject);

unsigned char GUI_InputReceiver(in_Event* event);
extern bool gui_guiactive;
extern vid_SceneGraph gui_guiGraph;

extern queue<gui_GUIObject*> deleteQueue;

//enum gui_EventTypes {CLOSE, }

class gui_GUIObject: public vid_SceneObject
{
public:
	enum gui_GUIObjectTypes {baseobject, window, guitext, cursor, banedtile, banedtilelist};
	enum Coordref {screen = 0, world};

	gui_GUIObject():
		vid_SceneObject(vid_SceneObject::guiobject), 
		guiobjecttype(baseobject), 
		dragable(false), 
		coordref(screen)
		{}
	gui_GUIObject(const gui_GUIObject& rhs):
		vid_SceneObject(rhs),
		guiobjecttype(rhs.guiobjecttype),
		dragable(rhs.dragable),
		coordref(rhs.coordref)
		{}
	gui_GUIObject(gui_GUIObjectTypes type):
		vid_SceneObject(vid_SceneObject::guiobject), 
		guiobjecttype(type), 
		dragable(false), 
		coordref(screen)
		{}
	~gui_GUIObject(){}

	virtual void onMouseClick(){}
	virtual void onMouseUp(){}
	virtual void onMouseHover(){}
	virtual void onMouseLeave(){}
	virtual void Event(){CON_Exec(event); /* execute the event command*/}

	bool dragable;

	gui_GUIObjectTypes guiobjecttype;
	Coordref coordref;
};

class gui_Window: public gui_GUIObject
{
public:
	gui_Window();
	gui_Window(v3d size);
	~gui_Window(){}

	void createControlButtons();
	void draw(DrawingModes mode, int layer);

	int textureId;
	string title;
};

class gui_Text: public gui_GUIObject
{
public:
	gui_Text();
	gui_Text(string newtext, v3d newsize, bool _editable = false);
	~gui_Text(){}

	void draw(DrawingModes mode, int layer);

	void setText(const string& _text);
	string getText();
	bool isEditable();
private:
	bool editable;
	string text;
};

class gui_Cursor: public gui_GUIObject
{
public:
	gui_Cursor():gui_GUIObject(cursor), beingDragged(NULL){textureId = Video.VideoResourceManager.LoadImageToVideoMemory("data/images/system/cursor.bmp"); size.x = 32; size.y = 32;}
	~gui_Cursor(){}

	void draw(DrawingModes mode, int _layer){if(_layer != layer) return; if(!visible)return; Video.BlitBitmap(textureId, v3d(collapseTransforms().getTranslation().x, collapseTransforms().getTranslation().y), v3d(size.x, size.y), 0, v3d(0,0), false, true);}
	void update(){}

	gui_GUIObject* beingDragged;
	v3d lastposition;
	int textureId;
};

class gui_BanedTile: public gui_GUIObject
{
public:
	gui_BanedTile();
	gui_BanedTile(const gui_BanedTile& rhs);
	gui_BanedTile(string name);
	~gui_BanedTile(){}

	void draw(DrawingModes mode, int layer);
	string file;
	int textureId;
};

class gui_BanedTileList: public gui_GUIObject
{
public:
	gui_BanedTileList():gui_GUIObject(banedtilelist){transform = matrix4x4::mtfTranslate(v3d(5,5));}
	~gui_BanedTileList(){}

	void clear();
	void removeTile(string name);
	bool tileExists(string name);
	void draw(DrawingModes mode, int layer);
	void scrollRight(){transform *= TranslationMatrix(v3d(-32, 0));}
	void scrollLeft(){transform *= TranslationMatrix(v3d(32, 0));}
};

class gui_Button: public gui_GUIObject
{
public:
	enum GraphicTypes {vector = 0, bitmap};
	enum States {up = 0, down};

	gui_Button()
	{
		graphicType = vector;
		state = up;
		text = "Button!";
	}
	~gui_Button()
	{

	}
	void onMouseClick()
	{
		state = down;
	}
	void onMouseLeave()
	{
		state = up;
	}
	void onMouseUp()
	{
		// do event here
		state = up;
	}
	void draw(DrawingModes mode, int _layer)
	{ 
		if(_layer != layer) 
			return; 

		v3d position = collapseTransforms().getTranslation();

		v3d
			p1(position),
			p2(position + v3d(0, size.y)),
			p3(position + size),
			p4(position + v3d(size.x, 0));

		switch(graphicType)
		{
		case vector:
			{
				Video.BlitRect(p1, p3, CColor(.4f, .4f, .4f), CColor(.4f, .4f, .4f));
				switch(state)
				{
				case up:
					{
						Video.DrawLine(p1, p4, _RGB32BIT(64, 64, 64), false);
						Video.DrawLine(p4, p3, _RGB32BIT(64, 64, 64), false);
						Video.DrawLine(p2, p3, _RGB32BIT(192, 192, 192), false);
						Video.DrawLine(p1, p2, _RGB32BIT(192, 192, 192), false);
					}
					break;
				case down:
					{
						Video.DrawLine(p1, p4, _RGB32BIT(192, 192, 192), false);
						Video.DrawLine(p4, p3, _RGB32BIT(192, 192, 192), false);
						Video.DrawLine(p2, p3, _RGB32BIT(64, 64, 64), false);
						Video.DrawLine(p1, p2, _RGB32BIT(64, 64, 64), false);
					}
					break;
				default:
					break;
				}
				Video.DrawTextShit(text, p1.x+((p4.x-p1.x)/2-text.size()*4), p1.y+((p2.y-p1.y)/2-4), 0);
			}
			break;
		case bitmap:
			{
				Video.BlitBitmap(textureId, v3d(position.x, position.y), v3d(size.x, size.y), collapseTransforms().getZRot()); 
			}
			break;
		default:
			break;
		}

	}
	void update(){ textureId = animationManager.update(); gui_GUIObject::update(); }

	vid_AnimManager animationManager;
	int textureId;

	string text;
	States state;
	GraphicTypes graphicType;
};

class gui_ButtonIC: public gui_Button
{
public:
	enum EventTypes {closeParent = 0, minimizeParent, maximizeParent };
	gui_ButtonIC():gui_Button(), eventType(closeParent){text = "X";}
	~gui_ButtonIC(){}

	void Event()
	{
		switch(eventType)
		{
		case closeParent:
			{
				if(parent)
				{
					deleteQueue.push((gui_GUIObject*)parent); // queue up the parent to be deleted
				}
				else
				{
					deleteQueue.push((gui_GUIObject*)this); // this is a lone button (which probably won't happen)
				}
			}
			break;
		case minimizeParent:
			{
			}
			break;
		case maximizeParent:
			{
			}
			break;
		default:
			break;
		}
	}

	EventTypes eventType;
};

extern gui_Cursor* cursor;

#endif