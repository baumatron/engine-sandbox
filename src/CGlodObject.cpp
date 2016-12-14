
#include "CGlodObject.h"

GLuint CGlodObject::glodObjectCounter = 0;


CGlodObject::CGlodObject(GLuint glodGroup)
{
	return;
	// get an unused glod object name (an id)
	m_glodObject = glodObjectCounter++;
	m_glodGroup = glodGroup;
	// create the new object
	glodNewObject(m_glodObject, m_glodGroup, GLOD_DISCRETE);
	GLenum error = glodGetError();
	switch(error)
	{
	case GLOD_INVALID_NAME:
		{
			error = 0;
		}
	default:
		{
			error = 1;
		}
		break;
	}
}
CGlodObject::~CGlodObject()
{
	return;
	glodDeleteObject(m_glodObject);
}

void CGlodObject::BuildObject()
{
	return;
	glodBuildObject(m_glodObject);
	GLenum error = glodGetError();
	switch(error)
	{
	case GLOD_INVALID_NAME:
	case GLOD_INVALID_STATE:
		{
			error = 0;
		}
	default:
		{
			error = 1;
		}
		break;
	}
}
