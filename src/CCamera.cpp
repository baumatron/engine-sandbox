#include "CCamera.h"
#include "sys_main.h"

CCamera::CCamera()
{
	m_transformation = matrix4x4::mtfIdentity();
	m_transitionType = none;
	m_transitionSpeed = 1.0f;
	m_transitionStart = matrix4x4::mtfIdentity();
	m_transitionEnd = matrix4x4::mtfIdentity();
	m_transitionProgress = 0.;
	m_transitionInProgress = false;
	m_scale = matrix4x4::mtfIdentity();
}
CCamera::CCamera(const CCamera& rhs)
{
	m_transformation = rhs.m_transformation;
	m_transitionType = rhs.m_transitionType;
	m_transitionSpeed = rhs.m_transitionSpeed;
	m_transitionStart = rhs.m_transitionStart;
	m_transitionEnd = rhs.m_transitionEnd;
	m_transitionProgress = rhs.m_transitionProgress;
	m_transitionInProgress = rhs.m_transitionInProgress;
	m_scale = rhs.m_scale;
}
CCamera::~CCamera()
{
}

void CCamera::Think()// This function MUST be called to update camera transitions. however, it is not required when transition is "none"
{
	if (this->m_isTracking)
	{
	//	this->EndTransition();
		this->SetTransformationGoal(TranslationMatrix(this->m_posFromObj) * this->m_focusObj->GetWorldMatrix());
		this->LookAt(this->GetCameraTarget());
	}
	if(m_transitionInProgress)
		ThinkTransition();
}

void CCamera::StartTracking(ILookAtMe &obj, float dist)
{
	this->m_focusObj = &obj;
	
	this->EndTransition();
//	this->SetTransitionType(CCamera::TransitionTypes::none);
//	this->SetTransitionSpeed(10);
	this->m_isTracking = true;
	
	this->m_posFromObj = v3d(0, dist/4, dist);
	this->SetTransformationGoal(( /*RotationMatrix( v3d(0, -90+180*(float)rand()/(float)RAND_MAX, 0)   ) * */TranslationMatrix(this->m_posFromObj)) * this->m_focusObj->GetWorldMatrix());
	this->LookAt(this->GetCameraTarget());
}

void CCamera::StopTracking(void)
{
	this->m_isTracking = false;
	this->m_focusObj = NULL;
}

v3d CCamera::GetCameraTarget(void)
{
	return this->m_focusObj->GetWorldMatrix().getTranslation();
}

void CCamera::SetTransformationGoal(matrix4x4 transformation)
{ 
	if(m_transitionInProgress)
	{
		ThinkTransition();
	}
	BeginTransition(m_transformation, transformation); 
	ThinkTransition();
}
	// make this function so it will update m_transformation if the transition mode is "none"
void CCamera::ApplyTransformationToGoal( matrix4x4 transformation ) 
{ 
	if(m_transitionInProgress)
	{
		ThinkTransition();
	}
	BeginTransition(m_transformation, transformation * m_transitionEnd);
}

void CCamera::Orbit(float xdelta, float ydelta)
{
	if (this->m_transitionInProgress)
		return;
	
	matrix4x4 cameraMatrix = this->GetTransformationGoal();
	matrix4x4 targetMatrix = TranslationMatrix(this->GetCameraTarget()); // to match objects orientation, use whole transformation
//	matrix4x4 targetToCameraMatrix = targetMatrix.getInverse() * cameraMatrix;
	v3d targetToCameraVector = cameraMatrix.getTranslation() - targetMatrix.getTranslation();
	v3d yaxis = v3d(0,1,0);
	v3d xaxisRotated = targetToCameraVector.cross(yaxis).getUnitVector();

	// in order to rotate on the x axis last, rotations will need to be applied in a different order
	matrix4x4 rotationMatrix = RotationMatrix( v3d(0, xdelta, 0) )* matrix4x4::mtfRotateOnAxis(xaxisRotated, ydelta);
	this->EndTransition();
	this->SetTransitionType(CCamera::none);
	this->m_posFromObj = rotationMatrix/*RotationMatrix(v3d(0, xdelta, 0))*/ * this->m_posFromObj;
	this->SetTransformationGoal(targetMatrix * 
								//targetToCameraMatrix.getTransform() *
								rotationMatrix* //RotationMatrix(v3d(0, xdelta, 0)) * 
								//targetToCameraMatrix.getTransform().getInverse() *
								targetMatrix.getInverse() * 
								cameraMatrix );
}

/*
// Here we let OpenGl's (most likely quite optimized) functions do the work.
// Note that its transformations already are in local coords.
void CCamera::RotateLocal(float deg, float x, float y, float z)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(Transform);
	glRotatef(deg, x,y,z);
	glGetFloatv(GL_MODELVIEW_MATRIX, this->m_transformation);
	glPopMatrix();
}

// We have to invert the rotations to get the global axes in local coords.
// Luckily thats just the transposed in this case.
void Camera::RotateGlobal(float deg, float x, float y, float z)
{
	float dx=x*Transform[0] + y*Transform[1] + z*Transform[2];
	float dy=x*Transform[4] + y*Transform[5] + z*Transform[6];
	float dz=x*Transform[8] + y*Transform[9] + z*Transform[10];
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(Transform);
	glRotatef(deg, dx,dy,dz);
	glGetFloatv(GL_MODELVIEW_MATRIX, this->m_transformation);
	glPopMatrix();
}
*/

void CCamera::SetScaleMatrix(matrix4x4& scale)
{
	m_scale = scale;
}


void CCamera::LookAt(v3d target) // modifies the camera goal x and y angles to view a specific point
{
	v3d cameraPosition(m_transitionEnd.getTranslation());
	v3d cameraVector = cameraPosition - target;
	v3d rotation = v3d( -180/pi * atan( cameraVector.y/sqrt(cameraVector.x*cameraVector.x + cameraVector.z*cameraVector.z)),
						((cameraVector.x < 0) ? -1 : 1) * 90 - 180/pi *atan( cameraVector.z/cameraVector.x ),
						0 );
	if(cameraVector.x == 0 && cameraVector.z == 0)
		rotation.y = 0;
	SetTransformationGoal(TranslationMatrix(cameraPosition) * RotationMatrix( rotation ));
}

v3d CCamera::WorldToCameraSpace(const v3d& rhs) // transform a point from world to camera space
{
	return m_transformation.getTransform() * (rhs - m_transformation.getTranslation());
}

void CCamera::BeginTransition(matrix4x4 start, matrix4x4 end)
{
	m_transitionProgress = 0.;
	m_transitionStart = start;
	m_transitionEnd = end;
	m_transitionInProgress = true;
}

void CCamera::EndTransition()
{
	m_transitionInProgress = false;
	m_transitionProgress = 1.;
}

void CCamera::ThinkTransition()
{
	switch(m_transitionType)
	{
	case none:
		{
			m_transformation = NoneTransitionEvaluator(m_transitionStart, m_transitionEnd, m_transitionProgress);
		}
		break;
	case linear:
		{
			m_transformation = LinearTransitionEvaluator(m_transitionStart, m_transitionEnd, m_transitionProgress);
		}
		break;
	case average:
		{
			m_transformation = AverageTransitionEvaluator(m_transitionStart, m_transitionEnd, m_transitionProgress);
		}
		break;
	}
	if(m_transitionProgress >= 1.)
	{
		EndTransition();
	}
}

matrix4x4 CCamera::AverageTransitionEvaluator(matrix4x4 start, matrix4x4 end, float x) // x is from 0 to 1
{
	m_transitionProgress += 5*(1.01- (double)fabs((double)(m_transitionProgress-.5))*2) * sys_frameTime * m_transitionSpeed;
	if(m_transitionProgress > 1.)
		m_transitionProgress = 1.;
	matrix4x4 translationMatrix = TranslationMatrix(start.getTranslation())*(1. - x) + TranslationMatrix(end.getTranslation())*(x);
	matrix4x4 result = end;
	result.setTranslation(translationMatrix.getTranslation());
	return result;
}

matrix4x4 CCamera::LinearTransitionEvaluator(matrix4x4 start, matrix4x4 end, float x) // x is from 0 to 1
{
	m_transitionProgress += sys_frameTime * m_transitionSpeed;
	if(m_transitionProgress > 1.)
		m_transitionProgress = 1.;
	matrix4x4 translationMatrix = TranslationMatrix(start.getTranslation())*(1. - x) + TranslationMatrix(end.getTranslation())*(x);
	matrix4x4 result = end;
	result.setTranslation(translationMatrix.getTranslation());
	return result;
}
matrix4x4 CCamera::NoneTransitionEvaluator(matrix4x4 start, matrix4x4 end, float x)
{
	m_transitionProgress = 1.;
	return end;//x > 0 ? end : start;
}

