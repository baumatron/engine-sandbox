/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */


/*
#ifdef WIN32
#include <windows.h>								// Header File For Windows
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <glod.h>
*/

#include "CCoreMesh.h"
#include "CBacon.h"

CCoreMesh::CCoreMesh()
{
	m_triangleCount = 0;
}
CCoreMesh::CCoreMesh(const CCoreMesh& rhs)
{
	CopyFrom(rhs);
}
CCoreMesh::~CCoreMesh()
{
//	bacon::cout << "Deleting CCoreMesh.\n";
}
CCoreMesh CCoreMesh::operator=(const CCoreMesh& rhs)
{
	if(&rhs == this)
		return *this;
	else
		CopyFrom(rhs);
	return *this;
}
void CCoreMesh::CopyFrom(const CCoreMesh &rhs)
{
	this->m_materialIndex = rhs.m_materialIndex;
	this->m_triangleCount = rhs.m_triangleCount;
	
	m_spVertexBuffer = CSmartPointer<CVertexBuffer>(new CVertexBuffer, false);
	(*this->m_spVertexBuffer) = (*rhs.m_spVertexBuffer);
}
void CCoreMesh::AllocateVertexBuffer(unsigned int vertexCount, unsigned char vertexBufferFlags)
{
	this->m_spVertexBuffer = CSmartPointer<CVertexBuffer>(new CVertexBuffer(vertexCount, vertexBufferFlags));
}
void CCoreMesh::InitializeGlod(GLuint objectName, GLuint patchName)
// sets up the glod arrays for this patch/mesh
// call only after vertex arrays are valid
{
	m_spGlodMesh = CSmartPointer<CGlodMesh>(new CGlodMesh(m_spVertexBuffer, objectName, patchName), false);
}
void CCoreMesh::EnableVertexBuffers()
{
	bool renderGlod = false;
	if(renderGlod)
		m_spGlodMesh->EnableVertexBuffers();
	else
		m_spVertexBuffer->EnableVertexBuffers();
}
void CCoreMesh::DisableVertexBuffers()
{
	bool renderGlod = false;
	if(renderGlod)
		bacon::cout << "Watch out... glod vertex arrays weren't disabled\n"; // TODO: put this in
	else
		m_spVertexBuffer->DisableVertexBuffers();
}	
void CCoreMesh::DrawTriangles()
{
	m_spVertexBuffer->DrawTriangles();
}


int CCoreMesh::GetMaterialIndex()
{
	return m_materialIndex;
}