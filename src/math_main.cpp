
#include "CVideoSubsystem.h"
#include "math_main.h"
#include "con_main.h"

const float pi(static_cast<float>(3.1415926535897));
const v3d nothing(-100000*pi, -100000*pi, -100000*pi);

///////////////////////////
// v3d
///////////////////////////


v3d::v3d():
	x(0),
	y(0),
	z(0)
{}
v3d::v3d(float xinit, float yinit, float zinit):
	x(xinit),
	y(yinit),
	z(zinit)
{}
v3d::v3d(const v3d& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
}

v3d::~v3d()
{}


v3d v3d::rotate(v3d pivot, float xAxisAngle, float yAxisAngle, float zAxisAngle) // rotate on x, then on y, then on z
{
	*this -= pivot;
	float magnitude;
	float angle;
	if(xAxisAngle != 0)
	{
		// use y/z to rotate on x axis
		magnitude = sqrt(z*z+y*y);
		if(magnitude != 0)
			{

			angle = atan(y/z); // atan returns value in radians
			angle = angle*180/pi;
			if(z < 0)
				angle += 180;

			z = magnitude * cos(angle*pi/180+(xAxisAngle*pi/180)); // cos and atan are in radians... ?
			y = magnitude * sin(angle*pi/180+(xAxisAngle*pi/180));
		}
	}
	if(yAxisAngle != 0)
	{
		// use z/x to rotate on y axis
		magnitude = sqrt(x*x+z*z);
		if(magnitude != 0)
		{
			angle = atan(z/x); // atan returns value in radians
			angle = angle*180/pi;
			if(x < 0)
				angle += 180;

			x = magnitude * cos(angle*pi/180+(yAxisAngle*pi/180)); // cos and atan are in radians... ?
			z = magnitude * sin(angle*pi/180+(yAxisAngle*pi/180));
		}
	}
	if(zAxisAngle != 0)
	{
		// use y/x to rotate on z axis
		magnitude = sqrt(x*x+y*y);
		if(magnitude != 0)
		{
			angle = atan(y/x); // atan returns value in radians
			angle = angle*180/pi;
			if(x < 0)
				angle += 180;

			x = magnitude * cos(angle*pi/180+(zAxisAngle*pi/180)); // cos and atan are in radians... ?
			y = magnitude * sin(angle*pi/180+(zAxisAngle*pi/180));
		}
	}


	*this += pivot;
	return *this;
}
v3d v3d::rotate(v3d pivot, v3d angle)
{
	return rotate(pivot, angle.x, angle.y, angle.z);
}

v3d v3d::makeUnitVector()
{
	float magnitude = sqrt(x*x+y*y+z*z);
	x = x / magnitude;
	y = y / magnitude;
	z = z / magnitude;
	return *this;
}

v3d v3d::getUnitVector() const
{
	v3d temp = *this;
	return temp.makeUnitVector();	
}

v3d v3d::getAnglesFromOrigin() const
{
	v3d result;

	// use y/x to find z axis rotation
	if((x != 0) /*|| (y != 0)*/)
	{
	/*	if(x == 0) // y is nonzero
		{
			result.z = 90.0; 
		}
		else*/
		{
			result.z = atan(y/x); // atan returns value in radians
			result.z *= 180/pi;
			if(x < 0)
				result.z += 180;
		}
	}


	// use z/x to rotate on y axis
	if((x != 0) /*||(z != 0)*/)
	{
	/*	if(x == 0)
		{
			result.y = 90.0; 
		}
		else*/
		{
			result.y = atan(z/x); // atan returns value in radians
			result.y *= 180/pi;
			if(x < 0)
				result.y += 180;
		}

	}

	// use y/z to rotate on x axis
	if(/*(y != 0) || */(z != 0))
	{
		/*if(z == 0)
		{
			result.x = 90.0; 
		}
		else*/
		{
			result.x = atan(y/z); // atan returns value in radians
			result.x *= 180/pi;
			if(z < 0)
				result.x += 180;
		}
	}

	return result;
}

v3d v3d::projected(v3d cameraPosition) const // cameraPosition is actual position of camera | this function takes absolute coordinates
{
	float x2d, y2d;
	x2d = Video.settings.getSw()/2 + Video.settings.getSw()*((x-cameraPosition.x) / (z-cameraPosition.z));
	y2d = Video.settings.getSh()/2 - Video.settings.getSh()*((y-cameraPosition.y) / (z-cameraPosition.z)); // - here changes the sign of the y axis

	return v3d(x2d, y2d);
}
v3d v3d::projected() const
{
	return projected(Video.camera.GetTransformation().getTranslation());
}/*
v3d v3d::alignToCamera(v3d cameraPosition, v3d cameraAngle, vid_SceneObject* parent)
{	
	v3d parentPosition(worldSpacePosition(parent)); // - *this ???
	*this += parentPosition-cameraPosition; // use world coordinates and rotate about the camera's position
	
	this->rotate(cameraPosition, cameraAngle.x, cameraAngle.y, cameraAngle.z);
	parentPosition.rotate(cameraPosition, cameraAngle.x, cameraAngle.y, cameraAngle.z);

	*this -= parentPosition-cameraPosition; // restore object coordinates
	return *this;
}
*/
/*v3d v3d::worldSpacePosition(vid_SceneObject* parent) const
{
	if(!parent)
		return *this;
	return (*this + parent->position.worldSpacePosition(parent->parent)); // recursively evaluate the world position
}*/

v3d v3d::camcoords(vid_Camera camera) const
{
	return v3d(*this-camera.getPosition());
}

v3d v3d::screencoords(vid_Camera camera) const
{
	v3d temp = *this;
	temp.rotate(camera.getPosition(), camera.getAngle()*-1.0f);
	temp -= camera.getPosition();
	temp += v3d(static_cast<float>(Video.settings.getSw())/2, static_cast<float>(Video.settings.getSh())/2);
	return temp;
}

float v3d::dot(const v3d& rhs) const
{
	return (x*rhs.x + y*rhs.y + z*rhs.z); //v3d(x*rhs.x, y*rhs.y, z*rhs.z);
}

v3d v3d::cross(const v3d& rhs) const // for use with vectors only
{
	return v3d((y*rhs.z - z*rhs.y), -(x*rhs.z - z*rhs.x), (x*rhs.y - y*rhs.x));
}


v3d v3d::operator+(const v3d& rhs) const
{
	return v3d(x+rhs.x, y+rhs.y, z+rhs.z);
}
v3d v3d::operator+=(const v3d& rhs)
{
/*	x += rhs.x;
	y += rhs.y;
	z += rhs.z;*/
	return v3d(x += rhs.x, y += rhs.y, z += rhs.z);
}
v3d v3d::operator-(const v3d& rhs) const
{
	return v3d(x-rhs.x, y-rhs.y, z-rhs.z);
}
v3d v3d::operator-=(const v3d& rhs)
{
/*	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;*/
	return v3d(x -= rhs.x, y -= rhs.y, z -= rhs.z);
}


v3d v3d::operator*(const float rhs) const
{
	return v3d(x*rhs, y*rhs, z*rhs);
}
v3d v3d::operator*=(const float rhs)
{
/*	x *= rhs;
	y *= rhs;
	z *= rhs;*/
	return v3d(x *= rhs, y *= rhs, z *= rhs);
}
v3d v3d::operator/(const float rhs) const
{
	return v3d(x/rhs, y/rhs, z/rhs);
}
v3d v3d::operator/=(const float rhs)
{
/*	x /= rhs;
	y /= rhs;
	z /= rhs;*/
	return v3d(x /= rhs, y /= rhs, z /= rhs);
}

con_Cout& operator<<(con_Cout& out, const v3d& rhs)
{
	return out << "(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";
}

///////////////////////////
// end v3d
///////////////////////////



// matrix storage specification
// stored as opengl expects
// if matrix[r][c] r = row, c = col
// [0][0] = [0]
// [1][0] = [1]
// [2][0] = [2]
// [0][1] = [4]
// etc

// matrix specification
// transform (rotate, scale)
// | | |
// v v v
// 0 4 8  12 <- translation x
// 1 5 9  13 <- translation y
// 2 6 10 14 <- translation z
// 3 7 11 15


matrix4x4::matrix4x4()
{
//	*this = matrix4x4::mtfIdentity();
	zero();
	matrix[0] = 1;
	matrix[5] = 1;
	matrix[10] = 1;
	matrix[15] = 1;
}

matrix4x4::matrix4x4(	const float& el00, const float& el01, const float& el02, const float& el03, 
			const float& el10, const float& el11, const float& el12, const float& el13, 
			const float& el20, const float& el21, const float& el22, const float& el23, 
			const float& el30, const float& el31, const float& el32, const float& el33)
{
	matrix[0] = el00;
	matrix[4] = el01;
	matrix[8] = el02;
	matrix[12] = el03;
	matrix[1] = el10;
	matrix[5] = el11;
	matrix[9] = el12;
	matrix[13] = el13;
	matrix[2] = el20;
	matrix[6] = el21;
	matrix[10] = el22;
	matrix[14] = el23;
	matrix[3] = el30;
	matrix[7] = el31;
	matrix[11] = el32;
	matrix[15] = el33;
}


matrix4x4::matrix4x4(const matrix4x4& rhs)
{
/*	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			matrix[i][j] = rhs.matrix[i][j];*/
	for(int i = 0; i < 16; i++)
		matrix[i] = rhs.matrix[i];
}

matrix4x4::~matrix4x4()
{
}

matrix4x4 matrix4x4::operator+ (const matrix4x4& rhs) const
{
	matrix4x4 result;
	for(int r = 0; r < 4; r++) // row is like x
		for(int c = 0; c < 4; c++) // col is like y
			result.matrix[r+c*4] = matrix[r+c*4] + rhs.matrix[r+c*4];
	return result;
}

matrix4x4 matrix4x4::operator+=(const matrix4x4& rhs)
{
	return *this = *this + rhs;
}

matrix4x4 matrix4x4::operator- (const matrix4x4& rhs) const
{
	matrix4x4 result;
	for(int r = 0; r < 4; r++)
		for(int c = 0; c < 4; c++)
			result.matrix[r+c*4] = matrix[r+c*4] - rhs.matrix[r+c*4];
	return result;
}

matrix4x4 matrix4x4::operator-=(const matrix4x4& rhs)
{
	return *this = *this - rhs;
}

matrix4x4 matrix4x4::operator* (const matrix4x4& rhs) const
{
	matrix4x4 result;
	int k;
	for(int r = 0; r < 4; r++)
		for(int c = 0; c < 4; c++)
			for(k = 0, result.matrix[r+c*4] = 0; k < 4; k++)
				result.matrix[r+c*4] += matrix[r+k*4] * rhs.matrix[k+c*4];
	return result;
}

matrix4x4 matrix4x4::operator*=(const matrix4x4& rhs)
{
	return *this = *this * rhs;
}



v3d matrix4x4::operator* (const v3d& rhs) const
{
	v3d result;

	result.x =	rhs.x * matrix[0] + 
				rhs.y * matrix[4] + 
				rhs.z * matrix[8] + 
				/*rhs.w == 1 * */matrix[12];
	result.y =	rhs.x * matrix[1] + 
				rhs.y * matrix[5] + 
				rhs.z * matrix[9] + 
				/*rhs.w == 1 * */matrix[13];
	result.z =	rhs.x * matrix[2] + 
				rhs.y * matrix[6] + 
				rhs.z * matrix[10] + 
				/*rhs.w == 1 * */matrix[14];

	return result;
}
matrix4x4 matrix4x4::operator* (const float& rhs)
{
	matrix4x4 result;
	for(int r = 0; r < 4; r++)
		for(int c = 0; c < 4; c++)
			result.matrix[r+c*4] = matrix[r+c*4]*rhs;
	return result;
}
matrix4x4 matrix4x4::operator*=(const float& rhs)
{
	for(int r = 0; r < 4; r++)
		for(int c = 0; c < 4; c++)
			matrix[r+c*4] *= rhs;
	return *this;
}

/*
matrix4x4 matrix4x4::operator /(const matrix4x4& rhs)
{
	v3d temp;
	temp.x = matrix[0][3]
}
TX = X - Matrix[3][0];
TY = Y - Matrix[3][1];
TZ = Z - Matrix[3][2];
NewX = TX * Matrix[0][0] + TY * Matrix[0][1] + TZ * Matrix[0][2];
NewY = TX * Matrix[1][0] + TY * Matrix[1][1] + TZ * Matrix[1][2];
NewZ = TX * Matrix[2][0] + TY * Matrix[2][1] + TZ * Matrix[2][2];

*/
bool matrix4x4::operator ==(const matrix4x4& rhs) const
{
	bool different(false);
	for(int r = 0; r < 4; r++)
		for(int c = 0; c < 4; c++)
			if(matrix[r+c*4] != rhs.matrix[r+c*4])
				different = true;
	return !different;
}

bool matrix4x4::operator !=(const matrix4x4& rhs) const
{
	return !(*this == rhs);
}

matrix4x4 matrix4x4::operator=(const matrix4x4& rhs)
{
	for(int r = 0; r < 4; r++)
		for(int c = 0; c < 4; c++)
			matrix[r+c*4] = rhs.matrix[r+c*4];
	return *this;
}

matrix4x4 matrix4x4::rotationFromQuaternion(const quaternion& rhs)
{
	matrix4x4 result;
	result = matrix4x4::mtfIdentity();
/*	result.matrix[0][0] = rhs.w*rhs.w + rhs.x*rhs.x - rhs.y*rhs.y - rhs.z *rhs.z;
	result.matrix[1][0] = 2*rhs.x*rhs.y + 2*rhs.w*rhs.z;
	result.matrix[2][0] = 2*rhs.x*rhs.z - 2*rhs.w*rhs.y;

	result.matrix[0][1] = 2*rhs.x*rhs.y - 2*rhs.w*rhs.z;
	result.matrix[1][1] = rhs.w*rhs.w - rhs.x*rhs.x + rhs.y*rhs.y - rhs.z*rhs.z;
	result.matrix[2][1] = 2*rhs.y*rhs.z + 2*rhs.w*rhs.x;

	result.matrix[0][2] = 2*rhs.x*rhs.z + 2*rhs.w*rhs.y;
	result.matrix[1][2] = 2*rhs.y*rhs.z - 2*rhs.w*rhs.x;
	result.matrix[2][2] = rhs.w*rhs.w - rhs.x*rhs.x - rhs.y*rhs.y + rhs.z*rhs.z;*/
/*	result.matrix[0] = rhs.w*rhs.w + rhs.x*rhs.x - rhs.y*rhs.y - rhs.z *rhs.z;
	result.matrix[1] = 2*rhs.x*rhs.y + 2*rhs.w*rhs.z;
	result.matrix[2] = 2*rhs.x*rhs.z - 2*rhs.w*rhs.y;

	result.matrix[4] = 2*rhs.x*rhs.y - 2*rhs.w*rhs.z;
	result.matrix[5] = rhs.w*rhs.w - rhs.x*rhs.x + rhs.y*rhs.y - rhs.z*rhs.z;
	result.matrix[6] = 2*rhs.y*rhs.z + 2*rhs.w*rhs.x;

	result.matrix[8] = 2*rhs.x*rhs.z + 2*rhs.w*rhs.y;
	result.matrix[9] = 2*rhs.y*rhs.z - 2*rhs.w*rhs.x;
	result.matrix[10] = rhs.w*rhs.w - rhs.x*rhs.x - rhs.y*rhs.y + rhs.z*rhs.z;	return result;*/



    float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;


    // calculate coefficients
    x2 = rhs.x + rhs.x; y2 = rhs.y + rhs.y;
    z2 = rhs.z + rhs.z;
    xx = rhs.x * x2; xy = rhs.x * y2; xz = rhs.x * z2;
    yy = rhs.y * y2; yz = rhs.y * z2; zz = rhs.z * z2;
    wx = rhs.w * x2; wy = rhs.w * y2; wz = rhs.w * z2;


    result.matrix[0] = 1.0 - (yy + zz); 
	result.matrix[1] = xy - wz;
    result.matrix[2] = xz + wy; 
	result.matrix[3] = 0.0;

    result.matrix[4] = xy + wz; 
	result.matrix[5] = 1.0 - (xx + zz);
    result.matrix[6] = yz - wx; 
	result.matrix[7] = 0.0;

    result.matrix[8] = xz - wy; 
	result.matrix[9] = yz + wx;
    result.matrix[10] = 1.0 - (xx + yy); 
	result.matrix[11] = 0.0;

    result.matrix[12] = 0; 
	result.matrix[13] = 0;
    result.matrix[14] = 0; 
	result.matrix[15] = 1;
	/*
Matrix =  [ w2 + x2 - y2 - z2       2xy - 2wz           2xz + 2wy
                2xy + 2wz       w2 - x2 + y2 - z2       2yz - 2wx
                2xz - 2wy           2yz + 2wx       w2 - x2 - y2 + z2 ]*/
	return result;
}

matrix4x4 matrix4x4::mtfIdentity()
{
	matrix4x4 result;
	result.zero();
	result.matrix[0] = 1;
	result.matrix[5] = 1;
	result.matrix[10] = 1;
	result.matrix[15] = 1;
	return result;
}

matrix4x4 matrix4x4::mtfXRot(float angle)
{
	matrix4x4 result;
	float cosVal = cos(angle*pi/180);
	float sinVal = sin(angle*pi/180);
	result.zero();
	result.matrix[0] = 1;
	result.matrix[5] = cosVal;
	result.matrix[6] = sinVal;
	result.matrix[9] = -sinVal;
	result.matrix[10] = cosVal;
	result.matrix[15] = 1;
	return result;
}

matrix4x4 matrix4x4::mtfYRot(float angle)
{
	matrix4x4 result;
	float cosVal = cos(angle*pi/180);
	float sinVal = sin(angle*pi/180);
	result.zero();
	result.matrix[0] = cosVal;
	result.matrix[2] = -sinVal;
	result.matrix[5] = 1;
	result.matrix[8] = sinVal;
	result.matrix[10] = cosVal;
	result.matrix[15] = 1;
	return result;
}

matrix4x4 matrix4x4::mtfZRot(float angle)
{
	matrix4x4 result;
	float cosVal = cos(angle*pi/180);
	float sinVal = sin(angle*pi/180);
	result.zero();
	result.matrix[0] = cosVal;
	result.matrix[1] = sinVal;
	result.matrix[4] = -sinVal;
	result.matrix[5] = cosVal;
	result.matrix[10] = 1;
	result.matrix[15] = 1;
	return result;
}
	
matrix4x4 matrix4x4::mtfRot(v3d angle)
{
	matrix4x4 xrot = matrix4x4::mtfXRot(angle.x);
	matrix4x4 yrot = matrix4x4::mtfYRot(angle.y);
	matrix4x4 zrot = matrix4x4::mtfZRot(angle.z);
//	return matrix4x4::mtfZRot(angle.z) * matrix4x4::mtfYRot(angle.y) * matrix4x4::mtfXRot(angle.x) ;
	return zrot * yrot * xrot;
}

matrix4x4 matrix4x4::mtfTranslate(v3d displacement)
{
	matrix4x4 result;
	result = matrix4x4::mtfIdentity();
	result.matrix[12] = displacement.x;
	result.matrix[13] = displacement.y;
	result.matrix[14] = displacement.z;
	return result;
}

matrix4x4 matrix4x4::mtfScale(v3d scale)
{
	matrix4x4 result;
	result.zero();
	result.matrix[0] = scale.x;
	result.matrix[5] = scale.y;
	result.matrix[10] = scale.z;
	result.matrix[15] = 1;
	return result;
}

matrix4x4 matrix4x4::mtfRotateOnAxis(const v3d& axis, const float& angle)
{
	matrix4x4 result;
	result = matrix4x4::mtfIdentity();
	float rcos = cos(angle*pi/180);
	float rsin = sin(angle*pi/180);
	result.matrix[0] = rcos + axis.x*axis.x*(1-rcos);
	result.matrix[1] =  axis.z * rsin + axis.y*axis.x*(1-rcos);
	result.matrix[2] = -axis.y * rsin + axis.z*axis.x*(1-rcos);
	result.matrix[4] = -axis.z * rsin + axis.x*axis.y*(1-rcos);
	result.matrix[5] = rcos + axis.y*axis.y*(1-rcos);
	result.matrix[6] =  axis.x * rsin + axis.z*axis.y*(1-rcos);
	result.matrix[8] =  axis.y * rsin + axis.x*axis.z*(1-rcos);
	result.matrix[9] = -axis.x * rsin + axis.y*axis.z*(1-rcos);
	result.matrix[10] = rcos + axis.z*axis.z*(1-rcos);
	return result;
}

matrix4x4 matrix4x4::setRot(v3d angle)
{
	matrix4x4 temp = *this;
	*this = matrix4x4::mtfRot(angle);
	setTranslation(temp.getTranslation());
	return *this;
}


matrix4x4 matrix4x4::setTranslation(v3d translation)
{
	matrix[12] = translation.x;
	matrix[13] = translation.y;
	matrix[14] = translation.z;
	return *this;
}


float matrix4x4::det2x2(float* matrix2x2v)
{
	/*ccout << "2x2 det ";
	ccout << matrix2x2v[0] << " " << matrix2x2v[2] << '\n';
	ccout << matrix2x2v[1] << " " << matrix2x2v[3] << '\n';
	ccout << "answer is: " << matrix2x2v[0]*matrix2x2v[3] - matrix2x2v[2]*matrix2x2v[1];*/
	return matrix2x2v[0]*matrix2x2v[3] - matrix2x2v[2]*matrix2x2v[1];
}
float matrix4x4::det3x3(float* matrix3x3v)
{
//	ccout << "3x3 det \n";
	float a[] = {matrix3x3v[4], matrix3x3v[5], matrix3x3v[7], matrix3x3v[8]};
	float b[] = {matrix3x3v[1], matrix3x3v[2], matrix3x3v[7], matrix3x3v[8]};
	float c[] = {matrix3x3v[1], matrix3x3v[2], matrix3x3v[4], matrix3x3v[5]};
/*	ccout << matrix3x3v[0] << " " << matrix3x3v[3] << " " << matrix3x3v[6] << " " <<
			matrix3x3v[1] << " " << matrix3x3v[4] << " " << matrix3x3v[7] << " " <<
			matrix3x3v[2] << " " << matrix3x3v[5] << " " << matrix3x3v[8] << " \n";
	ccout << "answer is: " << matrix3x3v[0] * det2x2(a) - matrix3x3v[3] * det2x2(b) + matrix3x3v[6] * det2x2(c);
*/	return matrix3x3v[0] * det2x2(a) - matrix3x3v[3] * det2x2(b) + matrix3x3v[6] * det2x2(c);
}


float matrix4x4::getDeterminant()
{
	float a[] = {matrix[5], matrix[6], matrix[7], 
				matrix[9], matrix[10], matrix[11], 
				matrix[13], matrix[14], matrix[15]};
	float b[] = {matrix[1], matrix[2], matrix[3], 
				matrix[9], matrix[10], matrix[11], 
				matrix[13], matrix[14], matrix[15]};
	float c[] = {matrix[1], matrix[2], matrix[3], 
				matrix[5], matrix[6], matrix[7], 
				matrix[13], matrix[14], matrix[15]};
	float d[] = {matrix[1], matrix[2], matrix[3], 
				matrix[5], matrix[6], matrix[7], 
				matrix[9], matrix[10], matrix[11]};
	return matrix[0] * det3x3(a) - matrix[4] * det3x3(b) + matrix[8] * det3x3(c)- matrix[12] * det3x3(d);
}

matrix4x4 matrix4x4::getTranspose()
{
	return matrix4x4( matrix[0], matrix[1], matrix[2], matrix[3],
					matrix[4], matrix[5], matrix[6], matrix[7],
					matrix[8], matrix[9], matrix[10], matrix[11],
					matrix[12], matrix[13], matrix[14], matrix[15] );
}

matrix4x4 matrix4x4::getInverse()
{
	if(!getDeterminant())
		return *this;
	matrix4x4 result;
	for(int r = 0; r < 4; r++)
		for(int c = 0; c < 4; c++)
		{
			int subCol0 = (c == 0) ? 1 : 0;
			int subCol1 = (c == 1) ? 2 : subCol0+1;
			int subCol2 = (c == 2) ? 3 : subCol1+1;
			int subRow0 = (r == 0) ? 1 : 0;
			int subRow1 = (r == 1) ? 2 : subRow0+1;
			int subRow2 = (r == 2) ? 3 : subRow1+1;
			float submatrix [] = {matrix[subRow0+subCol0*4], matrix[subRow1+subCol0*4], matrix[subRow2+subCol0*4],
								matrix[subRow0+subCol1*4], matrix[subRow1+subCol1*4], matrix[subRow2+subCol1*4],
								matrix[subRow0+subCol2*4], matrix[subRow1+subCol2*4], matrix[subRow2+subCol2*4]	};
			result.matrix[r+c*4] = (det3x3(submatrix) / getDeterminant()) * pow(-1, (int)r+(int)c); 
		}
	return result.getTranspose();
}

matrix4x4 matrix4x4::getTransform()
{
	return matrix4x4(matrix[0], matrix[4], matrix[7], 0,
					matrix[1], matrix[5], matrix[8], 0,
					matrix[2], matrix[6], matrix[9], 0,
					0,			0,			0,	1);
}


v3d matrix4x4::getTranslation()
{
	return v3d(matrix[12], matrix[13], matrix[14]);
}

float matrix4x4::getXRot()
{
	return getRot().x;
}
float matrix4x4::getYRot()
{
	return getRot().y;
}
float matrix4x4::getZRot()
{
	return getRot().z;;
}

v3d matrix4x4::getRot()
{
	//ccout << "WARNING, matrix4x4::getRot() returns incorrect values... uses wrong convention\n";
	v3d angles;
	float C, trix, triy;
	angles.y = asin(matrix[8]);
	C = cos(angles.y);
	angles.y *= 180/pi;

	if(fabs(C) > 0.005)             /* Gimball lock? */
	{
		trix = matrix[10]/C;           /* No, so get X-axis angle */
		triy = -matrix[9]/C;
		angles.x = atan2(triy, trix)*180/pi;
		trix =  matrix[0]/C;            /* Get Z-axis angle */
		triy = -matrix[4]/C;
		angles.z = atan2(triy, trix)*180/pi;
	}
	else                                 /* Gimball lock has occurred */
	{
		angles.x = 0;                      /* Set X-axis angle to zero */
		trix = matrix[5];                 /* And calculate Z-axis angle */
		triy = matrix[1];
		angles.z = atan2(triy, trix)*180/pi;
	//	ccout << "Gimball lock\n";
	//	ccout << "angles: " << angles << newl;
	//	paused = true;
	}

	/* return only positive angles in [0,360] */
	if (angles.x < 0) angles.x += 360;
	if (angles.y < 0) angles.y += 360;
	if (angles.z < 0) angles.z += 360;
	angles = angles * -1;
	return angles;
}


void matrix4x4::zero()
{
	for(int r = 0; r < 4; r++)
		for(int c = 0; c < 4; c++)
			matrix[r+c*4] = 0;
}

matrix4x4 RotationMatrix(v3d angle)
{
	return matrix4x4::mtfRot(angle);
}
matrix4x4 TranslationMatrix(v3d translation)
{
	return matrix4x4::mtfTranslate(translation);
}


con_Cout& operator<<(con_Cout& out, const matrix4x4& rhs)
{
	for(int i = 0; i < 4; i++)
	{
		out << "[";
		for(int j = 0; j < 4; j++)
		{
			out << rhs.matrix[i+j*4] << ((i == 3) && (j == 3) ? ' ' : ',') ;
		}
		out << "]" << newl;
	}
	return out;
}



/*


Vertex Matrix_Vertex_Multiply(Matrix mat, Vertex Vert) {
  Vertex v;

  v.x = vert.x * mat.matrix[0][0] +
        vert.y * mat.matrix[0][1] +
        vert.z * mat.matrix[0][2] +
                 mat.matrix[0][3];

  v.y = vert.x * mat.matrix[1][0] +
        vert.y * mat.matrix[1][1] +
        vert.z * mat.matrix[1][2] +
                 mat.matrix[1][3];

  v.z = vert.x * mat.matrix[2][0] +
        vert.y * mat.matrix[2][1] +
        vert.z * mat.matrix[2][2] +
                 mat.matrix[2][3];

  return v;
}        */


con_Cout& operator<<(con_Cout& out, const quaternion& rhs)
{
	ccout << "[ " << rhs.w << " , " << rhs.getVector() << " ] ";
	return out;
}


///////////////////////////
//	math_Ray
///////////////////////////

math_Ray::math_Ray():
start(0),
direction(1)
{}

math_Ray::math_Ray(v3d _start, v3d _direction):
start(_start),
direction(_direction)
{}

void math_Ray::setStart(const v3d& _start)
{
	start = _start;
}
void math_Ray::setDirection(const v3d& _direction)
{
	direction = _direction;
}


math_Ray::~math_Ray(){}

///////////////////////////
//	end math_Ray
///////////////////////////

///////////////////////////
//	math_LineSegment
///////////////////////////
math_LineSegment::math_LineSegment()
{
	defineLineSegment(0, 1);
}
math_LineSegment::math_LineSegment(v3d _p1, v3d _p2)
{
	defineLineSegment(_p1, _p2);
}

void math_LineSegment::defineLineSegment(v3d _p1, v3d _p2)
{
	start = _p1;
	end = _p2;
	direction = (end-start).getUnitVector();
	length = start.distance(end);
}

void math_LineSegment::setP1(v3d p1)
{
	defineLineSegment(p1, end);
}

void math_LineSegment::setP2(v3d p2)
{
	defineLineSegment(start, p2);
}

math_LineSegment::~math_LineSegment()
{
}

///////////////////////////
//	end math_LineSegment
///////////////////////////

///////////////////////////
//	math_Plane
///////////////////////////

math_Plane::math_Plane():
normal(1),
d(0)
{}

math_Plane::math_Plane(v3d p1, v3d p2)		// creates a line
{
	v3d p3 = p1+v3d(0,0,1); // makes the plane vertical, creating a line on the x,y axis
	definePlane(p1, p2, p3);
}

math_Plane::math_Plane(v3d p1, v3d p2, v3d p3) // creates a 3 dimensional plane
{
	definePlane(p1, p2, p3);
}

math_Plane::~math_Plane(){}

void math_Plane::definePlane(v3d p1, v3d p2)	// sets the normal and d based on 3 points on the plane
{
	v3d p3 = p1+v3d(0,0,1); // makes the plane vertical, creating a line on the x,y axis
	definePlane(p1, p2, p3);
}

void math_Plane::definePlane(v3d p1, v3d p2, v3d p3)	// sets the normal and d based on 3 points on the plane
{
	normal = (p2 - p1).cross(p3 - p1).makeUnitVector(); // a clockwise definition (looking at a triangle), defines the normal pointing back out of the screen
	d = normal.dot(p1);
}

v3d math_Plane::getNormal()
{
	return normal;
}


v3d math_Plane::intersectWith(const math_Ray& ray) const
{
	if(!normal.dot(ray.direction))	// does not intersect!
		// throw an exception??? no acceptable return value
		return nothing;
	float t = ( d - normal.dot(ray.start) ) / ( normal.dot(ray.direction) );
	if(t < 0) // does not intersect the ray! t cannot be negative
		return nothing;
		// throw exception
	return ray.start + ray.direction * t;
}

v3d math_Plane::intersectWith(const math_Line& line) const
{
	if(!normal.dot(line.direction))	// does not intersect!
		// throw an exception??? no acceptable return value
		return nothing;
	float t = ( d - normal.dot(line.start) ) / ( normal.dot(line.direction) );
	// doesn't matter if t is negative or not
	return line.start + line.direction * t;
}
v3d math_Plane::intersectWith(const math_LineSegment& lineSeg) const
{
	if(!normal.dot(lineSeg.direction))	// does not intersect!
		// throw an exception??? no acceptable return value
		return nothing;
	float t = ( d - normal.dot(lineSeg.start) ) / ( normal.dot(lineSeg.direction) );
	// doesn't matter if t is negative or not
	float tstart = 0; 
	float tend;
	if(lineSeg.direction.x != 0)
	/*if(fabs(lineSeg.direction.x) > 0.0001)*/
	{
		tend = (lineSeg.end.x - lineSeg.start.x)/lineSeg.direction.x;
	}
	else /*if(fabs(lineSeg.direction.y) > 0.0001)*/if(lineSeg.direction.y != 0)
	{
		tend = (lineSeg.end.y - lineSeg.start.y)/lineSeg.direction.y;
	}
	else /*if(fabs(lineSeg.direction.z) > 0.0001)*/if(lineSeg.direction.z != 0)
	{
		tend = (lineSeg.end.z - lineSeg.start.z)/lineSeg.direction.z;
	}
	else	// problem with line formulation
		return nothing;

	//ccout << "tstart = " << tstart << " t = " << t << " tend = " << tend << newl;
	if( ((tstart <= t) && (t <= tend)) ||
		((tend <= t) && (t <= tstart)) )
		return lineSeg.start + lineSeg.direction * t;
	else // doesn't touch the line segment
		return nothing;
}


float math_Plane::distance(const v3d& point) const
{
	return normal.dot(point) - d;
}

///////////////////////////
//	end math_Plane
///////////////////////////

///////////////////////////
//	math_ExtrudedLineSegment
///////////////////////////

math_ExtrudedLineSegment::math_ExtrudedLineSegment()
{}

math_ExtrudedLineSegment::~math_ExtrudedLineSegment()
{}

void math_ExtrudedLineSegment::defineLineSegment(v3d _p1, v3d _p2)
{
	p1 = _p1;
	p2 = _p2;

	v3d p3 = p1 + v3d(0, 0, 1); // the plane creates a line where it intersects the x,y plane
	plane.definePlane(p1, p2, p3);
}

void math_ExtrudedLineSegment::setP1(v3d _p1)
{
	p1 = _p1;

	v3d p3 = p1 + v3d(0, 0, 1); // the plane creates a line where it intersects the x,y plane
	plane.definePlane(p1, p2, p3);
}

void math_ExtrudedLineSegment::setP2(v3d _p2)
{
	p2 = _p2;

	v3d p3 = p1 + v3d(0, 0, 1); // the plane creates a line where it intersects the x,y plane
	plane.definePlane(p1, p2, p3);
}

v3d math_ExtrudedLineSegment::getP1()
{
	return p1;
}

v3d math_ExtrudedLineSegment::getP2()
{
	return p2;
}

v3d math_ExtrudedLineSegment::getNormal()
{
	return plane.getNormal();
}


v3d math_ExtrudedLineSegment::intersectWith(const math_Ray& ray) const
{
	v3d p = plane.intersectWith(ray);
	v3d direction = (p2 - p1).getUnitVector();
	float t1 = 0; 
	float t2;
	float t;
	if(direction.x != 0)
	{
		t2 = (p2.x - p1.x)/direction.x;
		t = (p.x - p1.x)/direction.x;
	}
	else if(direction.y != 0)
	{
		t2 = (p2.y - p1.y)/direction.y;
		t = (p.y - p1.y)/direction.y;
	}
	else if(direction.z != 0)
	{
		t2 = (p2.z - p1.z)/direction.z;
		t = (p.z - p1.z)/direction.z;
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

v3d math_ExtrudedLineSegment::intersectWith(const math_Line& line) const
{
	v3d p = plane.intersectWith(line);
	v3d direction = (p2 - p1).getUnitVector();
	float t1 = 0; // 0  = (p1.x - p1.x)/ray.direction.x	// derived from line equation, to find t
	//ccout << ((p2.x - p1.x)/direction.x) << '=' << (p2.y - p1.y)/direction.y << '=' << (p2.z - p1.z)/direction.z << newl;
	float t2;
	float t;
	if(direction.x != 0)
	{
		t2 = (p2.x - p1.x)/direction.x;
		t = (p.x - p1.x)/direction.x;
	}
	else if(direction.y != 0)
	{
		t2 = (p2.y - p1.y)/direction.y;
		t = (p.y - p1.y)/direction.y;
	}
	else if(direction.z != 0)
	{
		t2 = (p2.z - p1.z)/direction.z;
		t = (p.z - p1.z)/direction.z;
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

v3d math_ExtrudedLineSegment::intersectWith(const math_LineSegment& lineSeg) const
{
	v3d p = plane.intersectWith(lineSeg);
	v3d direction = (p2 - p1).getUnitVector();
	float t1 = 0; // 0  = (p1.x - p1.x)/ray.direction.x	// derived from line equation, to find t
	//ccout << ((p2.x - p1.x)/direction.x) << '=' << (p2.y - p1.y)/direction.y << '=' << (p2.z - p1.z)/direction.z << newl;
	float t2;
	float t;
	if(direction.x != 0)
	{
		t2 = (p2.x - p1.x)/direction.x;
		t = (p.x - p1.x)/direction.x;
	}
	else if(direction.y != 0)
	{
		t2 = (p2.y - p1.y)/direction.y;
		t = (p.y - p1.y)/direction.y;
	}
	else if(direction.z != 0)
	{
		t2 = (p2.z - p1.z)/direction.z;
		t = (p.z - p1.z)/direction.z;
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
float math_ExtrudedLineSegment::distance(const v3d& point) const
{
	return normal.dot(point) - d;
}
*/
///////////////////////////
//	end math_ExtrudedLineSegment
///////////////////////////


///////////////////////////
//	math_BSPTree
///////////////////////////

math_BSPTree::math_BSPTree():
front(0),
back(0)
{
	objectList = new vid_SceneObjectHandleList;
}
math_BSPTree::~math_BSPTree()
{
	if(objectList)
	{
		delete objectList;
		objectList = 0;
	}
}

math_BSPTree::math_BSPTree(vid_SceneObjectHandleList* source)
{
	partition.definePlane( v3d(0,1), v3d(0,-1) ); // divide left and right side of the world
	vid_SceneObjectHandleList leftSide;
	math_Plane leftSubPartition;
	leftSubPartition.definePlane( v3d(1,0), v3d(-1,0) );
	vid_SceneObjectHandleList rightSide;
	math_Plane rightSubPartition;
	leftSubPartition.definePlane( v3d(1,0), v3d(-1,0) );

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

void math_BSPTree::buildBSPTree(vid_SceneObjectHandleList* source, math_Plane partition)
{
	if(source->objectCount == 1)
	{
		*objectList = *source;
		return;
	}

	partition.definePlane( v3d(0,1), v3d(0,-1) ); // divide left and right side of the world
	vid_SceneObjectHandleList leftSide;
	math_Plane leftSubPartition;
	leftSubPartition.definePlane( v3d(1,0), v3d(-1,0) );
	vid_SceneObjectHandleList rightSide;
	math_Plane rightSubPartition;
	leftSubPartition.definePlane( v3d(1,0), v3d(-1,0) );

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


///////////////////////////
//	end math_BSPTree
///////////////////////////


con_Cout& operator<<(con_Cout& out, const math_Ray& ray)
{
	return out << "ray: start = " << ray.start << " direction = " << ray.direction;
}
con_Cout& operator<<(con_Cout& out, const math_Line& line)
{
	return out << "line: point = " << line.start << " direction = " << line.direction;
}
con_Cout& operator<<(con_Cout& out, const math_LineSegment& lineseg)
{
	return out << "line segment: p1 = " << lineseg.start << " p2 = " << lineseg.end;
}
con_Cout& operator<<(con_Cout& out, const math_ExtrudedLineSegment& extlineseg)
{
	return out << "extruded line segment: p1 = " << extlineseg.p1 << " p2 = " << extlineseg.p2;
}
