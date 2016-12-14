
#include "CBitmapImageLoader.h"
#include "CColor.h"
#include "con_main.h"
//#include <windows.h>
#include <fstream>
using namespace std;

CBitmapImageLoader BitmapImageLoader;

#pragma pack(2)
typedef struct tagBITMAPFILEHEADER {
        unsigned short    bfType;
        unsigned long   bfSize;
        unsigned short    bfReserved1;
        unsigned short    bfReserved2;
        unsigned long   bfOffBits;
} BITMAPFILEHEADER/*, far *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER*/;

typedef struct tagBITMAPINFOHEADER{
        unsigned long      biSize;
        long       biWidth;
        long       biHeight;
        unsigned short       biPlanes;
        unsigned short       biBitCount;
        unsigned long      biCompression;
        unsigned long      biSizeImage;
        long       biXPelsPerMeter;
        long       biYPelsPerMeter;
        unsigned long      biClrUsed;
        unsigned long      biClrImportant;
} BITMAPINFOHEADER/*, FAR *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER*/;

typedef struct tagPALETTEENTRY {
    unsigned char        peRed;
    unsigned char        peGreen;
    unsigned char        peBlue;
    unsigned char        peFlags;
} PALETTEENTRY/*, *PPALETTEENTRY, FAR *LPPALETTEENTRY*/;

typedef struct BITMAP_FILE_TAG
{
	BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
	BITMAPINFOHEADER bitmapinfoheader;  // this is all the info including the palette
	PALETTEENTRY     palette[256];      // we will store the palette here
	unsigned long    *buffer;           // this is a pointer to the data
} BITMAP_FILE, *BITMAP_FILE_PTR;
#pragma pack()

bool CBitmapImageLoader::IsReaderForFile(const string filename)
{
	BITMAP_FILE bitmap;

	//FILE *fp; // file pointer
	ifstream file(filename.c_str(), ios::binary);

	// first read the file to see if it exists
	if(file.fail())
	{
		return false;
	}

	// read in the file header
	//fread(&bitmap->bitmapfileheader, sizeof(BITMAPFILEHEADER), 1, fp);
	file.read((char*)&bitmap.bitmapfileheader, sizeof(BITMAPFILEHEADER));

	// check if file is a bitmap file - check 
	// for 'BM'reversed(ie is 'MB')
	// or check if the value is 0x4d42

	if(bitmap.bitmapfileheader.bfType != 'MB')
	{
		file.close(); // close the file as non-bmp 
		//files are not of our interest here
		return false;
	}
	return true;
}

bool CBitmapImageLoader::ReadFromFile(CImage& image, const string filename)
{
	BITMAP_FILE bitmap;

	ifstream file(filename.c_str(), ios::binary);

	// first read the file to see if it exists
	if(file.fail())
	{
		return false;
	}

	// read in the file header
	file.read((char*)&bitmap.bitmapfileheader, sizeof(BITMAPFILEHEADER));

	// check if file is a bitmap file - check 
	// for 'BM'reversed(ie is 'MB')
	// or check if the value is 0x4d42

	if(bitmap.bitmapfileheader.bfType != 'MB')
	{
		file.close(); // close the file as non-bmp 
		//files are not of our interest here
		return false;
	}


	// now read in the info header
	file.read((char*)&bitmap.bitmapinfoheader, sizeof(BITMAPINFOHEADER));

	// read the image size to check if we need to compute 
	//the value
	int size = bitmap.bitmapinfoheader.biSizeImage;
	if(size == 0)
	{
		size = bitmap.bitmapinfoheader.biWidth * 
			((bitmap.bitmapinfoheader.biBitCount) / 8) *
			bitmap.bitmapinfoheader.biHeight;
	}

	short bpp = bitmap.bitmapinfoheader.biBitCount;
	long width = bitmap.bitmapinfoheader.biWidth;
	image.width = width;
	long height = bitmap.bitmapinfoheader.biHeight;
	image.height = height;
	char * temp;

	if(bpp == 24 || bpp == 32) // allocate buffers and read in raw data
	{
		temp = new char[size];

		// allocate memory for image
		if(image.buffer)
			delete [] image.buffer;
		image.buffer = new rgba8888pixel [size/(bpp/8)];
		
		// read in the image
		file.read(temp, size);
	}

	if(bpp == 24)
	{
		//convert from 24 to 32 bit
		bgr888pixel* bitmapImage = (bgr888pixel*)temp;
		for(int x = 0; x < height; x++)
		{
			for(int y = 0; y < width; y++)
			{
				bgr888pixel pixel = bitmapImage[x+y*width];
				image.buffer[x+y*width].components.r = pixel.r;
				image.buffer[x+y*width].components.g = pixel.g;
				image.buffer[x+y*width].components.b = pixel.b;
				image.buffer[x+y*width].components.a = 255;
				if( (image.buffer[x+y*width].pixel ==  VID_COLORKEY) || (image.buffer[x+y*width].pixel == VID_COLORKEY2) )
					image.buffer[x+y*width].components.a = 0;
			}
		}

		delete [] temp;
	}
	else if(bpp == 32)
	{
		bgra8888pixel* bitmapImage = (bgra8888pixel*)temp;
		for(int x = 0; x < height; x++)
		{
			for(int y = 0; y < width; y++)
			{
				bgra8888pixel pixel = bitmapImage[x+y*width];
				image.buffer[x+y*width].components.r = pixel.components.r;
				image.buffer[x+y*width].components.g = pixel.components.g;
				image.buffer[x+y*width].components.b = pixel.components.b;
				image.buffer[x+y*width].components.a = 255;
				if( (image.buffer[x+y*width].pixel ==  VID_COLORKEY) || (image.buffer[x+y*width].pixel == VID_COLORKEY2) )
					image.buffer[x+y*width].components.a = 0;
			}
		}

		delete [] temp;
	}
	else
	{
		ccout << filename << " is of an unsupported bit depth." << newl;
		file.close();
		return false;
	}

	file.close();

	return true;
}

bool CBitmapImageLoader::SaveToFile(const CImage& image, const string filename, bool overwrite)
{
	// Try to open the file...
	ofstream fout;
	if (!overwrite)
	{
		if (M_FileExists(filename))
		{
			ccout << "Overwrite set to false and \"" << filename << "\" exists!!" << newl;
			return false;
		}
		else
			fout.open(filename.c_str(), ofstream::binary);
	}
	else // if (overwrite)
		fout.open(filename.c_str(), ofstream::binary | ofstream::trunc);
	
	// Should probably check to see if we actually opened it...
	if (fout.fail())
	{
		ccout << "Unable to open \"" << filename << "\" for writing!!" << newl;
		return false;
	}
	
	// The format of the bmp file...
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	
	bmih.biSize			= sizeof(BITMAPINFOHEADER);
	bmih.biWidth		= image.width;
	bmih.biHeight		= image.height;
	bmih.biPlanes		= 1;	// Only one plane
	bmih.biBitCount		= 24;	// 24-bit
	bmih.biCompression	= 0;	// No compression
	bmih.biSizeImage	= bmih.biWidth * (bmih.biBitCount/8) * bmih.biHeight;	// (24/8)=3 for 24-bit
	bmih.biXPelsPerMeter = 3780;	// Probably should be something...
	bmih.biYPelsPerMeter = 3780;	// Probably should be something...
	bmih.biClrUsed		= 0;	// Signals that no colour palette is used
	bmih.biClrImportant	= 0;	// Signals that all colours are important
	
	bmfh.bfOffBits		= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // Offset from start of file
	bmfh.bfReserved2	= 0;	// Whatever this is...
	bmfh.bfReserved1	= 0;	// Whatever this is...
	bmfh.bfSize			= bmih.biSizeImage + bmfh.bfOffBits; // Complete file size is the size of the image plus the size of the headers
	bmfh.bfType			= 0x4D42;	// MUST be "BM" (or 19778)
	
	// Write the file header...
	fout.write(reinterpret_cast<char*>(&bmfh), sizeof(BITMAPFILEHEADER));
	
	// Write the info header...
	fout.write(reinterpret_cast<char*>(&bmih), sizeof(BITMAPINFOHEADER));
	
	// Write each pixel (for ensuring char order, do it char-by-char)
	for(int i = 0; i < (bmih.biWidth * bmih.biHeight); i++)
		fout << image.buffer[i].components.b << image.buffer[i].components.g << image.buffer[i].components.r;
	
	// Flush and close the stream...
	fout.flush();
	fout.close();
	
	ccout << "Successfully wrote \"" << filename << "\"!" << newl;
	
	// Success!
	return true;
}