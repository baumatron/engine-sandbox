/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#ifndef CCAMERA_H
#define CCAMERA_H

#include "ILookAtMe.h"
//#include "math_main.h"
#include "CV3d.h"
#include "CMatrix4x4.h"
#include "CGraphNode.h"

class CCamera: public CGraphNode
{
public:
	enum TransitionTypes {none, average, linear};

	CCamera();
	CCamera(const CCamera& rhs);
	~CCamera();

	void Think(); // This function MUST be called to update camera transitions. however, it is not required when transition is "none"
	
	// Object Tracking
	void StartTracking(CSmartPointer<CGraphNode> obj, float dist);
	void StopTracking(void);
	CV3d GetCameraTarget(void);
	
	// Camera movement
	void LookAt(CV3d target); // modifies the camera GOAL x and y angles to view a specific point
	void Orbit(float xdelta, float ydelta);

	void SetScaleMatrix(CMatrix4x4& scale);

	void SetTransitionType(TransitionTypes transitionType) { m_transitionType = transitionType; }
	void SetTransitionSpeed(float speed) { m_transitionSpeed = speed; }
	TransitionTypes GetTransitionType() { return m_transitionType; }
	float GetTransitionSpeed() { return m_transitionSpeed; }

	void SetTransformationGoal(CMatrix4x4 transformation);	// make this function so it will update m_transformation if the transition mode is "none"
	void ApplyTransformationToGoal( CMatrix4x4 transformation );
	CMatrix4x4 GetTransformationGoal() { return m_transitionEnd; }
	CMatrix4x4 GetTransformation() { return m_localMatrix; }
	CMatrix4x4 GetModelViewMatrix() { return /*m_scale **/ /*m_transformation.getInverse();*/ this->GetRelativeMatrix().getInverse(); }
		CV3d WorldToCameraSpace(const CV3d& rhs); // transform a point from world to camera space

	void RotateLocal(float deg, float x, float y, float z);
	void RotateGlobal(float deg, float x, float y, float z);

	CSmartPointer<CGraphNode> m_spFocusObj;
	virtual void PrintGraph(int& spaces)
	{
		for(int i = 0; i < spaces; i++)
		{
			bacon::cout << " ";
		}
		bacon::cout << "CCamera at " << this->GetRelativeMatrix().getTranslation() << '\n';
		spaces++;
		for(vector< CSmartPointer<CGraphNode> >::iterator it = m_children.begin(); it != m_children.end(); it++)
		{
			(*it)->PrintGraph(spaces);
		}
		spaces--;
	}
private:
	void BeginTransition(CMatrix4x4 start, CMatrix4x4 end);
	void ThinkTransition();
	void EndTransition();
	CMatrix4x4 AverageTransitionEvaluator(CMatrix4x4 start, CMatrix4x4 end, float x); // x is from 0 to 1
	CMatrix4x4 LinearTransitionEvaluator(CMatrix4x4 start, CMatrix4x4 end, float x); // x is from 0 to 1
	CMatrix4x4 NoneTransitionEvaluator(CMatrix4x4 start, CMatrix4x4 end, float x);
	CMatrix4x4 m_transitionStart;
	CMatrix4x4 m_transitionEnd;
	float m_transitionProgress;
	bool m_transitionInProgress;
	CMatrix4x4 m_scale;
	
	bool m_isTracking;
	CV3d m_posFromObj;
	//CMatrix4x4 m_transformation;
	//CMatrix4x4 m_transformationGoal;
	TransitionTypes m_transitionType;
	float m_transitionSpeed;
};

#endif
