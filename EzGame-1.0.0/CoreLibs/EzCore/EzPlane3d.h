// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __EZ_PLANE_3D_H_INCLUDED__
#define __EZ_PLANE_3D_H_INCLUDED__

#include "EzMath.h"
#include "EzPoint3.h"

//! Enumeration for intersection relations of 3d objects
enum EIntersectionRelation3D
{
	ISREL3D_FRONT = 0,
	ISREL3D_BACK,
	ISREL3D_PLANAR,
	ISREL3D_SPANNING,
	ISREL3D_CLIPPED
};



//! Template plane class with some intersection testing methods.
class EZCORE_ENTRY EzPlane3d : public EzMemObject
{
	public:

	// Constructors

		EzPlane3d(): Normal(0,1,0) { recalculateD(EzPoint3(0,0,0)); }
		
		EzPlane3d(const EzPoint3& MPoint, const EzPoint3& Normal) : Normal(Normal) { recalculateD(MPoint); }
		
		EzPlane3d(float px, float py, float pz, float nx, float ny, float nz) : Normal(nx, ny, nz) { recalculateD(EzPoint3(px, py, pz)); }
		
		EzPlane3d(const EzPoint3& point1, const EzPoint3& point2, const EzPoint3& point3)
		{ setPlane(point1, point2, point3); }
		
		EzPlane3d(const EzPoint3& normal, const float d) : Normal(normal), D(d) { }

		// operators

		inline bool operator==(const EzPlane3d& other) const { return (equals(D, other.D) && Normal==other.Normal);}

		inline bool operator!=(const EzPlane3d& other) const { return !(*this == other);}

		// functions

		void setPlane(const EzPoint3& point, const EzPoint3& nvector)
		{
			Normal = nvector;
			recalculateD(point);
		}

		void setPlane(const EzPoint3& nvect, float d)
		{
			Normal = nvect;
			D = d;
		}

		void setPlane(const EzPoint3& point1, const EzPoint3& point2, const EzPoint3& point3)
		{
			// creates the plane from 3 memberpoints
			Normal = (point2 - point1).crossProduct(point3 - point1);
			Normal.normalize();

			recalculateD(point1);
		}


		//! Get an intersection with a 3d line.
		/** \param lineVect Vector of the line to intersect with.
		\param linePoint Point of the line to intersect with.
		\param outIntersection Place to store the intersection point, if there is one.
		\return True if there was an intersection, false if there was not.
		*/
		bool getIntersectionWithLine(const EzPoint3& linePoint,
				const EzPoint3& lineVect,
				EzPoint3& outIntersection) const
		{
			float t2 = Normal.dotProduct(lineVect);

			if (t2 == 0)
				return false;

			float t =- (Normal.dotProduct(linePoint) + D) / t2;
			outIntersection = linePoint + (lineVect * t);
			return true;
		}

		//! Get percentage of line between two points where an intersection with this plane happens.
		/** Only useful if known that there is an intersection.
		\param linePoint1 Point1 of the line to intersect with.
		\param linePoint2 Point2 of the line to intersect with.
		\return Where on a line between two points an intersection with this plane happened.
		For example, 0.5 is returned if the intersection happened exactly in the middle of the two points.
		*/
		float getKnownIntersectionWithLine(const EzPoint3& linePoint1,
			const EzPoint3& linePoint2) const
		{
			EzPoint3 vect = linePoint2 - linePoint1;
			float t2 = (float)Normal.dotProduct(vect);
			return (float)-((Normal.dotProduct(linePoint1) + D) / t2);
		}

		//! Get an intersection with a 3d line, limited between two 3d points.
		/** \param linePoint1 Point 1 of the line.
		\param linePoint2 Point 2 of the line.
		\param outIntersection Place to store the intersection point, if there is one.
		\return True if there was an intersection, false if there was not.
		*/
		bool getIntersectionWithLimitedLine(
				const EzPoint3& linePoint1,
				const EzPoint3& linePoint2,
				EzPoint3& outIntersection) const
		{
			return (getIntersectionWithLine(linePoint1, linePoint2 - linePoint1, outIntersection) &&
					outIntersection.isBetweenPoints(linePoint1, linePoint2));
		}

		//! Classifies the relation of a point to this plane.
		/** \param point Point to classify its relation.
		\return ISREL3D_FRONT if the point is in front of the plane,
		ISREL3D_BACK if the point is behind of the plane, and
		ISREL3D_PLANAR if the point is within the plane. */
		EIntersectionRelation3D classifyPointRelation(const EzPoint3& point) const
		{
			const float d = Normal.dotProduct(point) + D;

			if (d < -ROUNDING_ERROR_32)
				return ISREL3D_BACK;

			if (d > ROUNDING_ERROR_32)
				return ISREL3D_FRONT;

			return ISREL3D_PLANAR;
		}

		//! Recalculates the distance from origin by applying a new member point to the plane.
		void recalculateD(const EzPoint3& MPoint)
		{
			D = - MPoint.dotProduct(Normal);
		}

		//! Gets a member point of the plane.
		EzPoint3 getMemberPoint() const
		{
			return Normal * -D;
		}

		//! Tests if there is an intersection with the other plane
		/** \return True if there is a intersection. */
		bool existsIntersection(const EzPlane3d& other) const
		{
			EzPoint3 cross = other.Normal.crossProduct(Normal);
			return cross.getLength() > ROUNDING_ERROR_32;
		}

		//! Intersects this plane with another.
		/** \param other Other plane to intersect with.
		\param outLinePoint Base point of intersection line.
		\param outLineVect Vector of intersection.
		\return True if there is a intersection, false if not. */
		bool getIntersectionWithPlane(const EzPlane3d& other,
				EzPoint3& outLinePoint,
				EzPoint3& outLineVect) const
		{
			const float fn00 = Normal.getLength();
			const float fn01 = Normal.dotProduct(other.Normal);
			const float fn11 = other.Normal.getLength();
			const double det = fn00*fn11 - fn01*fn01;

			if (fabs(det) < ROUNDING_ERROR_64 )
				return false;

			const double invdet = 1.0 / det;
			const double fc0 = (fn11*-D + fn01*other.D) * invdet;
			const double fc1 = (fn00*-other.D + fn01*D) * invdet;

			outLineVect = Normal.crossProduct(other.Normal);
			outLinePoint = Normal*(float)fc0 + other.Normal*(float)fc1;
			return true;
		}

		//! Get the intersection point with two other planes if there is one.
		bool getIntersectionWithPlanes(const EzPlane3d& o1,
				const EzPlane3d& o2, EzPoint3& outPoint) const
		{
			EzPoint3 linePoint, lineVect;
			if (getIntersectionWithPlane(o1, linePoint, lineVect))
				return o2.getIntersectionWithLine(linePoint, lineVect, outPoint);

			return false;
		}

		//! Test if the EzTriangle would be front or backfacing from any point.
		/** Thus, this method assumes a camera position from
		which the EzTriangle is definitely visible when looking into
		the given direction.
		Note that this only works if the normal is Normalized.
		Do not use this method with points as it will give wrong results!
		\param lookDirection: Look direction.
		\return True if the plane is front facing and
		false if it is backfacing. */
		bool isFrontFacing(const EzPoint3& lookDirection) const
		{
			const float d = Normal.dotProduct(lookDirection);
			return float_LOWER_EQUAL_0 ( d );
		}

		//! Get the distance to a point.
		/** Note that this only works if the normal is normalized. */
		float getDistanceTo(const EzPoint3& point) const
		{
			return point.dotProduct(Normal) + D;
		}

		//! Normal vector of the plane.
		EzPoint3 Normal;
		//! Distance from origin.
		float D;
};


#endif

