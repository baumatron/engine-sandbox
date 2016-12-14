#include "CCardinalSpline.h"

CCardinalSpline::CCardinalSpline():
	numVerticies(0),
	m_alpha(5),
	m_length(1)
{
}

CCardinalSpline::CCardinalSpline(const CCardinalSpline& rhs)
{
	CopyFrom(rhs);
}


CCardinalSpline::~CCardinalSpline()
{
}

void CCardinalSpline::AddVertex(const v3d& vertex)
{
	if(numVerticies == 0)
	{
		// first point to be added
		// don't do much, just make a new spline and assign the point value
		CCubicBezierCurve bcurve;
		bcurve.SetPoint(0, vertex);
		m_spline.push_back(bcurve);
	}
	else if(numVerticies == 1)
	{
		// just set the second point of the bcurve and calc control points
		m_spline[0].SetPoint(3, vertex);
		CalculateControlPointsAt(0);
		CalculateControlPointsAt(1);
	}
	else
	{
		// a new bcurve must be added
		CCubicBezierCurve bcurve;
		bcurve.SetPoint(0, m_spline[m_spline.size()-1].GetPoint(3));
		bcurve.SetPoint(3, vertex);
		m_spline.push_back(bcurve);
		CalculateControlPointsAt(m_spline.size());
		CalculateControlPointsAt(m_spline.size()-1);
	}
	CalculateLength();
	numVerticies++;
}
void CCardinalSpline::RemoveVertex(const unsigned int& vertexNumber)
{
	CalculateLength();
	//m_spline[vertexNumber/2].points[(vertexNumber%2 == 0) ? 0 : 3] = vertex;
}
void CCardinalSpline::ChangeVertex(const unsigned int& vertexNumber, const v3d& vertex)
{
	if(vertexNumber == 0)
	{
		//first
		m_spline[0].SetPoint(0, vertex);
	}
	else if(vertexNumber == GetSize()-1)
	{
		//last
		m_spline[m_spline.size()-1].SetPoint(3, vertex);
	}
	else
	{
		//middle
		m_spline[vertexNumber-1].SetPoint(3, vertex);
		m_spline[vertexNumber].SetPoint(0, vertex);
	}
	CalculateLength();
}
v3d CCardinalSpline::GetVertex(const unsigned int& vertexNumber)
{
	unsigned int curveNumber;
	unsigned int curveVertexNumber;
	if(vertexNumber == 0)
	{
		curveNumber = 0;
		curveVertexNumber = 0;
	}
	else 
	{
		curveNumber = vertexNumber - 1;
		curveVertexNumber = 3;
	}
	return m_spline[curveNumber].GetPoint(curveVertexNumber);
}
unsigned int CCardinalSpline::GetSize()
{
//	return numVerticies;
	return m_spline.size() + 1; // number of cubics + 1
}

void CCardinalSpline::CalculateControlPoints(bool includeEndPoints) // calculate the middle two points of each bezier curve
{
	unsigned int start, stop;
	if(includeEndPoints)
	{
		start = 0;
		stop = m_spline.size(); // index = number of cubics - 1 + 1
	}
	else
	{
		start = 1;
		stop = m_spline.size();
	}
	for(unsigned int i = start; i < stop; i++)
	{
		CalculateControlPointsAt(i);
	}
}
void CCardinalSpline::CalculateControlPointsAt(const unsigned int& vertexNumber)
{
	if(vertexNumber == 0)
	{
		// its the very first point
		v3d vector = (m_spline[0].GetPoint(3) - m_spline[0].GetPoint(0))/m_alpha;
		m_spline[0].SetPoint(1, m_spline[0].GetPoint(0) + vector);
	}
	else if(vertexNumber == m_spline.size()) // index = number of cubics - 1 + 1
	{
		// its the very last point
		v3d vector = (m_spline[m_spline.size()-1].GetPoint(0) - m_spline[m_spline.size()-1].GetPoint(3))/m_alpha;
		m_spline[m_spline.size()-1].SetPoint(2, m_spline[m_spline.size()-1].GetPoint(3) + vector);
	}
	else
	{
		// the point is somewhere in the middle
		// need to calculate 2 vectors, one for each bcurve that contains the point
		// *-------*-------*-----*-------*
		// 5 points, 4 curves
		// point 1 is in curve 0 and curve 1
		// point 3 is in curve 2 and curve 3
		// so curve a = point-1, curve b = point
		CCubicBezierCurve& curve1 = m_spline[vertexNumber-1];
		CCubicBezierCurve& curve2 = m_spline[vertexNumber];

		// start with the vector for the first curve
		// it should be parallel to the line between curve1.point[0] and curve2.point[3]
		v3d vector = (curve1.GetPoint(0) - curve2.GetPoint(3))/m_alpha;
		// curve1.points[3] is the point were calculating the control points for
		curve1.SetPoint(2, curve1.GetPoint(3) + vector);
		// curve2.points[0] is the same as curve1.points[3]
		// the vector for the second curve is just the opposite of the vector for the first
		curve2.SetPoint(1, curve2.GetPoint(0) - vector);
	}
	CalculateLength();
}
void CCardinalSpline::CalculateLength()
{
	m_length = 0;
	for( int i = 0; i < m_spline.size(); i++)
	{
		m_length += m_spline[i].GetLength();
	}
}


void CCardinalSpline::SetEntryVector(v3d vector)
{
	if(m_spline.size())
	{
		m_spline[0].SetPoint(1, m_spline[0].GetPoint(0) + vector);
	}
	CalculateLength();
}
void CCardinalSpline::SetExitVector(v3d vector)
{
	if(m_spline.size())
	{
		m_spline[m_spline.size()-1].SetPoint(2, m_spline[m_spline.size()-1].GetPoint(3) + vector);
	}
	CalculateLength();
}


void CCardinalSpline::SetAlpha(float alpha)
{
	m_alpha = alpha;
	CalculateControlPoints();
	CalculateLength();
}

v3d CCardinalSpline::Evaluate(const float& t) // t is from 0 to 1;
{
	// the curve number depends on the length of each curve
	float temp = 0;
	float curveStartT = 0;
	float curveEndT;
	unsigned int curveNumber;
	for(curveNumber = 0; curveNumber < m_spline.size(); curveNumber++)
	{
		curveStartT = temp;
		temp += m_spline[curveNumber].GetLength()/m_length;
		curveEndT = temp;
		if(temp >= t)
			break;
	}
	// now figure out how far along the curve this is...
	float curveT = (t - curveStartT)/(curveEndT-curveStartT);
	return m_spline[curveNumber].Evaluate(curveT);
}

CCardinalSpline CCardinalSpline::operator=(const CCardinalSpline& rhs)
{
	CopyFrom(rhs);
	return *this;
}


void CCardinalSpline::CopyFrom(const CCardinalSpline& rhs)
{
	for(int i = 0; i < rhs.m_spline.size(); i++)
	{
		m_spline.push_back(rhs.m_spline[i]);
	}
	m_alpha = rhs.m_alpha;
	numVerticies = rhs.numVerticies;
	m_length = rhs.m_length;
}
