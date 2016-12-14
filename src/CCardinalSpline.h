#ifndef CCARDINALSPLINE_H
#define CCARDINALSPLINE_H

#include "CCubicBezierCurve.h"
#include "math_main.h"
#include <vector>
using namespace std;

// the continuous spline uses bezier curves between 2 points
// the only thing defined is the alpha value, which affects the smoothness of the curve,
// and the points the spline will pass through, and optionally the derivative at 
// the beginning and end of the spline

class CCardinalSpline
{
public:
	CCardinalSpline();
	CCardinalSpline(const CCardinalSpline& rhs);
	~CCardinalSpline();

	void AddVertex(const v3d& vertex);
	void RemoveVertex(const unsigned int& vertexNumber);
	void ChangeVertex(const unsigned int& vertexNumber, const v3d& vertex);
	v3d GetVertex(const unsigned int& vertexNumber);
	unsigned int GetSize();

	void CalculateControlPoints(bool includeEndPoints = true); // calculate the middle two points of each bezier curve
	void SetEntryVector(v3d vector);
	void SetExitVector(v3d vector);
	void SetAlpha(float alpha);
	void CalculateControlPointsAt(const unsigned int& vertexNumber);
	void CalculateLength();
	float GetLength() { return m_length; }

	v3d Evaluate(const float& t); // t is from 0 to 1;
	CCardinalSpline operator=(const CCardinalSpline& rhs);

	void CopyFrom(const CCardinalSpline& rhs);
private:
	float m_alpha;
	float m_length;
	unsigned int numVerticies;
	vector<CCubicBezierCurve> m_spline; 
	vector<float> m_splineLengths;
};


#endif