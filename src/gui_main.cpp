

#include "gui_main.h"
#include "CVideoSubsystem.h"
#include "con_main.h"
#include "m_misc.h"
//#include "bn_map.h"
#include "in_main.h"

#include <string>
#include <vector>
#include <stack>
#include <queue>
using namespace std;

queue<gui_GUIObject*> deleteQueue;


class vid_GUIObject;

vid_SceneGraph gui_guiGraph;

gui_Cursor* cursor;
bool gui_guiactive(false);
bool mouseLeftDrag(false);

FunctionNode* guiInitAuxList = 0;

struct inputTracker_t
{
	gui_GUIObject* pressedObject;
	bool mouseUp;
} inputTracker;

class gui_InputDirector
{
public:
	gui_InputDirector();
	~gui_InputDirector();

	void FocusOnObject(gui_GUIObject* object);
	unsigned char InputReceiver(in_Event* event);

private:
	gui_GUIObject* focusedObject;
};

gui_InputDirector inputDirector;


gui_InputDirector::gui_InputDirector():
focusedObject(NULL)
{
}

gui_InputDirector::~gui_InputDirector()
{
	FocusOnObject(NULL);
}

void gui_InputDirector::FocusOnObject(gui_GUIObject* object)
{
	if(focusedObject)
		focusedObject->handle = 0;

	focusedObject = object;
	if(object)
		object->handle = (vid_SceneObject**)&focusedObject;
}

unsigned char gui_InputDirector::InputReceiver(in_Event* event)
{
	if(!focusedObject)
		return 0;
	if(focusedObject->guiobjecttype == gui_GUIObject::guitext)
	{
		gui_Text* text = (gui_Text*)focusedObject;
		if(!text->isEditable())
			return 0;

		static bool shifted(false);
		switch(event->eventType)
		{
		case INEV_PRESS:
			{
				if(event->flags & INEVF_SHIFT)
					shifted = true;

				switch(event->key)
				{
				case INKEY_ENTER:
					{
					}
					break;
				case INKEY_BACKSPACE:
					{
						if(text->getText().size() > 0)
							text->setText(text->getText().substr(0, text->getText().size()-1));
						return 1;
					}
					break;
				default:
					{
						if(IN_GetCharacter(event->key) == '`')
						{
							return 0;
							break;
						}
						if(event->key < 128)
						{
							if(shifted)
							{
								text->setText(text->getText() + IN_GetShiftedCharacter(event->key));
								return 1;
								break;
							}	
							else
							{
								text->setText(text->getText() + IN_GetCharacter(event->key));
								return 1;
								break;
							}
						} // if event->key < 128
					}
					break;
				}
			}
		default:
			break;
		}
	}
	return 0;
}


void GUI_Init()
{
	gui_guiGraph.clipToCamera = false;
	cursor = new gui_Cursor;
	cursor->visible = true;
	gui_guiactive = true;
	inputTracker.mouseUp = true;
	inputTracker.pressedObject = NULL;

	if(guiInitAuxList)
	{
		for(FunctionNode* i = guiInitAuxList; i; i = i->next)
			i->handler();
	}
}

void GUI_Update()
{
	cursor->update();
	while(!deleteQueue.empty())
	{
		gui_guiGraph.deleteObject(deleteQueue.front());
		deleteQueue.pop();
	}
}

void GUI_Draw(DrawingModes mode)
{
/*	if(!gui_guiactive)
		return;*/
//	GUI_DrawGUI(gui_guigraph);
	bool lighting = Video.settings.lighting;
	Video.settings.lighting = false;
	for(int i = 0; i < 10; i++)
	{
		gui_guiGraph.draw(mode, i);
	}
//	Video.DrawScene(gui_guigraph);
	GUI_DrawGUI(cursor, ingame);
	Video.settings.lighting = lighting;
}
void GUI_Shutdown()
{
	inputDirector.FocusOnObject(NULL);
//	GUI_ClearGUI(gui_guigraph);
	gui_guiGraph.clear();
//	gui_guigraph = 0;
	delete cursor;
	cursor = 0;
}

void GUI_AddInitAuxFunction(void (* function)(void))
{
	// case 1: list is empty
	if(!guiInitAuxList)
	{
		guiInitAuxList = new FunctionNode;
		guiInitAuxList->handler = function;
		guiInitAuxList->next = 0;
	}
	else
	{
		FunctionNode* index;
		for(index = guiInitAuxList; index->next; index = index->next);
		index->next = new FunctionNode;
		index->next->handler = function;
		index->next->next = 0;		
	}
}

/*void GUI_AddObjectToGUI(gui_GUIObject* newObject)
{
//	if(!Video.initialized) return;
	if(!gui_guigraph)
		gui_guigraph = new gui_GUIObject;
	if(gui_guigraph)
		gui_guigraph->attachChild(newObject);
}*/

void GUI_UpdateGUI(gui_GUIObject* pObject)
{
//	if(!Video.initialized) return;
	// Base case for the recursion
	if (pObject == NULL)
		return;
	else
	{
		// Update this object.
		pObject->update();

	/*	// Recursively call all children
		vector<vid_SceneObject*>::iterator it =
					   pObject->getChildList()->begin();

		for (;it != pObject->getChildList()->end(); ++it)
		  GUI_UpdateGUI(*it);*/
		for(unsigned int i = 0; i < pObject->childlist.size(); i++)
		//for(gui_GUIObject* index = (gui_GUIObject*)pObject->childlist; index != 0; index = (gui_GUIObject*)index->next)
		{
			GUI_UpdateGUI((gui_GUIObject*)pObject->childlist[i]);
		}
	}
}


void GUI_DrawGUIHelper(gui_GUIObject* pObject, DrawingModes mode, short layer)
{	
	if (pObject == NULL)
		return;
	else
	{
		// Update this object.
		if(pObject->layer == layer)
		if(pObject->visible == false) // even though the respective draw function handles this, return to keep and sub objects from being drawn if the parent is not visible
			return;
		pObject->draw(mode, layer);

		for(unsigned int i = 0; i < pObject->childlist.size(); i++)
	//	for(gui_GUIObject* index = (gui_GUIObject*)pObject->childlist; index != 0; index = (gui_GUIObject*)index->next)
		{
			GUI_DrawGUIHelper((gui_GUIObject*)pObject->childlist[i], mode, layer);
		}

	}
}

void GUI_DrawGUI(gui_GUIObject* pObject, DrawingModes mode)
{
	for(short i = 0; i < 10; i++)
	{
		GUI_DrawGUIHelper(pObject, mode, i);
	}
}

void GUI_ClearGUI(gui_GUIObject* pObject)
{	
//	if(!Video.initialized) return;
	// Base case for the recursion
	if (pObject == NULL)
		return;
	else
	{
	/*	// Recursively call all children
		vector<vid_SceneObject*>::iterator it =
					   pObject->getChildList()->begin();

		for (;it != pObject->getChildList()->end(); ++it)
		{
			GUI_ClearGUI(*it);
		}*/
		vector<vid_SceneObject*> childListCopy = pObject->childlist; // make a copy in case it is modified by GUI_ClearGUI call
		for(vector<vid_SceneObject*>::iterator it = childListCopy.begin(); it != childListCopy.end(); it++)
//		for(gui_GUIObject* index = (gui_GUIObject*)pObject->childlist; index != 0; /*index = index->next*/)
		{
			//gui_GUIObject* temp = (gui_GUIObject*)index->next;	
			GUI_ClearGUI((gui_GUIObject*)*it);
			//index = temp;
		}


		if(pObject)
		{
			delete pObject;
			pObject = 0;
		}
	}
}

void GUI_OnObject(vector<gui_GUIObject*> &objects, gui_GUIObject* pObject, v3d mouse)
{
	if(!pObject)
		return; 

	v3d objectposition = pObject->collapseTransforms().getTranslation();

	if( (mouse.x >= objectposition.x) && (mouse.x <= (objectposition.x + pObject->getSize().x) )
		&& (mouse.y >= objectposition.y) && (mouse.y <= (objectposition.y + pObject->getSize().y) ) )
	{
		// inside bounds of the object
		objects.push_back(pObject);
	}
	
	// Recursively call all children

	for(vector<vid_SceneObject*>::iterator it = pObject->childlist.begin(); it != pObject->childlist.end(); it++)
	{
		GUI_OnObject(objects, (gui_GUIObject*)*it, mouse);
	}
}



unsigned char GUI_InputReceiver(in_Event* event)
{
	switch(event->eventType)
	{
	case INEV_PRESS:
		{
			if(event->key == INKEY_TAB)
				gui_guiactive = !gui_guiactive;
		}
	default:
		break;
	}

/*	if(!gui_guiactive)
		return 0;*/

	if(!gui_guiGraph.getRoot())
	{
		cursor->visible = false;
		return 0;
	}
	else
	{
		bool visible(false);
		for(int i = 0; i < gui_guiGraph.getRoot()->childlist.size(); i++)
		{
			if(gui_guiGraph.getRoot()->childlist[i]->visible)
			{
				visible = true;
				break;
			}
		}
		if(!visible)
		{
			cursor->visible = false;
			return 0;
		}
	}
	cursor->visible = true;

	if(inputDirector.InputReceiver(event))
		return 1;



	switch(event->eventType)
	{
	case INEV_MOUSEMOVE:
		{
			cursor->lastposition = cursor->collapseTransforms().getTranslation();
			cursor->transform = TranslationMatrix(v3d((float)event->mouseX, (float)(Video.settings.getSh() - event->mouseY - cursor->getSize().y/*- 16*/)));
		
			return 1;
		}
		break;
	case INEV_PRESS:
		{
			switch(event->key)
			{
			case INKEY_MOUSELEFT:
				{
					vector<gui_GUIObject*> objects;
					GUI_OnObject(objects, (gui_GUIObject*)gui_guiGraph.getRoot(), cursor->collapseTransforms().getTranslation() + v3d(0, cursor->getSize().y));
					if(objects.size())
					{
						inputDirector.FocusOnObject(objects.back());// focus on the object

						objects.back()->onMouseClick();

						if( objects.back()->guiobjecttype == gui_GUIObject::window ) // bring visually to front
						{
							gui_guiGraph.detatchObject(objects.back());
							gui_guiGraph.addObject(objects.back());
						}
						inputTracker.mouseUp = false;
						inputTracker.pressedObject = objects.back();
						cursor->beingDragged = objects.back(); // track which thing is being dragged

						//CON_Exec(objects.back()->event); // execute the event command
						return 1;
					}
				}
				break;
			case INKEY_MOUSERIGHT:
				{
				}
				break;
			case INKEY_MOUSEMIDDLE:
				{
				}
				break;
			default:
				//return 0;
				break;
			}
		}
		break;
	case INEV_DRAG:
		{
			switch(event->key)
			{
			case INKEY_MOUSELEFT:
				{
					vector<gui_GUIObject*> objects;
					GUI_OnObject(objects, (gui_GUIObject*)gui_guiGraph.getRoot(), cursor->lastposition + v3d(0, cursor->getSize().y));
					if(objects.size())
					{
						if(objects.back() != inputTracker.pressedObject)
							inputTracker.pressedObject->onMouseLeave();

						if(objects.back()->dragable)
						{
							v3d lastmouse = cursor->lastposition + v3d(0, cursor->getSize().y);
							if(cursor->beingDragged == objects.back())
							{
								if(lastmouse.x > (objects.back()->collapseTransforms().getTranslation() + v3d(objects.back()->getSize().x - 10)).x)
								{
									if(lastmouse.y < (objects.back()->collapseTransforms().getTranslation() + v3d(0, 10)).y)
									{
										objects.back()->setSize(objects.back()->getSize() + v3d(cursor->collapseTransforms().getTranslation().x, -cursor->collapseTransforms().getTranslation().y) - v3d(cursor->lastposition.x, -cursor->lastposition.y) );
									//	objects.back()->collapseTransforms().getTranslation().y += (cursor->collapseTransforms().getTranslation() - cursor->lastposition).y;
										objects.back()->transform *= TranslationMatrix(v3d(0, cursor->collapseTransforms().getTranslation().y - cursor->lastposition.y));
										v3d size = objects.back()->getSize();
										if(size.x < 150)
										{
											size.x = 150;
										}
										if(size.y < 100)
										{
											size.y = 100;
										}
										objects.back()->setSize(size);
									}
									else
										objects.back()->transform *= cursor->collapseTransforms() * TranslationMatrix(cursor->lastposition*-1);
									//	objects.back()->position += cursor->position - cursor->lastposition;
								}
								else
									objects.back()->transform *= cursor->collapseTransforms() * TranslationMatrix(cursor->lastposition*-1);
									//objects.back()->position += cursor->position - cursor->lastposition;
							}
						}
						cursor->lastposition = cursor->collapseTransforms().getTranslation();
						//mouseLeftDrag = true;
						return 1;
					}
				}
				break;
			case INKEY_MOUSERIGHT:
				{
				}
				break;
			case INKEY_MOUSEMIDDLE:
				{
				}
				break;
			default:
				//return 0;
				break;
			}
		}
		break;
	case INEV_RELEASE:
		{
			switch(event->key)
			{
			case INKEY_MOUSELEFT:
				{
					vector<gui_GUIObject*> objects;
					GUI_OnObject(objects, (gui_GUIObject*)gui_guiGraph.getRoot(), cursor->collapseTransforms().getTranslation() + v3d(0, cursor->getSize().y));
					if(objects.size())
					{
						if(objects.back() == inputTracker.pressedObject)
						{
							inputDirector.FocusOnObject(objects.back());// focus on the object

							objects.back()->onMouseUp();

							if( objects.back()->guiobjecttype == gui_GUIObject::window ) // bring visually to front
							{
								gui_guiGraph.detatchObject(objects.back());
								gui_guiGraph.addObject(objects.back());
							}

							cursor->beingDragged = objects.back(); // track which thing is being dragged

							objects.back()->Event();
							//CON_Exec(objects.back()->event); // execute the event command
						}
						return 1;
					}
					inputTracker.pressedObject = NULL;
					cursor->beingDragged = NULL;
					//mouseLeftDrag = false;
				}
				break;
			case INKEY_MOUSERIGHT:
				{
				}
				break;
			case INKEY_MOUSEMIDDLE:
				{
				}
				break;
			default:
				//return 0;
				break;
			}
		}
		break;
	default:
		return 0;
	}
	return 0;
}



gui_Window::gui_Window():gui_GUIObject(window)
{
	textureId = Video.VideoResourceManager.LoadImageToVideoMemory("data/images/system/window.bmp");// might want to change this in the future so it isn't hard coded
	size.x = 50;
	size.y = 50;
	transform = TranslationMatrix(v3d(0, Video.settings.getSh()-size.y));
}

gui_Window::gui_Window(v3d newsize):gui_GUIObject(window)
{
	textureId = Video.VideoResourceManager.LoadImageToVideoMemory("data/images/system/window.bmp");
	setSize(newsize);
}

void gui_Window::createControlButtons()
{
	gui_ButtonIC* xbox = new gui_ButtonIC;
	xbox->transform = TranslationMatrix(v3d(getSize().x - 13, getSize().y - 14));
	xbox->setSize(v3d(10,10));
	attachChild(xbox);
}

void gui_Window::draw(DrawingModes mode, int _layer)
{
	if(_layer != layer) return;
	if(!visible)
		return;

	const short titlepadding(17), borderpadding(3);
	const short textpadding(4);

	v3d position = collapseTransforms().getTranslation();

//	Video.BlitBitmap(textureId, v3d(position.x, position.y), v3d(size.x, size.y), angle.z);
	v3d p1, p2, p3, p4;
	p1 = v3d(position.x, position.y);
	p2 = v3d(position.x, position.y+size.y);
	p3 = v3d(position.x+size.x, position.y+size.y);
	p4 = v3d(position.x+size.x, position.y);

	Video.BlitRect(position, position+size, CColor(.35f, .35f, .35f));
	Video.BlitRect(v3d(position.x+textpadding-1, -8+position.y+size.y-textpadding-1), v3d(position.x+size.x-textpadding+1, position.y+size.y-textpadding+1), CColor(.5f,.5f,.5f));
	//Video.DrawTextShit(title, position.x+textpadding, -8+position.y+size.y-textpadding, size.y);
	Video.vcout.setPos(v3d(position.x+textpadding, -8+position.y+size.y-textpadding));
	Video.vcout << title;

	Video.DrawLine(p1, p4, _RGB32BIT(64, 64, 64), false);
	Video.DrawLine(p2, p3, _RGB32BIT(192, 192, 192), false);
	Video.DrawLine(p1, p2, _RGB32BIT(192, 192, 192), false);
	Video.DrawLine(p4, p3, _RGB32BIT(64, 64, 64), false);
	
	Video.BlitRect(p1 + v3d(borderpadding, borderpadding), p3 + v3d(-borderpadding, -titlepadding), CColor(.4f, .4f, .4f));
	Video.DrawLine(p1 + v3d(borderpadding, borderpadding), p4 + v3d(-borderpadding, borderpadding), _RGB32BIT(192,192,192), false);
	Video.DrawLine(p2 + v3d(borderpadding, -titlepadding), p3 + v3d(-borderpadding, -titlepadding), _RGB32BIT(64,64,64), false);
	Video.DrawLine(p1 + v3d(borderpadding, borderpadding), p2 + v3d(borderpadding, -titlepadding), _RGB32BIT(64,64,64), false);
	Video.DrawLine(p4 + v3d(-borderpadding, borderpadding), p3 + v3d(-borderpadding, -titlepadding), _RGB32BIT(192,192,192), false);
}

gui_Text::gui_Text():
gui_GUIObject(guitext),
editable(false)
{
	transform = TranslationMatrix(v3d(((float)Video.settings.getSw())/2.0f, ((float)Video.settings.getSh())/2.0f));
	text = "text not set";
}

gui_Text::gui_Text(string newtext, v3d newsize, bool _editable):
gui_GUIObject(guitext),
text(newtext),
editable(_editable)
{
	setSize(newsize);
	if(size.y == 0)
		size.y = 8;
}

void gui_Text::draw(DrawingModes mode, int _layer)
{
	if(_layer != layer) return;
	if(!visible)
		return;
	Video.DrawTextShit(text, (short)collapseTransforms().getTranslation().x, (short)collapseTransforms().getTranslation().y, (unsigned short)size.x);
}

void gui_Text::setText(const string& _text)
{
	text = _text;
}

string gui_Text::getText()
{
	return text;
}

bool gui_Text::isEditable()
{
	return editable;
}


gui_BanedTile::gui_BanedTile():gui_GUIObject(banedtile),file(""),textureId(-1)
{
	event = "echo tile set up incorrectly";
	size.x = 32;
	size.y = 32;
	transform = matrix4x4::mtfIdentity();
}

gui_BanedTile::gui_BanedTile(const gui_BanedTile& rhs):
gui_GUIObject(rhs),
file(rhs.file),
textureId(rhs.textureId)
{
	event = rhs.event;
	size = rhs.size;
	transform = rhs.transform;
}

gui_BanedTile::gui_BanedTile(string newname):gui_GUIObject(banedtile),file(newname),textureId(-1)
{
	event = "brush_SetTextureFileName " + file;
	textureId = Video.VideoResourceManager.LoadImageToVideoMemory(file);
	size.x = 32;
	size.y = 32;
	transform = matrix4x4::mtfIdentity();
}

void gui_BanedTile::draw(DrawingModes mode, int _layer)
{
	if(_layer != layer) return;
	if(!visible)
		return;

	Video.BlitBitmap(textureId, collapseTransforms().getTranslation(), v3d(size.x, size.y), collapseTransforms().getZRot());
}
/*
void gui_BanedTile::Event()
{
	mapEditorData.brush.SetTextureFileName(file);
}*/

void gui_BanedTileList::clear()
{
	while(!childlist.empty())
	{
		delete (*childlist.begin());
		childlist.erase(childlist.begin());
	}
}

void gui_BanedTileList::removeTile(string name)
{
	stack<vector<vid_SceneObject*>::iterator> itStack;
	for(vector<vid_SceneObject*>::iterator it = childlist.begin(); it != childlist.end(); it++)
	{
		if(((gui_BanedTile*)(*it))->file == name)
			itStack.push(it);
	}
	while(!itStack.empty())
	{
		delete (*itStack.top());
		childlist.erase(itStack.top());
		itStack.pop();
	}
}

bool gui_BanedTileList::tileExists(string name)
{
	for(vector<vid_SceneObject*>::iterator it = childlist.begin(); it != childlist.end(); it++)
	{
		if(((gui_BanedTile*)(*it))->file == name)
			return true;
	}
	return false;
}

void gui_BanedTileList::draw(DrawingModes mode, int _layer)
{
	if(_layer != layer) return;
	if(!visible)
		return;
	
	int i = 0;
	for(vector<vid_SceneObject*>::iterator it = childlist.begin(); it != childlist.end(); it++)
	{
		(*it)->transform = TranslationMatrix(v3d(collapseTransforms().getTranslation().x+(i++)*32, collapseTransforms().getTranslation().y));
		(*it)->draw(mode, _layer);
	}
}