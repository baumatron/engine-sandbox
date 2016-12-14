

#include "CTargaImageLoader.h"
#include "CColor.h"

CTargaImageLoader TargaImageLoader;

#pragma pack(1)

struct TargaColorMapSpecification
{
	unsigned short colorMapOrigin;
	unsigned short colorMapLength;
	unsigned char colorMapEntrySize;
};

struct TargaImageSpecification
{
	unsigned short xOrigin;
	unsigned short yOrigin;
	unsigned short width;
	unsigned short height;
	unsigned char pixelSize;
	union
	{
		unsigned char byte;
		struct
		{
			unsigned char pixelAttributeBits : 4;
			unsigned char reserved : 1;
			unsigned char screenOrigin : 1;
			unsigned char storageInterleavingFlag : 2;
		};
	} imageDescriptorByte;
};

struct TargaHeader
{
	unsigned char imageIdentificationFieldLength;
	unsigned char colorMapType;
	unsigned char imageType;
	TargaColorMapSpecification colorMapSpecification;
	TargaImageSpecification imageSpecification;
};

struct TargaImage
{
	TargaHeader header;
	unsigned char* imageIdentificationField;
	unsigned char* colorMapData;
	unsigned char* imageData;
	unsigned long imageDataSize;
};
#pragma pack()

bool CTargaImageLoader::IsReaderForFile(const string filename)
{
	if(filename.substr(filename.length()-4, 4) == ".tga")
		return true;
	else
		return false;
}

bool CTargaImageLoader::ReadFromFile(CImage& image, const string filename)
{
	TargaImage targaImage;
	targaImage.imageIdentificationField = 0;
	targaImage.colorMapData = 0;
	targaImage.imageData = 0;

	ifstream file(filename.c_str(), ios::binary);

	// first read the file to see if it exists
	if(file.fail())
	{
		return false;
	}

	// read in the file header
	file.read((char*)&targaImage.header, sizeof(TargaHeader));

	// check to see if there is an identification field, and if so, read it in
	if( targaImage.header.imageIdentificationFieldLength > 0 )
	{
		// there is an image identification field
		targaImage.imageIdentificationField = new unsigned char[ targaImage.header.imageIdentificationFieldLength ];
		// read in the image identification field now
		file.read( (char*)targaImage.imageIdentificationField, targaImage.header.imageIdentificationFieldLength );
	}

	// check to see if there is a color map, and if so, read it in
	if( targaImage.header.colorMapType != 0 )
	{
		// there is a color map
		targaImage.colorMapData = new unsigned char [ targaImage.header.colorMapSpecification.colorMapLength * targaImage.header.colorMapSpecification.colorMapEntrySize/8 ];
		// read it in
		file.read( (char*)targaImage.colorMapData, targaImage.header.colorMapSpecification.colorMapLength * targaImage.header.colorMapSpecification.colorMapEntrySize/8 );
	}



	// allocate space and read in the image data
	targaImage.imageDataSize = targaImage.header.imageSpecification.width * 
												targaImage.header.imageSpecification.height * 
												targaImage.header.imageSpecification.pixelSize/8;
	targaImage.imageData = new unsigned char  [	targaImage.imageDataSize ];

	// read in the image data
	file.read( (char*)targaImage.imageData, targaImage.imageDataSize );

	// prepare the CImage buffer
	if(image.buffer)
		delete [] image.buffer;
	image.buffer = new rgba8888pixel [ targaImage.header.imageSpecification.width * targaImage.header.imageSpecification.height ];


	image.width = targaImage.header.imageSpecification.width;
	image.height = targaImage.header.imageSpecification.height;

	// now convert the image data to the generic CImage format, RGBA 32 bit
	switch(targaImage.header.imageType)
	{
	case 2: // uncompressed, rgb
		{
			switch(targaImage.header.imageSpecification.pixelSize)
			{
			case 16:
				{
					if(targaImage.header.imageSpecification.imageDescriptorByte.screenOrigin == 0) // origin is lower left
					{
						for(int i = 0; i < targaImage.header.imageSpecification.width * targaImage.header.imageSpecification.height; i++)
						{
							if(targaImage.header.imageSpecification.imageDescriptorByte.pixelAttributeBits == 1)
							{
								bgr555pixel pixel = ((bgr555pixel*)targaImage.imageData)[i];
								image.buffer[i].components.r = (pixel.components.r * 255)/32;
								image.buffer[i].components.g = (pixel.components.g * 255)/32;
								image.buffer[i].components.b = (pixel.components.b * 255)/32;
							}
							else
							{
								bgr565pixel pixel = ((bgr565pixel*)targaImage.imageData)[i];
								image.buffer[i].components.r = (pixel.components.r * 255)/32;
								image.buffer[i].components.g = (pixel.components.g * 255)/32;
								image.buffer[i].components.b = (pixel.components.b * 255)/32;
							}
						}					
					}
					else // origin is upper left
					{
						for(int y = 0; y < targaImage.header.imageSpecification.height; y++)
						{
							for(int x = 0; x < targaImage.header.imageSpecification.width; x++)
							{
								if(targaImage.header.imageSpecification.imageDescriptorByte.pixelAttributeBits == 1)
								{
									bgr555pixel pixel = ((bgr555pixel*)targaImage.imageData)[x+(targaImage.header.imageSpecification.height-1-y)*targaImage.header.imageSpecification.width];
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.r = (pixel.components.r * 255)/32;
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.g = (pixel.components.g * 255)/32;
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.b = (pixel.components.b * 255)/32;
								}
								else
								{
									bgr565pixel pixel = ((bgr565pixel*)targaImage.imageData)[x+(targaImage.header.imageSpecification.height-1-y)*targaImage.header.imageSpecification.width];
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.r = (pixel.components.r * 255)/32;
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.g = (pixel.components.g * 255)/64;
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.b = (pixel.components.b * 255)/32;
								}
							}
						}
					}
				}
				break;
			case 24:
				{
					/*if(targaImage.header.imageSpecification.imageDescriptorByte.screenOrigin == 0) // origin is lower left
					{
						for(int i = 0; i < targaImage.header.imageSpecification.width * targaImage.header.imageSpecification.height; i++)
						{
							bgr888pixel pixel = ((bgr888pixel*)targaImage.imageData)[i];
							image.buffer[i].components.r = pixel.r;
							image.buffer[i].components.g = pixel.g;
							image.buffer[i].components.b = pixel.b;
						}
					}
					else // origin is upper left
					{*/
						for(int y = 0; y < targaImage.header.imageSpecification.height; y++)
						{
							bgr888pixel pixel;
							for(int x = 0; x < targaImage.header.imageSpecification.width; x++)
							{
								if(targaImage.header.imageSpecification.imageDescriptorByte.screenOrigin == 0) // lower left origin
									pixel = ((bgr888pixel*)targaImage.imageData)[x+y*targaImage.header.imageSpecification.width];
								else	// upper left origin
									pixel = ((bgr888pixel*)targaImage.imageData)[x+(targaImage.header.imageSpecification.height-1-y)*targaImage.header.imageSpecification.width];
		
								
								image.buffer[x+y*targaImage.header.imageSpecification.width].components.r = pixel.r;
								image.buffer[x+y*targaImage.header.imageSpecification.width].components.g = pixel.g;
								image.buffer[x+y*targaImage.header.imageSpecification.width].components.b = pixel.b;
							}
						}
						
					//}
				}
				break;
			case 32:
				{
					for(int y = 0; y < targaImage.header.imageSpecification.height; y++)
					{
						bgra8888pixel pixel;
						for(int x = 0; x < targaImage.header.imageSpecification.width; x++)
						{
							if(targaImage.header.imageSpecification.imageDescriptorByte.screenOrigin == 0) // lower left origin
								pixel = ((bgra8888pixel*)targaImage.imageData)[x+y*targaImage.header.imageSpecification.width];
							else	// upper left origin
								pixel = ((bgra8888pixel*)targaImage.imageData)[x+(targaImage.header.imageSpecification.height-1-y)*targaImage.header.imageSpecification.width];
							
							image.buffer[x+y*targaImage.header.imageSpecification.width].components.r = pixel.components.r;
							image.buffer[x+y*targaImage.header.imageSpecification.width].components.g = pixel.components.g;
							image.buffer[x+y*targaImage.header.imageSpecification.width].components.b = pixel.components.b;
							
							if(targaImage.header.imageSpecification.imageDescriptorByte.pixelAttributeBits == 8)
								image.buffer[x+y*targaImage.header.imageSpecification.width].components.a = pixel.components.a;
							else
								image.buffer[x+y*targaImage.header.imageSpecification.width].components.a = 255;
						}
					}
				}
				break;
			}
		}
		break;
	case 10: // runlength encoded, rgb
		{
			switch(targaImage.header.imageSpecification.pixelSize)
			{
			case 16:
				{
					if(targaImage.header.imageSpecification.imageDescriptorByte.screenOrigin == 0) // origin is lower left
					{
						for(int i = 0; i < targaImage.header.imageSpecification.width * targaImage.header.imageSpecification.height; i++)
						{
							if(targaImage.header.imageSpecification.imageDescriptorByte.pixelAttributeBits == 1)
							{
								bgr555pixel pixel = ((bgr555pixel*)targaImage.imageData)[i];
								image.buffer[i].components.r = (pixel.components.r * 255)/32;
								image.buffer[i].components.g = (pixel.components.g * 255)/32;
								image.buffer[i].components.b = (pixel.components.b * 255)/32;
							}
							else
							{
								bgr565pixel pixel = ((bgr565pixel*)targaImage.imageData)[i];
								image.buffer[i].components.r = (pixel.components.r * 255)/32;
								image.buffer[i].components.g = (pixel.components.g * 255)/32;
								image.buffer[i].components.b = (pixel.components.b * 255)/32;
							}
						}					
					}
					else // origin is upper left
					{
						for(int y = 0; y < targaImage.header.imageSpecification.height; y++)
						{
							for(int x = 0; x < targaImage.header.imageSpecification.width; x++)
							{
								if(targaImage.header.imageSpecification.imageDescriptorByte.pixelAttributeBits == 1)
								{
									bgr555pixel pixel = ((bgr555pixel*)targaImage.imageData)[x+(targaImage.header.imageSpecification.height-1-y)*targaImage.header.imageSpecification.width];
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.r = (pixel.components.r * 255)/32;
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.g = (pixel.components.g * 255)/32;
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.b = (pixel.components.b * 255)/32;
								}
								else
								{
									bgr565pixel pixel = ((bgr565pixel*)targaImage.imageData)[x+(targaImage.header.imageSpecification.height-1-y)*targaImage.header.imageSpecification.width];
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.r = (pixel.components.r * 255)/32;
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.g = (pixel.components.g * 255)/64;
									image.buffer[x+y*targaImage.header.imageSpecification.width].components.b = (pixel.components.b * 255)/32;
								}
							}
						}
					}
				}
				break;
			case 24:
				{
					if(targaImage.header.imageSpecification.imageDescriptorByte.screenOrigin == 0) // origin is lower left
					{
						for(int i = 0; i < targaImage.header.imageSpecification.width * targaImage.header.imageSpecification.height; i++)
						{
							bgr888pixel pixel = ((bgr888pixel*)targaImage.imageData)[i];
							image.buffer[i].components.r = pixel.r;
							image.buffer[i].components.g = pixel.g;
							image.buffer[i].components.b = pixel.b;
						}
					}
					else // origin is upper left
					{
						for(int y = 0; y < targaImage.header.imageSpecification.height; y++)
						{
							for(int x = 0; x < targaImage.header.imageSpecification.width; x++)
							{
								bgr888pixel pixel = ((bgr888pixel*)targaImage.imageData)[x+(targaImage.header.imageSpecification.height-1-y)*targaImage.header.imageSpecification.width];
								image.buffer[x+y*targaImage.header.imageSpecification.width].components.r = pixel.r;
								image.buffer[x+y*targaImage.header.imageSpecification.width].components.g = pixel.g;
								image.buffer[x+y*targaImage.header.imageSpecification.width].components.b = pixel.b;
							}
						}
						
					}
				}
				break;
			case 32:
				{
					for(int y = 0; y < targaImage.header.imageSpecification.height; y++)
					{
						for(int x = 0; x < targaImage.header.imageSpecification.width; x++)
						{
							bgra8888pixel pixel;
							if(targaImage.header.imageSpecification.imageDescriptorByte.screenOrigin == 0) // lower left origin
								pixel = ((bgra8888pixel*)targaImage.imageData)[x+y*targaImage.header.imageSpecification.width];
							else	// upper left origin
								pixel = ((bgra8888pixel*)targaImage.imageData)[x+(targaImage.header.imageSpecification.height-1-y)*targaImage.header.imageSpecification.width];
							
							image.buffer[x+y*targaImage.header.imageSpecification.width].components.r = pixel.components.r;
							image.buffer[x+y*targaImage.header.imageSpecification.width].components.g = pixel.components.g;
							image.buffer[x+y*targaImage.header.imageSpecification.width].components.b = pixel.components.b;
							
							if(targaImage.header.imageSpecification.imageDescriptorByte.pixelAttributeBits == 8)
								image.buffer[x+y*targaImage.header.imageSpecification.width].components.a = pixel.components.a;
							else
								image.buffer[x+y*targaImage.header.imageSpecification.width].components.a = 255;
						}
					}
				}
				break;
			}
		}
		break;
	default:
		{
			ccout << "The file read was a targa file, but the targa image type\n"
				<< "is not supported.\n";
		}
		break;
	}


	// clean up
	if(targaImage.imageIdentificationField)
		delete [] targaImage.imageIdentificationField;
	if(targaImage.colorMapData)
		delete [] targaImage.colorMapData;
	if(targaImage.imageData)
		delete [] targaImage.imageData;

	return false;
}

bool CTargaImageLoader::SaveToFile(const CImage& image, const string filename, bool overwrite)
{
	return false;
}
