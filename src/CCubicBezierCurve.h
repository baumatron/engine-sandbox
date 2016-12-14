#ifndef CCUBICBEZIERCURVE_H
#define CCUBICBEZIERCURVE_H

#include "math_main.h"


class CCubicBezierCurve
{
public:
	CCubicBezierCurve();
	CCubicBezierCurve(const CCubicBezierCurve& rhs);
	~CCubicBezierCurve();

	v3d Evaluate(const float& t); // t is from 0 to 1

	void CalculateLength(const unsigned int iterations);

	float GetLength();
	void SetPoint(const unsigned int pointNumber, const v3d& value);
	v3d GetPoint(const unsigned int pointNumber);
private:
	v3d points[4];
	float length;
};

#endif