#include "CModel.h"
#include "CXMLParser.h"
#include "CVideoSubsystem.h"
#include "CMilkShapeModel.h"

#include <string>
#include <fstream>
using namespace std;

CMaterial::CMaterial()
{
	for(int i = 0; i < 4; i++)
	{
		m_ambient[i] = 0.2f;
		m_diffuse[i] = 1.0f;
		m_specular[i] = 0.5f;
		m_emissive[i] = 0.0f;
	}
	m_ambient[3] = 1.0f;
	m_shininess = 0.5f;
}
void CMaterial::SetTexture(string file)
{
	m_texture = Video.VideoResourceManager.LoadImageToVideoMemory(file);
	m_textureFilename = file;
}


CMesh::CMesh()
{
	m_materialIndex = 0;
	m_lod = 1;
	m_triangleCount = 0;
}
CMesh::CMesh(CCoreMesh &coreMesh)
{
	m_spCoreMesh = CSmartPointer<CCoreMesh>(&coreMesh, false);
//	m_pCoreVertexArray = coreMesh.m_pVertexArray;
//	m_pCoreNormalArray = coreMesh.m_pNormalArray;
//	m_pCoreTextureCoordArray = coreMesh.m_pTextureCoordArray;
	m_materialIndex = coreMesh.m_materialIndex;
	m_lod = 1;
	m_triangleCount = coreMesh.m_triangleCount;
}
CMesh::~CMesh()
{
}
void CMesh::Initialize(CCoreMesh &coreMesh)
{
	m_spCoreMesh = CSmartPointer<CCoreMesh>(&coreMesh, false);
//	m_pCoreVertexArray = coreMesh.m_pVertexArray;
//	m_pCoreNormalArray = coreMesh.m_pNormalArray;
//	m_pCoreTextureCoordArray = coreMesh.m_pTextureCoordArray;
//	m_glodPatch = 0;
	m_materialIndex = coreMesh.m_materialIndex;
	m_lod = 1;	
	m_triangleCount = coreMesh.m_triangleCount;
}

void CMesh::CopyFrom(const CMesh& rhs)
{
	m_spCoreMesh = rhs.m_spCoreMesh;
//	m_glodPatch = rhs.m_glodPatch; // this ok?????
	m_lod = rhs.m_lod;
	m_materialIndex = rhs.m_materialIndex;
//	m_pCoreNormalArray = rhs.m_pCoreNormalArray;
//	m_pCoreTextureCoordArray = rhs.m_pCoreTextureCoordArray;
//	m_pCoreVertexArray = rhs.m_pCoreVertexArray;
	m_triangleCount = rhs.m_triangleCount;
}


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
//	DeleteVertexArrays();
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
	*(this->m_spVertexBuffer) = (*rhs.m_spVertexBuffer);
}
void CCoreMesh::PrepareVertexBuffer(unsigned int vertexCount, unsigned short vertexBufferFlags)
{
	this->m_spVertexBuffer = CSmartPointer<CVertexBuffer>(new CVertexBuffer(vertexCount, vertexBufferFlags), false);
}

void CCoreMesh::InitializeGlod(GLuint objectName, GLuint patchName)
// sets up the glod arrays for this patch/mesh
// call only after vertex arrays are valid
{
	m_spGlodMesh = CSmartPointer<CGlodMesh>(new CGlodMesh(m_spVertexBuffer, objectName, patchName), false);
}

/*void CCoreMesh::CreateVertexArrays(CSmartPointer<Triangle>& pTriangles, CSmartPointer<Vertex>& pVertices)
{
	DeleteVertexArrays(); // not necessary with smart pointers...
	m_pVertexArray = CSmartPointer<Vertex>(new Vertex [m_triangleCount * 3], true);
	m_pNormalArray = CSmartPointer<Normal>(new Normal [m_triangleCount * 3], true);
	m_pTextureCoordArray = CSmartPointer<TextureCoordinate>(new TextureCoordinate [m_triangleCount * 3], true);

	for(int j = 0; j < m_triangleCount; j++)
	{
		int triangleIndex = m_pTriangleIndices[j];
		for(int vertexNumber = 0; vertexNumber < 3; vertexNumber++)
		{
			memcpy(&m_pVertexArray.GetPointer()[j*3+vertexNumber], &pVertices[ pTriangles[ triangleIndex ].m_vertexIndices[ vertexNumber ] ], sizeof(Vertex));
			memcpy(&m_pNormalArray.GetPointer()[j*3+vertexNumber].m_normal, &pTriangles[ triangleIndex ].m_vertexNormals[ vertexNumber ], sizeof(float)*3);
			m_pTextureCoordArray.GetPointer()[j*3+vertexNumber].m_s = pTriangles[ triangleIndex ].m_s[vertexNumber];
			m_pTextureCoordArray.GetPointer()[j*3+vertexNumber].m_t = pTriangles[ triangleIndex ].m_t[vertexNumber];
		}
	}
}
void CCoreMesh::DeleteVertexArrays()
{
	// smart pointers
	m_pVertexArray.Release();
	m_pNormalArray.Release();
	m_pTextureCoordArray.Release();
}*/



CCoreModel::CCoreModel():
	m_vertexCount(0),
	m_triangleCount(0),
	m_meshCount(0),
	m_materialCount(0)
{
}
CCoreModel::~CCoreModel()
{
}
CCoreModel::CCoreModel(const CCoreModel& rhs)
{
	CopyFrom(rhs);
}

void CCoreModel::InitializeGlod(GLuint glodGroup)
{
	// first, create a glod object

	//m_spGlodObject = CSmartPointer<CGlodObject>(new CGlodObject(glodGroup), false);

	//// now, add all the patches (meshes)
	//for(int i = 0; i < this->m_meshCount; i++)
	//{
	//	m_pMeshes[i].InitializeGlod(m_spGlodObject->GetObjectName(), i);
	//}

	//// finally, build the object
	//m_spGlodObject->BuildObject();
}


void CCoreModel::CopyFrom(const CCoreModel& rhs)
{
	ccout << "Warning! CCoreModel::CopyFrom doesn't work for some reason!\n";
	// copy vertices
	this->m_vertexCount = rhs.m_vertexCount;
	this->m_pVertices = CSmartPointer<Vertex>(new Vertex[rhs.m_vertexCount], true);
	memcpy(m_pVertices.GetPointer(), rhs.m_pVertices.GetPointer(), sizeof(Vertex) * rhs.m_vertexCount);

	// copy triangles
	this->m_triangleCount = rhs.m_triangleCount;
	this->m_pTriangles = CSmartPointer<Triangle>(new Triangle[rhs.m_triangleCount], true);
	memcpy(m_pTriangles.GetPointer(), rhs.m_pTriangles.GetPointer(), sizeof(Triangle) * rhs.m_triangleCount);

	// copy meshes
	this->m_meshCount = rhs.m_meshCount;
	this->m_pMeshes = CSmartPointer<CCoreMesh>(new CCoreMesh[rhs.m_meshCount], true);
	for(int i = 0; i < rhs.m_meshCount; i++)
	{
		m_pMeshes[i] = rhs.m_pMeshes[i];
	}

	this->m_materialCount = rhs.m_materialCount;
	this->m_pMaterials = CSmartPointer<CMaterial>(new CMaterial[rhs.m_materialCount], true);
	for(int i = 0; i < rhs.m_materialCount; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			m_pMaterials[i].m_ambient[j] = rhs.m_pMaterials[i].m_ambient[j];
			m_pMaterials[i].m_diffuse[j] = rhs.m_pMaterials[i].m_diffuse[j];
			m_pMaterials[i].m_emissive[j] = rhs.m_pMaterials[i].m_emissive[j];
			m_pMaterials[i].m_specular[j] = rhs.m_pMaterials[i].m_specular[j];
		}
		m_pMaterials[i].m_shininess = rhs.m_pMaterials[i].m_shininess;
		m_pMaterials[i].m_textureFilename = rhs.m_pMaterials[i].m_textureFilename;
		m_pMaterials[i].m_texture = rhs.m_pMaterials[i].m_texture; // should clean up this, make some other
																			// sort of reference to textures,
																			// maybe just a smart pointer
																			// that way could manage them better
																			// especially when changing resolutions and stuff
	}
/*
#ifdef USE_GLOD
	// first, check to see if there is already a glod object, if so, kill it
	if(m_glodObject != 0)
		glodDeleteObject(m_glodObject);
	// get an unused glod object name (an id)
	m_glodObject = glodObjectCounter++;
	// create the new object
	glodNewObject(m_glodObject, GLOD_GLOBAL_GROUP, GLOD_DISCRETE);
	// now, add all the patches (meshes)
#endif
	for(int i = 0; i < this->m_meshCount; i++)
	{
		m_pMeshes[i].Initialize(m_glodObject, i);
	}
#ifdef USE_GLOD

	glodBuildObject(m_glodObject);
#endif*/
}

CCoreModel CCoreModel::operator=(const CCoreModel& rhs)
{
	if(&rhs == this)
		return *this;
	CopyFrom(rhs);
	return *this;
}


float CModel::trianglesRendered = 0;
float CModel::verticesRendered = 0;


CModel::CModel(CCoreModel& coreModel):
	m_vertexCount(coreModel.m_vertexCount),
	m_triangleCount(coreModel.m_triangleCount),
	m_meshCount(coreModel.m_meshCount),
	m_materialCount(coreModel.m_materialCount),
	m_renderMode(0)
{
	m_spCoreModel = CSmartPointer<CCoreModel>(&coreModel, false); // CAREFULL, THIS WORKS WITH CURRENT IMPLEMENTATION OF SMART POINTERS,
								// BUT MAY NOT WITH OTHERS
	m_pCoreVertices = coreModel.m_pVertices;
	m_pCoreTriangles = coreModel.m_pTriangles;
	m_pCoreMaterials = coreModel.m_pMaterials;
	m_pMeshes = CSmartPointer<CMesh>(new CMesh[coreModel.m_meshCount], true);
	for(int i = 0; i < coreModel.m_meshCount; i++)
	{
		m_pMeshes[i].Initialize(coreModel.m_pMeshes[i]);
	}
}
CModel::~CModel()
{
	// let smart pointers go out of scope
}
CModel::CModel(const CModel& rhs)
{
	CopyFrom(rhs);
}


void CModel::CopyFrom(const CModel& rhs)
{
	m_spCoreModel = rhs.m_spCoreModel;

	m_vertexCount = rhs.m_vertexCount;
	m_pCoreVertices = rhs.m_pCoreVertices;

	m_triangleCount = rhs.m_triangleCount;
	m_pCoreTriangles = rhs.m_pCoreTriangles;

	m_meshCount = rhs.m_meshCount;
	m_pMeshes = CSmartPointer<CMesh>(new CMesh[rhs.m_meshCount], true);
	for(int i = 0; i < rhs.m_meshCount; i++)
	{
		m_pMeshes[i].CopyFrom(rhs.m_pMeshes[i]);
	}

	m_materialCount = rhs.m_materialCount;
	m_pCoreMaterials = rhs.m_pCoreMaterials;

	m_lod = rhs.m_lod;
	m_renderMode = rhs.m_renderMode;
//	m_glodObject = rhs.m_glodObject;
}

CModel CModel::operator=(const CModel& rhs)
{
	if(&rhs == this)
		return *this;
	CopyFrom(rhs);
	return *this;
}

void CModel::Render(CCamera& camera, matrix4x4 worldMatrix)
{
//	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if(Video.settings.getSh())
		gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 5000000.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLoadMatrixf((camera.GetModelViewMatrix() * worldMatrix).matrix);

	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); // DISABLED BECAUSE OF BLENDING PROBLEMS /////////////////////////////////////////////////////////////////////
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if(m_renderMode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for(int i = 0; i < m_meshCount; i++)
	{
	//	if(!m_pMeshes[i].m_triangleCount)
	//		continue;
		int materialIndex = m_pMeshes[i].m_materialIndex;
		if (  materialIndex >= 0 )
		{
			glMaterialfv( GL_FRONT, GL_AMBIENT, m_pCoreMaterials[materialIndex].m_ambient );
			glMaterialfv( GL_FRONT, GL_DIFFUSE, m_pCoreMaterials[materialIndex].m_diffuse );
			glMaterialfv( GL_FRONT, GL_SPECULAR, m_pCoreMaterials[materialIndex].m_specular );
			glMaterialfv( GL_FRONT, GL_EMISSION, m_pCoreMaterials[materialIndex].m_emissive );
			glMaterialf( GL_FRONT, GL_SHININESS, m_pCoreMaterials[materialIndex].m_shininess );

			if (m_pCoreMaterials[materialIndex].m_texture > 0 )
			{
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, m_pCoreMaterials[materialIndex].m_texture );
			}
			else
				glDisable( GL_TEXTURE_2D );
		}
		else
		{
			glColor3f(1.0f, 1.0f, 1.0f);
			glDisable( GL_TEXTURE_2D );
		}
	/*
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
#ifndef USE_GLOD
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);


			glVertexPointer(3, GL_FLOAT,  sizeof(Vertex), // 1, because there is an extra byte in m_boneID
							m_pMeshes[i].m_pCoreVertexArray.GetPointer()); // start past the m_boneID
			glNormalPointer(GL_FLOAT, 0,
							m_pMeshes[i].m_pCoreNormalArray.GetPointer());
			glTexCoordPointer(2,
							GL_FLOAT,
							0,
							m_pMeshes[i].m_pCoreTextureCoordArray.GetPointer());
#endif
#ifdef USE_GLOD

			GLuint object = (*m_spCoreModel).m_glodObject;
			glodFillArrays((*m_spCoreModel).m_glodObject, i);
#endif
		
			
	//	void glMultiDrawArraysEXT( GLenum mode,   // maybe try using this extensions to render whole model in one call? wouldn't work with glod unless data is dumped from it into the class members
	//					GLint *first,
	//					GLsizei *count,
	//					GLsizei primcount)
*/
		bool renderGlod = false;
		if(renderGlod)
			m_pMeshes[i].m_spCoreMesh->m_spGlodMesh->EnableVertexBuffers();
		else
			m_pMeshes[i].m_spCoreMesh->m_spVertexBuffer->EnableVertexBuffers();

		glDrawArrays( GL_TRIANGLES, 0, m_pMeshes[i].m_triangleCount*3 );		// Draw All Of The Triangles At Once

		trianglesRendered += m_pMeshes[i].m_triangleCount;


		glDisableClientState( GL_VERTEX_ARRAY );				// Disable Vertex Arrays
		glDisableClientState( GL_NORMAL_ARRAY );				// Disable Vertex Arrays
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );				// Disable Texture Coord Arrays

	}
		
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

float CModel::FindMaximumRadius()
{
	float radius = 0.;

	for(int i = 0; i < this->m_vertexCount; i++)
	{
		float currentRadius = 
			sqrt(this->m_pCoreVertices[i].m_position[0] * this->m_pCoreVertices[i].m_position[0] +
				this->m_pCoreVertices[i].m_position[1] * this->m_pCoreVertices[i].m_position[1] +
				this->m_pCoreVertices[i].m_position[2] * this->m_pCoreVertices[i].m_position[2]);
		if(currentRadius > radius)
			radius = currentRadius;
	}
	
	return radius;
}
void CModel::SetLOD(float lod)
{
	m_lod = lod;
}
CModelRepository* CModelRepository::instance = 0;

CModelRepository::CModelRepository()
{
//	m_spGlodGroup = CSmartPointer<CGlodGroup>(new CGlodGroup, false);
}
CModelRepository::~CModelRepository()
{
	if(instance)
		delete instance;
	instance = 0;
}

CSmartPointer<CModel> CModelRepository::GetModelInstance(string filename)
{
	CSmartPointer<CCoreModel> coreModel;

	coreModel = m_resourcePool.GetSharedResource(filename);

	if(coreModel.Null())
	{
		coreModel = CMilkShapeModelLoader::GetInstance()->LoadModel(filename);
//		coreModel->InitializeGlod(this->m_spGlodGroup->GetGroupName());

		m_resourcePool.AddNewResource(filename, coreModel);
	}

	return CSmartPointer<CModel>(new CModel(*coreModel), false);
}
void CModelRepository::SetTriangleBudget(float triangles)
{
//	m_spGlodGroup->SetTriangleBudget(triangles);
}

CModelRepository* CModelRepository::Instance()
{
	if(!instance)
		instance = new CModelRepository;
	return instance;
}

void CModelRepository::UpdateGlod()
{
//	m_spGlodGroup->AdaptGroup();
}


/* old CModel class below... leave for good tidbits
CModel::CModel():
m_pVertices(0),
m_pTriangles(0),
m_pMeshes(0),
m_pMaterials(0),
m_vertexCount(0),
m_triangleCount(0),
m_meshCount(0),
m_materialCount(0),
m_displayList(0),
m_displayListUpdated(false)
{
	worldMatrix = matrix4x4::mtfIdentity();
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
m_materialCount(0),
m_displayList(0),
m_displayListUpdated(false)
{
	CopyFrom(rhs);
}
void CModel::Render(CCamera& camera)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	if(Video.settings.modelsUseDisplayLists)
	{
		if(!this->m_displayListUpdated)
		{
			this->m_displayList = Video.GenerateDisplayListFromModel(*this);
			this->m_displayListUpdated = true;
		}
		Video.DrawDisplayList(this->m_displayList, this->worldMatrix, camera);
	}
	else
	{
		Video.DrawModel(*this, camera);
	}
}


void CModel::CopyFrom(const CModel& rhs)
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
		//memcpy(&m_pMeshes[i], &rhs.m_pMeshes[i], sizeof(Mesh)); // warning: includes a pointer, must follow up with memory allocation and array copy
		m_pMeshes[i].m_materialIndex = rhs.m_pMeshes[i].m_materialIndex;
		m_pMeshes[i].m_triangleCount = rhs.m_pMeshes[i].m_triangleCount;
		m_pMeshes[i].m_pTriangleIndices = new unsigned int[rhs.m_pMeshes[i].m_triangleCount];
		for(int j = 0; j < rhs.m_pMeshes[i].m_triangleCount; j++)
		{
			m_pMeshes[i].m_pTriangleIndices[j] = rhs.m_pMeshes[i].m_pTriangleIndices[j];
		}
		m_pMeshes[i].CreateVertexArrays(this->m_pTriangles, this->m_pVertices);
	}

	m_materialCount = rhs.m_materialCount;
	m_pMaterials = new CMaterial[rhs.m_materialCount];
	for(int i = 0; i < rhs.m_materialCount; i++)
	{
		//memcpy(&m_pMaterials[i], &rhs.m_pMaterials[i], sizeof(Material)); // warning: includes a pointer, must follow up with memory allocation and array copy
		m_pMaterials[i]
		m_textureFilename = rhs.m_pMaterials[i].
		//if(rhs.m_pMaterials[i].m_pTextureFilename)
		//{
		//	m_pMaterials[i].m_pTextureFilename = new char[strlen(rhs.m_pMaterials[i].m_pTextureFilename)];
		//	strncpy( m_pMaterials[i].m_pTextureFilename, rhs.m_pMaterials[i].m_pTextureFilename, strlen(rhs.m_pMaterials[i].m_pTextureFilename) );
		//}	
	}

	m_displayList = rhs.m_displayList;
	m_displayListUpdated = rhs.m_displayListUpdated;
	worldMatrix = rhs.worldMatrix;
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

	if(m_pMeshes != 0)
	{
	//	for(int i = 0; i < m_meshCount; i++)
	//	{
	//		if(m_pMeshes[i].m_pTriangleIndices)
	//			delete [] m_pMeshes[i].m_pTriangleIndices;
	//	}
		delete [] m_pMeshes;
	}
	m_pMeshes = 0;
	m_meshCount = 0;

	if(m_pMaterials)
	{
		//for(int i = 0; i < m_materialCount; i++)
		//{
		//	if(m_pMaterials[i].m_pTextureFilename)
		//		delete [] m_pMaterials[i].m_pTextureFilename;
		//}
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

	CopyFrom(rhs);
	return *this;
}*/
