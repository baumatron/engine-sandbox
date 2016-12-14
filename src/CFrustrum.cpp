/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#include "CFrustrum.h"

void CFrustrum::AddClippingPlane(const CPlane& plane)
{
	m_clippingPlanes.push_back(plane);
}
bool CFrustrum::IsGeometryOccluded(const CSphere& geometry)
{
	bool occluded(false);
	for(vector<CPlane>::iterator it = m_clippingPlanes.begin(); it != m_clippingPlanes.end(); it++)
	{
		if(it->Distance(geometry) > geometry.GetRadius())
		{
			occluded = true;
			break;
		}
	}
	return occluded;
}
bool CFrustrum::IsGeometryOccluded(const CV3d& point)
{
	bool occluded(false);
	for(vector<CPlane>::iterator it = m_clippingPlanes.begin(); it != m_clippingPlanes.end(); it++)
	{
		if(it->Distance(point) > 0)
		{
			occluded = true;
			break;
		}
	}
	return occluded;
}
