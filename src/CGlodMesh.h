#ifndef CGLODMESH_H
#define CGLODMESH_H

#include "CSmartPointer.h"
#include "CVertexBuffer.h"

#ifdef WIN32
#include <windows.h>								// Header File For Windows
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include <glod.h>


class CGlodMesh
{
public:
	CGlodMesh(CSmartPointer<CVertexBuffer> source, GLuint objectName, GLuint patchName);
	CGlodMesh(const CGlodMesh& rhs)
	{
		int i = 0;
		i++;
	}
	~CGlodMesh();


	void EnableVertexBuffers(); // equivalent to CVertexBuffer::EnableVertexBuffers()
private:
	void CreateGlodPatch(CSmartPointer<CVertexBuffer> source, GLuint objectName, GLuint patchName);

	GLuint m_glodObject;
	GLuint m_glodPatch; // glod patch id(or name)
};

#endif