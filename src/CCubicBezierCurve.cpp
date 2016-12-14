
#include "CCubicBezierCurve.h"


CCubicBezierCurve::CCubicBezierCurve()
{
	points[0] = points[1] = points[2] = points[3] = v3d(0);
	length = 1.;
}

CCubicBezierCurve::CCubicBezierCurve(const CCubicBezierCurve& rhs)
{
	points[0] = rhs.points[0];
	points[1] = rhs.points[1];
	points[2] = rhs.points[2];
	points[3] = rhs.points[3];
	length = rhs.length;
}


CCubicBezierCurve::~CCubicBezierCurve()
{
}

v3d CCubicBezierCurve::Evaluate(const float& t) // t is from 0 to 1
{
	v3d result;
	// the cubic is: 1 = t^3 + 3t^2(1-t) + 3t(1-t)^2 + (1-t)^3
	float a = t*t*t;
	float b = 3*t*t*(1-t);
	float c = 3*t*(1-t)*(1-t);
	float d = (1-t)*(1-t)*(1-t);
	result.x = points[3].x*a + points[2].x*b + points[1].x*c + points[0].x*d;
	result.y = points[3].y*a + points[2].y*b + points[1].y*c + points[0].y*d;
	result.z = points[3].z*a + points[2].z*b + points[1].z*c + points[0].z*d;
	return result;
}

void CCubicBezierCurve::CalculateLength(const unsigned int iterations)
{
	float t = 0.;
	float delta = 1./(float)iterations;
	length = 0.;
	for( int i = 0; i < iterations; i++ )
	{
		v3d p1 = Evaluate( t );
		v3d p2 = Evaluate( t + delta );
		length += p1.distance(p2);
		t += delta;
	}
}

float CCubicBezierCurve::GetLength()
{
	return length;
}
void CCubicBezierCurve::SetPoint(const unsigned int pointNumber, const v3d& value)
{
	points[pointNumber] = value;
	CalculateLength(30);
}

v3d CCubicBezierCurve::GetPoint(const unsigned int pointNumber)
{
	return points[pointNumber];
}

