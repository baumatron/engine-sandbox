/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#ifndef CCOREMODEL_H
#define CCOREMODEL_H

#include "IResource.h"
#include "CSmartPointer.h"
#include "CMaterial.h"
#include "CGlodObject.h"
#include "CCoreMesh.h"

class CModel;
class CMilkShapeModelLoader;

struct Vertex
{
	float m_position[3];
//	char m_boneID;	// For Skeletal Animation
};


struct Triangle
{
	float m_vertexNormals[3][3];	// normals for each vertex
	float m_s[3], m_t[3];			// texture mapping coordinates for each vertex
	int m_vertexIndices[3];			// vertex indices
};

class CCoreModel: public IResource
{
public:
	friend class CModel;
	friend class CMilkShapeModelLoader;

	CCoreModel();
	~CCoreModel();
	CCoreModel(const CCoreModel& rhs);

	void InitializeGlod(GLuint glodGroup);

	void CopyFrom(const CCoreModel& rhs);
	CCoreModel operator=(const CCoreModel& rhs);

	CMaterial GetMaterial(int number);
	void SetMaterial(int number, CMaterial material);

protected:
	CSmartPointer<Vertex> m_pVertices; // pointer to all of the verticies in the mesh (no duplicates)
	unsigned short m_vertexCount; // number of verticies
	CSmartPointer<Triangle> m_pTriangles; // pointer to all triangles in the model, each triangle indexes 3 verticies
	unsigned short m_triangleCount; // number of triangles in model
	CSmartPointer< CSmartPointer<CCoreMesh> > m_spMeshesSP; // a smart pointer to an array of smart pointers   pointer to all meshes (groups) in this model
															// having a smart pointer for each individual item and allocating it individually allows a CMesh to 
															// reference these meshes individually with it's own smart pointer
	unsigned short m_meshCount; // number of meshes
	CSmartPointer<CMaterial> m_pMaterials;
	unsigned short m_materialCount;

	CSmartPointer<CGlodObject> m_spGlodObject;
};

#endif