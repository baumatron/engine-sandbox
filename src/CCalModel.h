
#ifndef CCalModel_H
#define CCalModel_H


#ifdef WIN32
#include <windows.h>								// Header File For Windows
#endif

#include <cal3d.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "math_main.h"
#include "CCamera.h"
#include "con_main.h"

#include <map>
#include <string>
using namespace std;

// this class provides the core model functionality of cal3d
class CCalCoreModel
{
public:
	CCalCoreModel();
	CCalCoreModel(const CCalCoreModel& rhs) {ccout << "CCalCoreModel(const CCalCoreModel& rhs) called." << newl;}
	~CCalCoreModel();

	bool LoadModel(string filename);
	void FreeMemory();
	string GetFilename(){return m_filename;}
	CalCoreModel* GetCalCoreModel(){return m_calCoreModel;}


	operator=(const CCalCoreModel& rhs) {ccout << "operator=(const CCalCoreModel& rhs) called." << newl;}

private:
	CalCoreModel* m_calCoreModel;
	string m_filename;
};

class CCalModel
{
public:
	CCalModel(CCalCoreModel& coreModel);
	CCalModel(const CCalModel& rhs){ccout << "CCalModel(const CCalModel& rhs) called." << newl;}
	~CCalModel();

	CCalModel& operator=(const CCalModel& rhs){ccout << "operator=(const CCalModel& rhs) called." << newl; return *this;}

	// file stuff
	// load model
	// set texture directory

	// vertex array updating
	// Think() // updates animation, updates verticies... perhaps shouldn't be called each frame, because 
	//				so many verticies will be copied...
	void Think(float timeDelta);

	// LOD
	// set lod (continuous, real value)
	// set number of increments
	//
	void SetLOD(float lodValue);
	void SetLODIncrements(int lodIncrements);

	void Render(CCamera& camera, matrix4x4& worldMatrix);
	// render-+
	//		  +-render model, high performance
	//		  +-(more render modes)

	string GetFilename(){return m_filename;}
	void SetRenderMode(int mode);

	float FindMaximumRadius();
private:

	void UpdateMesh(float timeDelta);

	string m_filename;
	CalModel* m_calModel;
	// vertex arrays 
	// vertex buffer object references
	// rendering properties...
	//		lod options...
	//			lod increments
	//			current lod real value
	//			current lod increment
	//		using vobs?
	int m_renderMode; // 0 = normal, 1 = no texture, 2 = wireframe
	// map<mesh, <submesh, vertex data>>
	map <int, map <int, float* > > m_vertices;
	map <int, map <int, float* > > m_normals;
	map <int, map <int, float* > > m_textureCoords;
	map <int, map <int, int* > >   m_faces;
	map <int, map <int, int> >     m_textureCoordCounts;
	map <int, map <int, int> >     m_faceCounts;
};

class CCalCoreModelRepository
{
public:
	CCalCoreModelRepository();
	~CCalCoreModelRepository();

	static CCalCoreModelRepository* GetInstance();

	// LoadModel
	// bool AddModel(string filename);

	CCalModel* GetModelInstance(string filename);
	void ReleaseModelInstance(CCalModel* model);

	// Reference
	// Unreference
private:
	void ReferenceModel(string filename); //
	void UnreferenceModel(string filename); //

	map<string, CCalCoreModel*> m_repository;
	map<string, int> m_references; // reference counter... unload model when references == 0

	static CCalCoreModelRepository* instance;
};


/* old shit (before cal3d implementation)

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

struct Material
{
	Material():m_pTextureFilename(0){}
	~Material()
	{
		if( m_pTextureFilename )
			delete [] m_pTextureFilename;
		m_pTextureFilename = 0;
	}
	float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
	float m_shininess;
	unsigned int m_texture; // GLuint
	char *m_pTextureFilename;
};

struct Mesh
{
	Mesh();
	~Mesh();
	void CreateVertexArrays(Triangle* pTriangles, Vertex* pVertices);
//	void DeleteVertexIndexBuffer();
	void DeleteVertexArrays();
	unsigned int m_materialIndex;
	unsigned int m_triangleCount;
	unsigned int *m_pTriangleIndices;
//	unsigned int *m_pTriangleVertexIndices;
	//Normal *m_pNormalVertexArray;

	// vertex array's used for rendering
	Vertex* m_pVertexArray;
	Normal* m_pNormalArray;
	TextureCoordinate* m_pTextureCoordArray;
//	VertexArrayElement* m_pInterleavedVertexArray;
};

struct KeyframeRotation
{

};


class CCalModel
{
public:
	CCalModel();
	~CCalModel();
	CCalModel(const CCalModel& rhs);

	void Render(CCamera& camera);

	void CopyFrom(const CCalModel& rhs);

	void FreeMemory();
	CCalModel operator=(const CCalModel& rhs);

	Vertex* m_pVertices; // pointer to all of the verticies in the mesh (no duplicates)
	unsigned short m_vertexCount; // number of verticies
	Triangle* m_pTriangles; // pointer to all triangles in the model, each triangle indexes 3 verticies
	unsigned short m_triangleCount; // number of triangles in model
	Mesh* m_pMeshes; // pointer to all meshes (groups) in this model
	unsigned short m_meshCount; // number of meshes
	Material *m_pMaterials;
	unsigned short m_materialCount;

	GLuint m_displayList;
	bool m_displayListUpdated;
	matrix4x4 worldMatrix; // used to transform from model to world space
};

*/
#endif