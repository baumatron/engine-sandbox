
#ifndef CGLODOBJECT_H
#define CGLODOBJECT_H

#ifdef WIN32
#include <windows.h>								// Header File For Windows
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include <glod.h>

class CGlodObject
{
public:
	CGlodObject(GLuint glodGroup);
	~CGlodObject();

	void BuildObject();

	GLuint GetObjectName(){return m_glodObject;}

private:
	GLuint m_glodObject;
	GLuint m_glodGroup;

	static GLuint glodObjectCounter;
};

#endif

/*

if(m_glodObject != 0)
		glodDeleteObject(m_glodObject);
	// get an unused glod object name (an id)
	m_glodObject = glodObjectCounter++;
	// create the new object
	glodNewObject(m_glodObject, GLOD_GLOBAL_GROUP, GLOD_DISCRETE);
	// now, add all the patches (meshes)
#endif
	for(int i = 0; i < this->m_meshCount; i++)
	{
		m_pMeshes[i].Initialize(m_glodObject, i);
	}
#ifdef USE_GLOD

	glodBuildObject(m_glodObject);
*/