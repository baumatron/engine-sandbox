#ifndef CFONTMANAGER_H
#define CFONTMANAGER_H

#ifdef WIN32

#pragma comment (lib, "libfreetype.lib")
#ifdef _DEBUG
#pragma comment (lib, "ftgl_static_MTD_d.lib") // should be for debug build only
#else
#pragma comment (lib, "ftgl_static_MTD.lib") // should be for debug build only
#endif

#else
// LINUX

#endif

#include "CSingleton.h"

#include    <map>
#include    <string>
//#include    <FTGLBitmapFont.h>
//#include <FTFont.h>
#include <glew.h>
#include    <FTGLTextureFont.h>


using namespace std;

typedef map< string, FTFont*> FontList;
typedef FontList::const_iterator FontIter;

class FTGLFontManager: public CSingleton<FTGLFontManager>
{
    public:
        // NOTE
        // This is shown here for brevity. The implementation should be in the source
        // file otherwise your compiler may inline the function resulting in 
        // multiple instances of FTGLFontManager
		friend class CSingleton<FTGLFontManager>;
        
		~FTGLFontManager();

		void Initialize();
		void Shutdown();
        
        FTFont* GetFont( const char *filename, int size);
    
        static string ClipText(string text, FTFont* font, float length);
    private:
        // Hide these 'cause this is a singleton.
        FTGLFontManager();
        FTGLFontManager( const FTGLFontManager&);
        FTGLFontManager& operator = ( const FTGLFontManager&);
        
        // container for fonts
        FontList fonts;
    
};


#endif