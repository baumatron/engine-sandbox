
#ifndef CTARGAIMAGELOADER_H
#define CTARGAIMAGELOADER_H

#include "CImage.h"

class CTargaImageLoader: public IImageLoader
{
public:
	virtual bool IsReaderForFile(const string filename);
	virtual bool ReadFromFile(CImage& image, const string filename);
	virtual bool SaveToFile(const CImage& image, const string filename, bool overwrite);
};

extern CTargaImageLoader TargaImageLoader;

#endif