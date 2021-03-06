#include "CMilkShapeModel.h"
#include "con_main.h"
#include "CVideoSubsystem.h"

#include <fstream>
#include <string>
#include <string.h>
using namespace std;

CMilkShapeModelLoader* CMilkShapeModelLoader::instance = 0;

CMilkShapeModelLoader* CMilkShapeModelLoader::GetInstance()
{
	if(instance == 0)
		instance = new CMilkShapeModelLoader();

	return instance;
}

CMilkShapeModelLoader::CMilkShapeModelLoader()
{
	m_pFileData = 0;
	m_fileSize = 0;
}
CMilkShapeModelLoader::~CMilkShapeModelLoader()
{
	FreeMemory();
	if(instance != 0)
		delete instance;
}
CSmartPointer<CCoreModel> CMilkShapeModelLoader::LoadModel(string filename)
{
	if(!ReadModelFromFile(filename))
		return CSmartPointer<CCoreModel>();

	CSmartPointer<CCoreModel> coreModel;
	LoadModel(coreModel);
	return coreModel;
}
/*
bool CMilkShapeModelLoader::LoadModel(CModel& model, string filename)
{
	if(!ReadModelFromFile(filename))
		return false;

	LoadModel(model);
	return true;
}*/


bool CMilkShapeModelLoader::ReadModelFromFile(string filename) // should open the model with filename specified and process it and then close the file
{
	ccout << "Reading model file: " << filename << newl;
	if(!ReadDataFromFile(filename)) // read the file data
		return false; // error reading the file

	const char* pFilePosition = m_pFileData; // start at the beginning

	const ms3d_header_t* pHeader = (const ms3d_header_t*)pFilePosition;
	pFilePosition+=sizeof(ms3d_header_t);
	if(strncmp(pHeader->id, "MS3D000000", 10) != 0)
	{
		FreeMemory();
		return false; // not a milkshape file
	}
	if(pHeader->version != 4)
	{
		FreeMemory();
		return false; // wrong version
	}

	// create a model
	m_pModel = CSmartPointer<CCoreModel>(new CCoreModel, false);

	// next read the number of verticies in the model
	m_pModel->m_vertexCount = *(const word*)pFilePosition;
	m_pModel->m_pVertices = CSmartPointer<Vertex>(new Vertex[m_pModel->m_vertexCount], true);
	pFilePosition += sizeof(word);

	//
	// Then comes nNumVertices * sizeof (ms3d_vertex_t)
	// read all vertex data
	for(unsigned short i = 0; i < m_pModel->m_vertexCount; i++)
	{
		const ms3d_vertex_t * ms3d_vertex = (const ms3d_vertex_t *) pFilePosition;
		

	//	m_pModel->m_pVertices[i].m_boneID = ms3d_vertex->boneId;
		m_pModel->m_pVertices[i].m_position[0] = ms3d_vertex->vertex[0];
		m_pModel->m_pVertices[i].m_position[1] = ms3d_vertex->vertex[1];
		m_pModel->m_pVertices[i].m_position[2] = ms3d_vertex->vertex[2];
	
		pFilePosition += sizeof( ms3d_vertex_t );
	}


	// next read the number of triangles in the model
	m_pModel->m_triangleCount = *(const word*)pFilePosition;
	m_pModel->m_pTriangles = CSmartPointer<Triangle>(new Triangle[m_pModel->m_triangleCount], true);
	pFilePosition += sizeof(word);

	//
	// nNumTriangles * sizeof (ms3d_triangle_t)
	//
	for(unsigned short i = 0; i < m_pModel->m_triangleCount; i++)
	{
		const ms3d_triangle_t * ms3d_triangle = (const ms3d_triangle_t *) pFilePosition;
		
		int vertexIndices[3] = { ms3d_triangle->vertexIndices[0], ms3d_triangle->vertexIndices[1], ms3d_triangle->vertexIndices[2] }; // stored as words in file, ints here
		memcpy( m_pModel->m_pTriangles[i].m_vertexIndices, vertexIndices, sizeof(int)*3 );
		
		memcpy( m_pModel->m_pTriangles[i].m_vertexNormals, ms3d_triangle->vertexNormals, sizeof(float)*3*3 );
		memcpy( m_pModel->m_pTriangles[i].m_s, ms3d_triangle->s, sizeof(float)*3 );

		float t[3] = { 1-ms3d_triangle->t[0], 1-ms3d_triangle->t[1], 1-ms3d_triangle->t[2] }; // invert y coordinates because of differing conventions
		memcpy( m_pModel->m_pTriangles[i].m_t, t, sizeof(float)*3 );
		
		pFilePosition += sizeof( ms3d_triangle_t );
	}

	// next read the number of groups or meshes in the model
	m_pModel->m_meshCount = *(const word*)pFilePosition;
	m_pModel->m_pMeshes = CSmartPointer<CCoreMesh>(new CCoreMesh[m_pModel->m_meshCount], true);
	pFilePosition += sizeof(word);

	//
	// nNumGroups * sizeof (ms3d_group_t)
	// 
	/*typedef struct
{
    char            flags;                              // SELECTED | HIDDEN
    char            name[32];                           //
    word            numtriangles;                       //
    word            triangleIndices/*[numtriangles]*/;      // the groups group the triangles
 /*   char            materialIndex;                      // -1 = no material
} ms3d_group_t;*/

	map<int, map<int, int> > triangleIndices;

	for(unsigned short i = 0; i < m_pModel->m_meshCount; i++)
	{
		//const ms3d_group_t * ms3d_group = (const ms3d_group_t *) pFilePosition;
		
		pFilePosition += sizeof(char); // eat flags

		//ccout << "Group Name: " << pFilePosition << newl;
		pFilePosition += sizeof(char)*32; // eat name

		m_pModel->m_pMeshes[i].m_triangleCount = *(const word*)pFilePosition;
		pFilePosition += sizeof(word); // eat numtriangles

//		CSmartPointer<int> triangleIndices = CSmartPointer<int>(new int[m_pModel->m_pMeshes[i].m_triangleCount], true);
		for(int j = 0; j < m_pModel->m_pMeshes[i].m_triangleCount; j++)
		{
			triangleIndices[i][j] = *(const word*)pFilePosition;
			pFilePosition += sizeof(word); // eat the triangle index
		}

		m_pModel->m_pMeshes[i].m_materialIndex = *(const char*)pFilePosition; // pFilePostition currently pointing to materialIndex;

		pFilePosition += sizeof( char );
	}

	// next read the number of groups or meshes in the model
	m_pModel->m_materialCount = *(const word*)pFilePosition;
	m_pModel->m_pMaterials = CSmartPointer<CMaterial>(new CMaterial[m_pModel->m_materialCount], true);
	pFilePosition += sizeof(word);

	//
	// nNumMaterials * sizeof (ms3d_material_t)
	//
/*	typedef struct
	{
		char            name[32];                           //
		float           ambient[4];                         //
		float           diffuse[4];                         //
		float           specular[4];                        //
		float           emissive[4];                        //
		float           shininess;                          // 0.0f - 128.0f
		float           transparency;                       // 0.0f - 1.0f
		char            mode;                               // 0, 1, 2 is unused now
		char            texture[128];                        // texture.bmp
		char            alphamap[128];                       // alpha.bmp
	} ms3d_material_t;*/
	for(unsigned short i = 0; i < m_pModel->m_materialCount; i++)
	{
		const ms3d_material_t * ms3d_material = (const ms3d_material_t *) pFilePosition;
		
		//ccout << "Material Name: " << ms3d_material->name;

		memcpy( m_pModel->m_pMaterials[i].m_ambient, ms3d_material->ambient, sizeof(float)*4 );
		memcpy( m_pModel->m_pMaterials[i].m_diffuse, ms3d_material->diffuse, sizeof(float)*4 );
		memcpy( m_pModel->m_pMaterials[i].m_specular, ms3d_material->specular, sizeof(float)*4 );
		memcpy( m_pModel->m_pMaterials[i].m_emissive, ms3d_material->emissive, sizeof(float)*4 );
		m_pModel->m_pMaterials[i].m_shininess = ms3d_material->shininess;
		m_pModel->m_pMaterials[i].m_textureFilename = ms3d_material->texture;
		//ccout << " Texture Name: " << ms3d_material->texture << newl;
		
		if(ms3d_material->texture[0] != '\0')
			m_pModel->m_pMaterials[i].m_texture = Video.VideoResourceManager.LoadImageToVideoMemory(ms3d_material->texture);
		else
			m_pModel->m_pMaterials[i].m_texture = 0;

		pFilePosition += sizeof( ms3d_material_t );
	}


	// now, take all the vertex data and put it in a vertex buffer for each mesh
	for(int i = 0; i < m_pModel->m_meshCount; i++)
	{
		unsigned char flags;
		flags = VB_NORMALS;

		if(m_pModel->m_pMaterials[m_pModel->m_pMeshes[i].m_materialIndex].m_texture >= 0)
			flags |= VB_TEXTURECOORDS;

		// now, add the data to our mesh
		m_pModel->m_pMeshes[i].PrepareVertexBuffer(m_pModel->m_pMeshes[i].m_triangleCount*3, flags);
		CVertexBuffer& vb = *(m_pModel->m_pMeshes[i].m_spVertexBuffer);
		for(int tri = 0; tri < m_pModel->m_pMeshes[i].m_triangleCount; tri++)
		{
			int triangleIndex = triangleIndices[i][tri];
			Triangle& triangle = m_pModel->m_pTriangles[ triangleIndex ];
			for(int vert = 0; vert < 3; vert++)
			{
				Vertex& vertex = m_pModel->m_pVertices[ triangle.m_vertexIndices[vert] ];
				
				vb.AddVertex(
					vertex.m_position[0],
					vertex.m_position[1],
					vertex.m_position[2]
					);
				vb.AddNormal(
					triangle.m_vertexNormals[vert][0],
					triangle.m_vertexNormals[vert][1],
					triangle.m_vertexNormals[vert][2]
					);
				if(flags & VB_TEXTURECOORDS)
				{
					vb.AddTextureCoordinate(
						triangle.m_s[vert],
						triangle.m_t[vert]
						);
				}
			}
		}
	}

	ccout << "Model file has been read.\n";
	return true;
}

void CMilkShapeModelLoader::LoadModel(CSmartPointer<CCoreModel>& model) // copy the model to the CModel object
{
	model = m_pModel;//CSmartPointer<CCoreModel>(new CCoreModel(*m_pModel), false);
	m_pModel.Release(); // smart pointer
}


bool CMilkShapeModelLoader::ReadDataFromFile(string filename) // read the data into m_pFileData
{
	ifstream fin( filename.c_str(), ios::in | ios::binary );

	if(!fin.is_open()) // error opening file
	{
		fin.close();
		return false;
	}

	FreeMemory(); // free memory and set pointers to NULL

	// find out the file size
	fin.seekg( 0, ios::end );
	m_fileSize = fin.tellg(); 
	fin.seekg( 0, ios::beg );

	// allocate memory
	m_pFileData = new char[m_fileSize];

	// read the data in
	fin.read(m_pFileData, m_fileSize);
	fin.close();

	return true;
}

void CMilkShapeModelLoader::FreeMemory() // free memory and set to NULL
{
	if(m_pFileData)
		delete [] m_pFileData;
	m_pFileData = NULL;
	m_fileSize = 0;

	//if(m_pModel)
	//	delete m_pModel;
	m_pModel.Release();
}
