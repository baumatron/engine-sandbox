#include "CModel.h"

#include <string.h>

CModel::CModel():
m_pVertices(0),
m_pTriangles(0),
m_pMeshes(0),
m_pMaterials(0),
m_vertexCount(0),
m_triangleCount(0),
m_meshCount(0),
m_materialCount(0)
{
}

CModel::~CModel()
{
	FreeMemory();
}

CModel::CModel(const CModel& rhs):
m_pVertices(0),
m_pTriangles(0),
m_pMeshes(0),
m_pMaterials(0),
m_vertexCount(0),
m_triangleCount(0),
m_meshCount(0),
m_materialCount(0)
{
	m_vertexCount = rhs.m_vertexCount;
	m_pVertices = new Vertex[rhs.m_vertexCount];
	for(int i = 0; i < rhs.m_vertexCount; i++)
	{
		memcpy(&m_pVertices[i], &rhs.m_pVertices[i], sizeof(Vertex));
	}

	m_triangleCount = rhs.m_triangleCount;
	m_pTriangles = new Triangle[rhs.m_triangleCount];
	for(int i = 0; i < rhs.m_triangleCount; i++)
	{
		memcpy(&m_pTriangles[i], &rhs.m_pTriangles[i], sizeof(Triangle));
	}

	m_meshCount = rhs.m_meshCount;
	m_pMeshes = new Mesh[rhs.m_meshCount];
	for(int i = 0; i < rhs.m_meshCount; i++)
	{
		memcpy(&m_pMeshes[i], &rhs.m_pMeshes[i], sizeof(Mesh)); // warning: includes a pointer, must follow up with memory allocation and array copy
		//m_pMeshes[i].m_materialIndex = rhs.m_pMeshes[i].m_materialIndex;
		//m_pMeshes[i].m_triangleCount = rhs.m_pMeshes[i].m_triangleCount;
		m_pMeshes[i].m_pTriangleIndices = new int[rhs.m_pMeshes[i].m_triangleCount];
		for(int j = 0; j < rhs.m_pMeshes[i].m_triangleCount; j++)
		{
			m_pMeshes[i].m_pTriangleIndices[j] = rhs.m_pMeshes[i].m_pTriangleIndices[j];
		}
	}

	m_materialCount = rhs.m_materialCount;
	m_pMaterials = new Material[rhs.m_materialCount];
	for(int i = 0; i < rhs.m_materialCount; i++)
	{
		memcpy(&m_pMaterials[i], &rhs.m_pMaterials[i], sizeof(Material)); // warning: includes a pointer, must follow up with memory allocation and array copy
		if(rhs.m_pMaterials[i].m_pTextureFilename)
		{
			m_pMaterials[i].m_pTextureFilename = new char[strlen(rhs.m_pMaterials[i].m_pTextureFilename)];
			strncpy( m_pMaterials[i].m_pTextureFilename, rhs.m_pMaterials[i].m_pTextureFilename, strlen(rhs.m_pMaterials[i].m_pTextureFilename) );
		}	
	}
}

void CModel::FreeMemory()
{
	if(m_pVertices)
		delete [] m_pVertices;
	m_pVertices = 0;
	m_vertexCount = 0;

	if(m_pTriangles)
		delete [] m_pTriangles;
	m_pTriangles = 0;
	m_triangleCount = 0;

	if(m_pMeshes)
	{
	/*	for(int i = 0; i < m_meshCount; i++)
		{
			if(m_pMeshes[i].m_pTriangleIndices)
				delete [] m_pMeshes[i].m_pTriangleIndices;
		}*/
		delete [] m_pMeshes;
	}
	m_pMeshes = 0;
	m_meshCount = 0;

	if(m_pMaterials)
	{
		/*for(int i = 0; i < m_materialCount; i++)
		{
			if(m_pMaterials[i].m_pTextureFilename)
				delete [] m_pMaterials[i].m_pTextureFilename;
		}*/
		delete [] m_pMaterials;
	}
	m_pMaterials = 0;
	m_materialCount = 0;
}

CModel CModel::operator=(const CModel& rhs)
{
	if(&rhs == this)
		return *this;

	FreeMemory();

	m_vertexCount = rhs.m_vertexCount;
	m_pVertices = new Vertex[rhs.m_vertexCount];
	for(int i = 0; i < rhs.m_vertexCount; i++)
	{
		memcpy(&m_pVertices[i], &rhs.m_pVertices[i], sizeof(Vertex));
	}

	m_triangleCount = rhs.m_triangleCount;
	m_pTriangles = new Triangle[rhs.m_triangleCount];
	for(int i = 0; i < rhs.m_triangleCount; i++)
	{
		memcpy(&m_pTriangles[i], &rhs.m_pTriangles[i], sizeof(Triangle));
	}

	m_meshCount = rhs.m_meshCount;
	m_pMeshes = new Mesh[rhs.m_meshCount];
	for(int i = 0; i < rhs.m_meshCount; i++)
	{
		memcpy(&m_pMeshes[i], &rhs.m_pMeshes[i], sizeof(Mesh)); // warning: includes a pointer, must follow up with memory allocation and array copy
		//m_pMeshes[i].m_materialIndex = rhs.m_pMeshes[i].m_materialIndex;
		//m_pMeshes[i].m_triangleCount = rhs.m_pMeshes[i].m_triangleCount;
		m_pMeshes[i].m_pTriangleIndices = new int[rhs.m_pMeshes[i].m_triangleCount];
		for(int j = 0; j < rhs.m_pMeshes[i].m_triangleCount; j++)
		{
			m_pMeshes[i].m_pTriangleIndices[j] = rhs.m_pMeshes[i].m_pTriangleIndices[j];
		}
	}

	m_materialCount = rhs.m_materialCount;
	m_pMaterials = new Material[rhs.m_materialCount];
	for(int i = 0; i < rhs.m_materialCount; i++)
	{
		memcpy(&m_pMaterials[i], &rhs.m_pMaterials[i], sizeof(Material)); // warning: includes a pointer, must follow up with memory allocation and array copy
		if(rhs.m_pMaterials[i].m_pTextureFilename)
		{
			m_pMaterials[i].m_pTextureFilename = new char[strlen(rhs.m_pMaterials[i].m_pTextureFilename)];
			strncpy( m_pMaterials[i].m_pTextureFilename, rhs.m_pMaterials[i].m_pTextureFilename, strlen(rhs.m_pMaterials[i].m_pTextureFilename) );
		}
	}
	return *this;
}
