/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */


#include "CCoreModel.h"

CCoreModel::CCoreModel():
	m_vertexCount(0),
	m_triangleCount(0),
	m_meshCount(0),
	m_materialCount(0)
{
}
CCoreModel::~CCoreModel()
{
//	bacon::cout << "Deleting CCoreModel\n";
}
CCoreModel::CCoreModel(const CCoreModel& rhs)
{
	CopyFrom(rhs);
}

void CCoreModel::InitializeGlod(GLuint glodGroup)
{
	// first, create a glod object

	m_spGlodObject = CSmartPointer<CGlodObject>(new CGlodObject(glodGroup));

	//// now, add all the patches (meshes)
	for(int i = 0; i < this->m_meshCount; i++)
	{
		m_spMeshesSP[i]->InitializeGlod(m_spGlodObject->GetObjectName(), i);
	}

	//// finally, build the object
	m_spGlodObject->BuildObject();
}


void CCoreModel::CopyFrom(const CCoreModel& rhs)
{
	bacon::cout << "Warning! CCoreModel::CopyFrom doesn't work for some reason!\n";
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
	this->m_spMeshesSP = CSmartPointer< CSmartPointer<CCoreMesh> >(new CSmartPointer<CCoreMesh>[rhs.m_meshCount], true, rhs.m_meshCount);
//	this->m_spMeshesSP = CSmartPointer< CCoreMesh >(new CCoreMesh[rhs.m_meshCount], true);
	for(int i = 0; i < rhs.m_meshCount; i++)
	{
		this->m_spMeshesSP[i] = CSmartPointer<CCoreMesh>(new CCoreMesh);
		(*this->m_spMeshesSP[i]) = (*rhs.m_spMeshesSP[i]);
		//m_pMeshes[i] = rhs.m_pMeshes[i];
	}

	this->m_materialCount = rhs.m_materialCount;
	this->m_pMaterials = CSmartPointer<CMaterial>(new CMaterial[rhs.m_materialCount], true);
	for(int i = 0; i < rhs.m_materialCount; i++)
	{
		this->m_pMaterials[i] = rhs.m_pMaterials[i];
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

CMaterial CCoreModel::GetMaterial(int number)
{
	return m_pMaterials[number];
}

void CCoreModel::SetMaterial(int number, CMaterial material)
{
	m_pMaterials[number] = material;
}
