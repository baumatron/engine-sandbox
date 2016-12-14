#ifndef CCOLOR_H
#define CCOLOR_H

#define _RGB32BIT(r,g,b) ( ((r%256) << 0) + ((g%256) << 8) + ((b%256) << 16) + (255 << 24) )
#define _RGBA32BIT(r,g,b,a) ( ((r%256) << 0) + ((g%256) << 8) +  ((b%256) << 16)+ ((a%256) << 24) )

#define _EXTRACTALPHA(color) ( ((color & 0xFF000000) >> 24) )
#define _EXTRACTRED(color) ( ((color & 0x000000FF) >> 0) )
#define _EXTRACTGREEN(color) ( ((color & 0x0000FF00) >> 8) )
#define _EXTRACTBLUE(color) ( ((color & 0x00FF0000) >> 16) )

#define VID_COLORKEY _RGB32BIT( 159, 91, 83 ) 
#define VID_COLORKEY2 _RGB32BIT( 139, 70, 66 ) 

#pragma pack(1)
union rgb555pixel
{
	unsigned short pixel;
	struct
	{
		unsigned short a : 1;
		unsigned short r : 5;
		unsigned short g : 5;
		unsigned short b : 5;
	} components;
};
union bgr555pixel
{
	unsigned short pixel;
	struct
	{
		unsigned short b : 5;
		unsigned short g : 5;
		unsigned short r : 5;
		unsigned short a : 1;
	} components;
};
union rgb565pixel
{
	unsigned short pixel;
	struct
	{
		unsigned short r : 5;
		unsigned short g : 6;
		unsigned short b : 5;
	} components;
};
union bgr565pixel
{
	unsigned short pixel;
	struct
	{
		unsigned short b : 5;
		unsigned short g : 6;
		unsigned short r : 5;
	} components;
};
struct rgb888pixel
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};
struct bgr888pixel
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
};

union rgba8888pixel
{
	unsigned long pixel;
	struct 
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	} components;
};
union bgra8888pixel
{
	unsigned long pixel;
	struct 
	{
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;
	} components;
};
#pragma pack()

class CColor
{
public:
	CColor():r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
	CColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {setColors(r, g, b, a);}
	CColor(float r, float g, float b, float a = 1.0f) {setColors(r, g, b, a);}
	~CColor() {}

	const float& getRFloat() const {return r;} // returns a float from 0.0 to 1.0
	const float& getGFloat() const {return g;}
	const float& getBFloat() const {return b;}
	const float& getAFloat() const {return a;}
	unsigned char getRChar() const {return (unsigned char)(255.0f * r);}
	unsigned char getGChar() const {return (unsigned char)(255.0f * g);} // returns an unsigned char from 0 to 255
	unsigned char getBChar() const {return (unsigned char)(255.0f * b);}
	unsigned char getAChar() const {return (unsigned char)(255.0f * b);}

	void setColors(const unsigned char& _r, const unsigned char _g, const unsigned char _b, const unsigned char _a = 255) {setR(_r); setG(_g); setB(_b); setA(_a); }
	void setColors(const float& _r, const float& _g, const float& _b, const float& _a = 1.0f) {setR(_r); setG(_g); setB(_b); setA(_a); }
	void setR(const unsigned char& _r) {r = ((float)_r)/255.0f;}
	void setG(const unsigned char& _g) {g = ((float)_g)/255.0f;}
	void setB(const unsigned char& _b) {b = ((float)_b)/255.0f;}
	void setA(const unsigned char& _a) {a = ((float)_a)/255.0f;}
	void setR(const float& _r) {r = _r;} // don't try to set this to greater than 1.0...
	void setG(const float& _g) {g = _g;}
	void setB(const float& _b) {b = _b;}
	void setA(const float& _a) {a = _a;}
private:
	float r, g, b, a;
};

#endif