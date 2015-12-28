// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __EZ_AABBOX_3D_H_INCLUDED__
#define __EZ_AABBOX_3D_H_INCLUDED__

#include "EzCoreLibType.h"
#include "EzMath.h"
#include "EzPlane3d.h"
#include "EzLine3d.h"

//! Axis aligned bounding box in 3d dimensional space.
/** Has some useful methods used with occlusion culling or clipping.
*/
class EZCORE_ENTRY EzAabbox3d
{
	public:

		//! Default Constructor.
		EzAabbox3d(): MinEdge(0,0,0), MaxEdge(0,0,0) {}
		//! Constructor with min edge and max edge.
		EzAabbox3d(const EzPoint3& min, const EzPoint3& max): MinEdge(min), MaxEdge(max) {}
		//! Constructor with only one point.
		EzAabbox3d(const EzPoint3& init): MinEdge(init), MaxEdge(init) {}
		//! Constructor with min edge and max edge as single values, not vectors.
		EzAabbox3d(float minx, float miny, float minz, float maxx, float maxy, float maxz): MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz) {}

		// operators
		//! Equality operator
		/** \param other box to compare with.
		\return True if both boxes are equal, else false. */
		inline bool operator==(const EzAabbox3d& other) const { return (MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);}
		//! Inequality operator
		/** \param other box to compare with.
		\return True if both boxes are different, else false. */
		inline bool operator!=(const EzAabbox3d& other) const { return !(MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);}

		// functions

		//! Adds a point to the bounding box
		/** The box grows bigger, if point was outside of the box.
		\param p: Point to add into the box. */
		void addInternalPoint(const EzPoint3& p)
		{
			addInternalPoint(p.X, p.Y, p.Z);
		}

		//! Adds another bounding box
		/** The box grows bigger, if the new box was outside of the box.
		\param b: Other bounding box to add into this box. */
		void addInternalBox(const EzAabbox3d& b)
		{
			addInternalPoint(b.MaxEdge);
			addInternalPoint(b.MinEdge);
		}

		//! Resets the bounding box to a one-point box.
		/** \param x X coord of the point.
		\param y Y coord of the point.
		\param z Z coord of the point. */
		void reset(float x, float y, float z)
		{
			MaxEdge.set(x,y,z);
			MinEdge = MaxEdge;
		}

		//! Resets the bounding box.
		/** \param initValue New box to set this one to. */
		void reset(const EzAabbox3d& initValue)
		{
			*this = initValue;
		}

		//! Resets the bounding box to a one-point box.
		/** \param initValue New point. */
		void reset(const EzPoint3& initValue)
		{
			MaxEdge = initValue;
			MinEdge = initValue;
		}

		//! Adds a point to the bounding box
		/** The box grows bigger, if point is outside of the box.
		\param x X coordinate of the point to add to this box.
		\param y Y coordinate of the point to add to this box.
		\param z Z coordinate of the point to add to this box. */
		void addInternalPoint(float x, float y, float z)
		{
			if (x>MaxEdge.X) MaxEdge.X = x;
			if (y>MaxEdge.Y) MaxEdge.Y = y;
			if (z>MaxEdge.Z) MaxEdge.Z = z;

			if (x<MinEdge.X) MinEdge.X = x;
			if (y<MinEdge.Y) MinEdge.Y = y;
			if (z<MinEdge.Z) MinEdge.Z = z;
		}

		//! Determines if a point is within this box.
		/** \param p: Point to check.
		\return True if the point is within the box and false if not */
		bool isPointInside(const EzPoint3& p) const
		{
			return (p.X >= MinEdge.X && p.X <= MaxEdge.X &&
				p.Y >= MinEdge.Y && p.Y <= MaxEdge.Y &&
				p.Z >= MinEdge.Z && p.Z <= MaxEdge.Z);
		}

		//! Determines if a point is within this box and its borders.
		/** \param p: Point to check.
		\return True if the point is within the box and false if not. */
		bool isPointTotalInside(const EzPoint3& p) const
		{
			return (p.X > MinEdge.X && p.X < MaxEdge.X &&
				p.Y > MinEdge.Y && p.Y < MaxEdge.Y &&
				p.Z > MinEdge.Z && p.Z < MaxEdge.Z);
		}

		//! Determines if the box intersects with another box.
		/** \param other: Other box to check a intersection with.
		\return True if there is an intersection with the other box,
		otherwise false. */
		bool intersectsWithBox(const EzAabbox3d& other) const
		{
			return (MinEdge <= other.MaxEdge && MaxEdge >= other.MinEdge);
		}

		//! Check if this box is completely inside the 'other' box.
		/** \param other: Other box to check against.
		\return True if this box is completly inside the other box,
		otherwise false. */
		bool isFullInside(const EzAabbox3d& other) const
		{
			return MinEdge >= other.MinEdge && MaxEdge <= other.MaxEdge;
		}

		//! Tests if the box intersects with a line
		/** \param line: Line to test intersection with.
		\return True if there is an intersection , else false. */
		bool intersectsWithLine(const EzLine3d& line) const
		{
			return intersectsWithLine(line.getMiddle(), line.getVector().normalize(), (float)(line.getLength() * 0.5));
		}

		//! Tests if the box intersects with a line
		/** \param linemiddle Center of the line.
		\param linevect Vector of the line.
		\param halflength Half length of the line.
		\return True if there is an intersection, else false. */
		bool intersectsWithLine(const EzPoint3& linemiddle,
					const EzPoint3& linevect,
					float halflength) const
		{
			const EzPoint3 e = getExtent() * 0.5;
			const EzPoint3 t = getCenter() - linemiddle;

			if ((fabs(t.X) > e.X + halflength * fabs(linevect.X)) ||
				(fabs(t.Y) > e.Y + halflength * fabs(linevect.Y)) ||
				(fabs(t.Z) > e.Z + halflength * fabs(linevect.Z)) )
				return false;

			float r = e.Y * (float)fabs(linevect.Z) + e.Z * (float)fabs(linevect.Y);
			if (fabs(t.Y*linevect.Z - t.Z*linevect.Y) > r )
				return false;

			r = e.X * (float)fabs(linevect.Z) + e.Z * (float)fabs(linevect.X);
			if (fabs(t.Z*linevect.X - t.X*linevect.Z) > r )
				return false;

			r = e.X * (float)fabs(linevect.Y) + e.Y * (float)fabs(linevect.X);
			if (fabs(t.X*linevect.Y - t.Y*linevect.X) > r)
				return false;

			return true;
		}

		//! Classifies a relation with a plane.
		/** \param plane Plane to classify relation to.
		\return Returns ISREL3D_FRONT if the box is in front of the plane,
		ISREL3D_BACK if the box is behind the plane, and
		ISREL3D_CLIPPED if it is on both sides of the plane. */
		EIntersectionRelation3D classifyPlaneRelation(const EzPlane3d& plane) const
		{
			EzPoint3 nearPoint(MaxEdge);
			EzPoint3 farPoint(MinEdge);

			if (plane.Normal.X > (float)0)
			{
				nearPoint.X = MinEdge.X;
				farPoint.X = MaxEdge.X;
			}

			if (plane.Normal.Y > (float)0)
			{
				nearPoint.Y = MinEdge.Y;
				farPoint.Y = MaxEdge.Y;
			}

			if (plane.Normal.Z > (float)0)
			{
				nearPoint.Z = MinEdge.Z;
				farPoint.Z = MaxEdge.Z;
			}

			if (plane.Normal.dotProduct(nearPoint) + plane.D > 0.0f)
				return ISREL3D_FRONT;

			if (plane.Normal.dotProduct(farPoint) + plane.D > 0)
				return ISREL3D_CLIPPED;

			return ISREL3D_BACK;
		}


		//! Get center of the bounding box
		/** \return Center of the bounding box. */
		EzPoint3 getCenter() const
		{
			return (MinEdge + MaxEdge) / 2;
		}


		//! Get extent of the box
		/** \return Extent of the bounding box. */
		EzPoint3 getExtent() const
		{
			return MaxEdge - MinEdge;
		}


		//! Stores all 8 edges of the box into an array
		/** \param edges: Pointer to array of 8 edges. */
		void getEdges(EzPoint3 *edges) const
		{
			const EzPoint3 middle = getCenter();
			const EzPoint3 diag = middle - MaxEdge;

			/*
			Edges are stored in this way:
			Hey, am I an ascii artist, or what? :) niko.
                  /3--------/7
                 /  |      / |
                /   |     /  |
                1---------5  |
                |   2- - -| -6
                |  /      |  /
                |/        | /
                0---------4/
			*/

			edges[0].set(middle.X + diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
			edges[1].set(middle.X + diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
			edges[2].set(middle.X + diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
			edges[3].set(middle.X + diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
			edges[4].set(middle.X - diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
			edges[5].set(middle.X - diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
			edges[6].set(middle.X - diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
			edges[7].set(middle.X - diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
		}


		//! Check if the box is empty.
		/** This means that there is no space between the min and max
		edge.
		\return True if box is empty, else false. */
		bool isEmpty() const
		{
			return MinEdge.equals ( MaxEdge );
		}


		//! Repairs the box.
		/** Necessary if for example MinEdge and MaxEdge are swapped. */
		void repair()
		{
			float t;

			if (MinEdge.X > MaxEdge.X)
				{ t=MinEdge.X; MinEdge.X = MaxEdge.X; MaxEdge.X=t; }
			if (MinEdge.Y > MaxEdge.Y)
				{ t=MinEdge.Y; MinEdge.Y = MaxEdge.Y; MaxEdge.Y=t; }
			if (MinEdge.Z > MaxEdge.Z)
				{ t=MinEdge.Z; MinEdge.Z = MaxEdge.Z; MaxEdge.Z=t; }
		}

		//! Calculates a new interpolated bounding box.
		/** \param other: other box to interpolate between
		\param d: value between 0.0f and 1.0f.
		\return Interpolated box. */
		EzAabbox3d getInterpolated(const EzAabbox3d& other, float d) const
		{
			float inv = 1.0f - d;
			return EzAabbox3d((other.MinEdge*inv) + (MinEdge*d),
				(other.MaxEdge*inv) + (MaxEdge*d));
		}

		//! Get the volume enclosed by the box in cubed units
		float getVolume() const
		{
			const EzPoint3 e = getExtent();
			return e.X * e.Y * e.Z;
		}

		//! Get the surface area of the box in squared units
		float getArea() const
		{
			const EzPoint3 e = getExtent();
			return 2*(e.X*e.Y + e.X*e.Z + e.Y*e.Z);
		}

		//! The near edge
		EzPoint3 MinEdge;
		//! The far edge
		EzPoint3 MaxEdge;
};


#endif


