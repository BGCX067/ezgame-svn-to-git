// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_TRIANGLE_3D_H_INCLUDED__
#define __IRR_TRIANGLE_3D_H_INCLUDED__

#include "EzPoint3.h"
#include "EzLine3d.h"
#include "EzPlane3d.h"
#include "EzAabbox3d.h"

//! 3d EzTriangle template class for doing collision detection and other things.
template <class T>
class EZCORE_ENTRY EzTriangle3D
{
public:

	//! Constructor for an all 0 EzTriangle
	EzTriangle3D() {}
	//! Constructor for EzTriangle with given three vertices
	EzTriangle3D(EzPoint3 v1, EzPoint3 v2, EzPoint3 v3) : pointA(v1), pointB(v2), pointC(v3) {}

	//! Equality operator
	bool operator==(const EzTriangle3D<T>& other) const
	{
		return other.pointA==pointA && other.pointB==pointB && other.pointC==pointC;
	}

	//! Inequality operator
	bool operator!=(const EzTriangle3D<T>& other) const
	{
		return !(*this==other);
	}

	//! Determines if the EzTriangle is totally inside a bounding box.
	/** \param box Box to check.
	\return True if EzTriangle is within the box, otherwise false. */
	bool isTotalInsideBox(const EzAabbox3d& box) const
	{
		return (box.isPointInside(pointA) &&
			box.isPointInside(pointB) &&
			box.isPointInside(pointC));
	}

	//! Get the closest point on a EzTriangle to a point on the same plane.
	/** \param p Point which must be on the same plane as the EzTriangle.
	\return The closest point of the EzTriangle */
	EzPoint3 closestPointOnTriangle(const EzPoint3& p) const
	{
		const EzPoint3 rab = EzLine3d<T>(pointA, pointB).getClosestPoint(p);
		const EzPoint3 rbc = EzLine3d<T>(pointB, pointC).getClosestPoint(p);
		const EzPoint3 rca = EzLine3d<T>(pointC, pointA).getClosestPoint(p);

		const T d1 = rab.getDistanceFrom(p);
		const T d2 = rbc.getDistanceFrom(p);
		const T d3 = rca.getDistanceFrom(p);

		if (d1 < d2)
			return d1 < d3 ? rab : rca;

		return d2 < d3 ? rbc : rca;
	}

	//! Check if a point is inside the EzTriangle
	/** \param p Point to test. Assumes that this point is already
	on the plane of the EzTriangle.
	\return True if the point is inside the EzTriangle, otherwise false. */
	bool isPointInside(const EzPoint3& p) const
	{
		return (isOnSameSide(p, pointA, pointB, pointC) &&
			isOnSameSide(p, pointB, pointA, pointC) &&
			isOnSameSide(p, pointC, pointA, pointB));
	}

	//! Check if a point is inside the EzTriangle.
	/** This method is an implementation of the example used in a
	paper by Kasper Fauerby original written by Keidy from
	Mr-Gamemaker.
	\param p Point to test. Assumes that this point is already
	on the plane of the EzTriangle.
	\return True if point is inside the EzTriangle, otherwise false. */
	bool isPointInsideFast(const EzPoint3& p) const
	{
		const EzPoint3 f = pointB - pointA;
		const EzPoint3 g = pointC - pointA;

		const float a = f.dotProduct(f);
		const float b = f.dotProduct(g);
		const float c = g.dotProduct(g);

		const EzPoint3 vp = p - pointA;
		const float d = vp.dotProduct(f);
		const float e = vp.dotProduct(g);

		float x = (d*c)-(e*b);
		float y = (e*a)-(d*b);
		const float ac_bb = (a*c)-(b*b);
		float z = x+y-ac_bb;

		// return sign(z) && !(sign(x)||sign(y))
		return (( (IR(z)) & ~((IR(x))|(IR(y))) ) & 0x80000000)!=0;
	}


	//! Get an intersection with a 3d line.
	/** \param line Line to intersect with.
	\param outIntersection Place to store the intersection point, if there is one.
	\return True if there was an intersection, false if not. */
	bool getIntersectionWithLimitedLine(const EzLine3d& line,
		EzPoint3& outIntersection) const
	{
		return getIntersectionWithLine(line.start,
			line.getVector(), outIntersection) &&
			outIntersection.isBetweenPoints(line.start, line.end);
	}


	//! Get an intersection with a 3d line.
	/** Please note that also points are returned as intersection which
	are on the line, but not between the start and end point of the line.
	If you want the returned point be between start and end
	use getIntersectionWithLimitedLine().
	\param linePoint Point of the line to intersect with.
	\param lineVect Vector of the line to intersect with.
	\param outIntersection Place to store the intersection point, if there is one.
	\return True if there was an intersection, false if there was not. */
	bool getIntersectionWithLine(const EzPoint3& linePoint,
		const EzPoint3& lineVect, EzPoint3& outIntersection) const
	{
		if (getIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
			return isPointInside(outIntersection);

		return false;
	}


	//! Calculates the intersection between a 3d line and the plane the EzTriangle is on.
	/** \param lineVect Vector of the line to intersect with.
	\param linePoint Point of the line to intersect with.
	\param outIntersection Place to store the intersection point, if there is one.
	\return True if there was an intersection, else false. */
	bool getIntersectionOfPlaneWithLine(const EzPoint3& linePoint,
		const EzPoint3& lineVect, EzPoint3& outIntersection) const
	{
		const EzPoint3 normal = getNormal().normalize();
		T t2;

		if ( iszero ( t2 = normal.dotProduct(lineVect) ) )
			return false;

		T d = pointA.dotProduct(normal);
		T t = -(normal.dotProduct(linePoint) - d) / t2;
		outIntersection = linePoint + (lineVect * t);
		return true;
	}


	//! Get the normal of the EzTriangle.
	/** Please note: The normal is not always normalized. */
	EzPoint3 getNormal() const
	{
		return (pointB - pointA).crossProduct(pointC - pointA);
	}

	//! Test if the EzTriangle would be front or backfacing from any point.
	/** Thus, this method assumes a camera position from which the
	EzTriangle is definitely visible when looking at the given direction.
	Do not use this method with points as it will give wrong results!
	\param lookDirection Look direction.
	\return True if the plane is front facing and false if it is backfacing. */
	bool isFrontFacing(const EzPoint3& lookDirection) const
	{
		const EzPoint3 n = getNormal().normalize();
		const float d = (float)n.dotProduct(lookDirection);
		return float_LOWER_EQUAL_0(d);
	}

	//! Get the plane of this EzTriangle.
	EzPlane3d getPlane() const
	{
		return EzPlane3d(pointA, pointB, pointC);
	}

	//! Get the area of the EzTriangle
	float getArea() const
	{
		return (pointB - pointA).crossProduct(pointC - pointA).getLength() * 0.5;

	}

	//! sets the EzTriangle's points
	void set(const EzPoint3& a, const EzPoint3& b, const EzPoint3& c)
	{
		pointA = a;
		pointB = b;
		pointC = c;
	}

	//! the three points of the EzTriangle
	EzPoint3 pointA;
	EzPoint3 pointB;
	EzPoint3 pointC;

private:
	bool isOnSameSide(const EzPoint3& p1, const EzPoint3& p2,
		const EzPoint3& a, const EzPoint3& b) const
	{
		EzPoint3 bminusa = b - a;
		EzPoint3 cp1 = bminusa.crossProduct(p1 - a);
		EzPoint3 cp2 = bminusa.crossProduct(p2 - a);
		return (cp1.dotProduct(cp2) >= 0.0f);
	}
};


//! Typedef for a float 3d EzTriangle.
typedef EzTriangle3D<float> triangle3df;

//! Typedef for an integer 3d EzTriangle.
typedef EzTriangle3D<int> triangle3di;



#endif

