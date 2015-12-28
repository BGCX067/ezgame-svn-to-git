// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __POINT_2D_H_INCLUDED__
#define __POINT_2D_H_INCLUDED__

#include "EzMath.h"
#include "EzMemObject.h"


//! 2d vector template class with lots of operators and methods.
class EZCORE_ENTRY EzPoint2 : public EzMemObject
{
public:
	//! Default constructor (null vector)
	EzPoint2() : X(0), Y(0) {}
	//! Constructor with two different values
	EzPoint2(float nx, float ny) : X(nx), Y(ny) {}
	//! Constructor with the same value for both members
	explicit EzPoint2(float n) : X(n), Y(n) {}
	//! Copy constructor
	EzPoint2(const EzPoint2& other) : X(other.X), Y(other.Y) {}

	// operators

	EzPoint2 operator-() const { return EzPoint2(-X, -Y); }

	EzPoint2& operator=(const EzPoint2& other) { X = other.X; Y = other.Y; return *this; }

	EzPoint2 operator+(const EzPoint2& other) const { return EzPoint2(X + other.X, Y + other.Y); }
	EzPoint2& operator+=(const EzPoint2& other) { X+=other.X; Y+=other.Y; return *this; }
	EzPoint2 operator+(const float v) const { return EzPoint2(X + v, Y + v); }
	EzPoint2& operator+=(const float v) { X+=v; Y+=v; return *this; }

	EzPoint2 operator-(const EzPoint2& other) const { return EzPoint2(X - other.X, Y - other.Y); }
	EzPoint2& operator-=(const EzPoint2& other) { X-=other.X; Y-=other.Y; return *this; }
	EzPoint2 operator-(const float v) const { return EzPoint2(X - v, Y - v); }
	EzPoint2& operator-=(const float v) { X-=v; Y-=v; return *this; }

	EzPoint2 operator*(const EzPoint2& other) const { return EzPoint2(X * other.X, Y * other.Y); }
	EzPoint2& operator*=(const EzPoint2& other) { X*=other.X; Y*=other.Y; return *this; }
	EzPoint2 operator*(const float v) const { return EzPoint2(X * v, Y * v); }
	EzPoint2& operator*=(const float v) { X*=v; Y*=v; return *this; }

	EzPoint2 operator/(const EzPoint2& other) const { return EzPoint2(X / other.X, Y / other.Y); }
	EzPoint2& operator/=(const EzPoint2& other) { X/=other.X; Y/=other.Y; return *this; }
	EzPoint2 operator/(const float v) const { return EzPoint2(X / v, Y / v); }
	EzPoint2& operator/=(const float v) { X/=v; Y/=v; return *this; }

	bool operator<=(const EzPoint2&other) const { return X<=other.X && Y<=other.Y; }
	bool operator>=(const EzPoint2&other) const { return X>=other.X && Y>=other.Y; }

	bool operator<(const EzPoint2&other) const { return X<other.X && Y<other.Y; }
	bool operator>(const EzPoint2&other) const { return X>other.X && Y>other.Y; }

	bool operator==(const EzPoint2& other) const { return equals(other); }
	bool operator!=(const EzPoint2& other) const { return !equals(other); }

	// functions

	//! Checks if this vector equals the other one.
	/** Takes floating point rounding errors into account.
	\param other Vector to compare with.
	\return True if the two vector are (almost) equal, else false. */
	bool equals(const EzPoint2& other) const
	{
		return ::equals(X, other.X) && ::equals(Y, other.Y);
	}

	EzPoint2& set(float nx, float ny) {X=nx; Y=ny; return *this; }
	EzPoint2& set(const EzPoint2& p) { X=p.X; Y=p.Y; return *this; }

	//! Gets the length of the vector.
	/** \return The length of the vector. */
	float getLength() const { return (float)sqrt((double)(X*X + Y*Y)); }

	//! Get the squared length of this vector
	/** This is useful because it is much faster than getLength().
	\return The squared length of the vector. */
	float getLengthSQ() const { return X*X + Y*Y; }

	//! Get the dot product of this vector with another.
	/** \param other Other vector to take dot product with.
	\return The dot product of the two vectors. */
	float dotProduct(const EzPoint2& other) const
	{
		return X*other.X + Y*other.Y;
	}

	//! Gets distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Distance from other point. */
	float getDistanceFrom(const EzPoint2& other) const
	{
		return EzPoint2(X - other.X, Y - other.Y).getLength();
	}

	//! Returns squared distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Squared distance from other point. */
	float getDistanceFromSQ(const EzPoint2& other) const
	{
		return EzPoint2(X - other.X, Y - other.Y).getLengthSQ();
	}

	//! rotates the point anticlockwise around a center by an amount of degrees.
	/** \param degrees Amount of degrees to rotate by, anticlockwise.
	\param center Rotation center.
	\return This vector after transformation. */
	EzPoint2& rotateBy(double degrees, const EzPoint2& center=EzPoint2())
	{
		degrees *= DEGTORAD64;
		const double cs = cos(degrees);
		const double sn = sin(degrees);

		X -= center.X;
		Y -= center.Y;

		set((float)(X*cs - Y*sn), (float)(X*sn + Y*cs));

		X += center.X;
		Y += center.Y;
		return *this;
	}

	//! Normalize the vector.
	/** The null vector is left untouched.
	\return Reference to this vector, after normalization. */
	EzPoint2& normalize()
	{
		float length = (float)(X*X + Y*Y);
		if (::equals(length, 0.f))
			return *this;
		length = reciprocal_squareroot ( length );
		X = (float)(X * length);
		Y = (float)(Y * length);
		return *this;
	}

	//! Calculates the angle of this vector in degrees in the trigonometric sense.
	/** 0 is to the left (9 o'clock), values increase clockwise.
	This method has been suggested by Pr3t3nd3r.
	\return Returns a value between 0 and 360. */
	double getAngleTrig() const
	{
		if (X == 0)
			return Y < 0 ? 270 : 90;
		else
		if (Y == 0)
			return X < 0 ? 180 : 0;

		if ( Y > 0)
			if (X > 0)
				return atan(Y/X) * RADTODEG64;
			else
				return 180.0-atan(Y/-X) * RADTODEG64;
		else
			if (X > 0)
				return 360.0-atan(-Y/X) * RADTODEG64;
			else
				return 180.0+atan(-Y/-X) * RADTODEG64;
	}

	//! Calculates the angle of this vector in degrees in the counter trigonometric sense.
	/** 0 is to the right (3 o'clock), values increase counter-clockwise.
	\return Returns a value between 0 and 360. */
	inline double getAngle() const
	{
		if (Y == 0) // corrected thanks to a suggestion by Jox
			return X < 0 ? 180 : 0;
		else if (X == 0)
			return Y < 0 ? 90 : 270;

		double tmp = Y / getLength();
		tmp = atan(sqrt(1 - tmp*tmp) / tmp) * RADTODEG64;

		if (X>0 && Y>0)
			return tmp + 270;
		else
		if (X>0 && Y<0)
			return tmp + 90;
		else
		if (X<0 && Y<0)
			return 90 - tmp;
		else
		if (X<0 && Y>0)
			return 270 - tmp;

		return tmp;
	}

	//! Calculates the angle between this vector and another one in degree.
	/** \param b Other vector to test with.
	\return Returns a value between 0 and 90. */
	inline double getAngleWith(const EzPoint2& b) const
	{
		double tmp = X*b.X + Y*b.Y;

		if (tmp == 0.0)
			return 90.0;

		tmp = tmp / sqrt((double)((X*X + Y*Y) * (b.X*b.X + b.Y*b.Y)));
		if (tmp < 0.0)
			tmp = -tmp;

		return atan(sqrt(1 - tmp*tmp) / tmp) * RADTODEG64;
	}

	//! Returns if this vector interpreted as a point is on a line between two other points.
	/** It is assumed that the point is on the line.
	\param begin Beginning vector to compare between.
	\param end Ending vector to compare between.
	\return True if this vector is between begin and end, false if not. */
	bool isBetweenPoints(const EzPoint2& begin, const EzPoint2& end) const
	{
		if (begin.X != end.X)
		{
			return ((begin.X <= X && X <= end.X) ||
				(begin.X >= X && X >= end.X));
		}
		else
		{
			return ((begin.Y <= Y && Y <= end.Y) ||
				(begin.Y >= Y && Y >= end.Y));
		}
	}

	//! Creates an interpolated vector between this vector and another vector.
	/** \param other The other vector to interpolate with.
	\param d Interpolation value between 0.0f (all the other vector) and 1.0f (all this vector).
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	\return An interpolated vector.  This vector is not modified. */
	EzPoint2 getInterpolated(const EzPoint2& other, double d) const
	{
		EZASSERT(d >= 0.0 || d <= 1.0);
		double inv = 1.0f - d;
		return EzPoint2((float)(other.X*inv + X*d), (float)(other.Y*inv + Y*d));
	}

	//! Creates a quadratically interpolated vector between this and two other vectors.
	/** \param v2 Second vector to interpolate with.
	\param v3 Third vector to interpolate with (maximum at 1.0f)
	\param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
	Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
	\return An interpolated vector. This vector is not modified. */
	EzPoint2 getInterpolated_quadratic(const EzPoint2& v2, const EzPoint2& v3, double d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const double inv = 1.0f - d;
		const double mul0 = inv * inv;
		const double mul1 = 2.0f * d * inv;
		const double mul2 = d * d;

		return EzPoint2 ( (float)(X * mul0 + v2.X * mul1 + v3.X * mul2),
					(float)(Y * mul0 + v2.Y * mul1 + v3.Y * mul2));
	}

	//! Sets this vector to the linearly interpolated vector between a and b.
	/** \param a first vector to interpolate with, maximum at 1.0f
	\param b second vector to interpolate with, maximum at 0.0f
	\param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	*/
	EzPoint2& interpolate(const EzPoint2& a, const EzPoint2& b, double d)
	{
		X = (float)((double)b.X + ( ( a.X - b.X ) * d ));
		Y = (float)((double)b.Y + ( ( a.Y - b.Y ) * d ));
		return *this;
	}

	//! X coordinate of vector.
	float X;
	//! Y coordinate of vector.
	float Y;
};

inline EzPoint2 operator*(const float &scalar, const EzPoint2& vector) { return vector*scalar; }


#endif

