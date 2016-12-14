
#ifndef CGLODGROUP_H
#define CGLODGROUP_H

#ifdef WIN32
#include <windows.h>								// Header File For Windows
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include <glod.h>

class CGlodGroup
{
public:
	CGlodGroup();
	~CGlodGroup();

	void SetTriangleBudget(int triangles);
	float GetTriangleBudget();

	GLuint GetGroupName(){return m_glodGroup;}

	void AdaptGroup();

private:
	GLuint m_glodGroup;

	static GLuint glodGroupCounter;
};

#endif

