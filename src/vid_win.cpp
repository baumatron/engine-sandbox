
#include "CVideoSubsystem.h"
#include "vid_win.h"
#include "sys_win.h"

#include "con_display.h"
#include "con_main.h"
#include "m_misc.h"
//#include "in_main.h"
//#include "bn_main.h"
#include "CModel.h"

#ifdef WIN32
#include <windows.h>								// Header File For Windows
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#ifdef WIN32
#include <SDL.h>								// Header File For Windows
#else
#include <SDL/SDL.h>
#endif

//#include <gl/glaux.h>




#include <string>
using namespace std;

#define NUM_LIGHTS 8

/*#ifdef WIN32
HGLRC           hRC=NULL;							// Permanent Rendering Context
HDC             hDC=NULL;							// Private GDI Device Context
#endif*/
////////////////////////////////////////////////////////////

VideoResourceID fontTextureId;




bool vidw_initialized(false);

class vidw_Light: public vid_SceneObject
{
public:
	vidw_Light(){}
	~vidw_Light(){}

	void update(){	
		if(!visible)
			return;
		if(!inuse)
			return;
		float r,g,b,a;
		r = _EXTRACTRED(ambient);
		g = _EXTRACTGREEN(ambient);
		b = _EXTRACTBLUE(ambient);
		a = _EXTRACTALPHA(ambient);
		r /= 256.0f;
		g /= 256.0f;
		b /= 256.0f;
		a /= 256.0f;
		GLfloat lightambient[] = {r, g, b, a};
		glLightfv(id, GL_AMBIENT, lightambient);				// Setup The Ambient Light
	
		r = _EXTRACTRED(diffuse);
		g = _EXTRACTGREEN(diffuse);
		b = _EXTRACTBLUE(diffuse);
		a = _EXTRACTALPHA(diffuse);
		r /= 256.0f;
		g /= 256.0f;
		b /= 256.0f;
		a /= 256.0f;	
		GLfloat lightdiffuse[] = {r, g, b, a};
		glLightfv(id, GL_DIFFUSE, lightdiffuse);				// Setup The Diffuse Light

		v3d temp = collapseTransforms().getTranslation();
		matrix4x4 trans;
		trans = TranslationMatrix(Video.camera.getPosition()*-1);
		temp = trans * temp;
		trans = RotationMatrix(Video.camera.getAngle()*-1);//mat.mtfXRot(Video.cam.getAngle().x*-1) * mat.mtfYRot(Video.cam.getAngle().y*-1) * mat.mtfZRot(Video.cam.getAngle().z*-1);
		temp = trans * temp;
	//	ccout << "light: " << collapseTransforms().getTranslation() << " light positon translated: " << temp << newl;
		/*v3d temp = position;
		temp.rotate(Video.cam.getPosition(), Video.cam.getAngle().x*-1, Video.cam.getAngle().y, Video.cam.getAngle().z*-1);
		temp -= Video.cam.getPosition();*/
/*	glRotatef(-Video.cam.getAngle().x,1, 0, 0);
	glRotatef(-Video.cam.getAngle().y,0, 1, 0);
	glRotatef(-Video.cam.getAngle().z,0, 0, 1);
	glTranslatef(-Video.cam.getPosition().x, -Video.cam.getPosition().y, -Video.cam.getPosition().z);*/

		//temp += v3d(vid_Settings.getSwScaled()/2, vid_Settings.getShScaled()/2);	////// ???????
		GLfloat lightposition[] = {temp.x, temp.y, temp.z, 1.0f};

		glLightfv(id, GL_POSITION, lightposition);			// Position The Light

		if(visible)
			glEnable(id);							// Enable Light One
		else
			glDisable(id);
		}

	short id;
	bool inuse; // if this is false, this light will be 
	unsigned long ambient;
	unsigned long diffuse;
};

short currentlightid(0); // incremented every time a light is made up to 7 then rolls over
vidw_Light lights[NUM_LIGHTS]; 


///////////////////////////////////////
void Flip();
//////////////////////////////


GLvoid VIDW_ReSizeGLScene(GLsizei width, GLsizei height)				// Resize And Initialize The GL Window
{
	if (height==0)								// Prevent A Divide By Zero By
	{
		height=1;							// Making Height Equal One
	}

	glViewport(0, 0, width, height);					// Reset The Current Viewport
	
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();							// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,width/height,2.0f,1000.0f);

	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	glLoadIdentity();							// Reset The Modelview Matrix
}

bool InitGL(GLvoid)								// All Setup For OpenGL Goes Here
{
	glEnable(GL_TEXTURE_2D);	// enables texturing
	glShadeModel(GL_SMOOTH);						// Enables Smooth Shading
	glClearColor(1.0f, 1.0f, 1.0f, 1);		
	glClearDepth(1.0f);								// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);							// The Type Of Depth Test To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Really Nice Perspective Calculations

	for(int i = 0; i < NUM_LIGHTS; i++) // tell each object which light it will use
	{
		lights[i].id = GL_LIGHT0+i; // this assumes that the real values of GL_LIGHT* are increasing by 1
	}

	
	return true;								// Initialization Went OK
}
void StartGLScene(GLvoid) // Here's Where We start All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// Clear The Screen And The Depth Buffer
}

GLvoid KillGLWindow(GLvoid)							// Properly Kill The Window
{
	SDL_Quit();
}

bool CreateGLWindow(char* title, int width, int height, int bpp, bool fullscreen)
{
	Video.settings.setBpp(bpp);
	Video.settings.setSw(width);
	Video.settings.setSh(height);
	Video.settings.setFullscreen(fullscreen);

	Uint32 flags;
	int size;

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't init SDL: %s\n", SDL_GetError());
		return false;
	}

	flags = SDL_OPENGL;
	if ( fullscreen ) {
		flags |= SDL_FULLSCREEN;
	}
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 );
	if ( SDL_SetVideoMode(width, height, 0, flags) == NULL ) {
		return false;
	}
	SDL_GL_GetAttribute( SDL_GL_STENCIL_SIZE, &size);

	VIDW_ReSizeGLScene(width, height);						// Set Up Our Perspective GL Screen

	if (!InitGL())										// Initialize Our Newly Created GL Window
	{
		KillGLWindow();									// Reset The Display
		return false;									// Return FALSE
	}

	return true;										// Success
}

void VIDW_Init()
{
	VIDW_Init(512, 384, 32, false);
}

void VIDW_Init(short width, short height, short bpp, bool fullscreen)
{
	if(vidw_initialized)
		return;
//	SYS_Message("VIDW_Init()");
	if(!CreateGLWindow("OpenGL Window", width, height, bpp, fullscreen))
	{
		//MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	if (!InitGL())								// Initialize Our Newly Created GL Window
	{
		KillGLWindow();							// Reset The Display
		//MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return;							// Return FALSE
	}

//	SYS_Message("VIDW_Init() returning");
	vidw_initialized = true;
}

void VIDW_UpdateLights()
{
	for(int i = 0; i < NUM_LIGHTS; i++)
	{
		lights[i].update();
	}
}

void VIDW_Frame()
{
	Flip();

}

void VIDW_Shutdown()
{
	if(!vidw_initialized)
		return;

	for(int i = 0; i < NUM_LIGHTS; i++)
	{
		VIDW_KillLight(i);
	}

	KillGLWindow();

	vidw_initialized = false;

	SDL_Quit();
}

void VIDW_DrawLine(v3d start, v3d end, unsigned long color, bool usecam)
{
	if(!vidw_initialized)
		return;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, Video.settings.getSw(), 0, Video.settings.getSh() );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	if(usecam)
	{	
		glTranslatef(Video.settings.getSw()/2, Video.settings.getSh()/2, 0);//Video.camPosition.x, Video.camPosition.y, Video.camPosition.z);
		glRotatef(-Video.camera.getAngle().z, 0, 0, 1);
		start-=Video.camera.getPosition();
		end-=Video.camera.getPosition();
	//	glTranslatef(/*Video.sw/2+*/pivot.camcoords(Video.cam).x, /*vid_Settings.getSh()/2+*/pivot.camcoords(Video.cam).y, 0);//Video.camPosition.x, Video.camPosition.y, Video.camPosition.z);
	//	position.x-=pivot.x;//Video.camPosition.x;
	//	position.y-=pivot.y;//Video.camPosition.y;
	//	glRotatef(rotateangle/*-Video.camAngle.z*/, 0, 0, 1);
	}
	else
	{
	//	glTranslatef(pivot.x, pivot.y, 0);
	//	position.x-=pivot.x;
	//	position.y-=pivot.y;
	//	glRotatef(rotateangle, 0, 0, 1);
	}

	if(/*Video.lighting*/false)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);


	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);						
		glColor3f( ((float)_EXTRACTRED(color))/256, ((float)_EXTRACTGREEN(color))/256, ((float)_EXTRACTBLUE(color))/256);
		glVertex3f(start.x,start.y,start.z); 

		glVertex3f(end.x,end.y,end.z); 
		glColor3f( 1.0, 1.0, 1.0 );
	glEnd();

//	glDisable(GL_BLEND);

//	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
}

void VIDW_DrawLineScaled(v3d start, v3d end, unsigned long color, bool usecam)
{
	if(!vidw_initialized)
		return;

	short width(Video.settings.getSwScaled()), height(Video.settings.getShScaled());

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, width, 0, height );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	if(usecam)
	{	
		glTranslatef(width/2, height/2, 0);
		glRotatef(-Video.camera.getAngle().z, 0, 0, 1);

		start-=Video.camera.getPosition();
		end-=Video.camera.getPosition();
/*		glTranslatef(pivot.camcoords(Video.cam).x, pivot.camcoords(Video.cam).y, 0);
		position.x-=pivot.x;
		position.y-=pivot.y;
		glRotatef(rotateangle, 0, 0, 1);*/
	}
	else
	{
/*		glTranslatef(pivot.x, pivot.y, 0);
		position.x-=pivot.x;
		position.y-=pivot.y;
		glRotatef(rotateangle, 0, 0, 1);*/
	}

/*	if(Video.lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);*/



	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing

	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);

//	glColor3f( 1, 1, 1 );
	glBegin(GL_LINES);
		glColor3f( ((float)_EXTRACTRED(color))/255.0, ((float)_EXTRACTGREEN(color))/255.0, ((float)_EXTRACTBLUE(color))/255.0 );
		glVertex3f(start.x,start.y,start.z);

		glVertex3f(end.x,end.y,end.z);
		glColor3f(1, 1, 1);
	glEnd();

//	glDisable(GL_BLEND);

//	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


}

void VIDW_DrawTriangle3d(vid_Point p1, vid_Point p2, vid_Point p3, v3d normal, short textureId)
{
	if(!vidw_initialized)
		return;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

//	if(vid_Settings.getShScaled())
//		gluPerspective(45.0f, ((GLsizei)vid_Settings.getSwScaled()) / ((GLsizei)vid_Settings.getShScaled()) ,0.1f,5000.0f);
		
	gluPerspective(85.0f, (double)Video.settings.getSw() / (double)Video.settings.getSh(), 2.0f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

/*	if(usecam)
	{	
		glTranslatef(width/2, height/2, 0);
		glRotatef(-Video.cam.getAngle().z, 0, 0, 1);

		glTranslatef(pivot.camcoords(Video.cam).x, pivot.camcoords(Video.cam).y, 0);
		position.x-=pivot.x;
		position.y-=pivot.y;
		glRotatef(rotateangle, 0, 0, 1);
	}
	else
	{
		glTranslatef(pivot.x, pivot.y, 0);
		position.x-=pivot.x;
		position.y-=pivot.y;
		glRotatef(rotateangle, 0, 0, 1);
	}*/


	glRotatef(-Video.camera.getAngle().x,1, 0, 0);
	glRotatef(-Video.camera.getAngle().y,0, 1, 0);
	glRotatef(-Video.camera.getAngle().z,0, 0, 1);
	glTranslatef(-Video.camera.getPosition().x, -Video.camera.getPosition().y, -Video.camera.getPosition().z);


	if(textureId >= 0)
	{
		/*if(GetTexture(textureId))
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureId);				// Select Our Texture
		}
		else*/
		{
			glDisable(GL_TEXTURE_2D);
		}
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	if(Video.settings.lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
//	glDisable(GL_BLEND);

	glBegin(GL_TRIANGLES);						// Drawing Using Triangles
		glNormal3f(normal.x, normal.y, normal.z);
		glColor3f( p1.color.getRFloat(), p1.color.getGFloat(), p1.color.getBFloat() );
		glTexCoord2f( p1.textureCoordinates.x, p1.textureCoordinates.y ); 
		glVertex3f( p1.position.x, p1.position.y, p1.position.z);	// Top

		glColor3f( p2.color.getRFloat(), p2.color.getGFloat(), p2.color.getBFloat() );
		glTexCoord2f( p2.textureCoordinates.x, p2.textureCoordinates.y ); 
		glVertex3f( p2.position.x, p2.position.y, p2.position.z);	// Top

		glColor3f( p3.color.getRFloat(), p3.color.getGFloat(), p3.color.getBFloat() );
		glTexCoord2f( p3.textureCoordinates.x, p3.textureCoordinates.y ); 
		glVertex3f( p3.position.x, p3.position.y, p3.position.z);	// Top
	glEnd();

	glColor3f(1,1,1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
void VIDW_DrawTriangle(float x1,float y1,float x2,float y2,float x3,float y3,unsigned long color)
{

}

void VIDW_DrawModel(const CModel& model)
{
	if(!vidw_initialized)
		return;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if(Video.settings.getSh())
		gluPerspective(85.0f, (double)Video.settings.getSw() / (double)Video.settings.getSh(), 2.0f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRotatef(-Video.camera.getAngle().x,1, 0, 0);
	glRotatef(-Video.camera.getAngle().y,0, 1, 0);
	glRotatef(-Video.camera.getAngle().z,0, 0, 1);
	glTranslatef(-Video.camera.getPosition().x, -Video.camera.getPosition().y, -Video.camera.getPosition().z);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	for(int i = 0; i < model.m_meshCount; i++)
	{
		int materialIndex = model.m_pMeshes[i].m_materialIndex;
		if ( materialIndex >= 0 )
		{
			glMaterialfv( GL_FRONT, GL_AMBIENT, model.m_pMaterials[materialIndex].m_ambient );
			glMaterialfv( GL_FRONT, GL_DIFFUSE, model.m_pMaterials[materialIndex].m_diffuse );
			glMaterialfv( GL_FRONT, GL_SPECULAR, model.m_pMaterials[materialIndex].m_specular );
			glMaterialfv( GL_FRONT, GL_EMISSION, model.m_pMaterials[materialIndex].m_emissive );
			glMaterialf( GL_FRONT, GL_SHININESS, model.m_pMaterials[materialIndex].m_shininess );

			if (model.m_pMaterials[materialIndex].m_texture > 0 )
			{
				glBindTexture( GL_TEXTURE_2D, model.m_pMaterials[materialIndex].m_texture );
				glEnable( GL_TEXTURE_2D );
			}
			else
				glDisable( GL_TEXTURE_2D );
		}
		else
		{
			glDisable( GL_TEXTURE_2D );
		}



		if(Video.settings.lighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
	//	glDisable(GL_BLEND);

		glBegin(GL_TRIANGLES);						// Drawing Using Triangles
			for(int j = 0; j < model.m_pMeshes[i].m_triangleCount; j++)
			{
				int index = model.m_pMeshes[i].m_pTriangleIndices[j];
				for(int k = 0; k < 3; k++)
				{
					glNormal3fv(  model.m_pTriangles[ index ].m_vertexNormals[ k ] );
					glTexCoord2f( model.m_pTriangles[ index ].m_s[ k ], model.m_pTriangles[ index ].m_t[ k ] ); 
					glVertex3fv( model.m_pVertices[ model.m_pTriangles[ index ].m_vertexIndices[ k ] ].m_position );	// Top
				}
			}
		glEnd();
	}
		
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void VIDW_BlitBitmap(unsigned long* data, v3d position, v3d size, float rotateangle, v3d pivot, bool usecam, bool stencil)
{
//	return;
	if(!vidw_initialized)
		return;
	/*unsigned char* temp = new unsigned char[size.x*size.y*4];
	for(int i = 0; i < size.x*size.y; i++)
	{
		temp[i*4] = _EXTRACTRED(data[i]);
		temp[i*4+1] = _EXTRACTGREEN(data[i]);
		temp[i*4+2] = _EXTRACTBLUE(data[i]);
		temp[i*4+3] = _EXTRACTALPHA(data[i]);
	}*/

	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glEnable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, Video.settings.getSw(), 0, Video.settings.getSh() );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	if(usecam)
	{
		pivot-=Video.camera.getPosition();
		glTranslatef(Video.settings.getSw()/2+pivot.x, Video.settings.getSh()/2+pivot.y, 0);//Video.camPosition.x, Video.camPosition.y, Video.camPosition.z);
		position.x-=Video.camera.getPosition().x+pivot.x;//Video.camPosition.x;
		position.y-=Video.camera.getPosition().y+pivot.y;//Video.camPosition.y;
		glRotatef(rotateangle-Video.camera.getAngle().z, 0, 0, 1);
	}
	else
	{
		glTranslatef(pivot.x, pivot.y, 0);
		position.x-=pivot.x;
		position.y-=pivot.y;
		glRotatef(rotateangle, 0, 0, 1);
	}

	glDisable(GL_TEXTURE_2D);

	if(Video.settings.lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
//	glDisable(GL_BLEND);

	glColor3f( 1.0, 1.0, 1.0 );

	glRasterPos2i (position.x, position.y);
	glDrawPixels(size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)data); 


	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

//	delete []temp;
}

void VIDW_BlitBitmap(short id, v3d position, v3d size, float rotateangle, v3d pivot, bool usecam, bool stencil)
{
	if(!vidw_initialized)
		return;
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, Video.settings.getSw(), 0, Video.settings.getSh() );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	if(usecam)
	{	
		glTranslatef(Video.settings.getSw()/2, Video.settings.getSh()/2, 0);//Video.camPosition.x, Video.camPosition.y, Video.camPosition.z);
		glRotatef(-Video.camera.getAngle().z, 0, 0, 1);

		glTranslatef(/*Video.sw/2+*/pivot.camcoords(Video.camera).x, /*vid_Settings.getSh()/2+*/pivot.camcoords(Video.camera).y, 0);//Video.camPosition.x, Video.camPosition.y, Video.camPosition.z);
		position.x-=pivot.x;//Video.camPosition.x;
		position.y-=pivot.y;//Video.camPosition.y;
		glRotatef(rotateangle/*-Video.camAngle.z*/, 0, 0, 1);
	}
	else
	{
		glTranslatef(pivot.x, pivot.y, 0);
		position.x-=pivot.x;
		position.y-=pivot.y;
		glRotatef(rotateangle, 0, 0, 1);
	}

	if(Video.settings.lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing

	glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glBindTexture(GL_TEXTURE_2D, id);				// Select Our Texture


	glColor3f( 1.0, 1.0, 1.0 );
	glBegin(GL_QUADS);						
		glTexCoord2f( 0, 0 );					// select character coordinate
		glVertex2f(position.x,position.y); //0,0

		glTexCoord2f( 0, 1 );
		glVertex2f(position.x,position.y+size.y); // 0,1

		glTexCoord2f( 1, 1 );
		glVertex2f(position.x+size.x,position.y+size.y);		// 1,1

		glTexCoord2f( 1, 0 );
		glVertex2f(position.x+size.x,position.y);	// 1,0
	glEnd();

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
}

void VIDW_BlitBitmapScaled(short id, v3d position, v3d size, float rotateangle, v3d pivot, bool usecam, bool stencil)
{
	if(!vidw_initialized)
		return;
//	short tempx = Video.sw, tempy = vid_Settings.getSh();
	short width(Video.settings.getSwScaled()), height(Video.settings.getShScaled());
//	Video.sw = 512;
//	vid_Settings.getSh() = 384;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, width, 0, height );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	if(usecam)
	{	
		glTranslatef(width/2, height/2, 0);
		glRotatef(-Video.camera.getAngle().z, 0, 0, 1);

		glTranslatef(pivot.camcoords(Video.camera).x, pivot.camcoords(Video.camera).y, 0);
		position.x-=pivot.x;
		position.y-=pivot.y;
		glRotatef(rotateangle, 0, 0, 1);
	}
	else
	{
		glTranslatef(pivot.x, pivot.y, 0);
		position.x-=pivot.x;
		position.y-=pivot.y;
		glRotatef(rotateangle, 0, 0, 1);
	}

	if(Video.settings.lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);


//	if(stencil) // CHECK OUT glStencilFunc
//	{

	glEnable(GL_TEXTURE_2D);


	glEnable(GL_DEPTH_TEST);							// Disables Depth Testing

	glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glBindTexture(GL_TEXTURE_2D, id);				// Select Our Texture


	glColor3f( 1.0, 1.0, 1.0 );
	glBegin(GL_QUADS);						
		glTexCoord2f( 0, 0 );					// select character coordinate
		glVertex2f(position.x,position.y); //0,0
	//	glVertex2f(0,0);

		glTexCoord2f( 0, 1 );
		glVertex2f(position.x,position.y+size.y); // 0,1
	//	glVertex2f(0,height);

		glTexCoord2f( 1, 1 );
		glVertex2f(position.x+size.x,position.y+size.y);		// 1,1
	//	glVertex2f(width, height);

		glTexCoord2f( 1, 0 );
		glVertex2f(position.x+size.x,position.y);	// 1,0
	//	glVertex2f(width, 0);
	glEnd();

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

//	Video.sw = tempx;
//	vid_Settings.getSh() = tempy;
}


void Flip()
{
  SDL_GL_SwapBuffers();
  //SDL_Flip();
//	SwapBuffers(hDC);			// Swap Buffers (Double Buffering)
}




void VIDW_ClearBackBuffer(unsigned long color)
{

}

void VIDW_BlitRect(v3d p1, v3d p2, CColor color)
{
	if(!vidw_initialized)
		return;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, Video.settings.getSw(), 0, Video.settings.getSh() );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glBegin(GL_QUADS);						
		glColor4f( color.getRFloat(), color.getGFloat(), color.getBFloat(), color.getAFloat() );
		glVertex2f(p1.x,p1.y); //0,0

		glVertex2f(p1.x,p2.y); // 0,1

		glVertex2f(p2.x,p2.y);		// 1,1

		glVertex2f(p2.x,p1.y);	// 1,0
		glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
}



/*
vidw_Texture::vidw_Texture()
{
}

vidw_Texture::~vidw_Texture()
{
	if(bitmap)
	{
		delete [] bitmap->buffer;
		delete bitmap;
		bitmap = 0;
	}
	// opengl texture unloading code goes here
}

void vidw_Texture::init(string newfilename, short newindex) // sets up the texture, requires index to have a valid number
{	
	index = newindex;
	filename = newfilename;
	nextTexture = 0;
	bitmap = new BITMAP_FILE;
	RES_LoadBitmapFile(bitmap, filename);

	glBindTexture(GL_TEXTURE_2D, index);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); // this fucker messes up blending

//	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bitmap->bitmapinfoheader.biWidth, bitmap->bitmapinfoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->buffer); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->bitmapinfoheader.biWidth, bitmap->bitmapinfoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->buffer);

	delete bitmap;
	bitmap = 0;
}

string VIDW_GetNameForTexId(short textureId)
{
	vidw_Texture* temp = GetTexture(textureId);
	return temp->filename;
}

short VIDW_LoadGLTexture(string filename) // takes a file name and returns an index
{
	// case 1: list is empty
	if(!textureList) // list is empty
	{
		textureList = new vidw_Texture();
		textureList->init(filename, 0);
//		CON_Message("Loading new texture: "+filename+" ID: "+M_itoa(textureList->index)+newl);
		return textureList->index;
	}
	else // add command to end of list
	{
		// search for the texture to see if it has been loaded already
		vidw_Texture* current = textureList; // initialize tracker
		while (current)
		{
			if(current->filename == filename)
			{
//				CON_Message("Found new instance of texture: "+filename+" ID: "+M_itoa(current->index)+newl);
				return current->index;
			}
			current = current->nextTexture;
		}

		current = textureList;
		while (current->nextTexture)
		{
			current = current->nextTexture;
		}
		current->nextTexture = new vidw_Texture();
		current->nextTexture->init(filename, current->index+1);
//		CON_Message("Loading new texture: "+filename+" ID: "+M_itoa(current->index+1)+newl);
		return current->nextTexture->index;
	}
}

vidw_Texture* GetTexture(short textureId)
{
	if(!textureList)
		return 0;  // whoever uses this function must take care of this situation
	vidw_Texture* current = textureList;
	while ((current->nextTexture) && (current->index != textureId)) // go to next in list if there is a next and the current index is not the one requested... note that if the list runs out before the index is reached, the last texture in the list will be used
	{
		current = current->nextTexture;
	}
	return current; // return the address of the requested texture object
}

void VIDW_KillTextureList() // cleans out the texture LL
{
	if(!textureList) // list is empty
		return;

	vidw_Texture* current = textureList;
	vidw_Texture* next = 0;

	while(current)
	{
		next = current->nextTexture;
		delete current;
		current = next;
	}

	textureList = 0;
}*/

void VIDW_LoadBitmapFont()
{
	fontTextureId = Video.VideoResourceManager.LoadImageToVideoMemory("data/images/system/conchars.tga");
}

void VIDW_DrawChar(char character, short x, short y)
{
	if(!vidw_initialized)
		return;

	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();
	gluOrtho2D( 0, Video.settings.getSw(), 0, Video.settings.getSh() );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE);					// Select The Type Of Blending
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//	glShadeModel(GL_SMOOTH);							// Enables Smooth Color Shading

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fontTextureId);				// Select Our Texture

	short row = character/16;
	short column = character%16;

	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin(GL_QUADS);						
		glTexCoord2f( column*.0625, (row+1)*.0625 );					// select character coordinate
		glVertex2f(x,y); //0,0

		glTexCoord2f( column*.0625, row*.0625 );
		glVertex2f(x,y+8); // 0,1

		glTexCoord2f( (column+1)*.0625, row*.0625 );
		glVertex2f(x+8,y+8);		// 1,1

		glTexCoord2f( (column+1)*.0625, (row+1)*.0625 );
		glVertex2f(x+8,y);	// 1,0
	glEnd();
	
	glDisable(GL_BLEND);
	
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
}


void VIDW_PutPixel(short x, short y, unsigned long color)
{
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, Video.settings.getSw(), 0, Video.settings.getSh() );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	glDisable(GL_TEXTURE_2D);
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, texture);				// Select Our Texture

//	glDisable(GL_BLEND);

	glColor3f( ((float)_EXTRACTRED(color))/256.0f , ((float)_EXTRACTGREEN(color))/256.0f , ((float)_EXTRACTBLUE(color))/256.0f );
	glTranslated(x, y, 0);
	glRecti(0, 0, 1, 1);

	/*	glBegin(GL_QUADS);						
		glTexCoord2f( 0, 1 );					// select character coordinate
		glVertex2f(x1,y1); //0,0

		glTexCoord2f( 0, 0 );
		glVertex2f(x1,y2); // 0,1

		glTexCoord2f( 1, 0 );
		glVertex2f(x2,y2);		// 1,1

		glTexCoord2f( 1, 1 );
		glVertex2f(x2,y1);	// 1,0
	glEnd();*/
	
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
}
short VIDW_MakeLight(unsigned long ambient, unsigned long diffuse) // returns an id to a useable light, -1 if a light is not available
{
	for(int i = 0; i < NUM_LIGHTS; i++)
	{
		if( lights[i].inuse == false )
		{
			lights[i].inuse = true;
			lights[i].ambient = ambient;
			lights[i].diffuse = diffuse;
//			ccout << "Light created at index: " << i << newl;
			return i;
		}
	}
//	ccout << "couldn't make a light." << newl;
	return -1;
}
void VIDW_KillLight(short id) // kills light with this id
{
//	ccout << "Killing light: " << id << newl;
	if( (id >= NUM_LIGHTS) || (id < 0) )
		return;
	lights[id].inuse = false;
	lights[id].visible = false;
}

vid_SceneObject* VIDW_GetLight(short id)
{
	if( (id >= NUM_LIGHTS) || (id < 0) )
		return 0;
	else
		return &lights[id];
}

void VIDW_SetLightAmbient(short id, unsigned long ambient)
{
	if(VIDW_GetLight(id))
		((vidw_Light*)VIDW_GetLight(id))->ambient = ambient;
}

void VIDW_SetLightDiffuse(short id, unsigned long diffuse)
{
	if(VIDW_GetLight(id))
		((vidw_Light*)VIDW_GetLight(id))->diffuse = diffuse;
}

vidw_Font::vidw_Font():
height(12),
weight(500)
{
}

vidw_Font::~vidw_Font()
{
	glDeleteLists(baseDisplayList, 96);		
}

void vidw_Font::SetFont(const string& _fontName)
{
	fontName = _fontName;
	RegenFont();
}

void vidw_Font::SetHeight(const unsigned short& _height) // height in pixels (scaled pixels, not actual resolution)
{
	height = _height;
	RegenFont();
}

void vidw_Font::SetWeight(const unsigned short& _weight) // a number from 0-1000 0 is light 1000 is dark
{
	weight = _weight;
	RegenFont();
}

void vidw_Font::RegenFont()
{
#ifdef WIN32
	HFONT font, oldFont;
	glDeleteLists(baseDisplayList, 96);		
	baseDisplayList = glGenLists(96);
	font = CreateFont(	-height, // height, - means based on CHARACTER height
						0, // width
						0, // angle of escapement
						0, // orientation angle
						weight, // weight
						FALSE, // italic
						FALSE, // underline
						FALSE, // strikeout
						ANSI_CHARSET /*SHIFTJIS_CHARSET*/, // TYPE OF CHARSET TO USE
						OUT_TT_PRECIS, // output precision
						CLIP_DEFAULT_PRECIS, // clipping precision
						ANTIALIASED_QUALITY, // Output Quality
						FF_DONTCARE|DEFAULT_PITCH,	// Family And Pitch
						fontName.c_str());			// Font Name

//	oldFont = (HFONT)SelectObject(hDC, font); // select the font and store old font in oldFont
//	wglUseFontBitmaps(hDC, 32, 96, baseDisplayList); // Builds 96 Characters Starting At Character 32
//	SelectObject(hDC, oldFont);	// set the font back to what it was
	DeleteObject(font);					// Delete The Font
#endif
}

void vidw_Font::PutText(string text) const
{
	glPushAttrib(GL_LIST_BIT);				// Pushes The Display List Bits
	glListBase(baseDisplayList - 32);					// Sets The Base Character to 32
	glCallLists(text.size(), GL_UNSIGNED_BYTE, text.c_str());	// Draws The Display List Text
	glPopAttrib();						// Pops The Display List Bits
}

void VIDW_PutText(string text, v3d position, CColor color, const vidw_Font &font)
{
	short width(Video.settings.getSw()), height(Video.settings.getSh());

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, width, 0, height );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glDisable(GL_TEXTURE_2D);

	glColor3f(color.getRFloat(), color.getGFloat(), color.getBFloat());

	// glut stroked
//	glTranslatef(position.x, position.y, 0.0);
//	glScalef(0.075, 0.075, 0.075);
//	for(int i = 0; i < text.size(); i++)
//		glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
//	glScalef(1, 1, 1);
	
	// glut bitmapped
	glRasterPos2f(position.x, position.y);
	for(int i = 0; i < text.size(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
	
	//font.PutText(text);

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

}