/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#include "CCamera.h"
//#include "sys_main.h"
#include "CTimer.h"

#define CCAM_PI ((float)3.1415926535897)

	


CCamera::CCamera()
{
//	m_transformation = CMatrix4x4::mtfIdentity();
	m_transitionType = none;
	m_transitionSpeed = 1.0f;
	m_transitionStart = CMatrix4x4::mtfIdentity();
	m_transitionEnd = CMatrix4x4::mtfIdentity();
	m_transitionProgress = 0.;
	m_transitionInProgress = false;

	m_scale = CMatrix4x4::mtfIdentity();
	m_isTracking = false;
	m_posFromObj = 0;
}
CCamera::CCamera(const CCamera& rhs)
{
//	m_transformation = rhs.m_transformation;
	m_transitionType = rhs.m_transitionType;
	m_transitionSpeed = rhs.m_transitionSpeed;
	m_transitionStart = rhs.m_transitionStart;
	m_transitionEnd = rhs.m_transitionEnd;
	m_transitionProgress = rhs.m_transitionProgress;
	m_transitionInProgress = rhs.m_transitionInProgress;
	m_scale = rhs.m_scale;
	m_isTracking = rhs.m_isTracking;
	m_spFocusObj = rhs.m_spFocusObj;
	m_posFromObj = rhs.m_posFromObj;
}
CCamera::~CCamera()
{
}

void CCamera::Think()// This function MUST be called to update camera transitions. however, it is not required when transition is "none"
{
	if (this->m_isTracking)
	{
	//	this->EndTransition();
		this->SetTransformationGoal(CMatrix4x4::mtfTranslate(this->m_posFromObj) /** this->m_spFocusObj->GetRelativeMatrix()*/);
		this->LookAt(this->GetCameraTarget());
	}
	if(m_transitionInProgress)
		ThinkTransition();
}

void CCamera::StartTracking(CSmartPointer<CGraphNode> obj, float dist)
{
	this->RemoveFromGraph();
	obj->AttachChild(CSmartPointer<CGraphNode>(this));
	CGraphNode::SetFrameOfReference(obj);

	this->m_spFocusObj = obj;
	
	this->EndTransition();
//	this->SetTransitionType(CCamera::TransitionTypes::none);
//	this->SetTransitionSpeed(10);
	this->m_isTracking = true;
	
	this->m_posFromObj = CV3d(0, dist/4, dist);
	this->SetTransformationGoal(( /*RotationMatrix( CV3d(0, -90+180*(float)rand()/(float)RAND_MAX, 0)   ) * */CMatrix4x4::mtfTranslate(this->m_posFromObj)) /** this->m_spFocusObj->GetRelativeMatrix()*/);
	this->LookAt(this->GetCameraTarget());
}

void CCamera::StopTracking(void)
{
	this->m_isTracking = false;
	this->m_spFocusObj.Release();
}

CV3d CCamera::GetCameraTarget(void)
{
	return this->m_spFocusObj->GetRelativeMatrix().getTranslation();
}

void CCamera::SetTransformationGoal(CMatrix4x4 transformation)
{ 
	if(m_transitionInProgress)
	{
		ThinkTransition();
	}
	BeginTransition(m_localMatrix, transformation); 
	ThinkTransition();
}
	// make this function so it will update m_transformation if the transition mode is "none"
void CCamera::ApplyTransformationToGoal( CMatrix4x4 transformation ) 
{ 
	if(m_transitionInProgress)
	{
		ThinkTransition();
	}
	BeginTransition(m_localMatrix, transformation * m_transitionEnd);
}

void CCamera::Orbit(float xdelta, float ydelta)
{
	if (this->m_transitionInProgress)
		return;

	// before relative shit
	/*
	CMatrix4x4 cameraMatrix = this->GetTransformationGoal();
	CMatrix4x4 targetMatrix = CMatrix4x4::mtfTranslate(this->GetCameraTarget()); // to match objects orientation, use whole transformation
//	CMatrix4x4 targetToCameraMatrix = targetMatrix.getInverse() * cameraMatrix;
	CV3d targetToCameraVector = cameraMatrix.getTranslation() - targetMatrix.getTranslation();
	CV3d yaxis = CV3d(0,1,0);
	CV3d xaxisRotated = targetToCameraVector.cross(yaxis).getUnitVector();

	// in order to rotate on the x axis last, rotations will need to be applied in a different order
	CMatrix4x4 rotationMatrix = CMatrix4x4::mtfRot( CV3d(0, xdelta, 0) )* CMatrix4x4::mtfRotateOnAxis(xaxisRotated, ydelta);
	this->EndTransition();
	this->SetTransitionType(CCamera::none);
	this->m_posFromObj = rotationMatrix * this->m_posFromObj;
	this->SetTransformationGoal(targetMatrix * 
								//targetToCameraMatrix.getTransform() *
								rotationMatrix* //RotationMatrix(CV3d(0, xdelta, 0)) * 
								//targetToCameraMatrix.getTransform().getInverse() *
								targetMatrix.getInverse() * 
								cameraMatrix );*/

	CV3d targetToCameraVector = this->m_posFromObj;//cameraMatrix.getTranslation() - targetMatrix.getTranslation();
	CV3d yaxis = CV3d(0,1,0);
	CV3d xaxisRotated = targetToCameraVector.cross(yaxis).getUnitVector();
	CMatrix4x4 rotationMatrix = CMatrix4x4::mtfRot( CV3d(0, xdelta, 0) )* CMatrix4x4::mtfRotateOnAxis(xaxisRotated, ydelta);
	this->m_posFromObj = rotationMatrix * this->m_posFromObj;
	this->SetTransformationGoal(rotationMatrix * this->GetTransformationGoal());

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

void CCamera::SetScaleMatrix(CMatrix4x4& scale)
{
	m_scale = scale;
}


void CCamera::LookAt(CV3d target) // modifies the camera goal x and y angles to view a specific point
{
	CV3d cameraPosition(m_transitionEnd.getTranslation());
	CV3d cameraVector = cameraPosition - target;
	CV3d rotation = CV3d( -180/CCAM_PI * atan( cameraVector.y/sqrt(cameraVector.x*cameraVector.x + cameraVector.z*cameraVector.z)),
						((cameraVector.x < 0) ? -1 : 1) * 90 - 180/CCAM_PI *atan( cameraVector.z/cameraVector.x ),
						0 );
	if(cameraVector.x == 0 && cameraVector.z == 0)
		rotation.y = 0;
	SetTransformationGoal(CMatrix4x4::mtfTranslate(cameraPosition) * CMatrix4x4::mtfRot( rotation ));
}

CV3d CCamera::WorldToCameraSpace(const CV3d& rhs) // transform a point from world to camera space
{
	return m_localMatrix.getTransform() * (rhs - m_localMatrix.getTranslation());
}

void CCamera::BeginTransition(CMatrix4x4 start, CMatrix4x4 end)
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
			m_localMatrix = NoneTransitionEvaluator(m_transitionStart, m_transitionEnd, m_transitionProgress);
		}
		break;
	case linear:
		{
			m_localMatrix = LinearTransitionEvaluator(m_transitionStart, m_transitionEnd, m_transitionProgress);
		}
		break;
	case average:
		{
			m_localMatrix = AverageTransitionEvaluator(m_transitionStart, m_transitionEnd, m_transitionProgress);
		}
		break;
	}
	if(m_transitionProgress >= 1.)
	{
		EndTransition();
	}
}

CMatrix4x4 CCamera::AverageTransitionEvaluator(CMatrix4x4 start, CMatrix4x4 end, float x) // x is from 0 to 1
{
	m_transitionProgress += 5*(1.01- (double)fabs((double)(m_transitionProgress-.5))*2) * CTimer::Instance()->GetLastFrameDuration() * m_transitionSpeed;
	if(m_transitionProgress > 1.)
		m_transitionProgress = 1.;
	CMatrix4x4 translationMatrix = CMatrix4x4::mtfTranslate(start.getTranslation())*(1. - x) + CMatrix4x4::mtfTranslate(end.getTranslation())*(x);
	CMatrix4x4 result = end;
	result.setTranslation(translationMatrix.getTranslation());
	return result;
}

CMatrix4x4 CCamera::LinearTransitionEvaluator(CMatrix4x4 start, CMatrix4x4 end, float x) // x is from 0 to 1
{
	m_transitionProgress += CTimer::Instance()->GetLastFrameDuration() * m_transitionSpeed;
	if(m_transitionProgress > 1.)
		m_transitionProgress = 1.;
	CMatrix4x4 translationMatrix = CMatrix4x4::mtfTranslate(start.getTranslation())*(1. - x) + CMatrix4x4::mtfTranslate(end.getTranslation())*(x);
	CMatrix4x4 result = end;
	result.setTranslation(translationMatrix.getTranslation());
	return result;
}
CMatrix4x4 CCamera::NoneTransitionEvaluator(CMatrix4x4 start, CMatrix4x4 end, float x)
{
	m_transitionProgress = 1.;
	return end;//x > 0 ? end : start;
}

