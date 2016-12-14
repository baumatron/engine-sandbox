#ifndef CSHIP_H
#define CSHIP_H

#include "CModel.h"
#include "CCardinalSpline.h"
#include "CCamera.h"
#include "ILookAtMe.h"
#include "CVideoSubsystem.h"
#include "ship.h" // prototype code

class CShip: public ILookAtMe
{
public:
	CShip();
	CShip(const CShip& rhs);
	~CShip();

	void SetPath(CCardinalSpline path);

	void Initialize(string modelFilename);
	void Think();
	void Draw(CCamera& camera);

	void Accelerate();
	void Decelerate();
	void ApplyAcceleration();
	void SetOrientationTangentToPath();
	matrix4x4 GetWorldMatrix(void);
	void SetWorldMatrix(matrix4x4& matrix);

	void CopyFrom(const CShip& rhs);

	CShip operator=(const CShip& rhs);

//	void SetModel(const CCalModel& model);

	CSmartPointer<CModel> m_model;//,
//			m_modelLow;
//	GLuint m_modelDisplayList;
	CCardinalSpline m_path;
	float m_pathProgress;
	float m_mass;
	float m_thrust;
	float m_maxThrust;
	float m_topSpeed;
	float m_velocity;
	v3d m_vector;
	v3d m_lastVector;
	float m_lastRoll;
	float m_rollAngle;
	v3d m_vectorDeltaAccumulator;
	matrix4x4 worldMatrixNoRoll;
	int m_owner;
private:
	matrix4x4 m_worldMatrix;
};

#endif