#include "CShip.h"
//#include "CMilkshapeModel.h"
#include "math_main.h"
#include "sys_main.h"
#include "CVideoSubsystem.h"
#include "CCamera.h"
#include "CModel.h"
#include "CHSISubsystem.h"

CShip::CShip()
{
	m_pathProgress = 0;
	m_mass = 10;
	m_maxThrust = 1000000;
	m_topSpeed = 500;
	m_velocity = 0;

	m_vector = v3d(0,0,1);
	m_thrust = 0;
	m_vectorDeltaAccumulator = 0;
	m_owner = 0;
}
CShip::~CShip()
{
	//if(m_model)
	//	CCalCoreModelRepository::GetInstance()->ReleaseModelInstance(m_model);
}
CShip::CShip(const CShip& rhs)
{
	CopyFrom(rhs);
}


void CShip::SetPath(CCardinalSpline path)
{
	m_path = path;
}

void CShip::Initialize(string modelFilename)
{
	m_model = CModelRepository::Instance()->GetModelInstance(modelFilename);//CCalCoreModelRepository::GetInstance()->GetModelInstance(modelFilename);
	// models
//	CCalModel theModel;
//	CMilkShapeModelLoader::GetInstance()->LoadModel(theModel, "supercarrier.ms3d");
//	this->SetModel(theModel);
//	m_modelLow = m_model;
}
void CShip::Think()
{
	return;
	/*
	Accelerate();

	ApplyAcceleration();

	v3d path = m_path.Evaluate(m_pathProgress/m_path.GetLength());
	v3d pathDelta = path - m_model.worldMatrix.getTranslation();
	v3d pathDirection = m_path.Evaluate(m_pathProgress/m_path.GetLength() + 0.01/m_path.GetLength()) - path;
	pathDirection.makeUnitVector();

	// calculate how much the ship should roll
	// this is to make the ship appear like its flying
	// to do this, find out how much the ship turns, relative to it's coordinate system in the x-y plane
	// simply rotate the model so it's top points directly at this point
	// this angle will be 
	matrix4x4 toModelSpace = m_model.worldMatrix.getInverse();
	v3d modelSpaceDelta = toModelSpace * path;
	//m_vectorDeltaAccumulator += modelSpaceDelta;
//	float rollAngle;
	matrix4x4 rollMatrix;
	//static float rollTimer = 0;
	//if(rollTimer < 0)
	//{
	//	m_vectorDeltaAccumulator.makeUnitVector();
		modelSpaceDelta.makeUnitVector();
		if(modelSpaceDelta.y)
			m_rollAngle = (180 / pi) * atan((double)modelSpaceDelta.x/(double)fabs(modelSpaceDelta.y));
		else
			m_rollAngle = 0;//m_vectorDeltaAccumulator.x;
		v3d dist = modelSpaceDelta;
		dist.z = 0;
//		if(dist.distance(0) > 0.01)
//			rollMatrix = matrix4x4::mtfRot(v3d(0,0,m_rollAngle-m_lastRoll));
		//ccout << "m_rollAngle: " << m_rollAngle << newl;
		//ccout << "modelSpaceDelta.y: " << modelSpaceDelta.y << newl;
		//ccout << "modelSpaceDelta.x: " << modelSpaceDelta.x << newl;
		//ccout << "modelSpaceDelta.x/modelSpaceDelta.y: " << modelSpaceDelta.x/modelSpaceDelta.y << newl;
		m_lastRoll = m_rollAngle;
//		m_vectorDeltaAccumulator = 0;
	//}
	//rollTimer -= sys_frameTime;
	//if(modelSpaceDelta.y < 0.00001)
	//	rollMatrix = matrix4x4::mtfIdentity();
	//if(modelSpaceDelta.x < 0.00001)
	//	rollMatrix = matrix4x4::mtfIdentity();
	// now, create a matrix to rotate the model onto this direction vector

	v3d rotationAxis = m_vector.cross(pathDirection); // axis = Vs X Vf
	float rotationAngle = (180/pi) * acos(m_vector.dot(pathDirection));
	matrix4x4 rotationMatrix;
//	matrix4x4 rollMatrix;
	if(rotationAxis.distance(0))
	{
		rotationAxis.makeUnitVector();
	//	float bankAngle = sin();
		rotationMatrix = matrix4x4::mtfRotateOnAxis(rotationAxis, rotationAngle);
	//	rollMatrix = matrix4x4::mtfRot(v3d(0,0,rotationAngle));
		//if(rotationAxis.y > 0.1)
		//	rotationMatrix *= matrix4x4::mtfRotateOnAxis(m_vector, rotationAngle*5);
		//if(rotationAxis.y < 0.1)
		//	rotationMatrix *= matrix4x4::mtfRotateOnAxis(m_vector, -rotationAngle*5);
	}
	else
		rotationMatrix = matrix4x4::mtfIdentity();
//	m_model.worldMatrix = 
	m_model.worldMatrix = TranslationMatrix(pathDelta) * TranslationMatrix(m_model.worldMatrix.getTranslation()) * rotationMatrix * TranslationMatrix(m_model.worldMatrix.getTranslation()*-1) * m_model.worldMatrix * rollMatrix;
	worldMatrixNoRoll = TranslationMatrix(pathDelta) * TranslationMatrix(m_model.worldMatrix.getTranslation()) * rotationMatrix * TranslationMatrix(m_model.worldMatrix.getTranslation()*-1) * m_model.worldMatrix;
	if(m_pathProgress > m_path.GetLength())
		m_pathProgress = 0;

	m_vector = pathDirection;*/
}
void CShip::Accelerate()
{
	m_thrust = m_maxThrust;// * curveFactor;
}
void CShip::Decelerate()
{
	m_thrust = -m_maxThrust;
}
void CShip::ApplyAcceleration()
{
	float acceleration = m_thrust / m_mass;
	m_velocity = m_velocity + acceleration * sys_frameTime;
	if(m_velocity > m_topSpeed)
		m_velocity = m_topSpeed;
	if(-m_velocity > m_topSpeed)
		m_velocity = -m_topSpeed;
	m_pathProgress += m_velocity*sys_frameTime;
}
void CShip::SetOrientationTangentToPath()
{

}
matrix4x4 CShip::GetWorldMatrix(void)
{
	return m_worldMatrix;
}
void CShip::SetWorldMatrix(matrix4x4& matrix)
{
	m_worldMatrix = matrix;
}
void CShip::Draw(CCamera& camera)
{
	////glEnable(GL_LIGHTING);
	//// set up the light for the star
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	//// Create light components
	//float ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//float diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//// Assign created components to GL_LIGHT0
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.25f);

	//matrix4x4 worldMatrix = this->GetWorldMatrix();
	//float lightRadius = m_model->FindMaximumRadius();//planets[i].GetRadius() * 4;
	//v3d starVectorSystemSpace = /*planets[0].GetWorldMatrix()*/Hsi.galaxy.GetPlanetLocation(0,0).getTranslation()-worldMatrix.getTranslation();//planets[i].GetWorldMatrix().getTranslation();
	//starVectorSystemSpace.makeUnitVector();
	//v3d lightPosition = worldMatrix.getTranslation() + (starVectorSystemSpace*lightRadius);

	//// set up the projection for the lighting position
	//glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	//glLoadIdentity();

	//if(Video.settings.getSh())
	//	gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 5000000.0f);

	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadIdentity();
	//// set up model view matrix with the camera
	//glLoadMatrixf((camera.GetTransformation().getInverse()).matrix);
	//
	//// set the light position
	//float position[] = { lightPosition.x, lightPosition.y, lightPosition.z, 1.0f };
	//glLightfv(GL_LIGHT0, GL_POSITION, position);
	//glMatrixMode(GL_PROJECTION);
	//glPopMatrix();

	//glMatrixMode(GL_MODELVIEW);
	//glPopMatrix();

	//m_model->Render(camera, m_worldMatrix);


	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glLoadIdentity();
	// set up model view matrix with the camera
	glLoadMatrixf((camera.GetModelViewMatrix() * this->m_worldMatrix).matrix);
	
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.75f, 0.0f);
		glVertex3f(0.0f, 0.0f, 50.0f);
		glVertex3f(25.0f, 0.0f, -50.0f);
		glVertex3f(-25.0f, 0.0f, -50.0f);
		glVertex3f(0.0f, 0.0f, 50.0f);
		glVertex3f(0.0f, 25.0f, -50.0f);
		glVertex3f(0.0f, -25.0f, -50.0f);
	glEnd();

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
void CShip::CopyFrom(const CShip& rhs)
{
	m_path = rhs.m_path;
	m_pathProgress = rhs.m_pathProgress;
	m_mass = rhs.m_mass;
	m_thrust = rhs.m_thrust;
	m_maxThrust = rhs.m_maxThrust;
	m_topSpeed = rhs.m_topSpeed;
	m_velocity = rhs.m_velocity;
	m_vector = rhs.m_vector;
	m_lastVector = rhs.m_lastVector;
	m_lastRoll = rhs.m_lastRoll;
	m_rollAngle = rhs.m_rollAngle;
	m_vectorDeltaAccumulator = rhs.m_vectorDeltaAccumulator;
	worldMatrixNoRoll = rhs.worldMatrixNoRoll;
	m_owner = rhs.m_owner;
	//if(m_model)
	//	CCalCoreModelRepository::GetInstance()->ReleaseModelInstance(m_model);
	//m_model = CCalCoreModelRepository::GetInstance()->GetModelInstance(rhs.m_model->GetFilename());
	m_model = CSmartPointer<CModel>(new CModel(*rhs.m_model), false);//new CModel(*rhs.m_model.GetPointer());
	//(*m_model).(*(rhs.m_model));
	m_worldMatrix = rhs.m_worldMatrix;
}

CShip CShip::operator=(const CShip& rhs)
{
	if(&rhs != this)
		CopyFrom(rhs);
	return *this;
}

/*
CCalModel m_model;
CCardinalSpline m_path;
float m_pathProgress;
float m_mass;
float m_maxThrust;
float m_topSpeed;*/