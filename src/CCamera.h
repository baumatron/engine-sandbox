#ifndef CCAMERA_H
#define CCAMERA_H

#include "ILookAtMe.h"
#include "math_main.h"

class CCamera
{
public:
	enum TransitionTypes {none, average, linear};

	CCamera();
	CCamera(const CCamera& rhs);
	~CCamera();

	void Think(); // This function MUST be called to update camera transitions. however, it is not required when transition is "none"
	
	// Object Tracking
	void StartTracking(ILookAtMe &obj, float dist);
	void StopTracking(void);
	v3d GetCameraTarget(void);
	
	// Camera movement
	void LookAt(v3d target); // modifies the camera GOAL x and y angles to view a specific point
	void Orbit(float xdelta, float ydelta);

	void SetScaleMatrix(matrix4x4& scale);

	void SetTransitionType(TransitionTypes transitionType) { m_transitionType = transitionType; }
	void SetTransitionSpeed(float speed) { m_transitionSpeed = speed; }
	TransitionTypes GetTransitionType() { return m_transitionType; }
	float GetTransitionSpeed() { return m_transitionSpeed; }

	void SetTransformationGoal(matrix4x4 transformation);	// make this function so it will update m_transformation if the transition mode is "none"
	void ApplyTransformationToGoal( matrix4x4 transformation );
	matrix4x4 GetTransformationGoal() { return m_transitionEnd; }
	matrix4x4 GetTransformation() { return m_transformation; }
	matrix4x4 GetModelViewMatrix() { return /*m_scale **/ m_transformation.getInverse(); }
		v3d WorldToCameraSpace(const v3d& rhs); // transform a point from world to camera space

	void RotateLocal(float deg, float x, float y, float z);
	void RotateGlobal(float deg, float x, float y, float z);

private:
	void BeginTransition(matrix4x4 start, matrix4x4 end);
	void ThinkTransition();
	void EndTransition();
	matrix4x4 AverageTransitionEvaluator(matrix4x4 start, matrix4x4 end, float x); // x is from 0 to 1
	matrix4x4 LinearTransitionEvaluator(matrix4x4 start, matrix4x4 end, float x); // x is from 0 to 1
	matrix4x4 NoneTransitionEvaluator(matrix4x4 start, matrix4x4 end, float x);
	matrix4x4 m_transitionStart;
	matrix4x4 m_transitionEnd;
	float m_transitionProgress;
	bool m_transitionInProgress;
	matrix4x4 m_scale;
	
	bool m_isTracking;
	ILookAtMe *m_focusObj;
	v3d m_posFromObj;
	matrix4x4 m_transformation;
	//matrix4x4 m_transformationGoal;
	TransitionTypes m_transitionType;
	float m_transitionSpeed;
};

#endif
