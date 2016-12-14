#ifndef CVIDEOSUBSYSTEM_H
#define CVIDEOSUBSYSTEM_H

typedef unsigned long VideoResourceID;

#include "ISubsystem.h"
#include "CImage.h"
#include "CColor.h"
#include "CImage.h"
#include "CModel.h"
#include "threads_main.h"
//#include "CMilkShapeModel.h"

#include "math_main.h"

#include <fstream>
#include <vector>
#include <string>
using namespace std;
class V_SystemData;


enum DrawingModes { ingame = 0, editor, debug};

struct collision_t
{
	vid_SceneObject* object;
	v3d normal;
	v3d point;
};

struct vid_Point
{
	v3d position;
	v3d textureCoordinates;
	CColor color;
};

class vid_SceneObject
{
public:
	enum vid_SOType {generic = 0, zone, face, camera, tile, guiobject, entity};
	vid_SOType sotype;

	// make a collision case matrix??? case for each object that hits each object
	enum vid_CollType {solid = 0, solidWithEvent, permiable, permiableWithEvent};
	vid_CollType colltype;

	enum vid_BoundType {circle = 0, box};
	vid_BoundType boundtype;
	float boundingCircleRadius;

	vid_SceneObject();
	vid_SceneObject(const vid_SceneObject &rhs);
	vid_SceneObject(vid_SOType type);

	virtual ~vid_SceneObject();

	void setParent(vid_SceneObject* pParent);	// Called internally to set the parent node
	void attachChild(vid_SceneObject* pChild);	// Called to attach a object as a child of this object

	virtual float distance(v3d rhs){
		rhs-=collapseTransforms().getTranslation();
		return sqrt(rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z);}

	virtual float distanceSquared(v3d rhs){
		rhs-=collapseTransforms().getTranslation();
		return rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z;}

	virtual void draw(DrawingModes mode, int layer){};
	virtual void update()
	{	
	/*	//f = ma
		acceleration = force / mass;
		// v = at
		velocity += acceleration * sys_frameTime * 320;
		matrix4x4 translation;
		translation.mtfTranslate(velocity * sys_frameTime);
		transform *= translation;*/
	};

	vid_SceneObject *parent, **handle;
	vector<vid_SceneObject*> childlist;

	//v3d position; // lower left
	//v3d angle;
	matrix4x4 transform;

	matrix4x4 collapseTransforms();

	short layer; // draw layer 0 drawn first, 9 drawn last(on top)
	bool visible;
	bool kill; // when scene is updated, this object is destroyed if this is true
	string event; // command string to execute in a certain circumstance

	const v3d& getSize(){ return size; }
	void setSize(const v3d& newSize){ size = newSize; boundingCircleRadius = (size/2).distance(size); }
protected:
	v3d size; // width and height
};

class vid_SceneObjectHandle
{
public:
	vid_SceneObjectHandle():nextHandle(0), object(0){}
	~vid_SceneObjectHandle(){}

	vid_SceneObjectHandle* nextHandle;
	vid_SceneObject* object;
};

class vid_SceneObjectHandleList
{
public:
	vid_SceneObjectHandleList():head(0), objectCount(0){}
	vid_SceneObjectHandleList(const vid_SceneObjectHandleList& rhs)
	{
		for(vid_SceneObjectHandle* i = rhs.head; i != 0; i = i->nextHandle)
		{
			vid_SceneObjectHandle* newHandle = new vid_SceneObjectHandle;
			newHandle->object = i->object;
			addToList(newHandle);
		}
	}
	~vid_SceneObjectHandleList()
	{
		for( vid_SceneObjectHandle* i = head; i != 0; )
		{
			vid_SceneObjectHandle* temp = i->nextHandle;
			delete i;
			i = temp;
		}
	}

	void addToList(vid_SceneObjectHandle* node)
	{
		if(!head)
		{
			head = node;
			head->nextHandle = 0;
		}
		else
		{
			vid_SceneObjectHandle* i;
			for(i = head; i->nextHandle != 0; i = i->nextHandle)
			{
			/*	if( (node->object->distance(position) > i->object->distance(position)) && (node->object->distance(position) < i->next->object->distance(position)) )
				{
					node->next = i->next;
					i->next = node;
					return;
				}*/
			}
			i->nextHandle = node;
			i->nextHandle->nextHandle = 0;
		}
		objectCount++;
	}
	void removeFromList( vid_SceneObjectHandle* node)
	{
		vid_SceneObjectHandle* prev = 0;
		for(vid_SceneObjectHandle* i = head; i != 0; i = i->nextHandle)
		{
			if(i == node)
			{
				if(i == head)
				{
					head = i->nextHandle;
					delete i;
					i = 0;
					break;
				}
				else
				{
					prev->nextHandle = i->nextHandle;
					delete i;
					i = 0;
					break;
				}
			}
			prev = i;
		}

	}

	vid_SceneObjectHandle* head;
	short objectCount;
};
class vid_Face: public vid_SceneObject // a coplanar arrangement of vertices connected in the order in which they are listed
{
public:
	vid_Face():vid_SceneObject(face),friction(1),hardness(1),textureId(-1){}
	~vid_Face(){}

	void addVertex(vid_Point newVertex); // must be done clockwise
	void rotate(v3d pivot, float xAxisAngle, float yAxisAngle, float zAxisAngle);
	void draw(DrawingModes mode, int layer); 
	void findNormal(); // needs clockwise points to work
	float distance(v3d rhs);
	void attachTexture(string fileName);
	
	vector<vid_Point> verts;
	v3d normal;
	float friction;
	float hardness;
	short textureId; // index of texture
};

class vid_Camera: public vid_SceneObject
{
public:
	vid_Camera():vid_SceneObject(camera),mode(free),transition(none),targetType(uid),speed(5.0f),destzoom(1.0f),targetUid(0){}
	~vid_Camera(){}
	void update();
	const v3d getPosition(){return collapseTransforms().getTranslation();}
	const v3d getAngle(){return collapseTransforms().getRot();}
	const v3d getDestpositionAngleFromOrigin();
	enum Mode {free, track, lock}; // track is x+y lock is x+y+angle
	enum TransType {none, average, linear};
	enum TargetType {uid, location};

	Mode mode;
	TransType transition;
	TargetType targetType;
	matrix4x4 destination;
	
	v3d up, view, position;
	v3d dup, dview, dposition;


/*	v3d destangle;
	v3d destposition;*/
	float destzoom;

	float speed;

	unsigned long targetUid;
private:
/*	v3d angle;
	v3d position;*/
};



class vid_SceneGraph
{
public:
	vid_SceneGraph();
	~vid_SceneGraph();

	DummyMonitor monitor;

	void addObject(vid_SceneObject* newObject, bool zoned = true);
	void findObject(vector<vid_SceneObject*> &objects, v3d position);
	void deleteObject(v3d position);
	void deleteObject(vid_SceneObject* removed);
	void detatchObject(vid_SceneObject* removed); // detaches removed from a scene
	void update();
	void draw(DrawingModes mode, int layer);
	void clear();
	void rotate(v3d pivot, float xaxisangle, float yaxisangle, float zaxisangle);
	void collisionTest(vector<collision_t> &collisions, vid_SceneObject* self, const v3d& start, const v3d& end);
	void findEntity(vector<vid_SceneObject*> &ents, int enttype);
	void writeToStream(ostream& out);

	vid_SceneObject* getRoot(){ return root;} 
	bool clipToCamera;
private:
	void findObject(vector<vid_SceneObject*> &objects, vid_SceneObject* subRoot, v3d position);
	void deleteObject(vid_SceneObject* subRoot, v3d position);
	void deleteObject(vid_SceneObject* subRoot, vid_SceneObject* removed);
	void detatchObject(vid_SceneObject* subRoot, vid_SceneObject* removed); // detaches removed from a scene
	void update(vid_SceneObject* subRoot);
	void draw(vid_SceneObject* subRoot, DrawingModes mode, short layer);
	void clear(vid_SceneObject*& subRoot);
	void rotate(vid_SceneObject* subRoot, v3d pivot, float xaxisangle, float yaxisangle, float zaxisangle);
	void collisionTest(vid_SceneObject* subRoot, vector<collision_t> &collisions, vid_SceneObject* self, const v3d& start, const v3d& end);
	void findEntity(vector<vid_SceneObject*> &ents, vid_SceneObject* subRoot, int enttype);
	void writeToStream(vid_SceneObject* subRoot, ostream& out);

	vid_SceneObject* root;
};

class vidw_Font;
class vid_VCout
{
public:
	vid_VCout();
	~vid_VCout();
	// basic types
	vid_VCout& operator<<(string rhs);
	vid_VCout& operator<<(const char* rhs);	
	vid_VCout& operator<<(char rhs);
	vid_VCout& operator<<(unsigned char rhs);
	vid_VCout& operator<<(double rhs);
	vid_VCout& operator<<(int rhs);
	vid_VCout& operator<<(long int rhs);
	vid_VCout& operator<<(unsigned long int rhs);
	vid_VCout& operator<<(short int rhs);
	vid_VCout& operator<<(unsigned short int rhs);
	vid_VCout& operator<<(bool rhs);

	vid_VCout& setFont(const string& fontName);
	vid_VCout& setWeight(const unsigned short weight);
	vid_VCout& setHeight(const unsigned short height);
	vid_VCout& setPos(v3d position);
	vid_VCout& setPosScaled(v3d position);
	vid_VCout& setColor(CColor color);
private:
	v3d drawPosition;
	CColor drawColor;
	vidw_Font* font;
	void print(string text);
};


class CImage;
class COpenGLTexture;
class CVideoResourceManager
{
public:
	enum MemoryTypes {none, system, video};

	CVideoResourceManager();
	~CVideoResourceManager();

	VideoResourceID LoadImageToSystemMemory(string filename); // returns a video resource id for the image loaded
															// this loads into the CImage class
	VideoResourceID LoadImageToVideoMemory(string filename);  // returns a texture index used when rendering
															// this creates a hardware
	void UnloadImage(VideoResourceID videoResourceID); // unloads image from wherever it resides

	CImage* GetImageFromSystemMemory(VideoResourceID videoResourceID);

	string GetNameFromResourceID(VideoResourceID videoResourceID);
	VideoResourceID GetResourceIDFromName(string name);
	MemoryTypes GetMemoryTypeFromResourceID(VideoResourceID videoResourceID);

	unsigned long GetSystemMemoryUsage();
	unsigned long GetVideoMemoryUsage();
private:
	struct TableEntry
	{
		string name;
		VideoResourceID videoResourceID;
		MemoryTypes memoryType;
	};

	void AddToTable(string name, VideoResourceID videoResourceID, MemoryTypes memoryType);
	VideoResourceID GetNewVideoResourceID(); // -1 is invalid id, all others are valid

	VideoResourceID resourceIdCount;
	unsigned long systemMemoryUsage;
	unsigned long videoMemoryUsage;
	vector<CImage*> systemImages;
	vector<COpenGLTexture*> openglImages;
	vector<TableEntry> resourceTable;
};

class vid_Settings
{
public:
	vid_Settings();
	~vid_Settings();

	void setBpp(const short& _bpp);
	void setSw(const short& _sw);
	void setSh(const short& _sh);
	void setSwScaled(const short& _sw);
	void setShScaled(const short& _sh);
	void setFullscreen(const bool& _fullscreen);
	void setZoom(const float& _zoom);

	const short& getBpp() const;
	const short& getSw() const;
	const short& getSh() const;
	short getSwScaled() const;
	short getShScaled() const;
	const bool& getFullscreen() const;
	const float& getZoom() const;

	bool lighting;

private:
	short swScaled;
	short shScaled;
	short bpp;
	short sw;
	short sh;
	bool fullscreen;
	float zoom;
};
class CVideoSubsystem: public IThinkSubsystem
{
public:
	CVideoSubsystem();
	virtual ~CVideoSubsystem();

	virtual bool PreInitialize(); // PreInitialize is called before Initialize, only links to other subsystems should be made here
	virtual bool Initialize();
	virtual bool Initialize(short width, short height, short bpp, bool fullscreen);
	virtual bool Shutdown();
	virtual void Think();

	virtual CRouterReturnCode EventReceiver(CEvent& event);

////////////////////////

	bool IsInitialized();

	void AddInitAuxFunction(void (* function)(void));

	void BeginDraw();
	void EndDraw();

	void ClearFrameBuffer(CColor color);

	void SetResolution(short width, short height, short bpp, bool fullscreen);

	/// do something here with a global resource manager

	void PutPixel(short x, short y, unsigned long color); 
	void PutPixelA(short x, short y, unsigned long color); // transparency
	void BlitBitmap(unsigned long* data,					v3d position, v3d size, float rotateangle = 0, v3d pivot = v3d(0,0,0), bool usecam = false, bool stencil = false);
	void BlitBitmap(VideoResourceID videoResourceID,		v3d position, v3d size, float rotateangle = 0, v3d pivot = v3d(0,0,0), bool usecam = false, bool stencil = false);
	void BlitBitmapScaled(VideoResourceID videoResourceID,	v3d position, v3d size, float rotateangle = 0, v3d pivot = v3d(0,0,0), bool usecam = false, bool stencil = false);
	void BlitRect(v3d p1, v3d p2, CColor color);
	void DrawLine(short x1, short y1, float zVal1, short x2, short y2, float zVal2, unsigned long color);
	void DrawLine(v3d start, v3d end, unsigned long color, bool usecam = true);
	void DrawLineScaled(v3d start, v3d end, unsigned long color, bool usecam = true);
	void DrawTriangle(short x1, short y1, short x2, short y2, short x3, short y3, unsigned long color); // wireframe for now
	void DrawTriangle3d(vid_Point p1, vid_Point p2, vid_Point p3, v3d normal, short textureId);
	void DrawModel(const CModel& model);
	void DrawTextShit(string text, short x, short y, unsigned short lineWidth);

	short MakeLight(unsigned long ambient, unsigned long diffuse); // returns an id to a useable light
	void KillLight(short id); // kills light with this id
	vid_SceneObject* GetLight(short id);
	void SetLightAmbient(short id, unsigned long ambient);
	void SetLightDiffuse(short id, unsigned long diffuse);

	vid_VCout vcout;
	CVideoResourceManager VideoResourceManager;
	vid_Camera camera;
	vid_Settings settings;
private:
	vector<void (*) (void)> m_initAuxList;
	V_SystemData* p_systemData; // for all the shit that requires funky headers and shit (audiere)
	
	//int idCount;
	bool initialized;
};

extern CVideoSubsystem Video;

class vid_Zone: public vid_SceneObject
{
public:
	vid_Zone():
		vid_SceneObject(zone)
	{
		layer = 2;
		colltype = permiable;
		boundtype = circle;
		boundingCircleRadius = 200; // slightly larger than distance to corner
	}
	~vid_Zone(){}

	void draw(DrawingModes mode, int _layer)
	{ 
		if(_layer != layer) return;

		v3d position = collapseTransforms().getTranslation();

		Video.DrawLineScaled( position + v3d(0, 0), position + v3d(0, 256), 0 );
		Video.DrawLineScaled( position + v3d(256, 0), position + v3d(256, 256), 0 );
		Video.DrawLineScaled( position + v3d(0, 256), position + v3d(256, 256), 0 );
		Video.DrawLineScaled( position + v3d(0, 0), position + v3d(256, 0), 0 );
//		Video.DrawTextShit("[ZONE]", position.worldSpacePosition(parent).x - Video.cam.getPosition().x, position.worldSpacePosition(parent).y - Video.cam.getPosition().y, 0);
	}
};
class vid_Tile: public vid_SceneObject
{
public:
	vid_Tile(v3d _position, v3d _size, string textureFileName);
	vid_Tile(const vid_Tile& rhs):vid_SceneObject(rhs),textureId(rhs.textureId){}
	~vid_Tile(){}

	vector<math_ExtrudedLineSegment> collisionBorders;

	void readTileProperties(ifstream& fin)
	{
		string token;
		fin >> token;
		while( (token != "begin_tile_properties") && !fin.eof() )
			fin >> token;
		
		fin >> token;
		while( (token != "end_tile_properties") && !fin.eof() )
		{
			if(token == "size")
			{
				fin >> size.x >> size.y;
				fin.ignore(1000, '\n');
			}

			fin >> token;
		}


		while( (token != "begin_collision_borders") && !fin.eof() ) // eat up anything before the collision map starts (comments, etc)
			fin >> token;

		collisionBorders.clear(); // empty the list

		fin >> token;
		while( (token != "end_collision_borders") && !fin.eof() )
		{
			if(token == "border")
			{
				// read the 2 points defining the line segment
				v3d p1, p2;
				fin >> p1.x >> p1.y >> p2.x >> p2.y;
				p1 += collapseTransforms().getTranslation();
				p2 += collapseTransforms().getTranslation();
				fin.ignore(1000, '\n');		// eat the rest of the line, comments, etc...

				// add the border to the list
				math_ExtrudedLineSegment border;
				border.defineLineSegment(p1, p2);
				collisionBorders.push_back(border);
			}

			fin >> token;
		}
	}

	void attachTexture(string fileName);

	void draw(DrawingModes mode, int layer);
	void rotate(v3d pivot, float xaxisangle, float yaxisangle, float zaxisangle);

	virtual float distance(v3d rhs){
		rhs-=(collapseTransforms().getTranslation()+size/2);
		return sqrt(rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z);}

	virtual float distanceSquared(v3d rhs){
		rhs-=(collapseTransforms().getTranslation()+size/2);
		return rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z;}

	short textureId; // used by the video subsystem to draw and manage the texture
};

class vid_Anim
{
public:
	vid_Anim();
	vid_Anim(vid_Anim& rhs);
	~vid_Anim();

	short update();
	void addFrame(short id, float duration); // add a frame to the animation
	short getFrame(){ if(head) return head->frametextureid; else return -1;} // get the currently active frame

	float nextframetime; // when sys_frameTime == this value, the frame changes
	//vid_Anim* parent; // this is the animation that continues after this animation
	bool loop; // if this is false, the animation plays once, then stops
	bool stack; // if this is true, the animation interrupts the current one then returns to that animation, otherwise it replaces the interrupted animation
	bool done; // this is true when the animation is done playing

	vid_Anim operator=(const vid_Anim& rhs);
private:

	class vid_AnimFrame
	{
	public:
		vid_AnimFrame():next(0), duration(0.0f), frametextureid(-1){	}
		~vid_AnimFrame();

		vid_AnimFrame* next;
		float duration;
		short frametextureid;
	};

	vid_AnimFrame* head;
};

class vid_AnimManager
{
private:
	class vid_AnimNode
	{
	public:
		vid_AnimNode():next(0), id(-1), animation(0) {}
		~vid_AnimNode()
		{ 
			if(animation) 
				delete animation; 
			animation = 0; 
		/*	if(next) 
				delete next; 
			next = 0;*/
		}

		vid_AnimNode* next;
		vid_Anim* animation;
		short id;
	};


	vid_AnimNode* animationpool;
	vid_AnimNode* currentAnimation;
public:

	vid_AnimManager():animationpool(0), currentAnimation(0){}
	vid_AnimManager(vid_AnimManager& rhs):animationpool(0), currentAnimation(0)
	{
		if(rhs.animationpool)
		{
			animationpool = new vid_AnimNode;
			vid_AnimNode* i = animationpool;
			for(vid_AnimNode* rhsi = rhs.animationpool; rhsi != 0; rhsi = rhsi->next)
			{
				i->animation = new vid_Anim;
				*(i->animation) = *(rhsi->animation);
				i->id = rhsi->id;

				if(rhsi->next)
				{
					i->next = new vid_AnimNode;
					i = i->next;
				}
			}
		}
		if(rhs.currentAnimation)
		{
			currentAnimation = 0;
			// todo
			// finish copying currentAnimation over
		}
	}
	vid_AnimManager operator=(const vid_AnimManager& rhs)
	{
		if(&rhs == this)
			return *this;

		if(rhs.animationpool)
		{
			animationpool = new vid_AnimNode;
			vid_AnimNode* i = animationpool;
			for(vid_AnimNode* rhsi = rhs.animationpool; rhsi != 0; rhsi = rhsi->next)
			{
				i->animation = new vid_Anim;
				*(i->animation) = *(rhsi->animation);
				i->id = rhsi->id;

				if(rhsi->next)
				{
					i->next = new vid_AnimNode;
					i = i->next;
				}
			}
		}
		if(rhs.currentAnimation)
		{
			currentAnimation = 0;
			// todo
			// finish copying currentAnimation over
		}
		return *this;
	}
	~vid_AnimManager()
	{
		for(vid_AnimNode* i = animationpool; i != 0; )
		{
			vid_AnimNode* temp = i->next;
			delete i;
			i = temp;
		}
		animationpool = 0; 

		for(vid_AnimNode* i = currentAnimation; i != 0; )
		{
			vid_AnimNode* temp = i->next;
			delete i;
			i = temp;
		}
		currentAnimation = 0;
	}

	void readFromFile(string file);
	short createAnim(short id, bool loop, bool stack);
	void addFrame(short animid, short textureid, float duration);
	bool playAnim(short id);
	short getCurrentAnimId();
	short getFrame();
	bool animationDone();
	short update();
};


class vid_AliasSet
{
public:
    vid_AliasSet();
	~vid_AliasSet();

	bool createAlias(vid_SceneObject* object, string name = "");
	void destroyAlias(string name);
	void destroyAlias(unsigned long uid);
	void destroyAlias(vid_SceneObject* object);

	unsigned long generateNewUid(string name);
	// post: uid assigned to correlating object is a new uid
	unsigned long generateNewUid(unsigned long uid);
	// post: uid assigned to correlating object is a new uid
	unsigned long generateNewUid(vid_SceneObject* object);
	// post: uid assigned to correlating object is a new uid

	void setName(string currentName, string newName);
	// pre: newName is not already registered
	void setName(unsigned long uid, string name);
	// pre: newName is not already registered
	void setName(vid_SceneObject* object, string name);
	// pre: newName is not already registered

	void setUid(string name, unsigned long uid);
	// pre: newUid is not already registered
	void setUid(unsigned long currentUid, unsigned long newUid);
	// pre: newUid is not already registered
	void setUid(vid_SceneObject* object, unsigned long uid);
	// pre: newUid is not already registered

	void setObject(string name, vid_SceneObject* object);
	// pre: newObject is not already registered
	void setObject(unsigned long uid, vid_SceneObject* object);
	// pre: newObject is not already registered
	void setObject(vid_SceneObject* currentObject, vid_SceneObject* newObject);
	// pre: newObject is not already registered

	string getName(unsigned long uid);
	string getName(vid_SceneObject* object);

	unsigned long getUid(string name);
	unsigned long getUid(vid_SceneObject* object);

	vid_SceneObject* getObject(string name);
	vid_SceneObject* getObject(unsigned long uid);

	bool uidIsUsed(unsigned long uid);

private:
	class vid_AliasNode
	{
	public:
		class vid_Alias
		{
		public:
			vid_Alias();
			// pre: none
			// post: object == 0, uid is 0(invalid), name is ""
			~vid_Alias();

			
			string _name; // an alias for human identifications (names are more natural than numbers to people)
			unsigned long _uid; // a unique number id
			vid_SceneObject* _object; // a pointer to the object this aliases
		};

		vid_AliasNode();
		~vid_AliasNode();

		vid_Alias alias;
		vid_AliasNode* next;
		vid_AliasNode* prev;
	};

	static unsigned long idCounter; // 0 is an invalid id... ?is that useful? ?for overflow?

	vid_AliasNode* head;
};
extern vid_AliasSet aliases;

class bn_Doodad: public vid_Tile
{
public:
	bn_Doodad():vid_Tile(0, v3d(32,32), ""),
				groundTexture(-1),
				shadowTexture(-1),
				objectTexture(-1),
				objectTextureLayer(6){};
	virtual ~bn_Doodad(){};

	void draw(DrawingModes mode, int layer);
protected:
	int objectTextureLayer;
	short	groundTexture,
			shadowTexture,
			objectTexture;
};

class bn_TreeDoodad: public bn_Doodad
{
public:
	bn_TreeDoodad():bn_Doodad()
	{
		//groundTexture = -1;//Video.VideoResourceManager.LoadImageToVideoMemory("data\images\world\land\Grass3x3seamless.bmp");
		shadowTexture = Video.VideoResourceManager.LoadImageToVideoMemory("data\\images\\world\\trees\\layered\\Large4x4shadow.bmp");
		objectTexture = Video.VideoResourceManager.LoadImageToVideoMemory("data\\images\\world\\trees\\layered\\Large4x4noaa.bmp");
		setSize(v3d(256, 256));
	}
	~bn_TreeDoodad(){}
};

#endif