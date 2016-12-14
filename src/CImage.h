#ifndef CIMAGE_H
#define CIMAGE_H

#include "CVideoSubsystem.h"
#include "CColor.h"

#include <string>
using namespace std;

class CBitmapImageLoader;
class CTargaImageLoader;
class CImage
{
public:
	friend class CBitmapImageLoader;
	friend class CTargaImageLoader;

	CImage();
	CImage(VideoResourceID newVideoResourceID);
	~CImage();
	CImage(const CImage& rhs);

	rgba8888pixel* GetBuffer();
	unsigned short GetWidth();
	unsigned short GetHeight();
	void SetVideoResourceID(VideoResourceID newVideoResourceID);
	VideoResourceID GetVideoResourceID();
	unsigned long GetMemoryUsage();
private:
	rgba8888pixel* buffer;
	unsigned short width;
	unsigned short height;
	VideoResourceID videoResourceID;
};

class COpenGLTexture
{
public:
	friend class CVideoResourceManager;

	COpenGLTexture():memoryUsage(0),width(0),height(0){}
	~COpenGLTexture(){}

	void SetVideoResourceID(VideoResourceID newVideoResourceID);
	VideoResourceID GetVideoResourceID();
	unsigned long GetMemoryUsage();
	unsigned short GetWidth();
	unsigned short GetHeight();
private:
	unsigned long memoryUsage;
	unsigned short width;
	unsigned short height;
	VideoResourceID videoResourceID;
};

class IImageLoader
{
public:
	virtual bool IsReaderForFile(const string filename) = 0;
	virtual bool ReadFromFile(CImage& image, const string filename) = 0;
	virtual bool SaveToFile(const CImage& image, const string filename, bool overwrite) = 0;
};




#endif