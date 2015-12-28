// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __EZ_POINT_3D_H_INCLUDED__
#define __EZ_POINT_3D_H_INCLUDED__

#include "EzMath.h"
#include "EzMemObject.h"

class EZCORE_ENTRY EzPoint3 : public EzMemObject
{
public:
	//! Default constructor (null vector).
	EzPoint3() : X(0), Y(0), Z(0) {}
	//! Constructor with three different values
	EzPoint3(float nx, float ny, float nz) : X(nx), Y(ny), Z(nz) {}
	//! Constructor with the same value for all elements
	explicit EzPoint3(float n) : X(n), Y(n), Z(n) {}
	//! Copy constructor
	EzPoint3(const EzPoint3& other) : X(other.X), Y(other.Y), Z(other.Z) {}

	// operators

	EzPoint3 operator-() const { return EzPoint3(-X, -Y, -Z); }

	EzPoint3& operator=(const EzPoint3& other) { X = other.X; Y = other.Y; Z = other.Z; return *this; }

	EzPoint3 operator+(const EzPoint3& other) const { return EzPoint3(X + other.X, Y + other.Y, Z + other.Z); }
	EzPoint3& operator+=(const EzPoint3& other) { X+=other.X; Y+=other.Y; Z+=other.Z; return *this; }
	EzPoint3 operator+(const float val) const { return EzPoint3(X + val, Y + val, Z + val); }
	EzPoint3& operator+=(const float val) { X+=val; Y+=val; Z+=val; return *this; }

	EzPoint3 operator-(const EzPoint3& other) const { return EzPoint3(X - other.X, Y - other.Y, Z - other.Z); }
	EzPoint3& operator-=(const EzPoint3& other) { X-=other.X; Y-=other.Y; Z-=other.Z; return *this; }
	EzPoint3 operator-(const float val) const { return EzPoint3(X - val, Y - val, Z - val); }
	EzPoint3& operator-=(const float val) { X-=val; Y-=val; Z-=val; return *this; }

	EzPoint3 operator*(const EzPoint3& other) const { return EzPoint3(X * other.X, Y * other.Y, Z * other.Z); }
	EzPoint3& operator*=(const EzPoint3& other) { X*=other.X; Y*=other.Y; Z*=other.Z; return *this; }
	EzPoint3 operator*(const float v) const { return EzPoint3(X * v, Y * v, Z * v); }
	EzPoint3& operator*=(const float v) { X*=v; Y*=v; Z*=v; return *this; }

	EzPoint3 operator/(const EzPoint3& other) const { return EzPoint3(X / other.X, Y / other.Y, Z / other.Z); }
	EzPoint3& operator/=(const EzPoint3& other) { X/=other.X; Y/=other.Y; Z/=other.Z; return *this; }
	EzPoint3 operator/(const float v) const { float i=(float)1.0/v; return EzPoint3(X * i, Y * i, Z * i); }
	EzPoint3& operator/=(const float v) { float i=(float)1.0/v; X*=i; Y*=i; Z*=i; return *this; }

	bool operator<=(const EzPoint3&other) const { return X<=other.X && Y<=other.Y && Z<=other.Z;}
	bool operator>=(const EzPoint3&other) const { return X>=other.X && Y>=other.Y && Z>=other.Z;}
	bool operator<(const EzPoint3&other) const { return X<other.X && Y<other.Y && Z<other.Z;}
	bool operator>(const EzPoint3&other) const { return X>other.X && Y>other.Y && Z>other.Z;}

	//! use weak float compare
	bool operator==(const EzPoint3& other) const
	{
		return this->equals(other);
	}

	bool operator!=(const EzPoint3& other) const
	{
		return !this->equals(other);
	}

	// functions

	//! returns if this vector equals the other one, taking floating point rounding errors into account
	bool equals(const EzPoint3& other, const float tolerance = (float)ROUNDING_ERROR_32 ) const
	{
		return ::equals(X, other.X, tolerance) &&
			::equals(Y, other.Y, tolerance) &&
			::equals(Z, other.Z, tolerance);
	}

	EzPoint3& set(const float nx, const float ny, const float nz) {X=nx; Y=ny; Z=nz; return *this;}
	EzPoint3& set(const EzPoint3& p) {X=p.X; Y=p.Y; Z=p.Z;return *this;}

	//! Get length of the vector.
	float getLength() const { return (float) sqrt((double)(X*X + Y*Y + Z*Z)); }

	//! Get squared length of the vector.
	/** This is useful because it is much faster than getLength().
	\return Squared length of the vector. */
	float getLengthSQ() const { return X*X + Y*Y + Z*Z; }

	//! Get the dot product with another vector.
	float dotProduct(const EzPoint3& other) const
	{
		return X*other.X + Y*other.Y + Z*other.Z;
	}

	//! Get distance from another point.
	/** Here, the vector is interpreted as point in 3 dimensional space. */
	float getDistanceFrom(const EzPoint3& other) const
	{
		return EzPoint3(X - other.X, Y - other.Y, Z - other.Z).getLength();
	}

	//! Returns squared distance from another point.
	/** Here, the vector is interpreted as point in 3 dimensional space. */
	float getDistanceFromSQ(const EzPoint3& other) const
	{
		return EzPoint3(X - other.X, Y - other.Y, Z - other.Z).getLengthSQ();
	}

	//! Calculates the cross product with another vector.
	/** \param p Vector to multiply with.
	\return Crossproduct of this vector with p. */
	EzPoint3 crossProduct(const EzPoint3& p) const
	{
		return EzPoint3(Y * p.Z - Z * p.Y, Z * p.X - X * p.Z, X * p.Y - Y * p.X);
	}

	//! Returns if this vector interpreted as a point is on a line between two other points.
	/** It is assumed that the point is on the line.
	\param begin Beginning vector to compare between.
	\param end Ending vector to compare between.
	\return True if this vector is between begin and end, false if not. */
	bool isBetweenPoints(const EzPoint3& begin, const EzPoint3& end) const
	{
		const float f = (end - begin).getLengthSQ();
		return getDistanceFromSQ(begin) <= f &&
			getDistanceFromSQ(end) <= f;
	}

	//! Normalizes the vector.
	/** In case of the 0 vector the result is still 0, otherwise
	the length of the vector will be 1.
	\return Reference to this vector after normalization. */
	EzPoint3& normalize()
	{
		float length = (float)(X*X + Y*Y + Z*Z);
		if (::equals(length, 0.f))
			return *this;
		length = reciprocal_squareroot ( (float)length );
		X = (float)(X * length);
		Y = (float)(Y * length);
		Z = (float)(Z * length);
		return *this;
	}

	//! Sets the length of the vector to a new value
	EzPoint3& setLength(float newlength)
	{
		normalize();
		return (*this *= newlength);
	}

	//! Inverts the vector.
	EzPoint3& invert()
	{
		X *= -1.0f;
		Y *= -1.0f;
		Z *= -1.0f;
		return *this;
	}

	//! Rotates the vector by a specified number of degrees around the Y axis and the specified center.
	/** \param degrees Number of degrees to rotate around the Y axis.
	\param center The center of the rotation. */
	void rotateXZBy(double degrees, const EzPoint3& center=EzPoint3())
	{
		degrees *= DEGTORAD64;
		double cs = cos(degrees);
		double sn = sin(degrees);
		X -= center.X;
		Z -= center.Z;
		set((float)(X*cs - Z*sn), Y, (float)(X*sn + Z*cs));
		X += center.X;
		Z += center.Z;
	}

	//! Rotates the vector by a specified number of degrees around the Z axis and the specified center.
	/** \param degrees: Number of degrees to rotate around the Z axis.
	\param center: The center of the rotation. */
	void rotateXYBy(double degrees, const EzPoint3& center=EzPoint3())
	{
		degrees *= DEGTORAD64;
		double cs = cos(degrees);
		double sn = sin(degrees);
		X -= center.X;
		Y -= center.Y;
		set((float)(X*cs - Y*sn), (float)(X*sn + Y*cs), Z);
		X += center.X;
		Y += center.Y;
	}

	//! Rotates the vector by a specified number of degrees around the X axis and the specified center.
	/** \param degrees: Number of degrees to rotate around the X axis.
	\param center: The center of the rotation. */
	void rotateYZBy(double degrees, const EzPoint3& center=EzPoint3())
	{
		degrees *= DEGTORAD64;
		double cs = cos(degrees);
		double sn = sin(degrees);
		Z -= center.Z;
		Y -= center.Y;
		set(X, (float)(Y*cs - Z*sn), (float)(Y*sn + Z*cs));
		Z += center.Z;
		Y += center.Y;
	}

	//! Creates an interpolated vector between this vector and another vector.
	/** \param other The other vector to interpolate with.
	\param d Interpolation value between 0.0f (all the other vector) and 1.0f (all this vector).
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	\return An interpolated vector.  This vector is not modified. */
	EzPoint3 getInterpolated(const EzPoint3& other, double d) const
	{
		const double inv = 1.0 - d;
		return EzPoint3((float)(other.X*inv + X*d), (float)(other.Y*inv + Y*d), (float)(other.Z*inv + Z*d));
	}

	//! Creates a quadratically interpolated vector between this and two other vectors.
	/** \param v2 Second vector to interpolate with.
	\param v3 Third vector to interpolate with (maximum at 1.0f)
	\param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
	Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
	\return An interpolated vector. This vector is not modified. */
	EzPoint3 getInterpolated_quadratic(const EzPoint3& v2, const EzPoint3& v3, double d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const double inv = (float) 1.0 - d;
		const double mul0 = inv * inv;
		const double mul1 = (float) 2.0 * d * inv;
		const double mul2 = d * d;

		return EzPoint3 ((float)(X * mul0 + v2.X * mul1 + v3.X * mul2),
				(float)(Y * mul0 + v2.Y * mul1 + v3.Y * mul2),
				(float)(Z * mul0 + v2.Z * mul1 + v3.Z * mul2));
	}

	//! Sets this vector to the linearly interpolated vector between a and b.
	/** \param a first vector to interpolate with, maximum at 1.0f
	\param b second vector to interpolate with, maximum at 0.0f
	\param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	*/
	EzPoint3& interpolate(const EzPoint3& a, const EzPoint3& b, double d)
	{
		X = (float)((double)b.X + ( ( a.X - b.X ) * d ));
		Y = (float)((double)b.Y + ( ( a.Y - b.Y ) * d ));
		Z = (float)((double)b.Z + ( ( a.Z - b.Z ) * d ));
		return *this;
	}


	//! Get the rotations that would make a (0,0,1) direction vector point in the same direction as this direction vector.
	/** Thanks to Arras on the Irrlicht forums for this method.  This utility method is very useful for
	orienting scene nodes towards specific targets.  For example, if this vector represents the difference
	between two scene nodes, then applying the result of getHorizontalAngle() to one scene node will point
	it at the other one.
	Example code:
	// Where target and seeker are of type ISceneNode*
	const EzPoint3 toTarget(target->getAbsolutePosition() - seeker->getAbsolutePosition());
	const EzPoint3 requiredRotation = toTarget.getHorizontalAngle();
	seeker->setRotation(requiredRotation); 

	\return A rotation vector containing the X (pitch) and Y (raw) rotations (in degrees) that when applied to a 
	+Z (e.g. 0, 0, 1) direction vector would make it point in the same direction as this vector. The Z (roll) rotation 
	is always 0, since two Euler rotations are sufficient to point in any given direction. */
	EzPoint3 getHorizontalAngle() const
	{
		EzPoint3 angle;

		angle.Y = (float)(atan2(X, Z) * RADTODEG64);

		if (angle.Y < 0.0f)
			angle.Y += 360.0f;
		if (angle.Y >= 360.0f)
			angle.Y -= 360.0f;

		const double z1 = sqrt(X*X + Z*Z);

		angle.X = (float)(atan2(z1, (double)Y) * RADTODEG64 - 90.0);

		if (angle.X < 0.0f)
			angle.X += 360.0f;
		if (angle.X >= 360.0f)
			angle.X -= 360.0f;

		return angle;
	}

	//! Builds a direction vector from (this) rotation vector.
	/** This vector is assumed to be a rotation vector composed of 3 Euler angle rotations, in degrees.
	The implementation performs the same calculations as using a matrix to do the rotation.

	\param[in] forwards  The direction representing "forwards" which will be rotated by this vector. 
	If you do not provide a direction, then the +Z axis (0, 0, 1) will be assumed to be forwards.
	\return A direction vector calculated by rotating the forwards direction by the 3 Euler angles 
	(in degrees) represented by this vector. */
	EzPoint3 rotationToDirection(const EzPoint3 & forwards = EzPoint3(0, 0, 1)) const
	{
		const double cr = cos( DEGTORAD64 * X );
		const double sr = sin( DEGTORAD64 * X );
		const double cp = cos( DEGTORAD64 * Y );
		const double sp = sin( DEGTORAD64 * Y );
		const double cy = cos( DEGTORAD64 * Z );
		const double sy = sin( DEGTORAD64 * Z );

		const double srsp = sr*sp;
		const double crsp = cr*sp;

		const double pseudoMatrix[] = {
			( cp*cy ), ( cp*sy ), ( -sp ),
			( srsp*cy-cr*sy ), ( srsp*sy+cr*cy ), ( sr*cp ),
			( crsp*cy+sr*sy ), ( crsp*sy-sr*cy ), ( cr*cp )};

		return EzPoint3(
			(float)(forwards.X * pseudoMatrix[0] +
				forwards.Y * pseudoMatrix[3] +
				forwards.Z * pseudoMatrix[6]),
			(float)(forwards.X * pseudoMatrix[1] +
				forwards.Y * pseudoMatrix[4] +
				forwards.Z * pseudoMatrix[7]),
			(float)(forwards.X * pseudoMatrix[2] +
				forwards.Y * pseudoMatrix[5] +
				forwards.Z * pseudoMatrix[8]));
	}

	//! Fills an array of 4 values with the vector data (usually floats).
	/** Useful for setting in shader constants for example. The fourth value
	will always be 0. */
	void getAs4Values(float* array) const
	{
		array[0] = X;
		array[1] = Y;
		array[2] = Z;
		array[3] = 0;
	}

	//! X coordinate of the vector
	float X;
	//! Y coordinate of the vector
	float Y;
	//! Z coordinate of the vector
	float Z;
};


inline EzPoint3 operator*(float scalar, const EzPoint3& vector) 
{ 
	return vector*scalar; 
}


#endif

