/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */



#include "BaneMath.h"
using namespace BaneMath;

CSphere::CSphere()
{
	m_radius = 1.0f;
}
CSphere::~CSphere()
{
}

void CSphere::SetPosition(const CV3d& position)
{
	m_position = position;
}
void CSphere::SetRadius(const float radius)
{
	m_radius = radius;
}

CV3d CSphere::GetPosition() const
{
	return m_position;
}
float CSphere::GetRadius() const
{
	return m_radius;
}

CV3d CSphere::IntersectWith(const CSphere& rhs) const
{
	if((m_radius + rhs.m_radius) >= m_position.distance(rhs.m_position))
	{
		CV3d directionVector = rhs.m_position - m_position;
		directionVector.makeUnitVector();
		directionVector *= m_radius;
		return m_position + directionVector;
	}
	return nothing;
}








///////////////////////////
//	CRay
///////////////////////////

CRay::CRay():
m_start(0),
m_direction(1)
{}

CRay::CRay(CV3d _start, CV3d _direction):
m_start(_start),
m_direction(_direction)
{}

void CRay::SetStart(const CV3d& _start)
{
	m_start = _start;
}
void CRay::SetDirection(const CV3d& _direction)
{
	m_direction = _direction;
}


CRay::~CRay(){}

///////////////////////////
//	end CRay
///////////////////////////

///////////////////////////
//	CLineSegment
///////////////////////////
CLineSegment::CLineSegment()
{
	DefineLineSegment(0, 1);
}
CLineSegment::CLineSegment(CV3d _p1, CV3d _p2)
{
	DefineLineSegment(_p1, _p2);
}

void CLineSegment::DefineLineSegment(CV3d _p1, CV3d _p2)
{
	m_start = _p1;
	m_end = _p2;
	m_direction = (m_end-m_start).getUnitVector();
	m_length = m_start.distance(m_end);
}

void CLineSegment::SetP1(CV3d p1)
{
	DefineLineSegment(p1, m_end);
}

void CLineSegment::SetP2(CV3d p2)
{
	DefineLineSegment(m_start, p2);
}

CLineSegment::~CLineSegment()
{
}

///////////////////////////
//	end CLineSegment
///////////////////////////

///////////////////////////
//	CPlane
///////////////////////////

CPlane::CPlane():
m_normal(1),
m_d(0)
{}

CPlane::CPlane(CV3d p1, CV3d p2)		// creates a line
{
	CV3d p3 = p1+CV3d(0,0,1); // makes the plane vertical, creating a line on the x,y axis
	DefinePlane(p1, p2, p3);
}

CPlane::CPlane(CV3d p1, CV3d p2, CV3d p3) // creates a 3 dimensional plane
{
	DefinePlane(p1, p2, p3);
}

CPlane::~CPlane(){}

void CPlane::DefinePlane(CV3d p1, CV3d p2)	// sets the normal and d based on 3 points on the plane
{
	CV3d p3 = p1+CV3d(0,0,1); // makes the plane vertical, creating a line on the x,y axis
	DefinePlane(p1, p2, p3);
}

void CPlane::DefinePlane(CV3d p1, CV3d p2, CV3d p3)	// sets the normal and d based on 3 points on the plane
{
	m_normal = (p2 - p1).cross(p3 - p1).makeUnitVector(); // a clockwise definition (looking at a triangle), defines the normal pointing back out of the screen
	m_d = m_normal.dot(p1);
}

CV3d CPlane::GetNormal()
{
	return m_normal;
}


CV3d CPlane::IntersectWith(const CRay& ray) const
{
	if(!m_normal.dot(ray.m_direction))	// does not intersect!
		// throw an exception??? no acceptable return value
		return nothing;
	float t = ( m_d - m_normal.dot(ray.m_start) ) / ( m_normal.dot(ray.m_direction) );
	if(t < 0) // does not intersect the ray! t cannot be negative
		return nothing;
		// throw exception
	return ray.m_start + ray.m_direction * t;
}

CV3d CPlane::IntersectWith(const CLine& line) const
{
	if(!m_normal.dot(line.m_direction))	// does not intersect!
		// throw an exception??? no acceptable return value
		return nothing;
	float t = ( m_d - m_normal.dot(line.m_start) ) / ( m_normal.dot(line.m_direction) );
	// doesn't matter if t is negative or not
	return line.m_start + line.m_direction * t;
}
CV3d CPlane::IntersectWith(const CLineSegment& lineSeg) const
{
	if(!m_normal.dot(lineSeg.m_direction))	// does not intersect!
		// throw an exception??? no acceptable return value
		return nothing;
	float t = ( m_d - m_normal.dot(lineSeg.m_start) ) / ( m_normal.dot(lineSeg.m_direction) );
	// doesn't matter if t is negative or not
	float tstart = 0; 
	float tend;
	if(lineSeg.m_direction.x != 0)
	/*if(fabs(lineSeg.direction.x) > 0.0001)*/
	{
		tend = (lineSeg.m_end.x - lineSeg.m_start.x)/lineSeg.m_direction.x;
	}
	else /*if(fabs(lineSeg.direction.y) > 0.0001)*/if(lineSeg.m_direction.y != 0)
	{
		tend = (lineSeg.m_end.y - lineSeg.m_start.y)/lineSeg.m_direction.y;
	}
	else /*if(fabs(lineSeg.direction.z) > 0.0001)*/if(lineSeg.m_direction.z != 0)
	{
		tend = (lineSeg.m_end.z - lineSeg.m_start.z)/lineSeg.m_direction.z;
	}
	else	// problem with line formulation
		return nothing;

	//bacon::cout << "tstart = " << tstart << " t = " << t << " tend = " << tend << newl;
	if( ((tstart <= t) && (t <= tend)) ||
		((tend <= t) && (t <= tstart)) )
		return lineSeg.m_start + lineSeg.m_direction * t;
	else // doesn't touch the line segment
		return nothing;
}


float CPlane::Distance(const CV3d& point) const
{
	return m_normal.dot(point) - m_d;
}

float CPlane::Distance(const CSphere& sphere) const
{
	return Distance(sphere.GetPosition() - sphere.GetRadius());
}


///////////////////////////
//	end CPlane
///////////////////////////

///////////////////////////
//	CExtrudedLineSegment
///////////////////////////

CExtrudedLineSegment::CExtrudedLineSegment()
{}

CExtrudedLineSegment::~CExtrudedLineSegment()
{}

void CExtrudedLineSegment::DefineLineSegment(CV3d _p1, CV3d _p2)
{
	m_p1 = _p1;
	m_p2 = _p2;

	CV3d p3 = m_p1 + CV3d(0, 0, 1); // the plane creates a line where it intersects the x,y plane
	m_plane.DefinePlane(m_p1, m_p2, p3);
}

void CExtrudedLineSegment::SetP1(CV3d _p1)
{
	m_p1 = _p1;

	CV3d p3 = m_p1 + CV3d(0, 0, 1); // the plane creates a line where it intersects the x,y plane
	m_plane.DefinePlane(m_p1, m_p2, p3);
}

void CExtrudedLineSegment::SetP2(CV3d _p2)
{
	m_p2 = _p2;

	CV3d p3 = m_p1 + CV3d(0, 0, 1); // the plane creates a line where it intersects the x,y plane
	m_plane.DefinePlane(m_p1, m_p2, p3);
}

CV3d CExtrudedLineSegment::GetP1()
{
	return m_p1;
}

CV3d CExtrudedLineSegment::GetP2()
{
	return m_p2;
}

CV3d CExtrudedLineSegment::GetNormal()
{
	return m_plane.GetNormal();
}


CV3d CExtrudedLineSegment::IntersectWith(const CRay& ray) const
{
	CV3d p = m_plane.IntersectWith(ray);
	CV3d direction = (m_p2 - m_p1).getUnitVector();
	float t1 = 0; 
	float t2;
	float t;
	if(direction.x != 0)
	{
		t2 = (m_p2.x - m_p1.x)/direction.x;
		t = (p.x - m_p1.x)/direction.x;
	}
	else if(direction.y != 0)
	{
		t2 = (m_p2.y - m_p1.y)/direction.y;
		t = (p.y - m_p1.y)/direction.y;
	}
	else if(direction.z != 0)
	{
		t2 = (m_p2.z - m_p1.z)/direction.z;
		t = (p.z - m_p1.z)/direction.z;
	}
	else	// problem with line formulation
		return nothing;
	// now, for the point to be between the two, t of p must be between t1 and t2
	
	if( ((t1 <= t) && (t <= t2)) ||
		((t2 <= t) && (t <= t1)) )
		return p; 
	else // doesn't touch the line segment
		return nothing;
}

CV3d CExtrudedLineSegment::IntersectWith(const CLine& line) const
{
	CV3d p = m_plane.IntersectWith(line);
	CV3d direction = (m_p2 - m_p1).getUnitVector();
	float t1 = 0; // 0  = (p1.x - p1.x)/ray.direction.x	// derived from line equation, to find t
	//bacon::cout << ((p2.x - p1.x)/direction.x) << '=' << (p2.y - p1.y)/direction.y << '=' << (p2.z - p1.z)/direction.z << newl;
	float t2;
	float t;
	if(direction.x != 0)
	{
		t2 = (m_p2.x - m_p1.x)/direction.x;
		t = (p.x - m_p1.x)/direction.x;
	}
	else if(direction.y != 0)
	{
		t2 = (m_p2.y - m_p1.y)/direction.y;
		t = (p.y - m_p1.y)/direction.y;
	}
	else if(direction.z != 0)
	{
		t2 = (m_p2.z - m_p1.z)/direction.z;
		t = (p.z - m_p1.z)/direction.z;
	}
	else	// problem with line formulation
		return nothing;
	// now, for the point to be between the two, t of p must be between t1 and t2
	
	if( ((t1 <= t) && (t <= t2)) ||
		((t2 <= t) && (t <= t1)) )
		return p; 
	else // doesn't touch the line segment
		return nothing;
}

CV3d CExtrudedLineSegment::IntersectWith(const CLineSegment& lineSeg) const
{
	CV3d p = m_plane.IntersectWith(lineSeg);
	CV3d direction = (m_p2 - m_p1).getUnitVector();
	float t1 = 0; // 0  = (p1.x - p1.x)/ray.direction.x	// derived from line equation, to find t
	//bacon::cout << ((p2.x - p1.x)/direction.x) << '=' << (p2.y - p1.y)/direction.y << '=' << (p2.z - p1.z)/direction.z << newl;
	float t2;
	float t;
	if(direction.x != 0)
	{
		t2 = (m_p2.x - m_p1.x)/direction.x;
		t = (p.x - m_p1.x)/direction.x;
	}
	else if(direction.y != 0)
	{
		t2 = (m_p2.y - m_p1.y)/direction.y;
		t = (p.y - m_p1.y)/direction.y;
	}
	else if(direction.z != 0)
	{
		t2 = (m_p2.z - m_p1.z)/direction.z;
		t = (p.z - m_p1.z)/direction.z;
	}
	else	// problem with line formulation
		return nothing;
	// now, for the point to be between the two, t of p must be between t1 and t2
	
	if( ((t1 <= t) && (t <= t2)) ||
		((t2 <= t) && (t <= t1)) )
		return p; 
	else // doesn't touch the line segment
		return nothing;
}

/*
float CExtrudedLineSegment::distance(const CV3d& point) const
{
	return normal.dot(point) - d;
}
*/
///////////////////////////
//	end CExtrudedLineSegment
///////////////////////////


///////////////////////////
//	CBSPTree
///////////////////////////
/*
CBSPTree::CBSPTree():
front(0),
back(0)
{
	objectList = new vid_SceneObjectHandleList;
}
CBSPTree::~CBSPTree()
{
	if(objectList)
	{
		delete objectList;
		objectList = 0;
	}
}

CBSPTree::CBSPTree(vid_SceneObjectHandleList* source)
{
	partition.definePlane( CV3d(0,1), CV3d(0,-1) ); // divide left and right side of the world
	vid_SceneObjectHandleList leftSide;
	CPlane leftSubPartition;
	leftSubPartition.definePlane( CV3d(1,0), CV3d(-1,0) );
	vid_SceneObjectHandleList rightSide;
	CPlane rightSubPartition;
	leftSubPartition.definePlane( CV3d(1,0), CV3d(-1,0) );

	for(vid_SceneObjectHandle* i = source->head; i != 0; )
	{
		vid_SceneObjectHandle* temp = new vid_SceneObjectHandle;
		temp->object = i->object;

		if( partition.distance( i->object->collapseTransforms().getTranslation() ) >= 0 )
		{
			rightSide.addToList(temp);
		}
		else
		{
			leftSide.addToList(temp);
		}

//		vid_SceneObjectHandle* toBeRemoved = i;
		i = i->nextHandle;
//		source->removeFromList(toBeRemoved);
	}
	// leftSide and rightSide are now full of stuff, and the stuff they contain is no longer in the source list
	buildBSPTree(&leftSide, leftSubPartition);
	buildBSPTree(&rightSide, rightSubPartition);
}

void CBSPTree::buildBSPTree(vid_SceneObjectHandleList* source, CPlane partition)
{
	if(source->objectCount == 1)
	{
		*objectList = *source;
		return;
	}

	partition.definePlane( CV3d(0,1), CV3d(0,-1) ); // divide left and right side of the world
	vid_SceneObjectHandleList leftSide;
	CPlane leftSubPartition;
	leftSubPartition.definePlane( CV3d(1,0), CV3d(-1,0) );
	vid_SceneObjectHandleList rightSide;
	CPlane rightSubPartition;
	leftSubPartition.definePlane( CV3d(1,0), CV3d(-1,0) );

	for(vid_SceneObjectHandle* i = source->head; i != 0; )
	{
		vid_SceneObjectHandle* temp = new vid_SceneObjectHandle;
		temp->object = i->object;

		if( partition.distance( i->object->collapseTransforms().getTranslation() ) >= 0 )
		{
			rightSide.addToList(temp);
		}
		else
		{
			leftSide.addToList(temp);
		}

//		vid_SceneObjectHandle* toBeRemoved = i;
		i = i->nextHandle;
//		source->removeFromList(toBeRemoved);
	}
	// leftSide and rightSide are now full of stuff, and the stuff they contain is no longer in the source list
	buildBSPTree(&leftSide, leftSubPartition);
	buildBSPTree(&rightSide, rightSubPartition);
}
*/

///////////////////////////
//	end CBSPTree
///////////////////////////

/*
con_Cout& operator<<(con_Cout& out, const CRay& ray)
{
	return out << "ray: start = " << ray.start << " direction = " << ray.direction;
}
con_Cout& operator<<(con_Cout& out, const CLine& line)
{
	return out << "line: point = " << line.start << " direction = " << line.direction;
}
con_Cout& operator<<(con_Cout& out, const CLineSegment& lineseg)
{
	return out << "line segment: p1 = " << lineseg.start << " p2 = " << lineseg.end;
}
con_Cout& operator<<(con_Cout& out, const CExtrudedLineSegment& extlineseg)
{
	return out << "extruded line segment: p1 = " << extlineseg.p1 << " p2 = " << extlineseg.p2;
}
*/