#ifndef CVERTEXBUFFER_H
#define CVERTEXBUFFER_H

#include "CSmartPointer.h"


#define VB_NORMALS 0x01
#define VB_TEXTURECOORDS 0x02
#define VB_COLORS 0x04

class CVertexBuffer
{
public:
	CVertexBuffer();
	CVertexBuffer(int vertexCount, unsigned char flags);
	CVertexBuffer(const CVertexBuffer& rhs);
	CVertexBuffer operator=(const CVertexBuffer& rhs);
	~CVertexBuffer();

	void AddVertex(float x, float y, float z);
	void AddNormal(float x, float y, float z);
	void AddTextureCoordinate(float u, float v);
	void AddColor(float r, float g, float b, float a);
	
	bool EnableVertexBuffers();

	CSmartPointer<float> GetVertices(){ return m_spVertices; }
	unsigned int GetVertexCount(){ return m_vertexCount; }
private:
//	void FreeMemory();
	void CopyFrom(const CVertexBuffer& rhs);

	unsigned int m_vertexCount;
	unsigned int m_vertexIndex;
	unsigned int m_normalIndex;
	unsigned int m_tcoordIndex;
	unsigned int m_colorIndex;
	CSmartPointer<float> m_spVertices;
	CSmartPointer<float> m_spNormals;
	CSmartPointer<float> m_spTextureCoordinates;
	CSmartPointer<float> m_spColors;
};

#endif