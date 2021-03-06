#ifndef MATH_H
#define MATH_H

#include <math.h>
#include "con_main.h"



class vid_SceneObject;
class vid_Camera;
class vid_SceneObjectHandleList;


extern const float pi;


class v3d // 3 dimensional vector class
{
public:
	float x;
	float y;
	float z;

	v3d();
	v3d(float xinit, float yinit = 0.0, float zinit = 0.0);
	v3d(const v3d& rhs);
	~v3d();

	inline float distance(v3d rhs) const
	{
		rhs-=*this;
		return sqrt(rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z);
	}
	inline float distanceSquared(v3d rhs) const
	{
		rhs-=*this;
		return (rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z);
	}


	v3d rotate(v3d pivot, float xAxisAngle, float yAxisAngle, float zAxisAngle); // in degrees
	v3d rotate(v3d pivot, v3d angle); // in degrees
	v3d makeUnitVector();
	v3d getUnitVector() const;
	v3d getAnglesFromOrigin() const;
	v3d projected(v3d cameraPosition) const; // gives a 2d vertex of the 3d vertex projected on the monitor
	v3d projected() const; // uses camera position for offset

//	v3d worldSpacePosition(vid_SceneObject* parent) const; // returns the value of this vertex combined with the positions of all parent objects
	v3d camcoords(vid_Camera camera) const;
	v3d screencoords(vid_Camera camera) const; // returns coordinates translated from world space to screen space
	float dot(const v3d& rhs) const; // dot product operator
	v3d cross(const v3d& rhs) const; // cross product operator
	float magnitude() const { return (sqrt(x*x+y*y+z*z)); }
	float magnitudeSquared() const { return (x*x+y*y+z*z); }

	v3d operator+ (const v3d& rhs) const;
	v3d operator+=(const v3d& rhs);
	v3d operator- (const v3d& rhs) const;
	v3d operator-=(const v3d& rhs);

	v3d operator* (const float rhs) const; // scalar
	v3d operator*=(const float rhs); // scalar
	v3d operator/ (const float rhs) const; // scalar
	v3d operator/=(const float rhs); // scalar

	bool operator ==(const v3d& rhs) const {return ( (x==rhs.x) && (y==rhs.y) && (z==rhs.z)); }
	bool operator !=(const v3d& rhs) const {return !(*this == rhs);}
	v3d operator = (const v3d& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; return *this;}

	float& operator[](int index) 
	{
		if(index == 0) 
			return x; 
		else if(index == 1) 
			return y; 
		else 
			return z;
	}
	const float& operator[](int index) const
	{
		if(index == 0) 
			return x; 
		else if(index == 1) 
			return y; 
		else 
			return z;
	}
	// user defined types
	friend con_Cout& operator<<(con_Cout& out, const v3d& rhs);
};
extern const v3d nothing;

con_Cout& operator<<(con_Cout& out, const v3d& rhs);

class CRectangle
{
public:
	CRectangle():
		leftx(0),
		bottomy(0),
		rightx(0),
		topy(0){}
	CRectangle(float _leftx, float _rightx, float _bottomy, float _topy):
		leftx(_leftx),
		bottomy(_bottomy),
		rightx(_rightx),
		topy(_topy){}
		
	v3d getLowerLeft()
	{
		return v3d(leftx, bottomy);
	}
	v3d getUpperLeft()
	{
		return v3d(leftx, topy);
	}
	v3d getUpperRight()
	{
		return v3d(rightx, topy);
	}
	v3d getLowerRight()
	{
		return v3d(rightx, bottomy);
	}	
	float getHeight()
	{
		return topy-bottomy;
	}
	float getWidth()
	{
		return rightx-leftx;
	}
	
	CRectangle operator + (const v3d vector)
	{
		return CRectangle(leftx+vector.x, rightx+vector.x, bottomy+vector.y, topy+vector.y);
	}
	CRectangle operator + (const CRectangle vector)
	{
		return CRectangle(leftx+vector.leftx, rightx+vector.rightx, bottomy+vector.bottomy, topy+vector.topy);
	}
	float leftx, bottomy, rightx, topy;
private:
};

class quaternion;

class matrix4x4
{
public:
	float matrix[16];

	matrix4x4();
	matrix4x4(	const float& el00, const float& el01, const float& el02, const float& el03, 
				const float& el10, const float& el11, const float& el12, const float& el13, 
				const float& el20, const float& el21, const float& el22, const float& el23, 
				const float& el30, const float& el31, const float& el32, const float& el33);
	matrix4x4(const matrix4x4& rhs);
	~matrix4x4();

	matrix4x4 operator+ (const matrix4x4& rhs) const;
	matrix4x4 operator+=(const matrix4x4& rhs);
	matrix4x4 operator- (const matrix4x4& rhs) const;
	matrix4x4 operator-=(const matrix4x4& rhs);

	matrix4x4 operator* (const matrix4x4& rhs) const; 
	matrix4x4 operator*=(const matrix4x4& rhs); 
	matrix4x4 operator/ (const matrix4x4& rhs) const;
	matrix4x4 operator/=(const matrix4x4& rhs);

	matrix4x4 operator* (const float& rhs);
	matrix4x4 operator*=(const float& rhs); 

	v3d operator* (const v3d& rhs) const; 
	v3d operator*=(const v3d& rhs); 

	bool operator ==(const matrix4x4& rhs) const;
	bool operator !=(const matrix4x4& rhs) const;

	matrix4x4 operator=(const matrix4x4& rhs);
	
	static matrix4x4 rotationFromQuaternion(const quaternion& rhs);

	static matrix4x4 mtfIdentity();
	static matrix4x4 mtfXRot(float angle);
	static matrix4x4 mtfYRot(float angle);
	static matrix4x4 mtfZRot(float angle);
	static matrix4x4 mtfRot(v3d angle);
	static matrix4x4 mtfTranslate(v3d displacement);
	static matrix4x4 mtfScale(v3d scale);
	static matrix4x4 mtfRotateOnAxis(const v3d& axis, const float& angle);

	matrix4x4 setRot(v3d angle);
	matrix4x4 setTranslation(v3d translation);

	float getDeterminant();
	matrix4x4 getTranspose();
	matrix4x4 getInverse();
	matrix4x4 getTransform();
	v3d getTranslation();
	float getXRot();
	float getYRot();
	float getZRot();
	v3d getRot();

	void zero();
private:
	float det2x2(float* matrix2x2v);
	float det3x3(float* matrix3x3v);
};

matrix4x4 RotationMatrix(v3d angle);
matrix4x4 TranslationMatrix(v3d translation);

con_Cout& operator<<(con_Cout& out, const matrix4x4& rhs);

class quaternion
{
public:
	float x, y, z, w; 

	quaternion():
	x(0),y(0),z(0),w(1)
	{
	}

	quaternion(float _x, float _y, float _z, float _w):
	x(_x),y(_y),z(_z),w(_w)
	{
	}

	float length() const // also known as norm
	{
		return sqrt( x*x + y*y + z*z + w*w );
	}

	void normalize()
	{
		float len = fabs(length());
		if(len == 0.0f)
			return;
		x /= len;
		y /= len;
		z /= len;
	}
	void makeConjugate()
	{
		x = -x;
		y = -y;
		z = -z;
	}
	quaternion getConjugate() const
	{
		return quaternion(-x, -y, -z, w);
	}
	v3d getVector() const
	{
		return v3d(x, y, z);
	}
	void applyRotation(v3d axis, float theta)
	{
		quaternion rotation = quaternion::rotation(axis, theta);
        *this = rotation * (*this) * rotation.getConjugate();
	}
	quaternion operator* (const quaternion& rhs) const
	{
		quaternion result;

		result.x = w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y;
		result.y = w*rhs.y - x*rhs.z + y*rhs.w + z*rhs.x;
		result.z = w*rhs.z + x*rhs.y - y*rhs.x + z*rhs.w;
		result.w = w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z;

		return result;
	}
	quaternion operator*= (const quaternion& rhs)
	{
		quaternion result;

		*this = *this * rhs;

		return result;
	}
	quaternion operator=(const quaternion& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
		return *this;
	}
	quaternion operator=(const matrix4x4& rhs)
	{
		//MatToQuat(float m[4][4], QUAT * quat)
		{
		float  tr, s, q[4];
		int    i, j, k;


		int nxt[3] = {1, 2, 0};


		tr = rhs.matrix[0] + rhs.matrix[5] + rhs.matrix[10];


		// check the diagonal
		if (tr > 0.0) {
			s = sqrt (tr + 1.0f);
			w = s / 2.0f;
			s = 0.5f / s;
			x = (rhs.matrix[9] - rhs.matrix[6]) * s;
			y = (rhs.matrix[2] - rhs.matrix[8]) * s;
			z = (rhs.matrix[4] - rhs.matrix[1]) * s;
		} else {		
			// diagonal is negative
    			i = 0;
				if (rhs.matrix[0] > rhs.matrix[5]) i = 1;
				if (rhs.matrix[10] > rhs.matrix[i+i*4]) i = 2;
					j = nxt[i];
					k = nxt[j];


					s = sqrt ((rhs.matrix[i+i*4] - (rhs.matrix[j+j*4] + rhs.matrix[k+k*4])) + 1.0f);
		      
				q[i] = s * 0.5f;
		            
					if (s != 0.0) s = 0.5f / s;


				q[3] = (rhs.matrix[j+k*4] - rhs.matrix[k+j*4]) * s;
					q[j] = (rhs.matrix[i+j*4] + rhs.matrix[j+i*4]) * s;
					q[k] = (rhs.matrix[i+k*4] + rhs.matrix[k+i*4]) * s;


			x = q[0];
			y = q[1];
			z = q[2];
			w = q[3];
		}
		}
		return *this;
	}

	static quaternion rotation(v3d axis, float theta)
	{
		quaternion result;
		float temp, dist;

		// normalize
		temp = axis.x*axis.x + axis.y*axis.y + axis.z*axis.z;

		dist = (float)(1.0 / sqrt(temp));

		axis.x *= dist;
		axis.y *= dist;
		axis.z *= dist;

		result.x = axis.x;
		result.y = axis.y;
		result.z = axis.z;

		result.w = (float)cos((theta * pi/180) / 2.0f);
		/*
		result.x = axis.x * sin(theta/2);
		result.y = axis.y * sin(theta/2);
		result.z = axis.z * sin(theta/2);
		result.w = cos(theta/2);*/
/*
		float cr, cp, cy, sr, sp, sy, cpcy, spsy;


		// calculate trig identities
		cr = cos(zrot/2);

		cp = cos(xrot/2);
		cy = cos(yrot/2);


		sr = sin(zrot/2);
		sp = sin(xrot/2);
		sy = sin(yrot/2);
		
		cpcy = cp * cy;
		spsy = sp * sy;


		result.w = cr * cpcy + sr * spsy;
		result.x = sr * cpcy - cr * spsy;
		result.y = cr * sp * cy + sr * cp * sy;
		result.z = cr * cp * sy - sr * sp * cy;
*/


		return result;
	}
};

con_Cout& operator<<(con_Cout& out, const quaternion& rhs);



/*
class math_Circle
{
public:
	math_Circle
}
*/
class math_Ray
{
public:
	math_Ray();
	math_Ray(v3d _start, v3d _direction);
	~math_Ray();

	void setStart(const v3d& _start);
	void setDirection(const v3d& _direction);

	friend class math_Plane;
	friend con_Cout& operator<<(con_Cout& out, const math_Ray& ray);
protected:
	v3d start;
	v3d direction;
};

class math_Line: public math_Ray
{
	friend con_Cout& operator<<(con_Cout& out, const math_Line& line);
};

class math_LineSegment: public math_Ray
{
public:
	math_LineSegment();
	math_LineSegment(v3d _p1, v3d _p2);
	~math_LineSegment();

	void defineLineSegment(v3d _p1, v3d _p2);
	void setP1(v3d _p1);
	void setP2(v3d _p2);

	friend class math_Plane;

	friend con_Cout& operator<<(con_Cout& out, const math_LineSegment& lineseg);
private:
	float length;
	v3d end;
};


class math_Plane
{
public:
	math_Plane(); // 
	math_Plane(v3d p1, v3d p2);		// creates a line
	math_Plane(v3d p1, v3d p2, v3d p3);	// creates a 3 dimensional plane
	~math_Plane();

	void definePlane(v3d p1, v3d p2);	// sets the normal and d based on 3 points on the plane
	void definePlane(v3d p1, v3d p2, v3d p3);	// sets the normal and d based on 3 points on the plane
	v3d getNormal();

	v3d intersectWith(const math_Ray& ray) const;
	v3d intersectWith(const math_Line& line) const;
	v3d intersectWith(const math_LineSegment& line) const;
	float distance(const v3d& point) const;
private:
	v3d normal;
	float d;
};

class math_ExtrudedLineSegment	// this is actually a 3 dimensional object
						// it is a plane with 2 points that are bounding a line segment where that plane intersects the x,y plane
						// should this be represented as p = p0 + direction * t ????? -> uses this equation for part of intersectWith function, should it use only this equation
						// p = p0 + direction * t = R0 + RDirection * Rt = Rp   lineEquation = rayEquation
						//	solve where t = Rt, resulting point should be point of intersection
{
public:
	math_ExtrudedLineSegment();
	~math_ExtrudedLineSegment();

	void defineLineSegment(v3d _p1, v3d _p2);
//	void defineLineSegment(v3d p1, v3d p2, v3d p3);
	void setP1(v3d _p1);
	void setP2(v3d _p2);
	v3d getP1();
	v3d getP2();
	v3d getNormal();

	v3d intersectWith(const math_Ray& ray) const;
	v3d intersectWith(const math_Line& line) const;
	v3d intersectWith(const math_LineSegment& line) const;

	friend con_Cout& operator<<(con_Cout& out, const math_ExtrudedLineSegment& extlineseg);
private:
	v3d p1, p2;	// these must satisfy the plane equation
	math_Plane plane;
};

class math_BSPTree	// designed for static objects
{
public:
	math_BSPTree();
	math_BSPTree(vid_SceneObjectHandleList* source);
	~math_BSPTree();

	void buildBSPTree(vid_SceneObjectHandleList* source, math_Plane partition);

	math_Plane partition;
	vid_SceneObjectHandleList* objectList;	// this is a pointer to appease the compiler (don't have to include the Video.main.h header)
	math_BSPTree *front, *back;
};

con_Cout& operator<<(con_Cout& out, const math_Ray& ray);
con_Cout& operator<<(con_Cout& out, const math_Line& line);
con_Cout& operator<<(con_Cout& out, const math_LineSegment& lineseg);
con_Cout& operator<<(con_Cout& out, const math_ExtrudedLineSegment& extlineseg);

/*
#include <ode/ode.h>
#include "Video.main.h"

class Video.PhysObject: public vid_SceneObject
{
public:
	dBodyID body;
	dGeomID geom;	
};
*/

#endif