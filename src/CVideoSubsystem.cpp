
#ifdef WIN32
#include <windows.h>								// Header File For Windows
#endif

#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glut.h>
#include <glod.h>


#ifdef WIN32
#include <SDL.h>								// Header File For Windows
#else
#include <SDL/SDL.h>
#endif

#include "CImage.h"
#include "CColor.h"
#include "CBitmapImageLoader.h"
#include "CTargaImageLoader.h"
#include "CVideoSubsystem.h"
#include "CFontManager.h"
#include "CXMLParser.h"

#include "con_main.h"
#include "con_display.h"
#include "vid_win.h"
//#include "in_main.h"
#include "sys_main.h"


#include <string>
#include <stack>
#include <vector>
using namespace std;

CVideoSubsystem Video;





class V_SystemData
{
public:
	vid_SceneGraph clientSceneGraph;
	vid_SceneGraph twoDStaticSceneGraph;
	vid_SceneGraph threeDStaticSceneGraph;
	vid_SceneGraph serverSceneGraph;
};




CVideoResourceManager::CVideoResourceManager():
	resourceIdCount(-1),
	systemMemoryUsage(0),
	videoMemoryUsage(0)
{
}
CVideoResourceManager::~CVideoResourceManager()
{
}

VideoResourceID CVideoResourceManager::LoadImageToSystemMemory(string filename) // returns a video resource id for the image loaded
														// this loads into the CImage class
{
	if(!M_FileExists(filename))
	{
		return -1;
	}

	CImage* image = new CImage;

	VideoResourceID videoResourceID;
	if((videoResourceID = GetResourceIDFromName(filename)) != -1) 
	{ // need to open and read the file because it is not already loaded
		// file has already been loaded
		// first see if the id corresponds to a system memory resource... if so, we're done
		if(GetMemoryTypeFromResourceID(videoResourceID) == system)
		{
			return videoResourceID;
		}
	}

	// file has not already been loaded
	// load it if we have an image loader for it
	if(TargaImageLoader.IsReaderForFile(filename))
	{
		// its a tga
		TargaImageLoader.ReadFromFile(*image, filename);
		image->SetVideoResourceID(GetNewVideoResourceID());
		systemImages.push_back(image);
		AddToTable(filename, image->GetVideoResourceID(), system);
		systemMemoryUsage += image->GetMemoryUsage();
		return image->GetVideoResourceID();
	}

	if(BitmapImageLoader.IsReaderForFile(filename))
	{
		// it's a bitmap
		BitmapImageLoader.ReadFromFile(*image, filename);
		image->SetVideoResourceID(GetNewVideoResourceID());
		systemImages.push_back(image);
		AddToTable(filename, image->GetVideoResourceID(), system);
		systemMemoryUsage += image->GetMemoryUsage();
		return image->GetVideoResourceID();
	}

	ccout << "Error loading video resource. There is no image loader class for the file specified.";
	return -1; // didn't have a loader for the file
}
VideoResourceID CVideoResourceManager::LoadImageToVideoMemory(string filename)  // returns a texture index used when rendering
														// this creates a hardware
{
	int videoResourceID;
	CImage* systemImage;
	bool leaveImageInSystemMemory(true);

	if((videoResourceID = GetResourceIDFromName(filename)) == -1) 
	{ // need to open and read the file because it is not already loaded
		leaveImageInSystemMemory = false;
		videoResourceID = LoadImageToSystemMemory(filename);
	}
	else
	{
		// file has been loaded
		// first see if the id corresponds to a video memory resource... if so, we're done
		if(GetMemoryTypeFromResourceID(videoResourceID) == video)
		{
			return videoResourceID;
		}
	}
	systemImage = GetImageFromSystemMemory(videoResourceID);

	if(videoResourceID == -1)
	{
		// image must not exist, or there is some other error loading it
		ccout << "Error loading " << filename << " to video memory.\n";
		return -1;
	}


// TODO: CHECK TO SEE IF THE IMAGE IS THE CORRECT DIMENSIONS TO WORK IN TEXTURE MEMORY
//		IF NOT, MAYBE SCALE IT?

	COpenGLTexture* texture = new COpenGLTexture;
	texture->SetVideoResourceID(GetNewVideoResourceID());
	texture->memoryUsage = systemImage->GetMemoryUsage(); // not exactly correct, probably
	openglImages.push_back(texture);
	AddToTable(filename, texture->GetVideoResourceID(), video);
	videoMemoryUsage += texture->GetMemoryUsage();
	
	// now opengl stuff

	glBindTexture(GL_TEXTURE_2D, texture->GetVideoResourceID());

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Mipmap Linear Filtering

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, systemImage->GetWidth(), systemImage->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, systemImage->GetBuffer()); 
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, systemImage->GetWidth(), systemImage->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, systemImage->GetBuffer());


	if(!leaveImageInSystemMemory)
		UnloadImage(videoResourceID);



	return texture->GetVideoResourceID();
}
void CVideoResourceManager::UnloadImage(VideoResourceID videoResourceID) // unloads image from wherever it resides
{
	// first check the table
	for(vector<TableEntry>::iterator it = resourceTable.begin(); it != resourceTable.end(); it++)
	{
		if(it->videoResourceID == videoResourceID)
		{
			resourceTable.erase(it);
		}
	}

	// now check system images
	for(vector<CImage*>::iterator it = systemImages.begin(); it != systemImages.end(); it++)
	{
		if((*it)->GetVideoResourceID() == videoResourceID)
		{
			systemMemoryUsage -= (*it)->GetMemoryUsage();
			delete *it;
			systemImages.erase(it);
			return;
		}
	}

	// now check video images
	for(vector<COpenGLTexture*>::iterator it = openglImages.begin(); it != openglImages.end(); it++)
	{
		if((*it)->GetVideoResourceID() == videoResourceID)
		{
			systemMemoryUsage -= (*it)->GetMemoryUsage();
			GLuint id = ((*it)->GetVideoResourceID());
			glDeleteTextures(1, &id);
			delete *it;
			openglImages.erase(it);
			return;
		}
	}
}

CImage* CVideoResourceManager::GetImageFromSystemMemory(VideoResourceID videoResourceID)
{
	for(vector<CImage*>::iterator it = systemImages.begin(); it != systemImages.end(); it++)
	{
		if((*it)->GetVideoResourceID() == videoResourceID)
		{
			return *it;
		}
	}
	return 0;
}

string CVideoResourceManager::GetNameFromResourceID(VideoResourceID videoResourceID)
{
	for(vector<TableEntry>::iterator it = resourceTable.begin(); it != resourceTable.end(); it++)
	{
		if(it->videoResourceID == videoResourceID)
		{
			return it->name;
		}
	}

	return "";
}
	
VideoResourceID CVideoResourceManager::GetResourceIDFromName(string name)
{
	for(vector<TableEntry>::iterator it = resourceTable.begin(); it != resourceTable.end(); it++)
	{
		if(it->name == name)
		{
			return it->videoResourceID;
		}
	}
	return -1;
}

CVideoResourceManager::MemoryTypes CVideoResourceManager::GetMemoryTypeFromResourceID(VideoResourceID videoResourceID)
{
	for(vector<TableEntry>::iterator it = resourceTable.begin(); it != resourceTable.end(); it++)
	{
		if(it->videoResourceID == videoResourceID)
		{
			return it->memoryType;
		}
	}
	return none;
}


unsigned long CVideoResourceManager::GetSystemMemoryUsage()
{
	return systemMemoryUsage;
}
unsigned long CVideoResourceManager::GetVideoMemoryUsage()
{
	return videoMemoryUsage;
}


void CVideoResourceManager::AddToTable(string name, VideoResourceID videoResourceID, MemoryTypes memoryType)
{
	TableEntry entry;
	entry.name = name;
	entry.videoResourceID = videoResourceID;
	entry.memoryType = memoryType;
	resourceTable.push_back(entry);
}


VideoResourceID CVideoResourceManager::GetNewVideoResourceID()
{
	return ++resourceIdCount;
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*CON_CMD(camera_reset)
{
	Video.camera.destination = TranslationMatrix(v3d(0,2,-50)) * RotationMatrix(v3d(10, 180));
	Video.camera.view = v3d(0,0,0);
}

CON_CMD(camera_back)
{
	v3d temp(0.0f,0.0f,100*sys_frameTime);
	Video.camera.destination *= TranslationMatrix(temp);
	Video.camera.position += temp;
}
CON_CMD(camera_forward)
{
	v3d temp(0.0f,0.0f,-100*sys_frameTime);
	Video.camera.destination *= TranslationMatrix(temp);
	Video.camera.position += temp;
}

CON_CMD(camera_left)
{
	v3d temp(-100*sys_frameTime,0.0f);
	Video.camera.destination *= TranslationMatrix(temp);
	Video.camera.position += temp;
}
CON_CMD(camera_right)
{
	v3d temp(100*sys_frameTime,0.0f);
	Video.camera.destination *= TranslationMatrix(temp);
	Video.camera.position += temp;
}
CON_CMD(camera_up)
{
	v3d temp(0.0f,100*sys_frameTime);
	Video.camera.destination *= TranslationMatrix(temp);
	Video.camera.position += temp;
}
CON_CMD(camera_down)
{
	v3d temp(0.0f,-100*sys_frameTime);
	Video.camera.destination *= TranslationMatrix(temp);
	Video.camera.position += temp;
}
CON_CMD(camera_orbit_left)
{

}
CON_CMD(camera_orbit_right)
{

}


CON_CMD(camera_yaw_left)
{
	Video.camera.destination *= matrix4x4::mtfRotateOnAxis(v3d(0,1,0), 90*sys_frameTime);
}
CON_CMD(camera_yaw_right)
{
	Video.camera.destination *= matrix4x4::mtfRotateOnAxis(v3d(0,1,0), -90*sys_frameTime);
}
CON_CMD(camera_pitch_up)
{
	v3d axis(1,0,0);
	axis.rotate(0, Video.camera.destination.getRot());
	Video.camera.destination *= matrix4x4::mtfRotateOnAxis(axis, 90*sys_frameTime);
}
CON_CMD(camera_pitch_down)
{
	v3d axis(1,0,0);
	axis.rotate(0, Video.camera.destination.getRot());
	Video.camera.destination *= matrix4x4::mtfRotateOnAxis(axis, -90*sys_frameTime);
}*/

void VID_CON_InitAux()
{
	/*CON_CMD_REG(camera_reset);
	CON_CMD_REG(camera_forward);
	CON_CMD_REG(camera_back);
	CON_CMD_REG(camera_left);
	CON_CMD_REG(camera_right);
	CON_CMD_REG(camera_up);
	CON_CMD_REG(camera_down);
	CON_CMD_REG(camera_orbit_left);
	CON_CMD_REG(camera_orbit_right);
	CON_CMD_REG(camera_yaw_left);
	CON_CMD_REG(camera_yaw_right);
	CON_CMD_REG(camera_pitch_up);
	CON_CMD_REG(camera_pitch_down);*/
}

CVideoSubsystem::CVideoSubsystem():
	initialized(false),
	m_backgroundRenderer(0)
{
	p_systemData = new V_SystemData;
}
CVideoSubsystem::~CVideoSubsystem()
{
	if(initialized)
		Shutdown();
	if(p_systemData)
		delete p_systemData;
}

bool CVideoSubsystem::PreInitialize() // PreInitialize is called before Initialize, only links to other subsystems should be made here
{
	CON_AddInitAuxFunction(VID_CON_InitAux);
	return true;
}
bool CVideoSubsystem::Initialize()
{
	return Initialize(640, 480, 32, false);
}

bool IsExtensionSupported( char* szTargetExtension )
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;

	// Extension names should not have spaces
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if( pszWhere || *szTargetExtension == '\0' )
		return false;

	// Get Extensions String
	pszExtensions = glGetString( GL_EXTENSIONS );

	// Search The Extensions String For An Exact Copy
	pszStart = pszExtensions;
	for(;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
		if( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szTargetExtension );
		if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
			if( *pszTerminator == ' ' || *pszTerminator == '\0' )
				return true;
		pszStart = pszTerminator;
	}
	return false;
}

bool CVideoSubsystem::Initialize(short width, short height, short bpp, bool fullscreen)
{
	if(Video.initialized) return true;

	VIDW_Init(width, height, bpp, fullscreen);

//	if(!glodInit())
//		ccout << "GLOD failed to initialize!\n";

	ResetViewportContexts();
	VIDW_LoadBitmapFont();

	Video.initialized = true;
	
	for(vector<void (*) (void)>::iterator it = m_initAuxList.begin(); it != m_initAuxList.end(); it++)
	{
		(*it)();
	}

	ifstream configFile("config.xml");
	string fontfile;
	int fontsize;
	if(configFile.fail())
	{
		ccout << "Warning: no config.xml file found in application directory.";
		fontfile = "arial.ttf";
		fontsize = 12;
	}
	else
	{
		string video = CXMLParser::GetInstance()->GetTagData(configFile, "video");
		string systemfont = CXMLParser::GetInstance()->GetTagData(video, "systemfont");
		fontfile = CXMLParser::GetInstance()->GetTagData(systemfont, "file");
		fontsize = atoi(CXMLParser::GetInstance()->GetTagData(systemfont, "size").c_str());
	}
	configFile.close();

	//vcout.setFont("C:\\windows\\Fonts\\arial.ttf");
	vcout.setFont(fontfile);
	//vcout.setWeight(500);
	vcout.setSize(fontsize);

	ccout << "OpenGL Device Information\n";
	ccout << "Vendor     : " << string((char*)glGetString( GL_VENDOR )) << newl;
	ccout << "Renderer   : " << string((char*)glGetString( GL_RENDERER )) << newl;
	ccout << "Version    : " << string((char*)glGetString( GL_VERSION )) << newl;
	ccout << "Extensions : " << string((char*)glGetString( GL_EXTENSIONS )) << newl << newl;
	ccout << "GL_ARB_vertex_buffer_object is " << (IsExtensionSupported( "GL_ARB_vertex_buffer_object" ) ? "supported.\n" : "not supported.\n");
	return true;
}
bool CVideoSubsystem::Shutdown()
{
	if(!Video.initialized) return true;
	Video.initialized = false;

	p_systemData->serverSceneGraph.clear();
	p_systemData->clientSceneGraph.clear();
	p_systemData->twoDStaticSceneGraph.clear();

//	glodShutdown();

	VIDW_Shutdown();
	return true;
}
void CVideoSubsystem::Think()
{
	if(!Video.initialized) return;
	camera.Think();
	VIDW_UpdateLights();
}

CRouterReturnCode CVideoSubsystem::EventReceiver(CRouterEvent& event)
{
	return CRouterReturnCode(true, false);
}

bool CVideoSubsystem::InputReceiver(const CInputEvent& event)
{
	return false;
}


////////////////////////
bool CVideoSubsystem::IsInitialized()
{
	return initialized;
}

void CVideoSubsystem::AddInitAuxFunction(void (* function)(void))
{
	m_initAuxList.push_back(function);
}

void CVideoSubsystem::BeginDraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// Clear The Screen And The Depth Buffer
	if(m_backgroundRenderer)
		m_backgroundRenderer();
}
void CVideoSubsystem::EndDraw()
{
	VIDW_Frame(); // flip the buffer
}

void CVideoSubsystem::ClearFrameBuffer(CColor color)
{
//	glClearColor(1.0f, 1.0f, 1.0f, 1);		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// Clear The Screen And The Depth Buffer
}

void CVideoSubsystem::SetResolution(short width, short height, short bpp, bool fullscreen)
{
	if(!Video.IsInitialized()) return;
	COND_Shutdown();
//	GUI_Shutdown();
	Shutdown();
	Initialize(width, height, bpp, fullscreen);
//	GUI_Init();
	COND_Init();
//	IN_ResetSublayer();
}

/// do something here with a global resource manager

void CVideoSubsystem::PutPixel(short x, short y, unsigned long color)
{
	VIDW_PutPixel(x, y, color);
}
void CVideoSubsystem::PutPixelA(short x, short y, unsigned long color) // transparency
{
	Video.PutPixel(x, y, color);
}
void CVideoSubsystem::BlitBitmap(unsigned long* data,					v3d position, v3d size, float rotateangle, v3d pivot, bool usecam, bool stencil)
{
	if(!Video.initialized) return;
	VIDW_BlitBitmap(data, position, size, rotateangle, pivot, usecam, stencil);
} 
void CVideoSubsystem::BlitBitmap(VideoResourceID videoResourceID,		v3d position, v3d size, float rotateangle, v3d pivot, bool usecam, bool stencil)
{
	if(!Video.initialized) return;
	if(videoResourceID == -1) return;
	VIDW_BlitBitmap(videoResourceID, position, size, rotateangle, pivot, usecam, stencil);
}
void CVideoSubsystem::BlitBitmapScaled(VideoResourceID videoResourceID,	v3d position, v3d size, float rotateangle, v3d pivot, bool usecam, bool stencil)
{
	if(!Video.initialized) return;
	if(videoResourceID == -1) return;
	VIDW_BlitBitmapScaled(videoResourceID, position, size, rotateangle, pivot, usecam, stencil);
}

void CVideoSubsystem::DrawRectangle4color(vid_Point p1, CColor p2color, vid_Point p3, CColor p4color) // only points p1 and p3 are used for coordinates, others are used for color points must be in rectangle shape... 4 points for color information
{
	if(!Video.initialized) return;
	
	//CColor color2(color);
	

	/*glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, Video.settings.getSw(), 0, Video.settings.getSh() );
*/
	PushProjection2d();


/*	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
*/
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glBegin(GL_QUADS);						
		glColor4f( p1.color.getRFloat(), p1.color.getGFloat(), p1.color.getBFloat(), p1.color.getAFloat() );
		glVertex2f(p1.position.x, p1.position.y); //0,0

		glColor4f( p2color.getRFloat(), p2color.getGFloat(), p2color.getBFloat(), p2color.getAFloat() );
		glVertex2f(p1.position.x, p3.position.y); // 0,1

		glColor4f( p3.color.getRFloat(), p3.color.getGFloat(), p3.color.getBFloat(), p3.color.getAFloat() );
		glVertex2f(p3.position.x, p3.position.y);		// 1,1

		glColor4f( p4color.getRFloat(), p4color.getGFloat(), p4color.getBFloat(), p4color.getAFloat() );
		glVertex2f(p3.position.x, p1.position.y);	// 1,0
		glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

	PopProjection();
	/*glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
*/}

void CVideoSubsystem::DrawRectangleGradient(v3d p1, v3d p2, CColor color1)
{
	if(!Video.initialized) return;
	
	CColor color2;
	CColor subtracted(0.2f, 0.2f, 0.2f, 0.0f);
	color2 = color1 - subtracted;

	PushProjection2d();
	
	/*glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, Video.settings.getSw(), 0, Video.settings.getSh() );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
*/
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glBegin(GL_QUADS);						
		glColor4f( color1.getRFloat(), color1.getGFloat(), color1.getBFloat(), color1.getAFloat() );
		glVertex2f(p1.x,p1.y); //0,0

		glColor4f( color2.getRFloat(), color2.getGFloat(), color2.getBFloat(), color2.getAFloat() );
		glVertex2f(p1.x,p2.y); // 0,1

		glColor4f( color2.getRFloat(), color2.getGFloat(), color2.getBFloat(), color2.getAFloat() );
		glVertex2f(p2.x,p2.y);		// 1,1

		glColor4f( color1.getRFloat(), color1.getGFloat(), color1.getBFloat(), color1.getAFloat() );
		glVertex2f(p2.x,p1.y);	// 1,0
		glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

	PopProjection();
	
/*	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
*/}

void CVideoSubsystem::DrawRectangle(v3d p1, v3d p2, CColor color)
{
	if(!Video.initialized) return;

	PushProjection2d();
	
	/*glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	gluOrtho2D( 0, Video.settings.getSw(), 0, Video.settings.getSh() );

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
*/
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
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

	/*glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
*/
	PopProjection();

}
void CVideoSubsystem::DrawLine(short x1, short y1, float zVal1, short x2, short y2, float zVal2, unsigned long color)
{
	if(!Video.initialized) return;
}
void CVideoSubsystem::DrawLine(v3d start, v3d end, rgba8888pixel color, bool usecam)
{
	if(!Video.initialized) return;
	
	PushProjection2d();
	
	if(usecam)
	{	
		glLoadMatrixf((camera.GetModelViewMatrix()).matrix);

/*		glTranslatef(Video.settings.getSw()/2, Video.settings.getSh()/2, 0);//Video.camPosition.x, Video.camPosition.y, Video.camPosition.z);
		glRotatef(-Video.camera.getAngle().z, 0, 0, 1);
		start-=Video.camera.getPosition();
		end-=Video.camera.getPosition();*/
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
		glColor3f( ((float)_EXTRACTRED(color.pixel))/256, ((float)_EXTRACTGREEN(color.pixel))/256, ((float)_EXTRACTBLUE(color.pixel))/256);
		glVertex3f(start.x,start.y,start.z); 

		glVertex3f(end.x,end.y,end.z); 
		glColor3f( 1.0, 1.0, 1.0 );
	glEnd();

//	glDisable(GL_BLEND);

//	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

	PopProjection();


}
void CVideoSubsystem::DrawLineScaled(v3d start, v3d end, unsigned long color, bool usecam)
{
	if(!Video.initialized) return;
	VIDW_DrawLineScaled(start, end, color, usecam);
}
void CVideoSubsystem::DrawTriangle(short x1, short y1, short x2, short y2, short x3, short y3, unsigned long color) // wireframe for now
{
	if(!Video.initialized) return;
	VIDW_DrawTriangle((float) x1,(float) y1,(float) x2,(float) y2,(float) x3,(float) y3,color);
}
void CVideoSubsystem::DrawTriangle3d(vid_Point p1, vid_Point p2, vid_Point p3, v3d normal, short textureId)
{
	if(!Video.initialized) return;
	VIDW_DrawTriangle3d(p1, p2, p3, normal, textureId);
}
/*
void CVideoSubsystem::DrawModel(const CCalModel& model, CCamera& camera)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if(Video.settings.getSh())
		gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 5000000.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLoadMatrixf((camera.GetModelViewMatrix() * model.worldMatrix).matrix);

	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); // DISABLED BECAUSE OF BLENDING PROBLEMS /////////////////////////////////////////////////////////////////////
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

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
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, model.m_pMaterials[materialIndex].m_texture );
			}
			else
				glDisable( GL_TEXTURE_2D );
		}
		else
		{
			glDisable( GL_TEXTURE_2D );
		}
	
		
		
		// Set up the vertex arrays
	//	glInterleavedArrays(GL_T2F_N3F_V3F, 0, model.m_pMeshes[i].m_pInterleavedVertexArray);

	//	glEnableClientState(GL_VERTEX_ARRAY);

	//	glVertexPointer(3, GL_FLOAT,  sizeof(Vertex), // 1, because there is an extra byte in m_boneID
	//					(reinterpret_cast<char*>(model.m_pVertices)) + 0); // start past the m_boneID

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT,  sizeof(Vertex), // 1, because there is an extra byte in m_boneID
						(reinterpret_cast<char*>(model.m_pMeshes[i].m_pVertexArray)) + 0); // start past the m_boneID
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0,
						reinterpret_cast<char*>(model.m_pMeshes[i].m_pNormalArray) + 0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,
						GL_FLOAT,
						0,
						reinterpret_cast<char*>(model.m_pMeshes[i].m_pTextureCoordArray) + 0);

		//glDrawElements(GL_TRIANGLES, model.m_pMeshes[i].m_triangleCount*3, GL_UNSIGNED_INT,
		//		reinterpret_cast<char*>(model.m_pMeshes[i].m_pTriangleVertexIndices));
		glDrawArrays( GL_TRIANGLES, 0, model.m_pMeshes[i].m_triangleCount*3 );		// Draw All Of The Triangles At Once

		glDisableClientState( GL_VERTEX_ARRAY );				// Disable Vertex Arrays
		glDisableClientState( GL_NORMAL_ARRAY );				// Disable Vertex Arrays
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );				// Disable Texture Coord Arrays
		
		
		
		//glBegin(GL_TRIANGLES);						// Drawing Using Triangles
		//	glColor3f(1.0, 1.0, 1.0);			
		//	for(int j = 0; j < model.m_pMeshes[i].m_triangleCount; j++)
		//	{
		//		int index = model.m_pMeshes[i].m_pTriangleIndices[j];
		//		for(int k = 0; k < 3; k++)
		//		{
		//			glNormal3fv(  model.m_pTriangles[ index ].m_vertexNormals[ k ] );
		//			glTexCoord2f( model.m_pTriangles[ index ].m_s[ k ], model.m_pTriangles[ index ].m_t[ k ] );
		//			glVertex3fv( model.m_pVertices[ model.m_pTriangles[ index ].m_vertexIndices[ k ] ].m_position );	// Top
		//		}
		//	}
		//glEnd();
	}
		
	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}*/
void CVideoSubsystem::DrawDisplayList(GLuint& list, matrix4x4& transformation, CCamera& camera)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if(Video.settings.getSh())
		gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 5000000.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLoadMatrixf((camera.GetModelViewMatrix() * transformation).matrix);

	glCallList(list);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
/*
GLuint CVideoSubsystem::GenerateDisplayListFromModel(const CCalModel& model)
{
	GLuint displayList = glGenLists(1); // make room for the display list

	glNewList(displayList, GL_COMPILE); // start the list

	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//	glBlendFunc(GL_ONE,GL_SRC_ALPHA);							// Set Blending Mode (Cheap / Quick)
//	glEnable(GL_BLEND);									// Enable Blending
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

			if ( model.m_pMaterials[materialIndex].m_texture > 0 )
			{
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, model.m_pMaterials[materialIndex].m_texture );
			}
			else
				glDisable( GL_TEXTURE_2D );
		}
		else
		{
			float white[] = {1.0f, 1.0f, 1.0f, 1.0f};
			float black[] = {1.0f, 1.0f, 1.0f, 1.0f};
			glMaterialfv( GL_FRONT, GL_AMBIENT, black );
			glMaterialfv( GL_FRONT, GL_DIFFUSE, white );
			glMaterialfv( GL_FRONT, GL_SPECULAR, black );
			glMaterialfv( GL_FRONT, GL_EMISSION, black );
			glMaterialf( GL_FRONT, GL_SHININESS, 0 );

			glDisable( GL_TEXTURE_2D );
		}
		
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
		
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopAttrib();
	glEndList(); // end the list
	return displayList;
}*/

void CVideoSubsystem::DrawStar(float starDiameter, matrix4x4 worldMatrix, const CCamera& camera)
{
	/*static CCalModel starModel;
	static VideoResourceID lensFlare = VideoResourceManager.LoadImageToVideoMemory("flare.tga");
	if(!starModel.m_pMeshes)
	{
		MilkshapeModelLoader.ReadModelFromFile("sun.ms3d");
		MilkshapeModelLoader.LoadModel(starModel);
	}
	
	// star diameter needs to match 5/32 of the flare image width to line up correctly
	// so, (starRadius*2) * (32/5) = flare width
	float flareWidth = starDiameter * 32./5.;

	DrawLensFlare(lensFlare, worldMatrix.getTranslation(), camera);
	*/
}

void CVideoSubsystem::DrawLensFlare(VideoResourceID texture, float width, v3d position, CCamera& camera)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if(Video.settings.getSh())
		gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 5000000.0f);

	matrix4x4 projectionMatrix;
	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix.matrix);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

/*	v3d worldPosition = (camera.transform.getInverse() * TranslationMatrix(position) * camera.transform.getTransform() ) * position;
	v3d screenPosition = projectionMatrix * worldPosition;*/

	glLoadMatrixf((camera.GetModelViewMatrix() * TranslationMatrix(position) * camera.GetTransformation().getTransform()).matrix);

	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glEnable(GL_BLEND);
	//glBlendFunc( GL_ONE, GL_ONE);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);				// Select Our Texture

	glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-width/2, width/2, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-width/2, -width/2, 0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(width/2, -width/2, 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(width/2, width/2, 0);
	glEnd();
//	glDisable(GL_TEXTURE_2D);
/*	glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(position.x-width/2,position.y+width/2,0.0f);
		glVertex3f(position.x-width/2,position.y-width/2,0.0f);
		glVertex3f(position.x+width/2,position.y-width/2,0.0f);
		glVertex3f(position.x+width/2,position.y+width/2,0.0f);
	glEnd();*/
		
	glDisable(GL_TEXTURE_2D);

/*	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);*/

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
void CVideoSubsystem::DrawLabel(string text, string fontname, int size, CColor color, v3d position, CCamera& camera)
{

	Video.PushProjection3d();
	glLoadMatrixf((camera.GetModelViewMatrix() * TranslationMatrix(position) * camera.GetTransformation().getTransform()).matrix);

	//	glLoadMatrixf((camera.GetTransformation().getInverse() * TranslationMatrix(modelHighRes.worldMatrix.getTranslation()) * camera.GetTransformation().getTransform()).matrix);


	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glColor4f(color.getRFloat(), color.getGFloat(), color.getBFloat(), color.getAFloat());	
	
	string currentLine;
	v3d currentDrawPosition = v3d(0,0);
	float ypitch(12);

	FTFont* font = FTGLFontManager::Instance().GetFont(fontname.c_str(), size);
	if(font)
	{
		ypitch = font->LineHeight();
	}
	else
	{
		return;
	}
		
	// for the texture font
	glEnable( GL_TEXTURE_2D);
	glDisable( GL_DEPTH_TEST);
	//setUpLighting();
	glNormal3f( 0.0, 0.0, 1.0);

	int offsetr = -1;
	int offsetl = 0;
	//for(int i = 0; i < text.size(); i++)
	bool done = false;
	glTranslatef(currentDrawPosition.x, currentDrawPosition.y/*+font->LineHeight()*/, 0.0f);
	while(!done)
	{
		offsetl = offsetr+1;
		offsetr = text.find('\n', offsetr+1);
		if(offsetr == -1)
		{
			done = true;
			offsetr = text.size();
		}
		glRasterPos2f(currentDrawPosition.x, currentDrawPosition.y);
		font->Render(text.substr(offsetl, offsetr-offsetl).c_str());
		glTranslatef(-font->Advance(text.substr(offsetl, offsetr-offsetl-1).c_str()), -ypitch, 0.0f);
		currentDrawPosition -= v3d(0, ypitch);
	}
	if(currentLine.size())
	{
		glRasterPos2f(currentDrawPosition.x, currentDrawPosition.y);
		font->Render(currentLine.c_str());
	}

/*	glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-100/2,position.y+100/2,0.0f);
		glVertex3f(-100/2,position.y-100/2,0.0f);
		glVertex3f(position.x+100/2,position.y-100/2,0.0f);
		glVertex3f(position.x+100/2,position.y+100/2,0.0f);
	glEnd();*/
	/*glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);*/

	glDisable(GL_SCISSOR_TEST);

		
	glDisable(GL_TEXTURE_2D);

	Video.PopProjection();
}



void CVideoSubsystem::DrawTextShit(string text, short x, short y, unsigned short lineWidth)
{
	if(!Video.initialized) return;
//	ccout << "Don't use DrawText... use vcout\n";
	Video.vcout.setColor(CColor(0.75f, 0.75f, 0.75f));
	vcout.setPos(v3d(x,y));
	vcout << text;
	return;
	if( (lineWidth > 0) && (text.length()*8 > lineWidth) )
	{
		// the text will need to wrap to fit
		int j = lineWidth/8; // this is the character at lineWidth
		while( (text[j] != ' ') && (text[j] != '\t') )
		{
			j--;
			if(j < 0)
				break;
		}
		if(j > 0)
		{
			string continued;
			for(unsigned int k = j+1; k < text.length(); k++)
			{
				continued += text[k];
			}
			text.erase(j, text.length()-1);
			Video.DrawTextShit(continued, x, y-8, lineWidth);
		}
	}

	short currentx = x; // so the original value is kept for later
	for(unsigned int i = 0; i < text.length(); i++)
	{

		if(text[i] == '\t')
		{
			for(int j = 0; j < 4; j++)
			{
				vcout.setPos(v3d(currentx, y));
				vcout << ' ';
				//VIDW_DrawChar(' ', currentx, y);
				currentx+=8;
			}
		}
		else
		{
			vcout.setPos(v3d(currentx, y));
			vcout << text[i];
			//VIDW_DrawChar(text[i], currentx, y);
		}

		currentx+=8;


		if(lineWidth && (i > 0))
		{
			if( (currentx+8 > x+lineWidth) && (text[i] == ' '))
			{
				currentx = x;
				y-=8;
			}
		}
	}
}

void CVideoSubsystem::DrawCardinalSpline(CCardinalSpline& spline, CCamera& camera)
{
	Video.PushProjection3d();

	glLoadMatrixf((camera.GetModelViewMatrix()).matrix);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glBegin(GL_LINE_STRIP);
		for(float t = 0; t <= 1.005f; t += 0.01)
		{
			v3d vertex = spline.Evaluate(t);
			glColor3f(t*t,2*t*(1-t),(1-t)*(1-t));
			glVertex3f(vertex.x, vertex.y, vertex.z);
		}
	glEnd();
	
	for(float t = 0; t <= 1.05f; t += 0.1)
	{
		v3d vertex = spline.Evaluate(t);
		Video.DrawLabel(M_ftoa(t), "arial.ttf", 14, CColor(1.0f, 1.0f, 1.0f), vertex, camera);
	}

	Video.PopProjection();
}

void CVideoSubsystem::DrawOrbit(v3d center, float radius, CColor& color, CCamera& camera) // for drawing orbits in the system view
{
	Video.PushProjection3d();

	glLoadMatrixf(camera.GetModelViewMatrix().matrix);

	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	v3d vect(radius, 0, 0);

	glBegin(GL_LINE_STRIP);
		glColor4f(color.getRFloat(), color.getGFloat(), color.getBFloat(), color.getAFloat());
		for(float angle = 0; angle <= 360; angle += 5.0f)
		{
			v3d vertex = center + v3d(vect).rotate(0, v3d(0,angle,0));
			glVertex3f(vertex.x, vertex.y, vertex.z);
			//vect.rotate(0, v3d(0,angle,0));
		}
	glEnd();

	glPopAttrib();

	Video.PopProjection();
}

v3d CVideoSubsystem::ProjectPoint(v3d worldSpacePoint) // returns a 2d projected point for the current viewport context and projection and modelview matrices
{
	matrix4x4 projectionMatrix, modelViewMatrix;
	v3d projectedPoint;

	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix.matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix.matrix);
	projectionMatrix = projectionMatrix * modelViewMatrix;

	// prerform matrix multiplication with a 4x4 matrix and a 4x1 matrix (vector, or v3d with 4th w component)
	projectedPoint = projectionMatrix * worldSpacePoint;
	// now, the 4th component of the vector remains
	float w = ( worldSpacePoint.x * projectionMatrix.matrix[3] +
			worldSpacePoint.y * projectionMatrix.matrix[7] +
			worldSpacePoint.z * projectionMatrix.matrix[11] +
			/*projectedPoint.w == 1 * */projectionMatrix.matrix[15]);
	projectedPoint /= w; // divide by w to get the (x, y, z, 1.0) vector, which is what we need

	// now, scale the point to match the size of the viewport
	projectedPoint.x *= Video.TopViewportContext().area.getWidth()/2; 
	projectedPoint.y *= Video.TopViewportContext().area.getHeight()/2;

	return projectedPoint;
}

void CVideoSubsystem::SetBackgroundRenderer(void (* renderer) (void))
{
	m_backgroundRenderer = renderer;
}


short CVideoSubsystem::MakeLight(unsigned long ambient, unsigned long diffuse) // returns an id to a useable light
{
	return 0;
}
void CVideoSubsystem::KillLight(short id) // kills light with this id
{
}
vid_SceneObject* CVideoSubsystem::GetLight(short id)
{
	return 0;
}
void CVideoSubsystem::SetLightAmbient(short id, unsigned long ambient)
{
}
void CVideoSubsystem::SetLightDiffuse(short id, unsigned long diffuse)
{
}


void CVideoSubsystem::PushProjection2d()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	
	gluOrtho2D( 0, viewportContextStack.back().virtualPixelWidth, 0, viewportContextStack.back().virtualPixelHeight );
	
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

void CVideoSubsystem::PushProjection3d()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if(Video.settings.getSh())
		gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 5000000.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void CVideoSubsystem::PopProjection()
{
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
}

void CVideoSubsystem::PushViewportContext(CViewportContext context)
{
	viewportContextStack.push_back(context);
	SetGLViewport();
}
CViewportContext CVideoSubsystem::PopViewportContext()
{
	CViewportContext result = viewportContextStack.back();
	viewportContextStack.pop_back();
	if(!viewportContextStack.size())
	{
		ResetViewportContexts();
	}
	else
	{
		SetGLViewport();
	}
	return result;
}
CViewportContext CVideoSubsystem::TopViewportContext()
{
	return viewportContextStack.back();
}
void CVideoSubsystem::ResetViewportContexts()
{
	while(viewportContextStack.size())
	{
		viewportContextStack.pop_back();
	}
	CViewportContext baseContext;
	baseContext.area.leftx = 0;
	baseContext.virtualPixelWidth = baseContext.area.rightx = settings.getSw();
	baseContext.area.bottomy = 0;
	baseContext.virtualPixelHeight = baseContext.area.topy = settings.getSh();
	baseContext.viewportContextType = CViewportContext::independent;
	PushViewportContext(baseContext);
	SetGLViewport();
}

void CVideoSubsystem::RenderText(string font, string text, v3d position, CColor color, CRectangle clippingRegion, bool clip)
{
	PushProjection2d();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glDisable(GL_TEXTURE_2D);

	if(clip)
	{
		glScissor(clippingRegion.leftx, clippingRegion.bottomy, clippingRegion.rightx-clippingRegion.leftx, clippingRegion.topy-clippingRegion.bottomy);
		glEnable(GL_SCISSOR_TEST);
	}

	glColor3f(color.getRFloat(), color.getGFloat(), color.getBFloat());

	glRasterPos2f(position.x, position.y);

	FTFont* ftglfont = FTGLFontManager::Instance().GetFont( font.c_str(), 12);
	if(ftglfont)
		ftglfont->Render( text.c_str() );

	glDisable(GL_SCISSOR_TEST);

	PopProjection();
}


/*void CVideoSubsystem::SetViewport()
{
	CViewport defaultViewport;
	defaultViewport.area.leftx = 0;
	defaultViewport.area.rightx = settings.getSw();
	defaultViewport.area.topy = settings.getSh();
	defaultViewport.area.bottomy = 0;
	defaultViewport.SetUpViewport();
}*/

/*void CVideoSubsystem::SetViewport(CViewport viewport)
{
	viewport.SetUpViewport();
}*/

void CVideoSubsystem::DrawButton(CRectangle area, CColor color, string caption, bool down)
{
	CColor brightColor = color + CColor(0.2f, 0.2f, 0.2f, 0.0f);
	CColor darkColor = color - CColor(0.2f, 0.2f, 0.2f, 0.0f);

	if(down) // swap the colors around
	{
		CColor temp;
		temp = brightColor;
		brightColor = darkColor;
		darkColor = temp;
	}

//	if(gradient)
		DrawRectangleGradient(area.getLowerLeft(), area.getUpperRight(), color);
//	else
//		DrawRectangle(area.getLowerLeft(), area.getUpperRight(), color);

	DrawLine(area.getLowerLeft(), area.getLowerRight(),  darkColor.getPixel(), false);
	DrawLine(area.getUpperLeft(), area.getUpperRight(), brightColor.getPixel(), false);
	DrawLine(area.getLowerLeft(), area.getUpperLeft(), brightColor.getPixel(), false);
	DrawLine(area.getLowerRight(), area.getUpperRight(), darkColor.getPixel(), false);

	float xcenter = area.leftx + (area.rightx-area.leftx)/2;
	float captionWidth = vcout.getAdvance(caption);

	float ycenter = area.bottomy + (area.topy-area.bottomy)/2 + vcout.getHeight()/2 - vcout.getAscender();
	float ascDescDifference = -vcout.getAscender() - vcout.getDescender();

	vcout.setPos(v3d( xcenter - captionWidth/2, ycenter));
	vcout.setColor(CColor(0.75f, 0.75f, 0.75f));
	vcout.setBounds(area);
	vcout.clipToBounds = true;
	vcout << caption;
	vcout.clipToBounds = false;
}

void CVideoSubsystem::DrawProgressBar(CRectangle area, float percent, CColor barColor, CColor backgroundColor)
{
	PushProjection2d();

	CColor barColor2 = barColor - CColor(0.2f, 0.2f, 0.2f, 0.0f);

	CRectangle barArea;
	barArea = area;
	barArea.leftx += 1;
	barArea.topy -= 1;
	barArea.bottomy += 1;
	barArea.rightx = area.leftx + 2 + (area.rightx-area.leftx-4)*percent;

	// background
	
	DrawRectangle(area.getLowerLeft(), area.getUpperRight(), backgroundColor);
	DrawLine(area.getLowerLeft(), area.getLowerRight(), _RGB32BIT(192, 192, 192), false);
	DrawLine(area.getUpperLeft(), area.getUpperRight(), _RGB32BIT(64, 64, 64), false);
	DrawLine(area.getLowerLeft(), area.getUpperLeft(), _RGB32BIT(64, 64, 64), false);
	DrawLine(area.getLowerRight(), area.getUpperRight(), _RGB32BIT(192, 192, 192), false);
	
	// progress bar
	
	DrawRectangleGradient(barArea.getLowerLeft(), barArea.getUpperRight(), barColor);
	DrawLine(barArea.getLowerLeft(), barArea.getLowerRight(),  barColor2.getPixel()/*_RGB32BIT(64, 64, 64)*/, false);
	DrawLine(barArea.getUpperLeft(), barArea.getUpperRight(), barColor.getPixel()/*_RGB32BIT(192, 192, 192)*/, false);
	DrawLine(barArea.getLowerLeft(), barArea.getUpperLeft(), barColor.getPixel()/*_RGB32BIT(192, 192, 192)*/, false);
	DrawLine(barArea.getLowerRight(), barArea.getUpperRight(), barColor2.getPixel()/*_RGB32BIT(64, 64, 64)*/, false);
	
	string temp = M_ftoa(percent*100) + "%";
	vcout.setPos(v3d( area.leftx + (area.rightx-area.leftx)/2, area.bottomy + (area.topy-area.bottomy)/2 - 4));
	vcout.setColor(CColor(1.0f, 1.0f, 1.0f, 1.0f));
	vcout.setBounds(area);
	vcout.clipToBounds = true;
	vcout << temp;
	vcout.clipToBounds = false;
	
	PopProjection();
}

void CVideoSubsystem::DrawSlider(CRectangle area, float percent)
{
	CRectangle trackArea = area;
	float middley = (area.topy-area.bottomy)/2;
	trackArea.topy = middley + 2 + area.bottomy;
	trackArea.bottomy = middley - 2 + area.bottomy;

	CRectangle sliderArea(area.leftx-2.5, area.leftx + 2.5, area.bottomy, area.topy);
	sliderArea.leftx += percent*(area.rightx-area.leftx);
	sliderArea.rightx += percent*(area.rightx-area.leftx);

	DrawRectangle(trackArea.getLowerLeft(), trackArea.getUpperRight(), CColor(0.75f, 0.75f, 0.75f));
	DrawLine(trackArea.getLowerLeft(), trackArea.getLowerRight(), _RGB32BIT(192, 192, 192), false);
	DrawLine(trackArea.getUpperLeft(), trackArea.getUpperRight(), _RGB32BIT(64, 64, 64), false);
	DrawLine(trackArea.getLowerLeft(), trackArea.getUpperLeft(), _RGB32BIT(64, 64, 64), false);
	DrawLine(trackArea.getLowerRight(), trackArea.getUpperRight(), _RGB32BIT(192, 192, 192), false);

	DrawRectangle(sliderArea.getLowerLeft(), sliderArea.getUpperRight(), CColor(0.75f, 0.75f, 0.75f));
	DrawLine(sliderArea.getLowerLeft(), sliderArea.getLowerRight(), _RGB32BIT(64, 64, 64), false);
	DrawLine(sliderArea.getUpperLeft(), sliderArea.getUpperRight(), _RGB32BIT(192, 192, 192), false);
	DrawLine(sliderArea.getLowerLeft(), sliderArea.getUpperLeft(), _RGB32BIT(192, 192, 192), false);
	DrawLine(sliderArea.getLowerRight(), sliderArea.getUpperRight(), _RGB32BIT(64, 64, 64), false);

}


void CVideoSubsystem::DrawTextField(CRectangle area, CColor textColor, CColor backgroundColor, CColor borderColor, string text)
{
	DrawRectangle(area.getLowerLeft(), area.getUpperRight(), backgroundColor);
	DrawLine(area.getLowerLeft(), area.getLowerRight(), _RGB32BIT(192, 192, 192), false);
	DrawLine(area.getUpperLeft(), area.getUpperRight(), _RGB32BIT(64, 64, 64), false);
	DrawLine(area.getLowerLeft(), area.getUpperLeft(), _RGB32BIT(64, 64, 64), false);
	DrawLine(area.getLowerRight(), area.getUpperRight(), _RGB32BIT(192, 192, 192), false);
	
	vcout.setBounds(area);
	vcout.clipToBounds = true;
	vcout.setPos(v3d( area.leftx+1, area.topy - vcout.getAscender()));
	vcout.setColor(textColor);
	vcout << text;
	vcout.clipToBounds = false;
}

void CVideoSubsystem::DrawTextStatic(CRectangle area, CColor textColor, string text)
{
	vcout.setBounds(area);
	vcout.clipToBounds = true;
	vcout.setPos(v3d( area.leftx+1, area.topy - vcout.getAscender()));
	vcout.setColor(textColor);
	vcout << text;
	vcout.clipToBounds = false;
}

void CVideoSubsystem::DrawWindow(CRectangle area, CColor backgroundColor, bool gradient, bool titleBar, CColor titleBarColor, string titleBarCaption)
{
	CColor brightColor = backgroundColor + CColor(0.2f, 0.2f, 0.2f, 0.0f);
	CColor darkColor = backgroundColor - CColor(0.2f, 0.2f, 0.2f, 0.0f);

	PushProjection2d();
	
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);		
		v3d p1 = area.getLowerLeft();
		v3d p2 = area.getUpperRight();
		glColor4f( backgroundColor.getRFloat(), backgroundColor.getGFloat(), backgroundColor.getBFloat(), backgroundColor.getAFloat() );
		glVertex2f(p1.x,p1.y); //0,0

		if(gradient)
			glColor4f( darkColor.getRFloat(), darkColor.getGFloat(), darkColor.getBFloat(), darkColor.getAFloat() );
		glVertex2f(p1.x,p2.y); // 0,1

		//glColor4f( darkColor.getRFloat(), darkColor.getGFloat(), darkColor.getBFloat(), darkColor.getAFloat() );
		glVertex2f(p2.x,p2.y);		// 1,1

		if(gradient)
			glColor4f( backgroundColor.getRFloat(), backgroundColor.getGFloat(), backgroundColor.getBFloat(), backgroundColor.getAFloat() );
		glVertex2f(p2.x,p1.y);	// 1,0
		glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINES);
		v3d start = area.getLowerLeft();
		v3d end = area.getLowerRight();
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

	glPopAttrib();
	
	PopProjection();

	if(titleBar)
	{
		brightColor = CColor(192, 192, 192);//;titleBarColor + CColor(0.2f, 0.2f, 0.2f, 0.0f);
		darkColor = CColor(64, 64, 64);//;titleBarColor - CColor(0.2f, 0.2f, 0.2f, 0.0f);
		float titleBarHeight = vcout.getHeight();
		CRectangle titleBarArea(area.leftx, area.rightx, area.topy-titleBarHeight, area.topy);

		DrawRectangle(titleBarArea.getLowerLeft(), titleBarArea.getUpperRight(), titleBarColor);
		DrawLine(titleBarArea.getLowerLeft(), titleBarArea.getLowerRight(),  darkColor.getPixel(), false);
		DrawLine(titleBarArea.getUpperLeft(), titleBarArea.getUpperRight(), brightColor.getPixel(), false);
		DrawLine(titleBarArea.getLowerLeft(), titleBarArea.getUpperLeft(), brightColor.getPixel(), false);
		DrawLine(titleBarArea.getLowerRight(), titleBarArea.getUpperRight(), darkColor.getPixel(), false);

		vcout.setPos(area.getUpperLeft() + v3d(5,-vcout.getAscender()));
		vcout.setColor(CColor(0.2f, 0.2f, 0.2f));
		vcout.setBounds(titleBarArea);
		vcout.clipToBounds = true;
		vcout << titleBarCaption;
		vcout.clipToBounds = false;
	}
}



void CVideoSubsystem::SetGLViewport()
{
	CViewportContext context;
	if(viewportContextStack.size())
	{
		context = TopViewportContext();
		if(context.viewportContextType == CViewportContext::additive) // add up all the contexts below it
		{
			vector<CViewportContext>::iterator it = viewportContextStack.end();
			it--; // -= 2???
			for(; it != viewportContextStack.begin(); it--)
			{
				context.area.leftx += it->area.leftx;
				context.area.rightx += it->area.leftx;
				context.area.bottomy += it->area.bottomy;
				context.area.topy += it->area.bottomy;
				if(it->viewportContextType != CViewportContext::additive)
					break;
			}
		}
	}
	else
	{
		ccout << "Context type is not additive SHOULD NOT HAPPEN ";
		context.area.leftx = 0;
		context.area.rightx = settings.getSw();
		context.area.bottomy = 0;
		context.area.topy = settings.getSh();
	}
	glViewport(context.area.leftx, context.area.bottomy, context.area.rightx-context.area.leftx, context.area.topy-context.area.bottomy);
}	

/*
private:
	V_SystemData* CVideoSubsystem::p_systemData; // for all the shit that requires funky headers and shit (audiere)
	
	//int idCount;
	bool CVideoSubsystem::initialized;
};
*/


vid_VCout::vid_VCout()
{
	clipToBounds = false;
//	font = new vidw_Font;
	drawPosition = 0;
	drawColor.setColors(1.0f, 1.0f, 1.0f);
}

vid_VCout::~vid_VCout()
{
//	if(font)
//		delete font;
}

vid_VCout& vid_VCout::operator << (string rhs)
{
	print(rhs);
	return *this;
}

vid_VCout& vid_VCout::operator << (const char* rhs)
{
	print(string(rhs));
	return *this;
}	
vid_VCout& vid_VCout::operator << (char rhs)
{
	char cstring[2] = {rhs, '\0'};
	print(cstring);
	return *this;
}
vid_VCout& vid_VCout::operator << (unsigned char rhs)
{
	char cstring[2] = {rhs, '\0'};
	print(cstring);
	return *this;
}
vid_VCout& vid_VCout::operator << (float rhs)
{
	print(M_ftoa(rhs));
	return *this;
}
vid_VCout& vid_VCout::operator << (int rhs)
{
	print(M_itoa(rhs));
	return *this;
}
vid_VCout& vid_VCout::operator << (long int rhs)
{
	print(M_itoa(rhs));
	return *this;
}
vid_VCout& vid_VCout::operator << (unsigned long int rhs)
{
	print(M_itoa(rhs));
	return *this;
}
vid_VCout& vid_VCout::operator << (short int rhs)
{
	print(M_itoa(rhs));
	return *this;
}
vid_VCout& vid_VCout::operator << (unsigned short int rhs)
{
	print(M_itoa(rhs));
	return *this;
}
vid_VCout& vid_VCout::operator << (bool rhs)
{
	print(M_btoa(rhs));
	return *this;
}

vid_VCout& vid_VCout::setFont(const string& fontName)
{
	fontname = fontName;
	return *this;
}

vid_VCout& vid_VCout::setBounds(CRectangle newBounds)
{
	bounds = newBounds;
	return *this;
}
vid_VCout& vid_VCout::setSize(const unsigned short newSize)
{
	size = newSize;
	return *this;
}
float vid_VCout::getHeight()
{
	FTFont* font = FTGLFontManager::Instance().GetFont(fontname.c_str(), size);
	if(font)
	{
		return font->LineHeight();
	}
	else
	{
		return 0.0f;
	}
}
float vid_VCout::getAscender()
{
	FTFont* font = FTGLFontManager::Instance().GetFont(fontname.c_str(), size);
	if(font)
	{
		return font->Ascender();
	}
	else
	{
		return 0.0f;
	}
}
float vid_VCout::getDescender()
{
	FTFont* font = FTGLFontManager::Instance().GetFont(fontname.c_str(), size);
	if(font)
	{
		return font->Descender();
	}
	else
	{
		return 0.0f;
	}
}
float vid_VCout::getAdvance(string text)
{
	FTFont* font = FTGLFontManager::Instance().GetFont(fontname.c_str(), size);
	if(font)
	{
		return font->Advance(text.c_str());
	}
	else
	{
		return 0.0f;
	}
}

vid_VCout& vid_VCout::setPos(v3d position)
{
	drawPosition = position;
	return *this;
}

vid_VCout& vid_VCout::setPosScaled(v3d position)
{
	drawPosition.x = position.x*Video.settings.getSw()/Video.settings.getSwScaled();
	drawPosition.y = position.y*Video.settings.getSh()/Video.settings.getShScaled();
	return *this;
}


vid_VCout& vid_VCout::setColor(CColor color)
{
	drawColor = color;
	return *this;
}

void vid_VCout::print(string text)
{

	//if(clipToBounds)
//	Video.DrawRectangle(bounds.getUpperRight(), bounds.getLowerLeft(), CColor(1.f,0.f,0.f,.25f));
	Video.PushProjection2d();

	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glDisable(GL_TEXTURE_2D);

	glColor3f(drawColor.getRFloat(), drawColor.getGFloat(), drawColor.getBFloat());	
	
	string currentLine;
	v3d currentDrawPosition = drawPosition;
	float ypitch(12);

	FTFont* font = FTGLFontManager::Instance().GetFont(fontname.c_str(), size);
	if(font)
	{
		ypitch = font->LineHeight();
	}
	else
	{
		return;
	}
	if(clipToBounds)
	{
		/*float plane0[4] = {0.0, 1.0, 0.0, -bounds.bottomy}; // bottom clipping plane
		float plane1[4] = {0.0, -1.0, 0.0, bounds.topy}; // top clipping plane
		float plane2[4] = {1.0, 0.0, 0.0, -bounds.leftx}; // left clipping plane
		float plane3[4] = {-1.0, 0.0, 0.0, bounds.rightx}; // right clipping plane

		glClipPlane(GL_CLIP_PLANE0, plane0);
		glEnable(GL_CLIP_PLANE0);
		glClipPlane(GL_CLIP_PLANE1, plane1);
		glEnable(GL_CLIP_PLANE1);
		glClipPlane(GL_CLIP_PLANE2, plane2);
		glEnable(GL_CLIP_PLANE2);
		glClipPlane(GL_CLIP_PLANE3, plane3);
		glEnable(GL_CLIP_PLANE3);*/

		glScissor(bounds.leftx, bounds.bottomy, bounds.rightx-bounds.leftx, bounds.topy-bounds.bottomy);
		glEnable(GL_SCISSOR_TEST);
	}
		
	// for the texture font
	glEnable( GL_TEXTURE_2D);
	glDisable( GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	//setUpLighting();
	glNormal3f( 0.0, 0.0, 1.0);

	//glTranslatef(currentDrawPosition.x, currentDrawPosition.y, 0.0f);
	int offsetr = -1;
	int offsetl = 0;
	//for(int i = 0; i < text.size(); i++)
	bool done = false;
	glTranslatef(currentDrawPosition.x, currentDrawPosition.y/*+font->LineHeight()*/, 0.0f);
	while(!done)
	{
		offsetl = offsetr+1;
		offsetr = text.find('\n', offsetr+1);
		if(offsetr == -1)
		{
			done = true;
			offsetr = text.size();
		}
		glRasterPos2f(currentDrawPosition.x, currentDrawPosition.y);
		if(offsetl != offsetr)
		{
			string substring = text.substr(offsetl, offsetr-offsetl);
			font->Render(text.substr(offsetl, offsetr-offsetl).c_str());
			glTranslatef(-font->Advance(text.substr(offsetl, offsetr-offsetl-1).c_str()), -ypitch, 0.0f);
			currentDrawPosition -= v3d(0, ypitch);
		}
		else
		{
			glTranslatef(0, -ypitch, 0.0f);
			currentDrawPosition -= v3d(0, ypitch);
		}
		/*
		if(text[i] == '\n')
		{
			glRasterPos2f(currentDrawPosition.x, currentDrawPosition.y);
			//glTranslatef(0.0f, -ypitch, 0.0f);
			font->Render(currentLine.c_str());
			currentDrawPosition -= v3d(0, ypitch);
			currentLine = "";
		}
		else
		{
			currentLine += text[i];
		}*/
	}
	/*if(currentLine.size())
	{
		glRasterPos2f(currentDrawPosition.x, currentDrawPosition.y);
		//glTranslatef(currentDrawPosition.x, currentDrawPosition.y, 0.0f);
		font->Render(currentLine.c_str());
	}*/


	/*glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);*/

	glDisable(GL_SCISSOR_TEST);

	glPopAttrib();

	Video.PopProjection();
}


vid_Settings::vid_Settings():
bpp(0),
sw(0),
sh(0),
swScaled(1024),
shScaled(768),
zoom(1.0),
fullscreen(true),
modelsUseDisplayLists(false)
{
}

vid_Settings::~vid_Settings()
{
}

void vid_Settings::setBpp(const short& _bpp)
{
	bpp = _bpp;
}
void vid_Settings::setSw(const short& _sw)
{
	sw = _sw;
}
void vid_Settings::setSh(const short& _sh)
{
	sh = _sh;
}
void vid_Settings::setSwScaled(const short& _swScaled)
{
	swScaled = _swScaled;
}
void vid_Settings::setShScaled(const short& _shScaled)
{
	shScaled = _shScaled;
}
void vid_Settings::setFullscreen(const bool& _fullscreen)
{
	fullscreen = _fullscreen;
}
void vid_Settings::setZoom(const float& _zoom)
{
	zoom = _zoom;
	if(zoom <= 0) zoom = 0.01f;
}

const short& vid_Settings::getBpp() const
{
	return bpp;
}
const short& vid_Settings::getSw() const
{
	return sw;
}
const short& vid_Settings::getSh() const
{
	return sh;
}
short vid_Settings::getSwScaled() const
{
	return (short)(((float)swScaled)*zoom);
}
short vid_Settings::getShScaled() const
{
	return (short)(((float)shScaled)*zoom);
}
const bool& vid_Settings::getFullscreen() const
{
	return fullscreen;
}
const float& vid_Settings::getZoom() const
{
	return zoom;
}


vid_SceneGraph::vid_SceneGraph():
root(0),
clipToCamera(false)
{}
vid_SceneGraph::~vid_SceneGraph()
{
	clear();
}

void vid_SceneGraph::addObject(vid_SceneObject* newObject, bool zoned)
{
	monitor.mutexOn();
	if(!Video.IsInitialized()) 
	{
		monitor.mutexOff();
		return;
	}
	if(!root)
		root = new vid_SceneObject;
	zoned = false;
	// put it in a zone
	/////////////////////

	// find out coordinates of the correct zone...
	if(zoned)
	{
		short x = newObject->collapseTransforms().getTranslation().x;
		short y = newObject->collapseTransforms().getTranslation().y;
		short z = newObject->collapseTransforms().getTranslation().z;

		// 256 is zone size
		short zonex = (x/256)*256/*+256/2*/;
		short zoney = (y/256)*256/*+256/2*/;
		short zonez = (z/256)*256;

		v3d zoneposition = v3d(zonex, zoney, zonez);

		bool newzone(true);
		// search to see if zone already exists (zones only exist in the root's childlist
		for(vector<vid_SceneObject*>::iterator it = root->childlist.begin(); it != root->childlist.end(); it++)
		//for(vid_SceneObject* index = head->childlist; index != 0; index = index->next)
		{
			if((*it)->sotype == vid_SceneObject::zone)
			{
				if((*it)->collapseTransforms().getTranslation() == zoneposition) // this is the zone we want
				{
					newObject->transform *= TranslationMatrix(zoneposition*-1);
					(*it)->attachChild(newObject);
					newzone = false;
				}
			}
		}

		if(newzone)
		{
			vid_Zone* zone = new vid_Zone;
			zone->transform = TranslationMatrix(zoneposition);
			newObject->transform *= TranslationMatrix(zoneposition*-1);			
			zone->attachChild(newObject);
			root->attachChild(zone);
			zone = 0;
		}
	}
	else
		root->attachChild(newObject);

	monitor.mutexOff();
}

void vid_SceneGraph::findObject(vector<vid_SceneObject*> &objects, v3d position)
{
	monitor.mutexOn();

	findObject(objects, root, position);

	monitor.mutexOff();
}

void vid_SceneGraph::findObject(vector<vid_SceneObject*> &objects, vid_SceneObject* subRoot, v3d position)
{
	// Recursively call all children	
	for(unsigned int i = 0; i < subRoot->childlist.size(); i++)
	{
		findObject(objects, subRoot->childlist[i], position);
	}

	if( (subRoot->collapseTransforms().getTranslation() == position) ) // found an instance
	{
		objects.push_back(subRoot);
	}
}

void vid_SceneGraph::deleteObject(v3d position)
{
	monitor.mutexOn();

	deleteObject(root, position);

	monitor.mutexOff();
}
void vid_SceneGraph::deleteObject(vid_SceneObject* subRoot, v3d position)
{
	if(!subRoot)
		return;

	// Recursively call all children	
	vector<vid_SceneObject*> childListCopy = subRoot->childlist; // make a copy just in case deleteObject changes the childlist
	for(unsigned int i = 0; i < childListCopy.size(); i++)
	//for(vid_SceneObject* index = subRoot->childlist; index != 0; /*index = index->next*/)
	{
		//vid_SceneObject* temp = index->next;
		deleteObject(childListCopy[i], position);
	//	index = temp;
	}

	if( (subRoot->collapseTransforms().getTranslation() == position) && (subRoot != root) ) // found an instance
	{
		if(subRoot->parent)
		{
			stack<vector<vid_SceneObject*>::iterator> itStack;
			for(vector<vid_SceneObject*>::iterator it = subRoot->parent->childlist.begin(); it != subRoot->parent->childlist.end(); it++)
			{
				if(*it == subRoot)
					itStack.push(it);
			}
			while(!itStack.empty())
			{
				subRoot->parent->childlist.erase(itStack.top());
				itStack.pop();
			}
		}
		clear(subRoot);
		subRoot = 0;
	}
}

void vid_SceneGraph::deleteObject(vid_SceneObject* removed)
{
	monitor.mutexOn();

	deleteObject(root, removed);

	monitor.mutexOff();
}

void vid_SceneGraph::deleteObject(vid_SceneObject* subRoot, vid_SceneObject* removed)
{
	if(!subRoot)
		return;
	
	// Recursively call all children	
	vector<vid_SceneObject*> childListCopy = subRoot->childlist; // make a copy just in case deleteObject changes the childlist
	for(unsigned int i = 0; i < childListCopy.size(); i++)
	{
		deleteObject(childListCopy[i], removed);
	}

	if( subRoot == removed ) // found an instance
	{			
		if(subRoot->parent)
		{
			for(vector<vid_SceneObject*>::iterator it = subRoot->parent->childlist.begin(); it != subRoot->parent->childlist.end(); it++)
			{
				if(*it == subRoot)
				{
					subRoot->parent->childlist.erase(it);
					break;
				}
			}
		}
		clear(subRoot);
		subRoot = 0;			
	}
}

void vid_SceneGraph::detatchObject(vid_SceneObject* removed) // detaches removed from a scene
{
	monitor.mutexOn();

	detatchObject(root, removed);
	
	monitor.mutexOff();
}

void vid_SceneGraph::detatchObject(vid_SceneObject* subRoot, vid_SceneObject* removed) // detaches removed from a scene
{
	if(!subRoot)
		return;
	// Recursively call all children	
	vector<vid_SceneObject*> childListCopy = subRoot->childlist; // make a copy just in case deleteObject changes the childlist
	for(unsigned int i = 0; i < childListCopy.size(); i++)
	{
		detatchObject(childListCopy[i], removed);
	}

		if( subRoot == removed ) // found an instance
		{		
			if(subRoot->parent)
			{
				stack<vector<vid_SceneObject*>::iterator> itStack;
				for(vector<vid_SceneObject*>::iterator it = subRoot->parent->childlist.begin(); it != subRoot->parent->childlist.end(); it++)
				{
					if(*it == subRoot)
						itStack.push(it);
				}
				while(!itStack.empty())
				{
					subRoot->parent->childlist.erase(itStack.top());
					itStack.pop();
				}
			}
		}
}

void vid_SceneGraph::update()
{
	monitor.mutexOn();

	update(root);

	monitor.mutexOff();
}

void vid_SceneGraph::update(vid_SceneObject* subRoot)
{
	if(!Video.IsInitialized()) return;
	// Base case for the recursion
	if (subRoot == NULL)
		return;
	else
	{
		// Update this object.
		subRoot->update();


		vector<vid_SceneObject*> childListCopy = subRoot->childlist; // make a copy just in case deleteObject changes the childlist
		for(unsigned int i = 0; i < childListCopy.size(); i++)
		//for(vid_SceneObject* index = subRoot->childlist; index != 0; /*index = index->next*/)
		{
			update(childListCopy[i]);
			if(childListCopy[i]->kill)
			{
				deleteObject(subRoot, childListCopy[i]);
			}
		}
	}
}

void vid_SceneGraph::draw(DrawingModes mode, int layer)
{	
	monitor.mutexOn();

	draw(root, mode, layer);

	monitor.mutexOff();
}
void vid_SceneGraph::draw(vid_SceneObject* subRoot, DrawingModes mode, short layer)
{
	if(!Video.IsInitialized()) return;
	// Base case for the recursion
	if (subRoot == NULL)
		return;
	else
	{
		if(!subRoot->visible)
			return;
		bool drawzone(true);
		// draw this object.
	//	if(subRoot->layer == layer)
		{
			if(clipToCamera)
			{
				float distanceSq = subRoot->collapseTransforms().getTranslation().distance(Video.camera.GetTransformation().getTranslation());
				distanceSq *= distanceSq; // square it
				float radiusSq = (float)(Video.settings.getSh()*Video.settings.getSh()+Video.settings.getSw()*Video.settings.getSw()); /*400*//*714*/
				if(distanceSq < radiusSq)
					subRoot->draw(mode, layer);
				else
					if(subRoot->sotype == vid_SceneObject::zone)
						drawzone = false;
			}
			else
				subRoot->draw(mode, layer);
		}


		// Recursively call all children
		if(drawzone)
		for(unsigned int i = 0; i < subRoot->childlist.size(); i++)
		//for(vid_SceneObject* index = subRoot->childlist; index != 0; index = index->next)
		{
			draw(subRoot->childlist[i], mode, layer);
		}
	
	}
}

void vid_SceneGraph::clear()
{
	monitor.mutexOn();

	clear(root);

	monitor.mutexOff();
}

void vid_SceneGraph::clear(vid_SceneObject*& subRoot)
{	
//	if(!vid_initialized) return;
	// Base case for the recursion
	if (subRoot == NULL)
		return;
	else
	{
		// Recursively call all children
		vector<vid_SceneObject*> childListCopy = subRoot->childlist; // make a copy so if something is removed there is no problem with the loop
		for(vector<vid_SceneObject*>::iterator it = childListCopy.begin(); it != childListCopy.end(); it++)
	//	for(vid_SceneObject* index = subRoot->childlist; index != 0; /*index = index->next*/)
		{
		//	vid_SceneObject* temp = index->next;
//			VID_ClearScene(index);
			clear(*it);
		//	index = temp;
		}


		if(subRoot)
		{
			delete subRoot;
			subRoot = 0;
		}
	}
}

void vid_SceneGraph::rotate(v3d pivot, float xaxisangle, float yaxisangle, float zaxisangle)
{
	monitor.mutexOn();

	rotate(root, pivot, xaxisangle, yaxisangle, zaxisangle);

	monitor.mutexOff();
}


void vid_SceneGraph::rotate(vid_SceneObject* subRoot, v3d pivot, float xaxisangle, float yaxisangle, float zaxisangle)
{
	if (subRoot == NULL)
		return;
	else
	{
		ccout << "Warning... \n\tvoid vid_SceneGraph::rotate(vid_SceneObject* subRoot, v3d pivot, float xaxisangle, float yaxisangle, float zaxisangle)\n\tno longer uses pivot point\n";
		subRoot->transform *= RotationMatrix(v3d(xaxisangle, yaxisangle, zaxisangle));
		//subRoot->rotate(pivot, xaxisangle, yaxisangle, zaxisangle);

		// Recursively call all children
		for(vector<vid_SceneObject*>::iterator it = subRoot->childlist.begin(); it != subRoot->childlist.end(); it++)
		//for(vid_SceneObject* index = subRoot->childlist; index != 0; index = index->next)
		{
//			VID_RotateScene(index, pivot, xaxisangle, yaxisangle, zaxisangle);
			rotate(*it, pivot, xaxisangle, yaxisangle, zaxisangle);
		}
	}
}

void vid_SceneGraph::collisionTest(vector<collision_t> &collisions, vid_SceneObject* self, const v3d& start, const v3d& end)
{
	monitor.mutexOn();

	collisionTest(root, collisions, self, start, end);

/*	if(collisions.size())
	{
		v3d closestCollision = collisions[0];
		for(int i = 1; i < collisions.size(); i++)
		{
			if(start.distance(collisions[i]) < start.distance(closestCollision))
				closestCollision = collisions[i];
		}
		end = closestCollision;
	}*/
	monitor.mutexOff();
}


void vid_SceneGraph::collisionTest(vid_SceneObject* subRoot, vector<collision_t> &collisions, vid_SceneObject* self, const v3d& start, const v3d& end)
{
/*	if(!vid_initialized)
		return;
	if(subRoot == NULL)
		return;
	
	if(subRoot != self)
	{
		bool checkEntity(false);
		if( subRoot->sotype == vid_SceneObject::entity )
		{
			if( subRoot->colltype <= vid_SceneObject::solidWithEvent )
			{
				bn_Ent* entity = (bn_Ent*)subRoot;
				if(entity->enttype != bn_Ent::projectile)
					if( !entity->isDead() )
						if( !entity->isDying() )
						{
							checkEntity = true;
						}
7			}
		}
		if( (subRoot->sotype == vid_SceneObject::tile) || checkEntity )
		{
			if(subRoot->colltype <= vid_SceneObject::solidWithEvent)
			{
7				if(subRoot->distance(start) <= subRoot->boundingCircleRadius+start.distance(end) ) // don't test if its too far away to collide
				{
					math_LineSegment traceLine;
					traceLine.defineLineSegment(start, end);
					for(unsigned int i = 0; i < subRoot->collisionBorders.size(); i++)
					{
						v3d collision;
						if((collision = subRoot->collisionBorders[i].intersectWith(traceLine)) != nothing)
						{
#ifdef DEBUG_RAY
				//			ccout << "collision = " << collision << newl;
					//		ccout << "traceLine = " << traceLine << newl;
					//		ccout << "border = " << subRoot->collisionBorders[i] << newl;
#endif
							collision_t temp;
							temp.object = subRoot;
							temp.point = collision;
							temp.normal = subRoot->collisionBorders[i].getNormal();
							collisions.push_back(temp);
						}
					}
				}
			}
		}
	}
	for(vector<vid_SceneObject*>::iterator it = subRoot->childlist.begin(); it != subRoot->childlist.end(); it++)
	//for(vid_SceneObject* index = subRoot->childlist; index != 0; index = index->next)
	{
		collisionTest(*it, collisions, self, start, end);
	}*/
}


void vid_SceneGraph::findEntity(vector<vid_SceneObject*> &ents, int enttype)
{
	monitor.mutexOn();

	findEntity(ents, root, enttype);

	monitor.mutexOff();
}

void vid_SceneGraph::findEntity(vector<vid_SceneObject*> &ents, vid_SceneObject* subRoot, int enttype)
{
	if(!Video.IsInitialized())
		return;
	if(subRoot == 0)
		return;
	
	/*if( subRoot->sotype == vid_SceneObject::entity )
	{
		bn_Ent* entity = (bn_Ent*)subRoot;
		if((int)entity->enttype == enttype)
		{
			ents.push_back(entity);
		}
						
	}

	for(vector<vid_SceneObject*>::iterator it = subRoot->childlist.begin(); it != subRoot->childlist.end(); it++)
	{
		findEntity(ents, *it, enttype);
	}*/
}

void vid_SceneGraph::writeToStream(ostream& out)
{
	monitor.mutexOn();

	writeToStream(root, out);

	monitor.mutexOff();
}

void vid_SceneGraph::writeToStream(vid_SceneObject* subRoot, ostream& out)
{
	// Base case for the recursion
	if (subRoot == NULL)
		return;
	else
	{
		// write info on this object
		switch(subRoot->sotype)
		{
		case vid_SceneObject::tile:
			{
				out << "tile ";
				vid_Tile* temp = (vid_Tile*)subRoot;
				out << /*VID_GetNameForTexId*/Video.VideoResourceManager.GetNameFromResourceID(temp->textureId) + " ";
				out << M_itoa( (short)temp->collapseTransforms().getTranslation().x ) + " ";
				out << M_itoa( (short)temp->collapseTransforms().getTranslation().y ) + '\n';
			}
			break;
		case vid_SceneObject::entity:
			{
				/*bn_Ent* entity = (bn_Ent*)subRoot;
				switch(entity->enttype)
				{
				case bn_Ent::factory:
					{
						bn_Factory* factory = (bn_Factory*)subRoot;
						switch(factory->fcttype)
						{
						case bn_Factory::akPickupFactory:
							{
								out << "ak_pickup_factory ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().x ) + " ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().y ) + '\n';
							}
							break;
						case bn_Factory::shotgunPickupFactory:
							{
								out << "shotgun_pickup_factory ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().x ) + " ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().y ) + '\n';
							}
							break;
						case bn_Factory::machineGunPickupFactory:
							{
								out << "machinegun_pickup_factory ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().x ) + " ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().y ) + '\n';
							}
							break;
						case bn_Factory::desertEaglePickupFactory:
							{
								out << "desert_eagle_pickup_factory ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().x ) + " ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().y ) + '\n';
							}
							break;
						case bn_Factory::rocketLauncherPickupFactory:
							{
								out << "rocket_launcher_pickup_factory ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().x ) + " ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().y ) + '\n';
							}
							break;
						case bn_Factory::healthFactory:
							{
								out << "health_factory ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().x ) + " ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().y ) + '\n';
							}
							break;
						case bn_Factory::playerFactory:
							{
								out << "player_factory ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().x ) + " ";
								out << M_itoa( (short)factory->collapseTransforms().getTranslation().y ) + '\n';	
							}
							break;
						default:
							break;
						}
					}
					break;
				default:
					break;
				}*/
			}
			break;
		default:
			// don't care
			break;
		}

	/*	// Recursively call all children
		vector<vid_SceneObject*>::iterator it = scene->getChildList()->begin();

		for (;it != scene->getChildList()->end(); ++it)
			BN_WriteScene(*it, fout);*/
				// Recursively call all children
		for(vector<vid_SceneObject*>::iterator it = subRoot->childlist.begin(); it != subRoot->childlist.end(); it++)
	//	for(vid_SceneObject* index = subRoot->childlist; index != 0; index = index->next)
		{
			writeToStream(*it, out);
		}
	}
}


///////////////////////////
// vid_SceneObject
///////////////////////////

vid_SceneObject::vid_SceneObject():
boundingCircleRadius(8),
boundtype(circle),
colltype(solid),
event(""),
handle(0),
kill(false),
layer(0),
parent(0),
size(32, 32),
sotype(generic),
visible(true)
{	

};

vid_SceneObject::vid_SceneObject(const vid_SceneObject &rhs):
boundingCircleRadius(rhs.boundingCircleRadius),
boundtype(rhs.boundtype),
colltype(rhs.colltype),
event(rhs.event),
handle(0),
kill(rhs.kill),
layer(rhs.kill),
parent(0),
size(rhs.size),
sotype(rhs.sotype),
visible(rhs.visible),
transform(rhs.transform)
{

};

vid_SceneObject::vid_SceneObject(vid_SOType type):
boundingCircleRadius(8),
boundtype(circle),
colltype(solid),
event(""),
handle(0),
kill(false),
layer(0),
parent(0),
size(32, 32),
sotype(type),
visible(true)
{
};

vid_SceneObject::~vid_SceneObject()
{ 
	if(handle) 
	{
		*handle = 0; 
		ccout << "Warning, *handle has been set to 0\n";
	} 
//	aliases.destroyAlias(this);
}

void vid_SceneObject::setParent(vid_SceneObject* pParent) 	// Called internally to set the parent node
{	
	parent = pParent;
}
void vid_SceneObject::attachChild(vid_SceneObject* pChild)	// Called to attach a object as a child of this object
{	
	pChild->setParent(this);

/*	vid_SceneObject* index;
	if(childlist) // add to end of existing list
	{
		for(index = childlist; index->next != 0; index = index->next);
		index->next = pChild;
		pChild->prev = index;
	}
	else // make a new list
	{
		childlist = pChild;
		childlist->prev = childlist->next = 0;
	}*/
	childlist.push_back(pChild);
}

/*
void vid_SceneObject::rotate(v3d pivot, float xAxisAngle, float yAxisAngle, float zAxisAngle)
{
	position.rotate(pivot, xAxisAngle, yAxisAngle, zAxisAngle);
}*/


matrix4x4 vid_SceneObject::collapseTransforms()
{
	if(parent)
		return transform*parent->collapseTransforms();
	else
		return transform;
}

///////////////////////////
// end vid_SceneObject
///////////////////////////

///////////////////////////
// vid_Face
///////////////////////////

void vid_Face::addVertex(vid_Point newVertex)
{
	verts.push_back(newVertex);
}

void vid_Face::rotate(v3d pivot, float xAxisAngle, float yAxisAngle, float zAxisAngle)
{
	for(unsigned int i = 0; i < verts.size(); i++)
	{
		verts[i].position.rotate(pivot, xAxisAngle, yAxisAngle, zAxisAngle);
	}
}

void vid_Face::draw(DrawingModes mode, int _layer)
{
	if(_layer != layer) return;
	/*if(!vid_initialized) return;
	if(verts.size() < 3) // not enough points to draw with
		return;*/

	matrix4x4 transform = collapseTransforms();

	vid_Point p1, p2, p3;

	p1 = verts[0];
	p1.position = transform * p1.position;

	for(unsigned int i = 1; i < verts.size() - 1; i++) // skip 0 since its the pivot vertex; draw as many triangles as needed to form the face (with a common vertex)
	{
		p2 = verts[i];
		p2.position = transform * p2.position;
		p3 = verts[i+1];
		p3.position = transform * p3.position;
		VIDW_DrawTriangle3d(p1, p2, p3, RotationMatrix(transform.getRot()) * normal, textureId);
	}
}

void vid_Face::findNormal()
{
	normal = (verts[2].position - verts[1].position).cross(verts[0].position - verts[1].position).makeUnitVector();
}

float vid_Face::distance(v3d rhs) // doesn't seem to like faces with verticies with negative values
{
	//v3d pq(rhs - verts[0].worldSpacePosition(this));
	//return pq.dot(normal);
	return (rhs - verts[0].position + collapseTransforms().getTranslation()).dot(normal);
}

void vid_Face::attachTexture(string fileName)
{
	textureId = Video.VideoResourceManager.LoadImageToVideoMemory(fileName);//VID_LoadTexture(fileName);
}
///////////////////////////
// end vid_Face
///////////////////////////

///////////////////////////
// vid_Tile
///////////////////////////

vid_Tile::vid_Tile(v3d _position, v3d _size, string textureFileName):vid_SceneObject(tile)
{ 
	transform = TranslationMatrix(_position);
	setSize(_size);
	if(textureFileName != "")
		attachTexture(textureFileName);
	string bcbFile = textureFileName.substr(0, textureFileName.size()-3) + "btp";
	if(M_FileExists(bcbFile))
	{
		ifstream fin;
		fin.open(bcbFile.c_str(), ifstream::in);
		readTileProperties(fin);
		fin.close();
	}
	else
	{
		ccout << "Warning: Did not find properties file for tile: " << textureFileName << newl;
	}

	if(!collisionBorders.empty())
		colltype = vid_SceneObject::solid;
	else
		colltype = vid_SceneObject::permiable;
}

void vid_Tile::attachTexture(string fileName)
{
	textureId = Video.VideoResourceManager.LoadImageToVideoMemory(fileName);//VID_LoadTexture(fileName);
}

void vid_Tile::draw(DrawingModes mode, int _layer)
{
	if(_layer != layer) return;
	if(!visible)
		return;
	//vid_lighting = true;

	Video.BlitBitmapScaled(textureId, collapseTransforms().getTranslation(), size, collapseTransforms().getZRot(), collapseTransforms().getTranslation(), true, true);
//	vid_lighting = true;


//#ifdef DEBUG_RAY
	if(mode == editor)
	{
		Video.DrawLineScaled( collapseTransforms().getTranslation(), collapseTransforms().getTranslation()+v3d(size.x), _RGB32BIT(255,255,255), true);
		Video.DrawLineScaled( collapseTransforms().getTranslation()+v3d(size.x), collapseTransforms().getTranslation()+size, _RGB32BIT(255,255,255), true);
		Video.DrawLineScaled( collapseTransforms().getTranslation()+size, collapseTransforms().getTranslation()+size-v3d(size.x), _RGB32BIT(255,255,255), true);
		Video.DrawLineScaled( collapseTransforms().getTranslation()+size-v3d(size.x), collapseTransforms().getTranslation(), _RGB32BIT(255,255,255), true);
		if(colltype <= solidWithEvent)
			for(vector<math_ExtrudedLineSegment>::iterator it = collisionBorders.begin(); it != collisionBorders.end(); it++)
				Video.DrawLineScaled(it->getP1(), it->getP2(), _RGB32BIT(0, 0, 255));
	}
//#endif
	//vid_lighting = false;
}

void vid_Tile::rotate(v3d pivot, float xaxisangle, float yaxisangle, float zaxisangle)
{
	ccout << "Warning... tile::rotate function doesn't use pivot\n";
	transform *= RotationMatrix(v3d(xaxisangle, yaxisangle, zaxisangle));
	//position.rotate(pivot, xaxisangle, yaxisangle, zaxisangle);
	//angle += v3d(xaxisangle, yaxisangle, zaxisangle);
}
///////////////////////////
// end vid_Face
///////////////////////////


///////////////////////////
// vid_Camera
///////////////////////////

void vid_Camera::update()
{
/*	view = (v3d(0,0,0)-position);
	up = v3d(0,1,0);
	v3d axis((view-position).cross(up)); // get x axis to rotate around
	axis.makeUnitVector();
	view.makeUnitVector();

	quaternion viewQuaternion(view.x, view.y, view.z, 0);
//	viewQuaternion.applyRotation(axis, 1);
//	viewQuaternion.applyRotation(v3d(0,1,0), dthetay);

	view = viewQuaternion.getVector();

	transform = matrix4x4::rotationFromQuaternion(viewQuaternion) * matrix4x4::mtfTranslate(position);

	ccout << "camera view: " << view << " camera position: " << position << newl;
	ccout << "positoin matrix: " << newl << matrix4x4::mtfTranslate(position) << newl;
	ccout << "rotation matrix: " << newl << matrix4x4::rotationFromQuaternion(viewQuaternion) << newl;*/
	
	switch(transition)
	{
	case none:
		{
			transform = destination;
			Video.settings.setZoom(destzoom);
		}
		break;
	case average:
		{
			v3d position = transform.getTranslation();
			v3d targetPosition = destination.getTranslation();
			transform = destination;
			transform.setTranslation(position + (targetPosition-position)*sys_frameTime*6);
			Video.settings.setZoom(destzoom);
		}
		break;
	case linear:
		{
			transform = destination;
			Video.settings.setZoom(destzoom);
		}
		break;
	default:
		break;
	}
}
const v3d vid_Camera::getDestpositionAngleFromOrigin()
{
	v3d result;

	// use y/x to find z axis rotation
	if((destination.getTranslation().x != 0) ||(destination.getTranslation().y != 0))
	{
		if(destination.getTranslation().x == 0)
		{
			result.z = 90.0; 
		}
		else
		{
			result.z = atan(destination.getTranslation().y/destination.getTranslation().x); // atan returns value in radians
			result.z *= 180/pi;
			if(destination.getTranslation().x < 0)
				result.z += 180;
		}
	}


	// use z/x to rotate on y axis
	if((destination.getTranslation().x != 0) ||(destination.getTranslation().z != 0))
	{
		if(destination.getTranslation().x == 0)
		{
			result.y = 90.0; 
		}
		else
		{
			result.y = atan(destination.getTranslation().z/destination.getTranslation().x); // atan returns value in radians
			result.y *= 180/pi;
			if(destination.getTranslation().x < 0)
				result.y += 180;
		}

	}

	// use y/z to rotate on x axis
	if((destination.getTranslation().y != 0) || (destination.getTranslation().x != 0))
	{
		if(destination.getTranslation().z == 0)
		{
			result.x = 90.0; 
		}
		else
		{
			result.x = atan(destination.getTranslation().y/destination.getTranslation().z); // atan returns value in radians
			result.x *= 180/pi;
			if(destination.getTranslation().z < 0)
				result.x += 180;
		}
	}

	return result;
}

v3d vid_Camera::worldToCameraSpace(const v3d& rhs)
{
	return transform.getTransform() * (rhs - transform.getTranslation());
}

void vid_Camera::lookAt(v3d lookAt)
{
	v3d cameraPosition(destination.getTranslation());
	v3d cameraVector = cameraPosition - lookAt;
	v3d rotation = v3d( 
																	-180/pi * atan( cameraVector.y/sqrt(cameraVector.x*cameraVector.x + cameraVector.z*cameraVector.z)),
																	((cameraVector.x < 0) ? -1 : 1) * 90 - 180/pi *atan( cameraVector.z/cameraVector.x ),
																	0 );
	destination = TranslationMatrix(cameraPosition) * RotationMatrix( v3d( 
																	-180/pi * atan( cameraVector.y/sqrt(cameraVector.x*cameraVector.x + cameraVector.z*cameraVector.z)),
																	((cameraVector.x < 0) ? -1 : 1) * 90 - 180/pi *atan( cameraVector.z/cameraVector.x ),
																	0 ));
//	destination = transform;
}


///////////////////////////
// end vid_Camera
///////////////////////////

///////////////////////////////
// Animation classes
///////////////////////////////

vid_Anim::vid_AnimFrame::~vid_AnimFrame()
{	
}


vid_Anim::vid_Anim()
:
head(0),
//parent(0),
nextframetime(0.0f),
loop(false),
done(false)
{
	head = new vid_AnimFrame;
}

vid_Anim::vid_Anim(vid_Anim& rhs)
:
head(0),
//parent(0),
nextframetime(0.0f),
loop(rhs.loop),
done(rhs.done)
{
	if(rhs.head)
	{
		head = new vid_AnimFrame;
		vid_AnimFrame* current = rhs.head;
		while(current)
		{
			addFrame(current->frametextureid, current->duration);
			current = current->next;
		} 
	}
}

vid_Anim::~vid_Anim()
{
	for(vid_AnimFrame* i = head; i != 0; )
	{
		vid_AnimFrame* temp = i->next;
		delete i;
		i = temp;
	}
	head = 0;
}

short vid_Anim::update()
{
	if(!head)
		return -1;

	if(nextframetime == 0.0f) // animation is updating for first time
	{
		nextframetime = sys_curTime + head->duration; // set the frame time out
	}
	
	if(sys_curTime >= nextframetime) // it's time for an update
	{
		if(loop)
		{
			// we need to place the current head at the end of the list
			// and make head->next the new head, or current frame
			if(head->next)
			{
				vid_AnimFrame* oldhead = head;
				head = head->next;
				addFrame(oldhead->frametextureid, oldhead->duration); // create a brand new node and put it at the end
				oldhead->next = 0; // isolate it from the list so it doesn't delete everything
				delete oldhead; // this can be deleted because its data was copied to a new object
				oldhead = 0;
			}
			nextframetime = sys_curTime + head->duration; // set the frame time out
			// else, leave the animation how it is, there is only one frame
		}
		else
		{
			// do animation once
			if(head->next)
			{
				vid_AnimFrame* oldhead = head;
				head = head->next;
				oldhead->next = 0; // isolate it from the list so it doesn't delete everything
				delete oldhead; // ditch this crap, we're only playing the anim once
				oldhead = 0;
				nextframetime = sys_curTime + head->duration; // set the frame time out
			}
			else
			{
				// at end of animation
				done = true;
			}
		}
	}
	return getFrame();
}

void vid_Anim::addFrame(short id, float duration) // add a frame to the animation
{
	// during lifespan, an Anim object always has a head, no need to create one here
	if(head->frametextureid == -1)
	{
		head->frametextureid = id;
		head->duration = duration;
	}
	else // head already has frame info
	{
		vid_AnimFrame* temp = new vid_AnimFrame;
		temp->frametextureid = id;
		temp->duration = duration;

		vid_AnimFrame* current = head; // traverse list until end node is found
		while(current->next)
		{
			current = current->next;
		}

		current->next = temp;
	}
}

vid_Anim vid_Anim::operator=(const vid_Anim& rhs)
{
	if(&rhs == this)
		return *this;

	// empty the list
	for(vid_AnimFrame* i = head; i != 0; )
	{
		vid_AnimFrame* temp = i->next;
		delete i;
		i = temp;
	}
	head = 0;

	loop = rhs.loop;
	done = rhs.done;
	nextframetime = 0;

	if(rhs.head)
	{
		head = new vid_AnimFrame;
		vid_AnimFrame* current = rhs.head;
		while(current)
		{
			addFrame(current->frametextureid, current->duration);
			current = current->next;
		} 
	}

	return *this;
}

void vid_AnimManager::readFromFile(string file)
{
	if(!M_FileExists(file))
	{
		ccout << "Error: animation file: " << file << " doesn't exist.\n";
		return;
	}

	ifstream fin;
	fin.open(file.c_str(), ifstream::in);

	string token;
	int anim(0);

	fin >> token;
	while((token != "begin_anim_list") && fin.good())
		fin >> token;

	fin >> token;
	while((token != "end_anim_list") && fin.good())
	{
	/*	if(token == "begin_attack_anim")
		{
			anim = (int)bn_Ent::anims::attack;
			fin >> token;
			bool loop = M_atob(token);
			fin >> token;
			bool stack = M_atob(token);
			createAnim(anim, loop, stack);
		}
		if(token == "begin_death_anim")
		{
			anim = (int)bn_Ent::anims::death;
			fin >> token;
			bool loop = M_atob(token);
			fin >> token;
			bool stack = M_atob(token);
			createAnim(anim, loop, stack);
		}
		if(token == "begin_idle_anim")
		{
			anim = (int)bn_Ent::anims::idle;
			fin >> token;
			bool loop = M_atob(token);
			fin >> token;
			bool stack = M_atob(token);
			createAnim(anim, loop, stack);
		}
		if(token == "begin_move_anim")
		{
			anim = (int)bn_Ent::anims::move;
			fin >> token;
			bool loop = M_atob(token);
			fin >> token;
			bool stack = M_atob(token);
			createAnim(anim, loop, stack);
		}
		if(token == "begin_pain_anim")
		{
			anim = (int)bn_Ent::anims::pain;
			fin >> token;
			bool loop = M_atob(token);
			fin >> token;
			bool stack = M_atob(token);
			createAnim(anim, loop, stack);
		}
		
		if(token == "frame")
		{
			string file;
			float duration;
			fin >> file >> duration;
			addFrame(anim, VID_LoadTexture(file), duration);
		}*/

		fin.ignore(1000, '\n');

		fin >> token;
	}

	fin.close();
}

short vid_AnimManager::createAnim(short id, bool loop, bool stack)
{
	if(!animationpool)
	{
		animationpool = new vid_AnimNode;
		animationpool->id = id;
		animationpool->animation = new vid_Anim;
		animationpool->animation->loop = loop;
		animationpool->animation->stack = stack;
		return animationpool->id;
	}
	else
	{
		vid_AnimNode* prev;
		for(vid_AnimNode* i = animationpool; i ; i = i->next)
		{
			prev = i;
			if(i->id == id)
				return -1; // error, this id already exists
		}
		prev->next = new vid_AnimNode;
		prev->next->id = id;
		prev->next->animation = new vid_Anim;
		prev->next->animation->loop = loop;
		prev->next->animation->stack = stack;
		return prev->next->id;
	}
}
void vid_AnimManager::addFrame(short animid, short textureid, float duration)
{
	for(vid_AnimNode* i = animationpool; i ; i = i->next)
	{
		if(i->id == animid)
		{
			i->animation->addFrame(textureid, duration);
		}
	}
}
bool vid_AnimManager::playAnim(short id)
{ 
	bool foundAnim(false);
	for(vid_AnimNode* i = animationpool; i ; i = i->next)
	{
		if(i->id == id)
		{
			if(i->animation->stack)
			{
				vid_AnimNode* parent = currentAnimation; // save address of current animation
				currentAnimation = new vid_AnimNode;
				currentAnimation->animation = new vid_Anim(*i->animation); // copy the animation into it
				currentAnimation->next = parent;
			}
			else
			{
				delete currentAnimation; // delete all stacked animations
				currentAnimation = new vid_AnimNode;
				currentAnimation->animation = new vid_Anim(*i->animation);
			}
			foundAnim = true;
			break;
		}
	}
	return foundAnim;
}

short vid_AnimManager::getCurrentAnimId()
{
	if(currentAnimation)
		return currentAnimation->id;
	else
		return -1;
}

short vid_AnimManager::getFrame()
{
	if(currentAnimation)
		return currentAnimation->animation->getFrame();
	else
		return -1;
}
bool vid_AnimManager::animationDone()
{
	if(currentAnimation)
		return (currentAnimation->animation->done);
	else
		return false;
}
short vid_AnimManager::update()
{
	if(currentAnimation)
	{
		if(!currentAnimation->animation->loop)
			if( currentAnimation->animation->done && currentAnimation->animation->stack )
			{
				if(currentAnimation->next)
				{
					vid_AnimNode* temp = currentAnimation->next;
					currentAnimation->next = 0; // so parent isn't deleted by constructor
					delete currentAnimation;
					currentAnimation = temp;
				}
			}
		return currentAnimation->animation->update();
	}
	else
		return -1;
}



unsigned long vid_AliasSet::idCounter = 1; // 0 is an invalid id... ?is that useful? ?for overflow?

vid_AliasSet::vid_AliasSet():
head(0)
{}

vid_AliasSet::~vid_AliasSet()
// pre: nothing
// post: list is clear, memory has been freed
{
	for(vid_AliasNode* i = head; i != 0; )
	{
		vid_AliasNode* temp = i->next;
		delete i;
		i = temp;
	}
}

bool vid_AliasSet::createAlias(vid_SceneObject* object, string name)
// post: return of false means error
{
	if(!head) // list is totally empty
	{
		head = new vid_AliasNode;
		// constructor sets next and previous to 0
		head->alias._object = object;
		head->alias._name = name;
		head->alias._uid = idCounter; // assign a brand new uid to it
		return true;
	}
	else
	{
		idCounter++; // now idCounter is our new UID, unless it's already used...
		vid_AliasNode* i(0);
		for(i = head; i->next != 0; i = i->next) // gets to the end of list, but ALSO checks for an already existing ID
		{
			if( (i->alias._object == object) ||
				(i->alias._name == name) && (name != "") ) 
			{
				// shit... already exists
				return false;
			}

			if( i->alias._uid == idCounter ) 
			{
				idCounter++;
				if(idCounter == 0) // very bad, ran out of ID's, not really feasable
				{
					ccout << "Ran out of UID's! How did you manage that?\n";
					return false;
				}
				i = head; // start over, because idCounter has changed. need to check all again!!!
			}
		}
		// i is now the last item in the list, next is NULL
		// idCounter is definitely unique to this system
		vid_AliasNode* newGuy = i->next = new vid_AliasNode; // tack this guy on the end of list
		// constructor sets next and previous to 0
		newGuy->prev = i; // the new guy points back to i
		newGuy->alias._object = object;
		newGuy->alias._name = name;
		newGuy->alias._uid = idCounter; // assign a brand new uid to it

		return true;
	}
}

void vid_AliasSet::destroyAlias(string name)
{
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( (i->alias._name == name) && (name != "") ) 
		{
			if(i->prev) // is anything but the head
			{
				i->prev->next = i->next; // point the previous to the one after one to be removed
				if(i->next)
					i->next->prev = i->prev; 
			}
			else // this is the head of the list
			{
				head = i->next;
				if(head)
					head->prev = 0;
			}
			i->next = 0;
			i->prev = 0;
			delete i;
			i = 0;
			return;
		}
	}
}
void vid_AliasSet::destroyAlias(unsigned long uid)
{
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( i->alias._uid == uid ) 
		{
			if(i->prev) // is anything but the head
			{
				i->prev->next = i->next; // point the previous to the one after one to be removed
				if(i->next)
					i->next->prev = i->prev; 
			}
			else // this is the head of the list
			{
				head = i->next;
				if(head)
					head->prev = 0;
			}
			i->next = 0;
			i->prev = 0;
			delete i;
			i = 0;
			return;
		}
	}
}
void vid_AliasSet::destroyAlias(vid_SceneObject* object)
{
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( i->alias._object == object ) 
		{
			if(i->prev) // is anything but the head
			{
				i->prev->next = i->next; // point the previous to the one after one to be removed
				if(i->next)
					i->next->prev = i->prev; 
			}
			else // this is the head of the list
			{
				head = i->next;
				if(head)
					head->prev = 0;
			}
			i->next = 0;
			i->prev = 0;
			delete i;
			i = 0;
			return;
		}
	}
}

unsigned long vid_AliasSet::generateNewUid(string name)
{
	idCounter++; // now idCounter is our new UID, unless it's already used...

	while(uidIsUsed(idCounter))			// if it's already in use, increment the counter
		idCounter++;

	setUid(name, idCounter);
	return idCounter;
}

unsigned long vid_AliasSet::generateNewUid(unsigned long uid)
{
	idCounter++; // now idCounter is our new UID, unless it's already used...

	while(uidIsUsed(idCounter))			// if it's already in use, increment the counter
		idCounter++;

	setUid(uid, idCounter);
	return idCounter;
}

unsigned long vid_AliasSet::generateNewUid(vid_SceneObject* object)
{
	idCounter++; // now idCounter is our new UID, unless it's already used...

	while(uidIsUsed(idCounter))			// if it's already in use, increment the counter
		idCounter++;

	setUid(object, idCounter);
	return idCounter;
}



void vid_AliasSet::setName(string currentName, string newName)
{
	if(!head)
		return;
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( (i->alias._name == newName) && (newName != "") ) 
		{
			// shit... already exists
			return;
		}
	}
	for(vid_AliasNode* i = head; i != 0; i = i->next) 
	{
		if(i->alias._name == currentName)
		{
			i->alias._name = newName;
			return;
		}
	}
}
// pre: newName is not already registered
void vid_AliasSet::setName(unsigned long uid, string name)
{
	if(!head)
		return;
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( (i->alias._name == name) && (name != "") ) 
		{
			// shit... already exists
			return;
		}
	}
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if(i->alias._uid == uid)
		{
			i->alias._name = name;
			return;
		}
	}
}

// pre: newName is not already registered
void vid_AliasSet::setName(vid_SceneObject* object, string name)
{
	if(!head)
		return;
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( (i->alias._name == name) && (name != "") ) 
		{
			// shit... already exists
			return;
		}
	}
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if(i->alias._object == object)
		{
			i->alias._name = name;
			return;
		}
	}
}
// pre: newName is not already registered

void vid_AliasSet::setUid(string name, unsigned long uid)
{
	if(!head)
		return;
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( i->alias._uid == uid ) 
		{
			// shit... already exists
			return;
		}
	}
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if(i->alias._name == name)
		{
			i->alias._uid = uid;
			return;
		}
	}
}
// pre: newUid is not already registered
void vid_AliasSet::setUid(unsigned long currentUid, unsigned long newUid)
{
	if(!head)
		return;
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( i->alias._uid == newUid ) 
		{
			// shit... already exists
			return;
		}
	}
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if(i->alias._uid == currentUid)
		{
			i->alias._uid = newUid;
			return;
		}
	}
}
// pre: newUid is not already registered
void vid_AliasSet::setUid(vid_SceneObject* object, unsigned long uid)
{
	if(!head)
		return;
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( i->alias._uid == uid ) 
		{
			// shit... already exists
			return;
		}
	}
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if(i->alias._object == object)
		{
			i->alias._uid = uid;
			return;
		}
	}
}
// pre: newUid is not already registered

void vid_AliasSet::setObject(string name, vid_SceneObject* object)
{
	if(!head)
		return;
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( i->alias._object == object ) 
		{
			// shit... already exists
			return;
		}
	}
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if(i->alias._name == name)
		{
			i->alias._object = object;
			return;
		}
	}
}
// pre: newObject is not already registered
void vid_AliasSet::setObject(unsigned long uid, vid_SceneObject* object)
{
	if(!head)
		return;
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( i->alias._object == object ) 
		{
			// shit... already exists
			return;
		}
	}
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if(i->alias._uid == uid)
		{
			i->alias._object = object;
			return;
		}
	}
}
// pre: newObject is not already registered
void vid_AliasSet::setObject(vid_SceneObject* currentObject, vid_SceneObject* newObject)
{
	if(!head)
		return;
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( i->alias._object == newObject ) 
		{
			// shit... already exists
			return;
		}
	}
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if(i->alias._object == currentObject)
		{
			i->alias._object = newObject;
			return;
		}
	}
}
// pre: newObject is not already registered

string vid_AliasSet::getName(unsigned long uid)
{
	if(!head)
		return string("");
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if( i->alias._uid == uid ) 
		{
			return i->alias._name;
		}
	}
	return string("");
}
string vid_AliasSet::getName(vid_SceneObject* object)
{
	if(!head)
		return string("");
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if( i->alias._object == object ) 
		{
			return i->alias._name;
		}
	}
	return string("");
}

unsigned long vid_AliasSet::getUid(string name)
{
	if(!head)
		return 0;
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if( i->alias._name == name ) 
		{
			return i->alias._uid;
		}
	}
	return 0;
}
unsigned long vid_AliasSet::getUid(vid_SceneObject* object)
{
	if(!head)
		return 0;
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if( i->alias._object == object ) 
		{
			return i->alias._uid;
		}
	}
	return 0;
}

vid_SceneObject* vid_AliasSet::getObject(string name)
{
	if(!head)
		return 0;
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if( i->alias._name == name ) 
		{
			return i->alias._object;
		}
	}
	return 0;
}
vid_SceneObject* vid_AliasSet::getObject(unsigned long uid)
{
	if(!head)
		return 0;
	for(vid_AliasNode* i = head; i != 0; i = i->next)
	{
		if( i->alias._uid == uid ) 
		{
			return i->alias._object;
		}
	}
	return 0;
}

bool vid_AliasSet::uidIsUsed(unsigned long uid)
{
	for(vid_AliasNode* i = head; i != 0; i = i->next) // checks for an already existing ID
	{
		if( i->alias._uid == uid ) 
		{
			return true;
		}
	}
	return false;
}


vid_AliasSet::vid_AliasNode::vid_Alias::vid_Alias():
_object(0),
_uid(0),
_name("")
{}
// pre: nothing
// post: object == 0, uid is 0 (invalid), name is ""
vid_AliasSet::vid_AliasNode::vid_Alias::~vid_Alias()
{}



vid_AliasSet::vid_AliasNode::vid_AliasNode():
next(0),
prev(0)
{}
vid_AliasSet::vid_AliasNode::~vid_AliasNode()
{
}


vid_AliasSet aliases;

void bn_Doodad::draw(DrawingModes mode, int layer)
{
	if(!visible) return;
	/*if(layer == 0)
		VID_BlitBitmapScaled(groundTexture, position.worldSpacePosition(this->parent), size, angle.z, position.worldSpacePosition(parent), true, true);
*/	if(layer == 1)
		Video.BlitBitmapScaled(shadowTexture, collapseTransforms().getTranslation(), size, collapseTransforms().getZRot(), collapseTransforms().getTranslation(), true, true);
	if(layer == objectTextureLayer)
		Video.BlitBitmapScaled(objectTexture, collapseTransforms().getTranslation(), size, collapseTransforms().getZRot(), collapseTransforms().getTranslation(), true, true);
}
