/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#ifndef BANEMATH_H
#define BANEMATH_H

#include "CV3d.h"
/*
class math_Circle
{
public:
	math_Circle
}
*/
namespace BaneMath
{

class CSphere
{
public:
	CSphere();
	~CSphere();

	void SetPosition(const CV3d& position);
	void SetRadius(const float radius);

	CV3d GetPosition() const;
	float GetRadius() const;

	CV3d IntersectWith(const CSphere& rhs) const;
private:
	CV3d m_position;
	float m_radius;
};

class CRay
{
public:
	CRay();
	CRay(CV3d _start, CV3d _direction);
	~CRay();

	void SetStart(const CV3d& _start);
	void SetDirection(const CV3d& _direction);

	friend class CPlane;
//	friend con_Cout& operator<<(con_Cout& out, const CRay& ray);
protected:
	CV3d m_start;
	CV3d m_direction;
};

class CLine: public CRay
{
//	friend con_Cout& operator<<(con_Cout& out, const CLine& line);
};

class CLineSegment: public CRay
{
public:
	CLineSegment();
	CLineSegment(CV3d _p1, CV3d _p2);
	~CLineSegment();

	void DefineLineSegment(CV3d _p1, CV3d _p2);
	void SetP1(CV3d _p1);
	void SetP2(CV3d _p2);

	friend class CPlane;

//	friend con_Cout& operator<<(con_Cout& out, const CLineSegment& lineseg);
private:
	float m_length;
	CV3d m_end;
};


class CPlane
{
public:
	CPlane(); // 
	CPlane(CV3d p1, CV3d p2);		// creates a line
	CPlane(CV3d p1, CV3d p2, CV3d p3);	// creates a 3 dimensional plane
	~CPlane();

	void DefinePlane(CV3d p1, CV3d p2);	// sets the normal and d based on 3 points on the plane
	void DefinePlane(CV3d p1, CV3d p2, CV3d p3);	// sets the normal and d based on 3 points on the plane
	CV3d GetNormal();

	CV3d IntersectWith(const CRay& ray) const;
	CV3d IntersectWith(const CLine& line) const;
	CV3d IntersectWith(const CLineSegment& line) const;

	float Distance(const CV3d& point) const;
	float Distance(const CSphere& sphere) const;
private:
	CV3d m_normal;
	float m_d;
};

class CExtrudedLineSegment	// this is actually a 3 dimensional object
						// it is a plane with 2 points that are bounding a line segment where that plane intersects the x,y plane
						// should this be represented as p = p0 + direction * t ????? -> uses this equation for part of intersectWith function, should it use only this equation
						// p = p0 + direction * t = R0 + RDirection * Rt = Rp   lineEquation = rayEquation
						//	solve where t = Rt, resulting point should be point of intersection
{
public:
	CExtrudedLineSegment();
	~CExtrudedLineSegment();

	void DefineLineSegment(CV3d _p1, CV3d _p2);
//	void defineLineSegment(CV3d p1, CV3d p2, CV3d p3);
	void SetP1(CV3d _p1);
	void SetP2(CV3d _p2);
	CV3d GetP1();
	CV3d GetP2();
	CV3d GetNormal();

	CV3d IntersectWith(const CRay& ray) const;
	CV3d IntersectWith(const CLine& line) const;
	CV3d IntersectWith(const CLineSegment& line) const;

//	friend con_Cout& operator<<(con_Cout& out, const CExtrudedLineSegment& extlineseg);
private:
	CV3d m_p1, m_p2;	// these must satisfy the plane equation
	CPlane m_plane;
};

/*
class CBSPTree	// designed for static objects
{
public:
	CBSPTree();
	CBSPTree(vid_SceneObjectHandleList* source);
	~CBSPTree();

	void buildBSPTree(vid_SceneObjectHandleList* source, CPlane partition);

	CPlane partition;
	vid_SceneObjectHandleList* objectList;	// this is a pointer to appease the compiler (don't have to include the Video.main.h header)
	CBSPTree *front, *back;
};*/

}

/*

con_Cout& operator<<(con_Cout& out, const CRay& ray);
con_Cout& operator<<(con_Cout& out, const CLine& line);
con_Cout& operator<<(con_Cout& out, const CLineSegment& lineseg);
con_Cout& operator<<(con_Cout& out, const CExtrudedLineSegment& extlineseg);
*/
#endif