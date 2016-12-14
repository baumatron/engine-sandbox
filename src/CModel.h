
#ifndef CModel_H
#define CModel_H


#ifdef WIN32
#include <windows.h>								// Header File For Windows
#endif

#include <GL/gl.h>
#include <GL/glu.h>
//#define USE_GLOD
#include <glod.h>

#include "math_main.h"
#include "CCamera.h"
#include "con_main.h"
#include "CSmartPointer.h"
#include "CVertexBuffer.h"
#include "CGlodMesh.h"
#include "CGlodObject.h"
#include "CGlodGroup.h"

#include <map>
#include <string>
using namespace std;

#define GLOD_GLOBAL_GROUP 0

struct Vertex
{
	float m_position[3];
//	char m_boneID;	// For Skeletal Animation
};

struct Normal
{
	float m_normal[3];
};

struct TextureCoordinate
{
	float m_s;
	float m_t;
};

struct Triangle
{
	float m_vertexNormals[3][3];	// normals for each vertex
	float m_s[3], m_t[3];			// texture mapping coordinates for each vertex
	int m_vertexIndices[3];			// vertex indices
};

struct VertexArrayElement // GL_T2F_N3F_V3F
{
	TextureCoordinate textureCoordinate;
	Normal normal;
	Vertex vertex;
};

class CMaterial
{
public:
	CMaterial();
	~CMaterial(){}
	void SetTexture(string file);

	float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
	float m_shininess;
	unsigned int m_texture; // GLuint
	string m_textureFilename;
};


// a core mesh has only one instance for each model file
// as much common data as possible is stored in a core mesh
// model instances use the CMesh class
class CCoreMesh
{
public:
	CCoreMesh();
	CCoreMesh(const CCoreMesh& rhs);
	~CCoreMesh();
	CCoreMesh operator=(const CCoreMesh& rhs);

	void CopyFrom(const CCoreMesh& rhs);

	void PrepareVertexBuffer(unsigned int vertexCount, unsigned short vertexBufferFlags);
	void InitializeGlod(GLuint objectName, GLuint patchName); 

	unsigned int m_materialIndex;
	unsigned int m_triangleCount;

	CSmartPointer<CVertexBuffer> m_spVertexBuffer;
	CSmartPointer<CGlodMesh> m_spGlodMesh;
};

class CMesh
{
public:
	CMesh();
	CMesh(CCoreMesh& coreMesh);
	~CMesh();

	void SetLOD(float lod);
	float GetLOD(){ return m_lod; }

	void Initialize(CCoreMesh& coreMesh);

	void CopyFrom(const CMesh& rhs);


	unsigned int m_materialIndex;
	unsigned int m_triangleCount;

//	CSmartPointer<Vertex> m_pCoreVertexArray;
//	CSmartPointer<Normal> m_pCoreNormalArray;
//	CSmartPointer<TextureCoordinate> m_pCoreTextureCoordArray;

	CSmartPointer<CCoreMesh> m_spCoreMesh;
	//GLuint m_glodPatch; // glod patch id for lod mesh
private:
	float m_lod;
};


class CCoreModel
{
public:
	CCoreModel();
	~CCoreModel();
	CCoreModel(const CCoreModel& rhs);

	void InitializeGlod(GLuint glodGroup);

	void CopyFrom(const CCoreModel& rhs);
	CCoreModel operator=(const CCoreModel& rhs);

	CSmartPointer<Vertex> m_pVertices; // pointer to all of the verticies in the mesh (no duplicates)
	unsigned short m_vertexCount; // number of verticies
	CSmartPointer<Triangle> m_pTriangles; // pointer to all triangles in the model, each triangle indexes 3 verticies
	unsigned short m_triangleCount; // number of triangles in model
	CSmartPointer<CCoreMesh> m_pMeshes; // pointer to all meshes (groups) in this model
	unsigned short m_meshCount; // number of meshes
	CSmartPointer<CMaterial> m_pMaterials;
	unsigned short m_materialCount;

	//GLuint m_glodObject; // glod object id(or name)
	CSmartPointer<CGlodObject> m_spGlodObject;
};

class CModel
{
public:
	CModel(CCoreModel& coreModel);
	CModel(const CModel& rhs);
	~CModel();

	void Render(CCamera& camera, matrix4x4 worldMatrix);

	void SetLOD(float lod);
	float GetLOD(){ return m_lod; }

	float FindMaximumRadius();

	void CopyFrom(const CModel& rhs);
	CModel operator=(const CModel& rhs);

	CSmartPointer<CCoreModel> GetCoreModel(){return m_spCoreModel;}


	static float trianglesRendered;
	static float verticesRendered;
private:
	CSmartPointer<Vertex> m_pCoreVertices; // pointer to all of the verticies in the mesh (no duplicates)
	unsigned short m_vertexCount; // number of verticies
	CSmartPointer<Triangle> m_pCoreTriangles; // pointer to all triangles in the model, each triangle indexes 3 verticies
	unsigned short m_triangleCount; // number of triangles in model
	CSmartPointer<CMesh> m_pMeshes; // pointer to all meshes (groups) in this model
	unsigned short m_meshCount; // number of meshes
	CSmartPointer<CMaterial> m_pCoreMaterials;
	unsigned short m_materialCount;

	float m_lod; // level of detail
	int m_renderMode; // 0 = normal, 1 = wireframe

	CSmartPointer<CCoreModel> m_spCoreModel;
};



class CModelRepository
{
public:
	CModelRepository();
	~CModelRepository();

	//CModel* BorrowModelInstance(string filename);
	//void ReturnModelInstance(CModel* modelInstance);
	CSmartPointer<CModel> GetModelInstance(string filename);
	void Purge(){ m_resourcePool.Purge(); }

	void PrintMap(){ m_resourcePool.PrintMap(); }

	static CModelRepository* Instance();

	void UpdateGlod();

	void SetTriangleBudget(float triangles);
	float GetTriangleBudget(){return 0;/*m_spGlodGroup->GetTriangleBudget();*/}
private:
	CSharedResourcePool<CCoreModel> m_resourcePool;

//	float m_triangleBudget;
	CSmartPointer<CGlodGroup> m_spGlodGroup;

	static CModelRepository* instance;
};


#endif