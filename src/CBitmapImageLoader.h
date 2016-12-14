
#ifndef CBITMAPIMAGELOADER_H
#define CBITMAPIMAGELOADER_H

#include "CImage.h"

class CBitmapImageLoader: public IImageLoader
{
public:
	virtual bool IsReaderForFile(const string filename);
	virtual bool ReadFromFile(CImage& image, const string filename);
	virtual bool SaveToFile(const CImage& image, const string filename, bool overwrite);
};

extern CBitmapImageLoader BitmapImageLoader;

#endif