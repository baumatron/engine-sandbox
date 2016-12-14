
#ifndef CMODEL_H
#define CMODEL_H

struct Vertex
{
	char m_boneID;	// For Skeletal Animation
	float m_position[3];
};

struct Triangle
{
	float m_vertexNormals[3][3];	// normals for each vertex
	float m_s[3], m_t[3];			// texture mapping coordinates for each vertex
	int m_vertexIndices[3];			// vertex indices
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
	Mesh():m_pTriangleIndices(0){}
	~Mesh()
	{
		if( m_pTriangleIndices ) 
			delete [] m_pTriangleIndices; // why does this fuck up?
		m_pTriangleIndices = 0;
	}
	int m_materialIndex;
	int m_triangleCount;
	int *m_pTriangleIndices;
};

struct KeyframeRotation
{

};


class CModel
{
public:
	CModel();
	~CModel();
	CModel(const CModel& rhs);

	void FreeMemory();
	CModel operator=(const CModel& rhs);

	Vertex* m_pVertices; // pointer to all of the verticies in the mesh (no duplicates)
	unsigned short m_vertexCount; // number of verticies
	Triangle* m_pTriangles; // pointer to all triangles in the model, each triangle indexes 3 verticies
	unsigned short m_triangleCount; // number of triangles in model
	Mesh* m_pMeshes; // pointer to all meshes (groups) in this model
	unsigned short m_meshCount; // number of meshes
	Material *m_pMaterials;
	unsigned short m_materialCount;
};


#endif