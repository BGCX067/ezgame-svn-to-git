// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __EZ_LINE_2D_H_INCLUDED__
#define __EZ_LINE_2D_H_INCLUDED__

#include "EzPoint2.h"
#include "EzMemObject.h"


//! 2D line between two points with intersection methods.
class EZCORE_ENTRY EzLine2d : public EzMemObject
{
	public:
		//! Default constructor for line going from (0,0) to (1,1).
		EzLine2d() : start(0,0), end(1,1) 
		{
		
		}
		//! Constructor for line between the two points.
		EzLine2d(float xa, float ya, float xb, float yb) : start(xa, ya), end(xb, yb) 
		{
		}
		//! Constructor for line between the two points given as vectors.
		EzLine2d(const EzPoint2& start, const EzPoint2& end) : start(start), end(end) {}
		//! Copy constructor.
		EzLine2d(const EzLine2d& other) : start(other.start), end(other.end) {}

		// operators

		EzLine2d operator+(const EzPoint2& point) const { return EzLine2d(start + point, end + point); }
		EzLine2d& operator+=(const EzPoint2& point) { start += point; end += point; return *this; }

		EzLine2d operator-(const EzPoint2& point) const { return EzLine2d(start - point, end - point); }
		EzLine2d& operator-=(const EzPoint2& point) { start -= point; end -= point; return *this; }

		bool operator==(const EzLine2d& other) const
		{ return (start==other.start && end==other.end) || (end==other.start && start==other.end);}
		bool operator!=(const EzLine2d& other) const
		{ return !(start==other.start && end==other.end) || (end==other.start && start==other.end);}

		// functions
		//! Set this line to new line going through the two points.
		void setLine(const float& xa, const float& ya, const float& xb, const float& yb){start.set(xa, ya); end.set(xb, yb);}
		//! Set this line to new line going through the two points.
		void setLine(const EzPoint2& nstart, const EzPoint2& nend){start.set(nstart); end.set(nend);}
		//! Set this line to new line given as parameter.
		void setLine(const EzLine2d& line){start.set(line.start); end.set(line.end);}

		//! Get length of line
		/** \return Length of the line. */
		double getLength() const { return start.getDistanceFrom(end); }

		//! Get squared length of the line
		/** \return Squared length of line. */
		float getLengthSQ() const { return start.getDistanceFromSQ(end); }

		//! Get middle of the line
		/** \return center of the line. */
		EzPoint2 getMiddle() const
		{
			return (start + end) * 0.5;
		}

		//! Get the vector of the line.
		/** \return The vector of the line. */
		EzPoint2 getVector() const { return EzPoint2(start.X - end.X, start.Y - end.Y); }

		//! Tests if this line intersects with another line.
		/** \param l: Other line to test intersection with.
		\param out: If there is an intersection, the location of the
		intersection will be stored in this vector.
		\return True if there is an intersection, false if not. */
		bool intersectWith(const EzLine2d& l, EzPoint2& out) const
		{
			// Uses the method given at:
			// http://local.wasp.uwa.edu.au/~pbourke/geometry/lineEzLine2d/ 
			const float commonDenominator = (l.end.Y - l.start.Y)*(end.X - start.X) -
											(l.end.X - l.start.X)*(end.Y - start.Y);

			const float numeratorA = (l.end.X - l.start.X)*(start.Y - l.start.Y) -
											(l.end.Y - l.start.Y)*(start.X -l.start.X);

			const float numeratorB = (end.X - start.X)*(start.Y - l.start.Y) -
											(end.Y - start.Y)*(start.X -l.start.X); 

			if(equals(commonDenominator, 0.f))
			{ 
				// The lines are either coincident or parallel
				if(equals(numeratorA, 0.f) && equals(numeratorB, 0.f))
				{
					// Try and find a common endpoint
					if(l.start == start || l.end == start)
						out = start;
					else if(l.end == end || l.start == end)
						out = end;
					else
						// one line is contained in the other, so for lack of a better
						// answer, pick the average of both lines
						out = ((start + end + l.start + l.end) * 0.25f);

					return true; // coincident
				}

				return false; // parallel
			}

			// Get the point of intersection on this line, checking that
			// it is within the line segment.
			const float uA = numeratorA / commonDenominator;
			if(uA < 0.f || uA > 1.f)
				return false; // Outside the line segment

			const float uB = numeratorB / commonDenominator;
			if(uB < 0.f || uB > 1.f)
				return false; // Outside the line segment

			// Calculate the intersection point.
			out.X = start.X + uA * (end.X - start.X);
			out.Y = start.Y + uA * (end.Y - start.Y);
			return true; 
		}

		//! Get unit vector of the line.
		/** \return Unit vector of this line. */
		EzPoint2 getUnitVector() const
		{
			float len = (float)(1.0 / getLength());
			return EzPoint2((end.X - start.X) * len, (end.Y - start.Y) * len);
		}

		//! Get angle between this line and given line.
		/** \param l Other line for test.
		\return Angle in degrees. */
		double getAngleWith(const EzLine2d& l) const
		{
			EzPoint2 vect = getVector();
			EzPoint2 vect2 = l.getVector();
			return vect.getAngleWith(vect2);
		}

		//! Tells us if the given point lies to the left, right, or on the line.
		/** \return 0 if the point is on the line
		<0 if to the left, or >0 if to the right. */
		float getPointOrientation(const EzPoint2& point) const
		{
			return ( (end.X - start.X) * (point.Y - start.Y) -
					(point.X - start.X) * (end.Y - start.Y) );
		}

		//! Check if the given point is a member of the line
		/** \return True if point is between start and end, else false. */
		bool isPointOnLine(const EzPoint2& point) const
		{
			float d = getPointOrientation(point);
			return (d == 0 && point.isBetweenPoints(start, end));
		}

		//! Check if the given point is between start and end of the line.
		/** Assumes that the point is already somewhere on the line. */
		bool isPointBetweenStartAndEnd(const EzPoint2& point) const
		{
			return point.isBetweenPoints(start, end);
		}

		//! Get the closest point on this line to a point
		EzPoint2 getClosestPoint(const EzPoint2& point) const
		{
			EzPoint2 c = point - start;
			EzPoint2 v = end - start;
			float d = v.getLength();
			v /= d;
			float t = v.dotProduct(c);

			if (t < 0.0) return start;
			if (t > d) return end;

			v *= t;
			return start + v;
		}

		//! Start point of the line.
		EzPoint2 start;
		//! End point of the line.
		EzPoint2 end;
};


#endif

