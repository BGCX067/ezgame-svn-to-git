// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __EZ_LINE_3D_H_INCLUDED__
#define __EZ_LINE_3D_H_INCLUDED__

#include "EzPoint3.h"


//! 3D line between two points with intersection methods.
class EZCORE_ENTRY EzLine3d : public EzMemObject
{
	public:

		//! Default constructor
		/** line from (0,0,0) to (1,1,1) */
		EzLine3d() : start(0,0,0), end(1,1,1) {}
		//! Constructor with two points
		EzLine3d(float xa, float ya, float za, float xb, float yb, float zb) : start(xa, ya, za), end(xb, yb, zb) {}
		//! Constructor with two points as vectors
		EzLine3d(const EzPoint3& start, const EzPoint3& end) : start(start), end(end) {}

		// operators

		EzLine3d operator+(const EzPoint3& point) const { return EzLine3d(start + point, end + point); }
		EzLine3d& operator+=(const EzPoint3& point) { start += point; end += point; return *this; }

		EzLine3d operator-(const EzPoint3& point) const { return EzLine3d(start - point, end - point); }
		EzLine3d& operator-=(const EzPoint3& point) { start -= point; end -= point; return *this; }

		bool operator==(const EzLine3d& other) const
		{ return (start==other.start && end==other.end) || (end==other.start && start==other.end);}
		bool operator!=(const EzLine3d& other) const
		{ return !(start==other.start && end==other.end) || (end==other.start && start==other.end);}

		// functions
		//! Set this line to a new line going through the two points.
		void setLine(const float& xa, const float& ya, const float& za, const float& xb, const float& yb, const float& zb)
		{start.set(xa, ya, za); end.set(xb, yb, zb);}
		//! Set this line to a new line going through the two points.
		void setLine(const EzPoint3& nstart, const EzPoint3& nend)
		{start.set(nstart); end.set(nend);}
		//! Set this line to new line given as parameter.
		void setLine(const EzLine3d& line)
		{start.set(line.start); end.set(line.end);}

		//! Get length of line
		/** \return Length of line. */
		float getLength() const { return start.getDistanceFrom(end); }

		//! Get squared length of line
		/** \return Squared length of line. */
		float getLengthSQ() const { return start.getDistanceFromSQ(end); }

		//! Get middle of line
		/** \return Center of line. */
		EzPoint3 getMiddle() const
		{
			return (start + end) * 0.5;
		}

		//! Get vector of line
		/** \return vector of line. */
		EzPoint3 getVector() const
		{
			return end - start;
		}

		//! Check if the given point is between start and end of the line.
		/** Assumes that the point is already somewhere on the line.
		\param point The point to test.
		\return True if point is on the line between start and end, else false.
		*/
		bool isPointBetweenStartAndEnd(const EzPoint3& point) const
		{
			return point.isBetweenPoints(start, end);
		}

		//! Get the closest point on this line to a point
		/** \param point The point to compare to.
		\return The nearest point which is part of the line. */
		EzPoint3 getClosestPoint(const EzPoint3& point) const
		{
			EzPoint3 c = point - start;
			EzPoint3 v = end - start;
			float d = v.getLength();
			v /= d;
			float t = v.dotProduct(c);

			if (t < 0.0)
				return start;
			if (t > d)
				return end;

			v *= t;
			return start + v;
		}

		//! Check if the line intersects with a shpere
		/** \param sorigin: Origin of the shpere.
		\param sradius: Radius of the sphere.
		\param outdistance: The distance to the first intersection point.
		\return True if there is an intersection.
		If there is one, the distance to the first intersection point
		is stored in outdistance. */
		bool getIntersectionWithSphere(EzPoint3 sorigin, float sradius, double& outdistance) const
		{
			const EzPoint3 q = sorigin - start;
			float c = q.getLength();
			float v = q.dotProduct(getVector().normalize());
			float d = sradius * sradius - (c*c - v*v);

			if (d < 0.0)
				return false;

			outdistance = v - sqrt((double)d);
			return true;
		}

		// member variables

		//! Start point of line
		EzPoint3 start;
		//! End point of line
		EzPoint3 end;
};


#endif

