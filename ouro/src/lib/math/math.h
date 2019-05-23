// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_MATH_H
#define OURO_MATH_H

#include <string>
#include "common/common.h"

// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#include <errno.h>
#endif
//#define USE_D3DX
#if OURO_PLATFORM == PLATFORM_WIN32 && defined(USE_D3DX)
#pragma comment(lib,"d3dx9.lib")
#include <d3dx9math.h>
typedef D3DXMATRIX								Matrix;
typedef D3DXQUATERNION							Quaternion;
typedef D3DXVECTOR2								Vector2;
typedef D3DXVECTOR3								Vector3;
typedef D3DXVECTOR4								Vector4;

#define OURO_PI									D3DX_PI
#define OURO_2PI									OURO_PI * 2
#define OURO_DegreeToRadian						D3DXToRadian
#define OURO_RadianToDegree						D3DXToDegree
	
#define OUROVec2Length							D3DXVec2Length
#define OUROVec2LengthSq							D3DXVec2LengthSq
#define OUROVec2Normalize						D3DXVec2Normalize
#define OUROVec2Dot								D3DXVec2Dot
#define OUROVec3Length							D3DXVec3Length
#define OUROVec3LengthSq							D3DXVec3LengthSq
#define OUROVec3Dot								D3DXVec3Dot
#define OUROVec3Cross							D3DXVec3Cross
#define OUROVec3Normalize						D3DXVec3Normalize
#define OUROVec3Lerp								D3DXVec3Lerp
#define OUROVec3Transform						D3DXVec3Transform
#define OUROVec3TransformCoord					D3DXVec3TransformCoord
#define OUROVec3TransformNormal					D3DXVec3TransformNormal
#define OUROVec4Transform						D3DXVec4Transform
#define OUROVec4Length							D3DXVec4Length
#define OUROVec4LengthSq							D3DXVec4LengthSq
#define OUROVec4Normalize						D3DXVec4Normalize
#define OUROVec4Lerp								D3DXVec4Lerp
#define OUROVec4Dot								D3DXVec4Dot
#define OUROMatrixIdentity						D3DXMatrixIdentity
#define OUROMatrixInverse						D3DXMatrixInverse
#define OUROMatrixRotationQuaternion				D3DXMatrixRotationQuaternion
#define OUROMatrixTranspose						D3DXMatrixTranspose
#define OUROMatrixfDeterminant					D3DXMatrixDeterminant
#define OUROMatrixOrthoLH						D3DXMatrixOrthoLH
#define OUROMatrixLookAtLH						D3DXMatrixLookAtLH
#define OUROMatrixMultiply						D3DXMatrixMultiply
#define OUROMatrixPerspectiveFovLH				D3DXMatrixPerspectiveFovLH
#define OUROMatrixRotationX						D3DXMatrixRotationX
#define OUROMatrixRotationY						D3DXMatrixRotationY
#define OUROMatrixRotationZ						D3DXMatrixRotationZ
#define OUROMatrixScaling						D3DXMatrixScaling
#define OUROMatrixTranslation					D3DXMatrixTranslation
#define OUROMatrixRotationYawPitchRoll			D3DXMatrixRotationYawPitchRoll
#define OUROQuaternionDot						D3DXQuaternionDot
#define OUROQuaternionNormalize					D3DXQuaternionNormalize
#define OUROQuaternionRotationMatrix				D3DXQuaternionRotationMatrix
#define OUROQuaternionSlerp						D3DXQuaternionSlerp
#define OUROQuaternionRotationAxis				D3DXQuaternionRotationAxis
#define OUROQuaternionMultiply					D3DXQuaternionMultiply
#define OUROQuaternionInverse					D3DXQuaternionInverse

template <class TYPE>
inline TYPE OUROClamp(TYPE value, TYPE minValue, TYPE maxValue)
{
	return value < minValue ? minValue :
	( value > maxValue ? maxValue : value);
}

#else
#include "G3D/g3dmath.h"
#include "G3D/Vector2.h"
#include "G3D/Vector3.h"
#include "G3D/Vector4.h"
#include "G3D/Matrix3.h"
#include "G3D/Quat.h"

typedef G3D::Matrix3							Matrix;
typedef G3D::Quat								Quaternion;
typedef G3D::Vector2							Vector2;
typedef G3D::Vector3							Vector3;
typedef G3D::Vector4							Vector4;

#define OURO_PI									3.1415926535898
#define OURO_2PI									6.2831853071796
#define OURO_DegreeToRadian						G3D::toRadians
#define OURO_RadianToDegree						G3D::toRadians
	
#define OUROVec2Length(v)						(v)->length()
#define OUROVec2LengthSq(v)						(v)->squaredLength()
#define OUROVec2Normalize(v, vv)					(v)->normalise()
#define OUROVec2Dot(v, vv)						(v)->dot(static_cast<const G3D::Vector2 &>(*(vv)))
#define OUROVec3Length(v)						(v)->length()
#define OUROVec3LengthSq(v)						(v)->squaredLength()
#define OUROVec3Dot(v, vv)						(v)->dot(static_cast<const G3D::Vector3 &>(*(vv)))
#define OUROVec3Cross(v)							(v)->cross()
#define OUROVec3Normalize(v, vv)					(v)->normalise()
#define OUROVec3Lerp(v)							(v)->lerp()
#define OUROVec3Transform(v)						D3DXVec3Transform
#define OUROVec3TransformCoord(v)				D3DXVec3TransformCoord
#define OUROVec3TransformNormal(v)				D3DXVec3TransformNormal
#define OUROVec4Transform(v)						D3DXVec4Transform
#define OUROVec4Length(v)						(v)->length()
#define OUROVec4LengthSq(v)						(v)->squaredLength()
#define OUROVec4Normalize(v, vv)					(v)->normalise()
#define OUROVec4Lerp(v)							(v)->lerp()
#define OUROVec4Dot(v, vv)						(v)->dot(static_cast<const G3D::Vector4 &>(*(vv)))
#define OUROMatrixIdentity						Matrix3::identity()
#define OUROMatrixInverse(v)						Matrix3::inverse()
#define OUROMatrixRotationQuaternion(v)			D3DXMatrixRotationQuaternion
#define OUROMatrixTranspose(v)					D3DXMatrixTranspose
#define OUROMatrixfDeterminant(v)				D3DXMatrixDeterminant
#define OUROMatrixOrthoLH(v)						D3DXMatrixOrthoLH
#define OUROMatrixLookAtLH(v)					D3DXMatrixLookAtLH
#define OUROMatrixMultiply(v)					D3DXMatrixMultiply
#define OUROMatrixPerspectiveFovLH(v)			D3DXMatrixPerspectiveFovLH
#define OUROMatrixRotationX(v)					D3DXMatrixRotationX
#define OUROMatrixRotationY(v)					D3DXMatrixRotationY
#define OUROMatrixRotationZ(v)					D3DXMatrixRotationZ
#define OUROMatrixScaling(v)						D3DXMatrixScaling
#define OUROMatrixTranslation(v)					D3DXMatrixTranslation
#define OUROMatrixRotationYawPitchRoll(v)		D3DXMatrixRotationYawPitchRoll
#define OUROQuaternionDot(v)						D3DXQuaternionDot
#define OUROQuaternionNormalize(v)				D3DXQuaternionNormalize
#define OUROQuaternionRotationMatrix(v)			D3DXQuaternionRotationMatrix
#define OUROQuaternionSlerp(v)					D3DXQuaternionSlerp
#define OUROQuaternionRotationAxis(v)			D3DXQuaternionRotationAxis
#define OUROQuaternionMultiply(v)				D3DXQuaternionMultiply
#define OUROQuaternionInverse(v)					D3DXQuaternionInverse
#define OUROClamp								G3D::clamp
#endif

// Calculate 2d length from 2 3d vectors ignore y
inline float OUROVec3CalcVec2Length(const Vector3& v1, const Vector3& v2)
{
	float x = v1.x - v2.x;
	float z = v1.z - v2.z;
	return sqrt(x*x + z*z);
}

inline float int82angle(Ouroboros::int8 angle, bool half = false)
{
	return float(angle) * float((OURO_PI / (half ? 254.f : 128.f)));
}

inline Ouroboros::int8 angle2int8(float v, bool half = false)
{
	Ouroboros::int8 angle = 0;
	if(!half)
	{
		angle = (Ouroboros::int8)floorf((v * 128.f) / float(OURO_PI) + 0.5f);
	}
	else
	{
		angle = (Ouroboros::int8)OUROClamp(floorf( (v * 254.f) / float(OURO_PI) + 0.5f ), -128.f, 127.f );
	}

	return angle;
}

typedef Vector3 Position3D; // indicates the 3D position variable type
typedef OUROShared_ptr< std::vector<Position3D> > VECTOR_POS3D_PTR; // smart pointer type declaration pointing to the Position3D array

struct Direction3D // indicates direction position variable type
{
	Direction3D():dir(0.f, 0.f, 0.f) {};
	Direction3D(const Vector3 & v):dir(v){}
	Direction3D(float r, float p, float y):dir(r, p, y){}
	Direction3D(const Direction3D & v) :dir(v.dir){}

	Direction3D& operator=(const Direction3D& v)
	{
		dir = v.dir;
		return *this;
	}

	float roll() const{ return dir.x; }		
	float pitch() const{ return dir.y; }		
	float yaw() const{ return dir.z; }		

	void roll(float v){ dir.x = v; }		
	void pitch(float v){ dir.y = v; }		
	void yaw(float v){ dir.z = v; }	

	// roll, pitch, yaw
	Vector3 dir;
};

/** Comparison of floating point numbers*/
#define floatEqual(v1, v3) (abs(v1 - v2) < std::numeric_limits<float>::epsilon())
inline bool almostEqual(const float f1, const float f2, const float epsilon = 0.0004f)
{
	return fabsf( f1 - f2 ) < epsilon;
}

inline bool almostEqual(const double d1, const double d2, const double epsilon = 0.0004)
{
	return fabs( d1 - d2 ) < epsilon;
}

inline bool almostZero(const float f, const float epsilon = 0.0004f)
{
	return f < epsilon && f > -epsilon;
}

inline bool almostZero(const double d, const double epsilon = 0.0004)
{
	return d < epsilon && d > -epsilon;
}

template<typename T>
inline bool almostEqual(const T& c1, const T& c2, const float epsilon = 0.0004f)
{
	if( c1.size() != c2.size() )
		return false;
	typename T::const_iterator iter1 = c1.begin();
	typename T::const_iterator iter2 = c2.begin();
	for( ; iter1 != c1.end(); ++iter1, ++iter2 )
		if( !almostEqual( *iter1, *iter2, epsilon ) )
			return false;
	return true;
}

#endif // OURO_MATH_H
