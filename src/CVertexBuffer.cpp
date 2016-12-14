
#include "CVertexBuffer.h"

#ifdef WIN32
#include <windows.h>								// Header File For Windows
#endif

#include <GL/gl.h>
#include <GL/glu.h>
//#ifdef USE_GLOD
//#include <glod.h>
//#endif

CVertexBuffer::CVertexBuffer()
{
	m_vertexIndex = 0;
	m_normalIndex = 0;
	m_tcoordIndex = 0;
	m_colorIndex = 0;	
	m_vertexCount = 0;
}

CVertexBuffer::CVertexBuffer(int vertexCount, unsigned char flags)
{
	m_vertexIndex = 0;
	m_normalIndex = 0;
	m_tcoordIndex = 0;
	m_colorIndex = 0;
	m_vertexCount = vertexCount;

	m_spVertices.New(3*vertexCount);

	if(flags & VB_NORMALS)
	{
		m_spNormals.New(3*vertexCount);// = CSmartPointer<float>(new float[3*vertexCount], true);
	}
	if(flags & VB_TEXTURECOORDS)
	{
		m_spTextureCoordinates.New(2*vertexCount);// = CSmartPointer<float>(new float[2*vertexCount], true);
	}
	if(flags & VB_COLORS)
	{
		m_spColors.New(4*vertexCount);// = CSmartPointer<float>(new float[4*vertexCount], true);
	}
}
CVertexBuffer::CVertexBuffer(const CVertexBuffer& rhs)
{
	CopyFrom(rhs);
}
CVertexBuffer CVertexBuffer::operator=(const CVertexBuffer& rhs)
{
	if(&rhs == this)
		return *this;

//	FreeMemory();
	CopyFrom(rhs);
	return *this;
}
CVertexBuffer::~CVertexBuffer()
{
}

void CVertexBuffer::AddVertex(float x, float y, float z)
{
	m_spVertices[m_vertexIndex+0] = x;
	m_spVertices[m_vertexIndex+1] = y;
	m_spVertices[m_vertexIndex+2] = z;
	m_vertexIndex+=3;
}
void CVertexBuffer::AddNormal(float x, float y, float z)
{
	m_spNormals[m_normalIndex+0] = x;
	m_spNormals[m_normalIndex+1] = y;
	m_spNormals[m_normalIndex+2] = z;
	m_normalIndex+=3;
}
void CVertexBuffer::AddTextureCoordinate(float u, float v)
{
	m_spTextureCoordinates[m_tcoordIndex+0] = u;
	m_spTextureCoordinates[m_tcoordIndex+1] = v;
	m_tcoordIndex+=2;
}
void CVertexBuffer::AddColor(float r, float g, float b, float a)
{
	m_spColors[m_colorIndex+0] = r;
	m_spColors[m_colorIndex+1] = g;
	m_spColors[m_colorIndex+2] = b;
	m_spColors[m_colorIndex+3] = a;
	m_colorIndex+=4;
}
	
bool CVertexBuffer::EnableVertexBuffers()
{
	if(!m_spVertices.Null())
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer( 3, GL_FLOAT, 0, m_spVertices.GetPointer() );
	}
	else
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	if(!m_spNormals.Null())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, m_spNormals.GetPointer() );
	}
	else
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	if(!m_spTextureCoordinates.Null())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, m_spTextureCoordinates.GetPointer() );
	}
	else
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if(!m_spColors.Null())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, m_spColors.GetPointer() );
	}
	else
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}

	return ( GL_NO_ERROR == glGetError() );	
}


/*
void CVertexBuffer::FreeMemory()
{
	m_vertexCount = 0;
	m_spVertices.Release();
	m_spNormals.Release();
	m_spTextureCoordinates.Release();
	m_spColors.Release();
}*/

void CVertexBuffer::CopyFrom(const CVertexBuffer& rhs)
{
	m_vertexIndex = rhs.m_vertexIndex;
	m_normalIndex = rhs.m_normalIndex;
	m_tcoordIndex = rhs.m_tcoordIndex;
	m_colorIndex = rhs.m_colorIndex;	
	m_vertexCount = rhs.m_vertexCount;
	
	m_spVertices.New(3*rhs.m_vertexCount);// = CSmartPointer<float>(new float[3*vertexCount], true);
	m_spVertices.MemCopy(rhs.m_spVertices);

	if(!rhs.m_spNormals.Null())
	{
		m_spNormals.New(3*rhs.m_vertexCount);// = CSmartPointer<float>(new float[3*vertexCount], true);
		m_spNormals.MemCopy(rhs.m_spNormals);
	}
	else
	{
		m_spNormals.Release();
	}

	if(!rhs.m_spTextureCoordinates.Null())
	{
		m_spTextureCoordinates.New(2*rhs.m_vertexCount);// = CSmartPointer<float>(new float[2*vertexCount], true);
		m_spTextureCoordinates.MemCopy(rhs.m_spTextureCoordinates);
	}
	else
	{
		m_spTextureCoordinates.Release();
	}

	if(!rhs.m_spColors.Null())
	{
		m_spColors.New(4*rhs.m_vertexCount);// = CSmartPointer<float>(new float[4*vertexCount], true);
		m_spColors.MemCopy(rhs.m_spColors);
	}
	else
	{
		m_spColors.Release();
	}
}
