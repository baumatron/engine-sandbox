#include "CImage.h"

#ifdef WIN32
#include <windows.h>								// Header File For Windows
#endif
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glaux.h>

CImage::CImage():
	buffer(0),
	width(0),
	height(0),
	videoResourceID(0)
{
}
CImage::CImage(VideoResourceID newVideoResourceID):
	buffer(0),
	width(0),
	height(0),
	videoResourceID(newVideoResourceID)
{
}

CImage::~CImage()
{
	if(buffer)
		delete [] buffer;
}

CImage::CImage(const CImage& rhs):
	buffer(0),
	width(0),
	height(0),
	videoResourceID(0)
{
	if(rhs.buffer)
	{
		buffer = new rgba8888pixel [rhs.width*rhs.height];
		for(int i = 0; i < (rhs.width*rhs.height); i++)
		{
			buffer[i] = rhs.buffer[i];
		}
	}
}


rgba8888pixel* CImage::GetBuffer()
{
	return buffer;
}
unsigned short CImage::GetWidth()
{
	return width;
}
unsigned short CImage::GetHeight()
{
	return height;
}

void CImage::SetVideoResourceID(VideoResourceID newVideoResourceID)
{
	videoResourceID = newVideoResourceID;
}

VideoResourceID CImage::GetVideoResourceID()
{
	return videoResourceID;
}

unsigned long CImage::GetMemoryUsage()
{
	return (sizeof(CImage)+width*height*4);
}

void COpenGLTexture::SetVideoResourceID(VideoResourceID newVideoResourceID)
{
	videoResourceID = newVideoResourceID;
}

VideoResourceID COpenGLTexture::GetVideoResourceID()
{
	return videoResourceID;
}

unsigned long COpenGLTexture::GetMemoryUsage()
{
	return memoryUsage;
}
unsigned short COpenGLTexture::GetWidth()
{
	return width;
}
unsigned short COpenGLTexture::GetHeight()
{
	return height;
}

