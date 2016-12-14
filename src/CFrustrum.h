/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#ifndef CFRUSTRUM_H
#define CFRUSTRUM_H

#include <vector>
using namespace std;

#include "BaneMath.h"
using namespace BaneMath;

class CFrustrum
{
	void AddClippingPlane(const CPlane& plane);
	bool IsGeometryOccluded(const CSphere& geometry);
	bool IsGeometryOccluded(const CV3d& point);
private:
	vector<CPlane> m_clippingPlanes;
};

#endif