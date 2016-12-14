#include "CCalModel.h"
#include "CXMLParser.h"
#include "CVideoSubsystem.h"

#include <string>
#include <fstream>
using namespace std;

CCalCoreModel::CCalCoreModel():
	m_calCoreModel(0)
{
}

CCalCoreModel::~CCalCoreModel()
{
	FreeMemory();
}

bool CCalCoreModel::LoadModel(string filename)
{
	ifstream fin(filename.c_str());
	if(!fin.good())
	{
		ccout << "Error opening model file " << filename << newl;
		return false;
	}
	m_filename = filename;
	string directory;
	for(int i = filename.length()-1; i >= 0; i--)
	{
		if(filename[i] == '/')
		{
			directory = filename.substr(0, i+1);
			break;
		}
	}

	string file = CXMLParser::GetInstance()->GetTagData(fin, "calmodel");
	fin.close();
	string description = CXMLParser::GetInstance()->GetTagData(file, "description");

	m_calCoreModel = new CalCoreModel(description);

	/*if(!m_calCoreModel->create(description))
	{
		ccout << "Error creating core model with description: " << description << newl;
		// error handling ...
		return false;
	}*/

	// now load skeleton
	string skeleton = CXMLParser::GetInstance()->GetTagData(file, "skeleton");
	if(!m_calCoreModel->loadCoreSkeleton(directory + skeleton))
	{
		ccout << "Error loading core skeleton file: " << directory + skeleton << newl;
		// error handling ...
		return false;
	}

	// now the animation data
	string animationsSection = CXMLParser::GetInstance()->GetTagData(file, "animations");
	vector<string> animations = CXMLParser::GetInstance()->GetTagDataSet(animationsSection, "animation");
	for(vector<string>::iterator it = animations.begin(); it != animations.end(); it++)
	{
		if(/*int id = */m_calCoreModel->loadCoreAnimation(directory + *it) == -1)
		{
			ccout << "Error loading core animation file: " << directory + *it << newl;
			return false;
		}
	}

	// now the mesh data
	string meshSection = CXMLParser::GetInstance()->GetTagData(file, "meshes");
	vector<string> meshes = CXMLParser::GetInstance()->GetTagDataSet(meshSection, "mesh");
	for(vector<string>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		if(/*int id = */m_calCoreModel->loadCoreMesh(directory + *it) == -1)
		{
			ccout << "Error loading core animation file: " << directory + *it << newl;
			return false;
		}
	}

	// now the material data
	string materialSection = CXMLParser::GetInstance()->GetTagData(file, "materials");
	vector<string> materials = CXMLParser::GetInstance()->GetTagDataSet(materialSection, "material");
	for(vector<string>::iterator it = materials.begin(); it != materials.end(); it++)
	{
		if(/*int id = */m_calCoreModel->loadCoreMaterial(directory + *it) == -1)
		{
			ccout << "Error loading core animation file: " << directory + *it << newl;
			return false;
		}
	}

	
	// load all textures and store their identifier as user-data in the corresponding core material map
	for(int materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
	{
		// get the current core material
		CalCoreMaterial *pCoreMaterial;
		pCoreMaterial = m_calCoreModel->getCoreMaterial(materialId);

		// loop through all the maps of the current core material
		for(int mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
		{
			// get the filename of the map
			std::string strFilename;
			strFilename = pCoreMaterial->getMapFilename(mapId);

			// load the texture from the file
			Cal::UserData textureId;
			textureId = (Cal::UserData)Video.VideoResourceManager.LoadImageToVideoMemory(directory + strFilename);//myTextureLoadingFunction(strFilename);

			// store the texture id in the user data of the map
			pCoreMaterial->setMapUserData(mapId, textureId);
		}
	}

  // make one material thread for each material
  // NOTE: this is not the right way to do it, but this viewer can't do the right
  // mapping without further information on the model etc.
  for(materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
  {
    // create the a material thread
    m_calCoreModel->createCoreMaterialThread(materialId);

    // initialize the material thread
    m_calCoreModel->setCoreMaterialId(materialId, 0, materialId);
  }
    m_calCoreModel->getCoreSkeleton()->calculateBoundingBoxes(m_calCoreModel);

	return true;
}

void CCalCoreModel::FreeMemory()
{
	if(m_calCoreModel)
	{
//		m_calCoreModel->destroy();
		delete m_calCoreModel;
		m_calCoreModel = 0;
	}
}


CCalModel::CCalModel(CCalCoreModel& coreModel)
{
	m_filename = coreModel.GetFilename();
	m_calModel = new CalModel(coreModel.GetCalCoreModel());

	//coreModel.m_calCoreModel->getCoreMeshCount()
	for(int i = 0; i < coreModel.GetCalCoreModel()->getCoreMeshCount(); i++)
	{
		m_calModel->attachMesh(i);
	}

	m_calModel->setLodLevel(0.1f);
	m_calModel->setMaterialSet(0);
  // set initial animation state
//  m_state = STATE_MOTION;
//  m_calModel->getMixer()->blendCycle(0, 1.0, 0.0f);
//  m_calModel->getMixer()->blendCycle(m_animationId[0 + 1], m_motionBlend[1], 0.0f);
//  m_calModel->getMixer()->blendCycle(m_animationId[0 + 2], m_motionBlend[2], 0.0f);

	//Find the amounts of memory we will need for each submesh
	CalRenderer* renderer = m_calModel->getRenderer();

	int meshCount = renderer->getMeshCount();

	for(int i = 0; i < meshCount; i++)
	{
		int submeshCount = renderer->getSubmeshCount(i);
		for(int j = 0; j < submeshCount; j++)
		{
			renderer->selectMeshSubmesh( i, j );

			//Get vertex count (equal numbers of vertices, normals, and texture coords)
			int vertexCount = renderer->getVertexCount();

			//Get face count
			int faceCount = renderer->getFaceCount();

			//allocate all the memory
			m_vertices[i][j] = new float[vertexCount * 3];
			m_normals[i][j] = new float[vertexCount * 3];
			m_faces[i][j] = new int[faceCount * 3];

			// set the texture coordinate buffer and state if necessary
			if(renderer->getMapCount() > 0)
			{
				m_textureCoords[i][j] = new float[vertexCount * 2];
			}
			else
			{
				//If there are no texture coords, allocate space for a dummy value
				m_textureCoords[i][j] = new float[1];
			}
		}
	}
	Think(1.0f);
}
CCalModel::~CCalModel()
{
	CalRenderer* renderer = m_calModel->getRenderer();

	int meshCount = renderer->getMeshCount();

	for(int i = 0; i < meshCount; i++)
	{
		int submeshCount = renderer->getSubmeshCount( i );
		for (int j = 0; j < submeshCount; j++)
		{
			delete [] m_vertices[i][j];
			delete [] m_normals[i][j];
			delete [] m_textureCoords[i][j];
			delete [] m_faces[i][j];
		}
	}

	delete m_calModel;
}


// file stuff
// load model
// set texture directory

// vertex array updating
// Think() // updates animation, updates verticies... perhaps shouldn't be called each frame, because 
//				so many verticies will be copied...
void CCalModel::Think(float timeDelta)
{
	UpdateMesh(timeDelta);
}

// LOD
// set lod (continuous, real value)
// set number of increments
//
void CCalModel::SetLOD(float lodValue)
{
	m_calModel->setLodLevel(lodValue);
}
void CCalModel::SetLODIncrements(int lodIncrements)
{
}
void CCalModel::Render(CCamera& camera, matrix4x4& worldMatrix)
{
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
	//glEnable(GL_BLEND); // DISABLED BECAUSE OF BLENDING PROBLEMS /////////////////////////////////////////////////////////////////////
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// get the renderer of the model
	CalRenderer *pCalRenderer;
	pCalRenderer = m_calModel->getRenderer();

	// begin the rendering loop
	if(!pCalRenderer->beginRendering())
	{
	// error handling ...
	}

	/*
 // draw the bone lines
  float lines[1024][2][3];
  int nrLines;
  nrLines =  m_calModel->getSkeleton()->getBoneLines(&lines[0][0][0]);
//  nrLines = m_calModel->getSkeleton()->getBoneLinesStatic(&lines[0][0][0]);

  glLineWidth(3.0f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_LINES);
    int currLine;
    for(currLine = 0; currLine < nrLines; currLine++)
    {
      glVertex3f(lines[currLine][0][0], lines[currLine][0][1], lines[currLine][0][2]);
      glVertex3f(lines[currLine][1][0], lines[currLine][1][1], lines[currLine][1][2]);
    }
  glEnd();
  glLineWidth(1.0f);

  // draw the bone points
  float points[1024][3];
  int nrPoints;
  nrPoints =  m_calModel->getSkeleton()->getBonePoints(&points[0][0]);
//  nrPoints = m_calModel->getSkeleton()->getBonePointsStatic(&points[0][0]);



  glPointSize(4.0f);
  glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 1.0f);
    int currPoint;
    for(currPoint = 0; currPoint < nrPoints; currPoint++)
    {
      glVertex3f(points[currPoint][0], points[currPoint][1], points[currPoint][2]);
    }
  glEnd();
  glPointSize(1.0f);

CalSkeleton *pCalSkeleton = m_calModel->getSkeleton();

   std::vector<CalBone*> &vectorCoreBone = pCalSkeleton->getVectorBone();

   glColor3f(1.0f, 1.0f, 1.0f);
   glBegin(GL_LINES);      

   for(size_t boneId=0;boneId<vectorCoreBone.size();++boneId)
   {
      CalBoundingBox & calBoundingBox  = vectorCoreBone[boneId]->getBoundingBox();

	  CalVector p[8];
	  calBoundingBox.computePoints(p);

	  
	  glVertex3f(p[0].x,p[0].y,p[0].z);
	  glVertex3f(p[1].x,p[1].y,p[1].z);

	  glVertex3f(p[0].x,p[0].y,p[0].z);
	  glVertex3f(p[2].x,p[2].y,p[2].z);

	  glVertex3f(p[1].x,p[1].y,p[1].z);
	  glVertex3f(p[3].x,p[3].y,p[3].z);

	  glVertex3f(p[2].x,p[2].y,p[2].z);
	  glVertex3f(p[3].x,p[3].y,p[3].z);

  	  glVertex3f(p[4].x,p[4].y,p[4].z);
	  glVertex3f(p[5].x,p[5].y,p[5].z);

	  glVertex3f(p[4].x,p[4].y,p[4].z);
	  glVertex3f(p[6].x,p[6].y,p[6].z);

	  glVertex3f(p[5].x,p[5].y,p[5].z);
	  glVertex3f(p[7].x,p[7].y,p[7].z);

	  glVertex3f(p[6].x,p[6].y,p[6].z);
	  glVertex3f(p[7].x,p[7].y,p[7].z);

	  glVertex3f(p[0].x,p[0].y,p[0].z);
	  glVertex3f(p[4].x,p[4].y,p[4].z);

	  glVertex3f(p[1].x,p[1].y,p[1].z);
	  glVertex3f(p[5].x,p[5].y,p[5].z);

	  glVertex3f(p[2].x,p[2].y,p[2].z);
	  glVertex3f(p[6].x,p[6].y,p[6].z);

	  glVertex3f(p[3].x,p[3].y,p[3].z);
	  glVertex3f(p[7].x,p[7].y,p[7].z);  

   }

   glEnd();
*/


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	if(m_renderMode == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// get the number of meshes
	int meshCount = pCalRenderer->getMeshCount();

	// loop through all meshes of the model
	for(int meshId = 0; meshId < meshCount; meshId++)
	{
		//if(meshId != 1)
		//	continue;
		// get the number of submeshes
		int submeshCount;
		submeshCount = pCalRenderer->getSubmeshCount(meshId);

		// loop through all submeshes of the mesh
		int submeshId;
		for(submeshId = 0; submeshId < submeshCount; submeshId++)
		{
			// select mesh and submesh for further data access
			if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
			{
				
				// get the material colors
				unsigned char ambientColor[4], diffuseColor[4], specularColor[4];
				pCalRenderer->getAmbientColor(ambientColor);
				pCalRenderer->getDiffuseColor(diffuseColor);
				pCalRenderer->getSpecularColor(specularColor);
				// get the material shininess factor
				float shininess;
				shininess = pCalRenderer->getShininess();

				// set the material color properties
				float materialColor[4];
				materialColor[0] = ambientColor[0] / 255.0f;
				materialColor[1] = ambientColor[1] / 255.0f;
				materialColor[2] = ambientColor[2] / 255.0f;
				materialColor[3] = ambientColor[3] / 255.0f;
				glMaterialfv( GL_FRONT, GL_AMBIENT, materialColor );
				materialColor[0] = diffuseColor[0] / 255.0f;
				materialColor[1] = diffuseColor[1] / 255.0f;
				materialColor[2] = diffuseColor[2] / 255.0f;
				materialColor[3] = diffuseColor[3] / 255.0f;
				glMaterialfv( GL_FRONT, GL_DIFFUSE, materialColor );
				materialColor[0] = specularColor[0] / 255.0f;
				materialColor[1] = specularColor[1] / 255.0f;
				materialColor[2] = specularColor[2] / 255.0f;
				materialColor[3] = specularColor[3] / 255.0f;
				glMaterialfv( GL_FRONT, GL_SPECULAR, materialColor );
				//glMaterialfv( GL_FRONT, GL_EMISSION, model.m_pMaterials[materialIndex].m_emissive );
				glMaterialf( GL_FRONT, GL_SHININESS, shininess );

				// get the stored texture identifier
				// (only for the first map as example, others can be accessed in the same way though)
				if(m_renderMode != 1)
					if((pCalRenderer->getMapCount() > 0) && (m_textureCoordCounts[meshId][submeshId] > 0))
					{
						glEnable( GL_TEXTURE_2D );
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glBindTexture( GL_TEXTURE_2D, (GLuint)pCalRenderer->getMapUserData(0) );
						glTexCoordPointer(2,
										GL_FLOAT,
										0,
										m_textureCoords[meshId][submeshId]);
						glEnable(GL_COLOR_MATERIAL);
						glColor3f(1.0f, 1.0f, 1.0f);
					}

				// TODO MAKE Multi MAPS WORK
			
				// set up the vertex arrays
				glVertexPointer(3, GL_FLOAT,  0, // 1, because there is an extra byte in m_boneID
								m_vertices[meshId][submeshId]); // start past the m_boneID
				glNormalPointer(GL_FLOAT, 0,
								m_normals[meshId][submeshId]);

				// get the faces of the submesh
				// draw the submesh
				glDrawElements(GL_TRIANGLES, m_faceCounts[meshId][submeshId] * 3, GL_UNSIGNED_INT, m_faces[meshId][submeshId]) ;
			
				if(m_renderMode != 1)
					if((pCalRenderer->getMapCount() > 0) && (m_textureCoordCounts[meshId][submeshId] > 0))
					{
						glDisable(GL_COLOR_MATERIAL) ;

						glDisableClientState(GL_TEXTURE_COORD_ARRAY) ;

						glDisable(GL_TEXTURE_2D) ;
					}
			}
		}
	}

	glDisableClientState( GL_VERTEX_ARRAY );				// Disable Vertex Arrays
	glDisableClientState( GL_NORMAL_ARRAY );				// Disable Vertex Arrays
	
	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// end the rendering of the model
	pCalRenderer->endRendering();
}

float CCalModel::FindMaximumRadius()
{
	float radius = 0.;
	
	CalRenderer* renderer = m_calModel->getRenderer();

	//Get the mesh data for all meshes and submeshes
	int meshCount = renderer->getMeshCount();

	// get all meshes of the model
	for(int meshId = 0; meshId < meshCount; meshId++)
	{
		int submeshCount;
		submeshCount = renderer->getSubmeshCount( meshId );

		// get all submeshes of the mesh
		int submeshId;
		for(submeshId = 0; submeshId < submeshCount; submeshId++)
		{
			// select mesh and submesh and grab all the data
			if ( renderer->selectMeshSubmesh(meshId, submeshId) )
			{
				for(int vertex = 0; vertex < renderer->getVertexCount(); vertex++)
				{
					// get the transformed vertices of the submesh
					float currentRadius = 
						sqrt(m_vertices[meshId][submeshId][vertex*3+0] * m_vertices[meshId][submeshId][vertex*3+0] +
							m_vertices[meshId][submeshId][vertex*3+1] * m_vertices[meshId][submeshId][vertex*3+1] +
							m_vertices[meshId][submeshId][vertex*3+2] * m_vertices[meshId][submeshId][vertex*3+2]);
					if(currentRadius > radius)
						radius = currentRadius;
				}
			}
			else
			{
				ccout << "Trying to access an invalid mesh in CCalModel.\n";
			}
		}
	}
	
	return radius;
}

void CCalModel::UpdateMesh(float timeDelta)
{
	m_calModel->update(timeDelta);
	m_calModel->getPhysique()->update(); 
	m_calModel->getSpringSystem()->update( timeDelta ); 

	CalRenderer* renderer = m_calModel->getRenderer();

	renderer->beginRendering();

	//Get the mesh data for all meshes and submeshes
	int meshCount = renderer->getMeshCount();
	int meshId = 0;
	// get all meshes of the model
	for( ; meshId < meshCount; meshId++)
	{
		int submeshCount = renderer->getSubmeshCount( meshId );

		// get all submeshes of the mesh
		for(int submeshId = 0; submeshId < submeshCount; submeshId++)
		{
			// select mesh and submesh and grab all the data
			if ( renderer->selectMeshSubmesh(meshId, submeshId) )
			{
				// get the transformed vertices of the submesh
				renderer->getVertices( m_vertices[meshId][submeshId] );

				// get the transformed normals of the submesh
				renderer->getNormals( m_normals[meshId][submeshId] );

				// get the texture coordinates of the submesh
				m_textureCoordCounts[meshId][submeshId] = renderer->getTextureCoordinates(0, m_textureCoords[meshId][submeshId] );

				// get the faces of the submesh
				m_faceCounts[meshId][submeshId] = renderer->getFaces( m_faces[meshId][submeshId] );
			}
			else
			{
				ccout << "Trying to update an invalid mesh in CCalModel.\n";
			}

		}
	}

	renderer->endRendering();
}
// render-+
//		  +-render model, high performance
//		  +-(more render modes)


// vertex arrays 
// vertex buffer object references
// rendering properties...
//		lod options...
//			lod increments
//			current lod real value
//			current lod increment
//		using vobs?



CCalCoreModelRepository* CCalCoreModelRepository::instance = 0;

CCalCoreModelRepository* CCalCoreModelRepository::GetInstance()
{
	if(!instance)
		instance = new CCalCoreModelRepository;

	return instance;
}

CCalCoreModelRepository::CCalCoreModelRepository()
{
}
CCalCoreModelRepository::~CCalCoreModelRepository()
{
	if(!m_repository.empty())
		ccout << "Warning: The core model repository should be empty if it is being destroyed!\n";

	if(instance)
		delete instance;
	instance = 0; //????
}

	// LoadModel
	// bool AddModel(string filename);

CCalModel* CCalCoreModelRepository::GetModelInstance(string filename)
{
	filename = "data/models/skeleton/skeleton.xml";
	CCalCoreModel* theCoreModel = 0;
	map<string, CCalCoreModel*>::iterator modelIt = m_repository.find(filename);
	
	if(modelIt != m_repository.end())
	{
		// model is already loaded
		theCoreModel = modelIt->second;
	}
	else
	{
		// need to load the model
		theCoreModel = new CCalCoreModel;
		theCoreModel->LoadModel(filename);
		m_repository[filename] = theCoreModel; // add core model to repository
	}

	ReferenceModel(filename); // increase the reference count

	// now take the core model and create an instance of it
	CCalModel* theModel = new CCalModel(*theCoreModel);

	return theModel; // return a new model that must be released by this class later
}
void CCalCoreModelRepository::ReleaseModelInstance(CCalModel* model)
{
	if(!model)
		return;

	string filename = model->GetFilename();

	delete model;
	model = 0;

	UnreferenceModel(filename);
}
void CCalCoreModelRepository::ReferenceModel(string filename)
{
	map<string, int>::iterator referenceCountIt = m_references.find(filename);
	if(referenceCountIt != m_references.end())
	{
		// found reference count
		referenceCountIt->second++;
	}
	else
	{
		m_references[filename] = 1; // add new entry
	}
}
void CCalCoreModelRepository::UnreferenceModel(string filename)
{
	map<string, int>::iterator referenceCountIt = m_references.find(filename);
	if(referenceCountIt != m_references.end())
	{
		// found reference count
		referenceCountIt->second--;
		if(referenceCountIt->second == 0)
		{
			// there are no more references, so unload this model ... this could be a bad idea... check only at level changes?
			m_references.erase(filename);
			delete m_repository[filename];
			m_repository.erase(filename);
		}
	}
	else
	{
		// something is ammiss
		ccout << "Warning: unreferenced a model that doesn't exist.\n";
	}
}



/*
Mesh::Mesh():m_pTriangleIndices(0),
		//	m_pTriangleVertexIndices(0),
			m_pVertexArray(0),
			m_pNormalArray(0),
			m_pTextureCoordArray(0)//,
		//	m_pInterleavedVertexArray(0)
	{}
Mesh::~Mesh()
	{
	//	if( m_pTriangleIndices ) 
	//		delete [] m_pTriangleIndices; // why does this fuck up?
	//	m_pTriangleIndices = 0;

	//	DeleteVertexIndexBuffer();
		DeleteVertexArrays();
	}
	void Mesh::CreateVertexArrays(Triangle* pTriangles, Vertex* pVertices)
	{
	//	DeleteVertexIndexBuffer();
		DeleteVertexArrays();
	//	m_pTriangleVertexIndices = new unsigned int [m_triangleCount * 3];
		m_pVertexArray = new Vertex [m_triangleCount * 3];
		m_pNormalArray = new Normal [m_triangleCount * 3];
		m_pTextureCoordArray = new TextureCoordinate [m_triangleCount * 3];
	//	m_pInterleavedVertexArray = new VertexArrayElement [m_triangleCount * 3];

		for(int j = 0; j < m_triangleCount; j++)
		{
			int triangleIndex = m_pTriangleIndices[j];
			for(int vertexNumber = 0; vertexNumber < 3; vertexNumber++)
			{
	//			m_pTriangleVertexIndices[j*3+vertexNumber] = pTriangles[ triangleIndex ].m_vertexIndices[ vertexNumber ];
				//m_pVertexArray[j*3+vertexNumber] = pVertices[ pTriangles[ triangleIndex ].m_vertexIndices[ vertexNumber ] ];
				memcpy(&m_pVertexArray[j*3+vertexNumber], &pVertices[ pTriangles[ triangleIndex ].m_vertexIndices[ vertexNumber ] ], sizeof(Vertex));
				memcpy(&m_pNormalArray[j*3+vertexNumber].m_normal, &pTriangles[ triangleIndex ].m_vertexNormals[ vertexNumber ], sizeof(float)*3);
				m_pTextureCoordArray[j*3+vertexNumber].m_s = pTriangles[ triangleIndex ].m_s[vertexNumber];
				m_pTextureCoordArray[j*3+vertexNumber].m_t = pTriangles[ triangleIndex ].m_t[vertexNumber];
				
	//			memcpy(&m_pInterleavedVertexArray[j*3+vertexNumber].vertex, &pVertices[ pTriangles[ triangleIndex ].m_vertexIndices[ vertexNumber ] ], sizeof(Vertex));
	//			memcpy(&m_pInterleavedVertexArray[j*3+vertexNumber].normal.m_normal, &pTriangles[ triangleIndex ].m_vertexNormals[ vertexNumber ], sizeof(float)*3);
	//			m_pInterleavedVertexArray[j*3+vertexNumber].textureCoordinate.m_s = pTriangles[ triangleIndex ].m_s[vertexNumber];
	//			m_pInterleavedVertexArray[j*3+vertexNumber].textureCoordinate.m_t = pTriangles[ triangleIndex ].m_t[vertexNumber];

			}
		}
	}
//	void Mesh::DeleteVertexIndexBuffer()
//	{
//		if( m_pTriangleVertexIndices )
//			delete [] m_pTriangleVertexIndices;
//		m_pTriangleVertexIndices = 0;
//	}
	void Mesh::DeleteVertexArrays()
	{
		if( m_pVertexArray )
			delete [] m_pVertexArray;
		m_pVertexArray = 0;
		if( m_pNormalArray )
			delete [] m_pNormalArray;
		m_pNormalArray = 0;
		if( m_pTextureCoordArray )
			delete [] m_pTextureCoordArray;
		m_pTextureCoordArray = 0;
//		if ( m_pInterleavedVertexArray )
//			delete [] m_pInterleavedVertexArray;
//		m_pInterleavedVertexArray = 0;
	}

CCalModel::CCalModel():
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

CCalModel::~CCalModel()
{
	FreeMemory();
}

CCalModel::CCalModel(const CCalModel& rhs):
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
void CCalModel::Render(CCamera& camera)
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

void CCalModel::CopyFrom(const CCalModel& rhs)
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

	m_displayList = rhs.m_displayList;
	m_displayListUpdated = rhs.m_displayListUpdated;
	worldMatrix = rhs.worldMatrix;
}


void CCalModel::FreeMemory()
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

CCalModel CCalModel::operator=(const CCalModel& rhs)
{
	if(&rhs == this)
		return *this;

	FreeMemory();

	CopyFrom(rhs);
	return *this;
}*/
