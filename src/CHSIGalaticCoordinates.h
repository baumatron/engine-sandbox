#ifndef CHSIRELATIVECOORDINATES_H
#define CHSIRELATIVECOORDINATES_H

#include "ILookAtMe.h"

class CHSIFrameOfReference: public ILookAtMe
{
public:
	virtual matrix4x4 GetWorldMatrix(void)
	{ return m_worldMatrix; }
	void SetWorldMatrix(const matrix4x4 newMatrix)
	{ m_worldMatrix = newMatrix; }
private:
	matrix4x4 m_worldMatrix;
}

class CHSIRelavtiveCoordinate
{
public:
	CHSIRelativeCoordinate();
	~CHSIRelativeCoordinate();

	void ChangeReferenceFrame(CHSIFrameOfReference* newReferenceFrame);
	/*
		ChangeReferencePoint
			If there is an existing reference point, this function changes reference points
			while automatically adjusting the local coordinates

			If there is no existing reference point, the local coordinates are unchanged
	*/
	bool SameReferenceFrame(const CHSIRelativeCoordinate& rhs);
	/*
		compares the reference objects of this and another object, and if they are the same,
		returns true
	*/

	matrix4x4 getLocalMatrix(); // returns only coordinate in local frame
	matrix4x4 getWorldMatrix(); // combines local and reference coordinates to get an absolute coordinate value
private:
	matrix4x4 m_localMatrix;
	CHSIFrameOfReference* m_frameOfReference; // the origin for the relative coordinate system
};

#endif