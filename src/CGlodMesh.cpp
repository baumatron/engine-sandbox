#include "CGlodMesh.h"

CGlodMesh::CGlodMesh(CSmartPointer<CVertexBuffer> source, GLuint objectName, GLuint patchName)
{
	return;
	CreateGlodPatch(source, objectName, patchName);
}
CGlodMesh::~CGlodMesh()
{

}

void CGlodMesh::EnableVertexBuffers() // equivalent to CVertexBuffer::EnableVertexBuffers()
{
	return;
	glodFillArrays(m_glodObject, m_glodPatch);
	GLenum error = glodGetError();
	switch(error)
	{
	case GLOD_INVALID_NAME:
	case GLOD_INVALID_STATE:
	case GLOD_INVALID_PATCH:
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

void CGlodMesh::CreateGlodPatch(CSmartPointer<CVertexBuffer> source, GLuint objectName, GLuint patchName)
{
	return;
	m_glodObject = objectName;
	m_glodPatch = patchName;

	GLenum error;
	if(!source->EnableVertexBuffers())
		error = 1;

//	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glodInsertArrays(m_glodObject, m_glodPatch, GL_TRIANGLES, 0, source->GetVertexCount(), 0, 0); // source->GetVertexCount() == triangles*3
	error = glodGetError();
	switch(error)
	{
	case GLOD_INVALID_NAME:
	case GLOD_INVALID_STATE:
	case GLOD_INVALID_DATA_FORMAT:
		{
			error = 1;
		}
	default:
		{
			error = 0;
		}
		break;
	}
	//GLOD_INVALID_NAME is generated if an object of the specified \c name does not exist
	//GLOD_INVALID_STATE is generated if this object has already been built into a hierarchy
	//GLOD_INVALID_DATA_FORMAT 
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}