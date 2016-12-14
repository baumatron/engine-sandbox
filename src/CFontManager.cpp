#include "CFontManager.h"
#include "COpenglImage.h"
#include <ftfont.h>

FTGLFontManager* CSingleton<FTGLFontManager>::singleton = 0;

FTGLFontManager::~FTGLFontManager()
{

}
void FTGLFontManager::Initialize()
{
	COpenGLImage::ReserveOpenGLTextureID(0);
	COpenGLImage::ReserveOpenGLTextureID(1);
}
void FTGLFontManager::Shutdown()
{
    FontIter font;
    for( font = fonts.begin(); font != fonts.end(); font++)
    {
        delete (*font).second;
    }

    fonts.clear();
}
	


FTFont* FTGLFontManager::GetFont( const char *filename, int size)
{
    char buf[256];
    sprintf(buf, "%s%i", filename, size);
    string fontKey = string(buf);
    
    FontIter result = fonts.find( fontKey);
    if( result != fonts.end())
    {
//                LOGMSG( "Found font %s in list", filename);
        return result->second;
    }


//           FTFont* font = new FTGLBitmapFont(filename);
	string fontsDir = "data/fonts/";
    FTFont* font = new FTGLTextureFont( (fontsDir + string(filename)).c_str() );
	    
    if( font->Error())
    {
        //LOGERROR( "Font %s failed to open", fullname.c_str());
        delete font;
        return NULL;
    }
    
    if( !font->FaceSize( size))
    {
        //LOGERROR( "Font %s failed to set size %i", filename, size);
        delete font;
        return NULL;
    }

    fonts[fontKey] = font;
    
    return font;

}

string FTGLFontManager::ClipText(string text, FTFont* font, float length)
{
	while(font->Advance(text.c_str()) > length)
		text = text.substr(0, text.size()-1);
	return text;
}

FTGLFontManager::FTGLFontManager()
{
}
FTGLFontManager::FTGLFontManager( const FTGLFontManager&)
{
}
FTGLFontManager& FTGLFontManager::operator = ( const FTGLFontManager&)
{ 
	return *this;
}