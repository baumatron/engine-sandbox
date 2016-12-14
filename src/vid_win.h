#ifndef VID_WIN_H
#define VID_WIN_H

#include "CVideoSubsystem.h"
#include "CModel.h"
#include <string>


using namespace std;


void VIDW_Init();
void VIDW_Init(short width, short height, short bpp, bool fullscreen);
void VIDW_Frame();
void VIDW_Shutdown();

void VIDW_UpdateLights();

void StartGLScene();
void VIDW_SetResolution(short width, short height, short bpp, bool fullscreen);

void VIDW_DrawLine(v3d start, v3d end, unsigned long color, bool usecam = true);
void VIDW_DrawLineScaled(v3d start, v3d end, unsigned long color, bool usecam = true);

void VIDW_DrawTriangle3d(vid_Point p1, vid_Point p2, vid_Point p3, v3d normal, short textureId);
void VIDW_DrawTriangle(float x1,float y1,float x2,float y2,float x3,float y3,unsigned long color);

void VIDW_DrawModel(const CModel& model);

void VIDW_BlitBitmap(unsigned long* data, v3d position, v3d size, float rotateangle = 0, v3d pivot = v3d(0,0,0), bool usecam = false, bool stencil = false);
void VIDW_BlitBitmap(short id           , v3d position, v3d size, float rotateangle = 0, v3d pivot = v3d(0,0,0), bool usecam = false,  bool stencil = false);
void VIDW_BlitBitmapScaled(short id     , v3d position, v3d size, float rotateangle = 0, v3d pivot = v3d(0,0,0), bool usecam = false,  bool stencil = false);
void VIDW_BlitRect(v3d p1, v3d p2, CColor color);

void VIDW_ReSizeGLScene(int width, int height);

void VIDW_ClearBackBuffer(unsigned long color);

string VIDW_GetNameForTexId(short textureId);

short VIDW_LoadGLTexture(string fileName); // takes a file name and returns an index
void VIDW_KillTextureList(); // cleans out the texture LL
void VIDW_LoadBitmapFont();
void VIDW_DrawChar(char character, short x, short y);
void VIDW_PutPixel(short x, short y, unsigned long color); 
short VIDW_MakeLight(unsigned long ambient, unsigned long diffuse); // returns an id to a useable light
void VIDW_KillLight(short id); // kills light with this id
vid_SceneObject* VIDW_GetLight(short id);
void VIDW_SetLightAmbient(short id, unsigned long ambient);
void VIDW_SetLightDiffuse(short id, unsigned long diffuse);

class vidw_Texture;

extern bool vidw_initialized;

class vidw_Font
{
public:
	vidw_Font();
	~vidw_Font();

	void SetFont(const string& _fontName);
	void SetHeight(const unsigned short& _height); // height in pixels (scaled pixels, not actual resolution)
	void SetWeight(const unsigned short& _weight); // a number from 0-1000 0 is light 1000 is dark
	void PutText(string text) const;
private:
	void RegenFont();
	string fontName;
	unsigned short height, weight;
	unsigned int baseDisplayList;							// Base Display List For The Font Set
};


void VIDW_PutText(string text, v3d position, CColor color, const vidw_Font& font);

#endif