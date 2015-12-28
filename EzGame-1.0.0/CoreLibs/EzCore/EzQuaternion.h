// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __EZ_QUATERNION_H_INCLUDED__
#define __EZ_QUATERNION_H_INCLUDED__

#include "EzMath.h"
#include "EzMatrix4.h"
#include "EzPoint3.h"
#include "EzMemObject.h"


//! Quaternion class for representing rotations.
/** It provides cheap combinations and avoids gimbal locks.
Also useful for interpolations. */
class EZCORE_ENTRY EzQuaternion : public EzMemObject
{
	public:

		//! Default Constructor
		EzQuaternion() : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f) {}

		//! Constructor
		EzQuaternion(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) { }

		//! Constructor which converts euler angles (radians) to a EzQuaternion
		EzQuaternion(float x, float y, float z);

		//! Constructor which converts euler angles (radians) to a EzQuaternion
		EzQuaternion(const EzPoint3& vec);

		//! Constructor which converts a matrix to a EzQuaternion
		EzQuaternion(const EzMatrix4& mat);

		//! Equalilty operator
		bool operator==(const EzQuaternion& other) const;

		//! Assignment operator
		inline EzQuaternion& operator=(const EzQuaternion& other);

		//! Matrix assignment operator
		inline EzQuaternion& operator=(const EzMatrix4& other);

		//! Add operator
		EzQuaternion operator+(const EzQuaternion& other) const;

		//! Multiplication operator
		EzQuaternion operator*(const EzQuaternion& other) const;

		//! Multiplication operator with scalar
		EzQuaternion operator*(float s) const;

		//! Multiplication operator with scalar
		EzQuaternion& operator*=(float s);

		//! Multiplication operator
		EzPoint3 operator*(const EzPoint3& v) const;

		//! Multiplication operator
		EzQuaternion& operator*=(const EzQuaternion& other);

		EzQuaternion operator-() const;

		//! Calculates the dot product
		inline float dotProduct(const EzQuaternion& other) const;

		//! Sets new EzQuaternion
		inline EzQuaternion& set(float x, float y, float z, float w);

		//! Sets new EzQuaternion based on euler angles (radians)
		inline EzQuaternion& set(float x, float y, float z);

		//! Sets new EzQuaternion based on euler angles (radians)
		inline EzQuaternion& set(const EzPoint3& vec);

		//! Normalizes the EzQuaternion
		inline EzQuaternion& normalize();

		//! Creates a matrix from this EzQuaternion
		EzMatrix4 getMatrix() const;

		//! Creates a matrix from this EzQuaternion
		void getMatrix( EzMatrix4 &dest ) const;

		//! Creates a matrix from this EzQuaternion
		inline void getMatrix_transposed( EzMatrix4 &dest ) const;

		//! Inverts this EzQuaternion
		EzQuaternion& makeInverse();

		//! Set this EzQuaternion to the result of the interpolation between two quaternions
		EzQuaternion& slerp( EzQuaternion q1, EzQuaternion q2, float interpolate );

		//! Create EzQuaternion from rotation angle and rotation axis.
		/** Axis must be unit length.
		The EzQuaternion representing the rotation is
		q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k).
		\param angle Rotation Angle in radians.
		\param axis Rotation axis. */
		EzQuaternion& fromAngleAxis (float angle, const EzPoint3& axis);

		//! Fills an angle (radians) around an axis (unit vector)
		void toAngleAxis (float &angle, EzPoint3& axis) const;

		//! Output this EzQuaternion to an euler angle (radians)
		void toEuler(EzPoint3& euler) const;

		//! Set EzQuaternion to identity
		EzQuaternion& makeIdentity();

		//! Set EzQuaternion to represent a rotation from one vector to another.
		EzQuaternion& rotationFromTo(const EzPoint3& from, const EzPoint3& to);

		//! Quaternion elements.
		float X, Y, Z, W;
};


// Constructor which converts euler angles to a EzQuaternion
inline EzQuaternion::EzQuaternion(float x, float y, float z)
{
	set(x,y,z);
}


// Constructor which converts euler angles to a EzQuaternion
inline EzQuaternion::EzQuaternion(const EzPoint3& vec)
{
	set(vec.X,vec.Y,vec.Z);
}


// Constructor which converts a matrix to a EzQuaternion
inline EzQuaternion::EzQuaternion(const EzMatrix4& mat)
{
	(*this) = mat;
}


// equal operator
inline bool EzQuaternion::operator==(const EzQuaternion& other) const
{
	return ((X == other.X) &&
		(Y == other.Y) &&
		(Z == other.Z) &&
		(W == other.W));
}


// assignment operator
inline EzQuaternion& EzQuaternion::operator=(const EzQuaternion& other)
{
	X = other.X;
	Y = other.Y;
	Z = other.Z;
	W = other.W;
	return *this;
}


// matrix assignment operator
inline EzQuaternion& EzQuaternion::operator=(const EzMatrix4& m)
{
	const float diag = m(0,0) + m(1,1) + m(2,2) + 1;

	if( diag > 0.0f )
	{
		const float scale = sqrtf(diag) * 2.0f; // get scale from diagonal

		// TODO: speed this up
		X = ( m(2,1) - m(1,2)) / scale;
		Y = ( m(0,2) - m(2,0)) / scale;
		Z = ( m(1,0) - m(0,1)) / scale;
		W = 0.25f * scale;
	}
	else
	{
		if ( m(0,0) > m(1,1) && m(0,0) > m(2,2))
		{
			// 1st element of diag is greatest value
			// find scale according to 1st element, and double it
			const float scale = sqrtf( 1.0f + m(0,0) - m(1,1) - m(2,2)) * 2.0f;

			// TODO: speed this up
			X = 0.25f * scale;
			Y = (m(0,1) + m(1,0)) / scale;
			Z = (m(2,0) + m(0,2)) / scale;
			W = (m(2,1) - m(1,2)) / scale;
		}
		else if ( m(1,1) > m(2,2))
		{
			// 2nd element of diag is greatest value
			// find scale according to 2nd element, and double it
			const float scale = sqrtf( 1.0f + m(1,1) - m(0,0) - m(2,2)) * 2.0f;

			// TODO: speed this up
			X = (m(0,1) + m(1,0) ) / scale;
			Y = 0.25f * scale;
			Z = (m(1,2) + m(2,1) ) / scale;
			W = (m(0,2) - m(2,0) ) / scale;
		}
		else
		{
			// 3rd element of diag is greatest value
			// find scale according to 3rd element, and double it
			const float scale = sqrtf( 1.0f + m(2,2) - m(0,0) - m(1,1)) * 2.0f;

			// TODO: speed this up
			X = (m(0,2) + m(2,0)) / scale;
			Y = (m(1,2) + m(2,1)) / scale;
			Z = 0.25f * scale;
			W = (m(1,0) - m(0,1)) / scale;
		}
	}

	return normalize();
}


// multiplication operator
inline EzQuaternion EzQuaternion::operator*(const EzQuaternion& other) const
{
	EzQuaternion tmp;

	tmp.W = (other.W * W) - (other.X * X) - (other.Y * Y) - (other.Z * Z);
	tmp.X = (other.W * X) + (other.X * W) + (other.Y * Z) - (other.Z * Y);
	tmp.Y = (other.W * Y) + (other.Y * W) + (other.Z * X) - (other.X * Z);
	tmp.Z = (other.W * Z) + (other.Z * W) + (other.X * Y) - (other.Y * X);

	return tmp;
}


// multiplication operator
inline EzQuaternion EzQuaternion::operator*(float s) const
{
	return EzQuaternion(s*X, s*Y, s*Z, s*W);
}

// multiplication operator
inline EzQuaternion& EzQuaternion::operator*=(float s)
{
	X*=s;
	Y*=s;
	Z*=s;
	W*=s;
	return *this;
}

// multiplication operator
inline EzQuaternion& EzQuaternion::operator*=(const EzQuaternion& other)
{
	return (*this = other * (*this));
}

// add operator
inline EzQuaternion EzQuaternion::operator+(const EzQuaternion& b) const
{
	return EzQuaternion(X+b.X, Y+b.Y, Z+b.Z, W+b.W);
}


// Creates a matrix from this EzQuaternion
inline EzMatrix4 EzQuaternion::getMatrix() const
{
	EzMatrix4 m;
	getMatrix_transposed(m);
	return m;
}


// Creates a matrix from this EzQuaternion
inline void EzQuaternion::getMatrix( EzMatrix4 &dest ) const
{
	dest[0] = 1.0f - 2.0f*Y*Y - 2.0f*Z*Z;
	dest[1] = 2.0f*X*Y + 2.0f*Z*W;
	dest[2] = 2.0f*X*Z - 2.0f*Y*W;
	dest[3] = 0.0f;

	dest[4] = 2.0f*X*Y - 2.0f*Z*W;
	dest[5] = 1.0f - 2.0f*X*X - 2.0f*Z*Z;
	dest[6] = 2.0f*Z*Y + 2.0f*X*W;
	dest[7] = 0.0f;

	dest[8] = 2.0f*X*Z + 2.0f*Y*W;
	dest[9] = 2.0f*Z*Y - 2.0f*X*W;
	dest[10] = 1.0f - 2.0f*X*X - 2.0f*Y*Y;
	dest[11] = 0.0f;

	dest[12] = 0.f;
	dest[13] = 0.f;
	dest[14] = 0.f;
	dest[15] = 1.f;
}

// Creates a matrix from this EzQuaternion
inline void EzQuaternion::getMatrix_transposed( EzMatrix4 &dest ) const
{
	dest[0] = 1.0f - 2.0f*Y*Y - 2.0f*Z*Z;
	dest[4] = 2.0f*X*Y + 2.0f*Z*W;
	dest[8] = 2.0f*X*Z - 2.0f*Y*W;
	dest[12] = 0.0f;

	dest[1] = 2.0f*X*Y - 2.0f*Z*W;
	dest[5] = 1.0f - 2.0f*X*X - 2.0f*Z*Z;
	dest[9] = 2.0f*Z*Y + 2.0f*X*W;
	dest[13] = 0.0f;

	dest[2] = 2.0f*X*Z + 2.0f*Y*W;
	dest[6] = 2.0f*Z*Y - 2.0f*X*W;
	dest[10] = 1.0f - 2.0f*X*X - 2.0f*Y*Y;
	dest[14] = 0.0f;

	dest[3] = 0.f;
	dest[7] = 0.f;
	dest[11] = 0.f;
	dest[15] = 1.f;
}



// Inverts this EzQuaternion
inline EzQuaternion& EzQuaternion::makeInverse()
{
	X = -X; Y = -Y; Z = -Z;
	return *this;
}

// sets new EzQuaternion
inline EzQuaternion& EzQuaternion::set(float x, float y, float z, float w)
{
	X = x;
	Y = y;
	Z = z;
	W = w;
	return *this;
}


// sets new EzQuaternion based on euler angles
inline EzQuaternion& EzQuaternion::set(float x, float y, float z)
{
	double angle;

	angle = x * 0.5;
	const double sr = sin(angle);
	const double cr = cos(angle);

	angle = y * 0.5;
	const double sp = sin(angle);
	const double cp = cos(angle);

	angle = z * 0.5;
	const double sy = sin(angle);
	const double cy = cos(angle);

	const double cpcy = cp * cy;
	const double spcy = sp * cy;
	const double cpsy = cp * sy;
	const double spsy = sp * sy;

	X = (float)(sr * cpcy - cr * spsy);
	Y = (float)(cr * spcy + sr * cpsy);
	Z = (float)(cr * cpsy - sr * spcy);
	W = (float)(cr * cpcy + sr * spsy);

	return normalize();
}

// sets new EzQuaternion based on euler angles
inline EzQuaternion& EzQuaternion::set(const EzPoint3& vec)
{
	return set(vec.X, vec.Y, vec.Z);
}

// normalizes the EzQuaternion
inline EzQuaternion& EzQuaternion::normalize()
{
	const float n = X*X + Y*Y + Z*Z + W*W;

	if (n == 1)
		return *this;

	//n = 1.0f / sqrtf(n);
	return (*this *= reciprocal_squareroot ( n ));
}


// set this EzQuaternion to the result of the interpolation between two quaternions
inline EzQuaternion& EzQuaternion::slerp(EzQuaternion q1, EzQuaternion q2, float time)
{
	float angle = q1.dotProduct(q2);

	if (angle < 0.0f)
	{
		q1 *= -1.0f;
		angle *= -1.0f;
	}

	float scale;
	float invscale;

	if ((angle + 1.0f) > 0.05f)
	{
		if ((1.0f - angle) >= 0.05f) // spherical interpolation
		{
			const float theta = acosf(angle);
			const float invsintheta = reciprocal(sinf(theta));
			scale = sinf(theta * (1.0f-time)) * invsintheta;
			invscale = sinf(theta * time) * invsintheta;
		}
		else // linear interploation
		{
			scale = 1.0f - time;
			invscale = time;
		}
	}
	else
	{
		q2.set(-q1.Y, q1.X, -q1.W, q1.Z);
		scale = sinf(PI * (0.5f - time));
		invscale = sinf(PI * time);
	}

	return (*this = (q1*scale) + (q2*invscale));
}


// calculates the dot product
inline float EzQuaternion::dotProduct(const EzQuaternion& q2) const
{
	return (X * q2.X) + (Y * q2.Y) + (Z * q2.Z) + (W * q2.W);
}


//! axis must be unit length
//! angle in radians
inline EzQuaternion& EzQuaternion::fromAngleAxis(float angle, const EzPoint3& axis)
{
	const float fHalfAngle = 0.5f*angle;
	const float fSin = sinf(fHalfAngle);
	W = cosf(fHalfAngle);
	X = fSin*axis.X;
	Y = fSin*axis.Y;
	Z = fSin*axis.Z;
	return *this;
}


inline void EzQuaternion::toAngleAxis(float &angle, EzPoint3 &axis) const
{
	const float scale = sqrtf(X*X + Y*Y + Z*Z);

	if (iszero(scale) || W > 1.0f || W < -1.0f)
	{
		angle = 0.0f;
		axis.X = 0.0f;
		axis.Y = 1.0f;
		axis.Z = 0.0f;
	}
	else
	{
		const float invscale = reciprocal(scale);
		angle = 2.0f * acosf(W);
		axis.X = X * invscale;
		axis.Y = Y * invscale;
		axis.Z = Z * invscale;
	}
}

inline void EzQuaternion::toEuler(EzPoint3& euler) const
{
	const double sqw = W*W;
	const double sqx = X*X;
	const double sqy = Y*Y;
	const double sqz = Z*Z;

	// heading = rotation about z-axis
	euler.Z = (float) (atan2(2.0 * (X*Y +Z*W),(sqx - sqy - sqz + sqw)));

	// bank = rotation about x-axis
	euler.X = (float) (atan2(2.0 * (Y*Z +X*W),(-sqx - sqy + sqz + sqw)));

	// attitude = rotation about y-axis
	euler.Y = asinf( clamp(-2.0f * (X*Z - Y*W), -1.0f, 1.0f) );
}


inline EzPoint3 EzQuaternion::operator* (const EzPoint3& v) const
{
	// nVidia SDK implementation

	EzPoint3 uv, uuv;
	EzPoint3 qvec(X, Y, Z);
	uv = qvec.crossProduct(v);
	uuv = qvec.crossProduct(uv);
	uv *= (2.0f * W);
	uuv *= 2.0f;

	return v + uv + uuv;
}

// set EzQuaternion to identity
inline EzQuaternion& EzQuaternion::makeIdentity()
{
	W = 1.f;
	X = 0.f;
	Y = 0.f;
	Z = 0.f;
	return *this;
}

inline EzQuaternion& EzQuaternion::rotationFromTo(const EzPoint3& from, const EzPoint3& to)
{
	// Based on Stan Melax's article in Game Programming Gems
	// Copy, since cannot modify local
	EzPoint3 v0 = from;
	EzPoint3 v1 = to;
	v0.normalize();
	v1.normalize();

	const float d = v0.dotProduct(v1);
	if (d >= 1.0f) // If dot == 1, vectors are the same
	{
		return makeIdentity();
	}

	const float s = sqrtf( (1+d)*2 ); // optimize inv_sqrt
	const float invs = 1.f / s;
	const EzPoint3 c = v0.crossProduct(v1)*invs;

	X = c.X;
	Y = c.Y;
	Z = c.Z;
	W = s * 0.5f;

	return *this;
}

inline EzQuaternion EzQuaternion::operator-() const
{
	return EzQuaternion(-X, -Y, -Z, -W);
}


#endif

