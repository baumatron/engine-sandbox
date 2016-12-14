/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#ifndef CCOREMESH_H
#define CCOREMESH_H

#include "CVertexBuffer.h"
#include "CGlodMesh.h"
#include "CSmartPointer.h"

class CMilkShapeModelLoader;

// a core mesh has only one instance for each model file
// as much common data as possible is stored in a core mesh
// model instances use the CMesh class
class CCoreMesh
{
public:
	friend class CMilkShapeModelLoader;

	CCoreMesh();
	CCoreMesh(const CCoreMesh& rhs);
	~CCoreMesh();
	CCoreMesh operator=(const CCoreMesh& rhs);

	void CopyFrom(const CCoreMesh& rhs);

	void AllocateVertexBuffer(unsigned int vertexCount, unsigned char vertexBufferFlags);
	void InitializeGlod(GLuint objectName, GLuint patchName); 

	void EnableVertexBuffers();
	void DisableVertexBuffers();
	void DrawTriangles();

	int GetMaterialIndex();
protected:
	unsigned int m_materialIndex;
	unsigned int m_triangleCount;

	CSmartPointer<CVertexBuffer> m_spVertexBuffer;
	CSmartPointer<CGlodMesh> m_spGlodMesh;
};

#endif