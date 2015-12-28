// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __RECT_H_INCLUDED__
#define __RECT_H_INCLUDED__

#include "EzDimension2d.h"
#include "EzPoint2.h"
#include "EzMemObject.h"

//! Rectangle template.
/** Mostly used by 2D GUI elements and for 2D drawing methods.
It has 2 positions instead of position and dimension and a fast
method for collision detection with other rectangles and points.

Coordinates are (0,0) for top-left corner, and increasing to the right
and to the bottom.
*/
class EZCORE_ENTRY EzRect : public EzMemObject
{
public:

	EzRect() : UpperLeftCorner(0,0), LowerRightCorner(0,0) {}

	EzRect(float x, float y, float x2, float y2)
		: UpperLeftCorner(x,y), LowerRightCorner(x2,y2) {}

	EzRect(const EzPoint2& upperLeft, const EzPoint2& lowerRight)
		: UpperLeftCorner(upperLeft), LowerRightCorner(lowerRight) {}

	EzRect(const EzPoint2& pos, const EzDimension2d& size)
		: UpperLeftCorner(pos), LowerRightCorner(pos.X + size.Width, pos.Y + size.Height) {}


	EzRect operator+(const EzPoint2& pos) const
	{
		EzRect ret(*this);
		return ret+=pos;
	}

	EzRect& operator+=(const EzPoint2& pos)
	{
		UpperLeftCorner += pos;
		LowerRightCorner += pos;
		return *this;
	}

	EzRect operator-(const EzPoint2& pos) const
	{
		EzRect ret(*this);
		return ret-=pos;
	}

	EzRect& operator-=(const EzPoint2& pos)
	{
		UpperLeftCorner -= pos;
		LowerRightCorner -= pos;
		return *this;
	}

	bool operator==(const EzRect& other) const
	{
		return (UpperLeftCorner == other.UpperLeftCorner &&
			LowerRightCorner == other.LowerRightCorner);
	}


	bool operator!=(const EzRect& other) const
	{
		return (UpperLeftCorner != other.UpperLeftCorner ||
			LowerRightCorner != other.LowerRightCorner);
	}

	// compares size of rectangles
	bool operator<(const EzRect& other) const
	{
		return getArea() < other.getArea();
	}

	//! Returns size of rectangle
	float getArea() const
	{
		return getWidth() * getHeight();
	}

	//! Returns if a 2d point is within this rectangle.
	/** \param pos Position to test if it lies within this rectangle.
	\return True if the position is within the rectangle, false if not. */
	bool isPointInside(const EzPoint2& pos) const
	{
		return (UpperLeftCorner.X <= pos.X &&
			UpperLeftCorner.Y <= pos.Y &&
			LowerRightCorner.X >= pos.X &&
			LowerRightCorner.Y >= pos.Y);
	}

	//! Returns if the rectangle collides with another rectangle.
	bool isRectCollided(const EzRect& other) const
	{
		return (LowerRightCorner.Y > other.UpperLeftCorner.Y &&
			UpperLeftCorner.Y < other.LowerRightCorner.Y &&
			LowerRightCorner.X > other.UpperLeftCorner.X &&
			UpperLeftCorner.X < other.LowerRightCorner.X);
	}

	//! Clips this rectangle with another one.
	void clipAgainst(const EzRect& other)
	{
		if (other.LowerRightCorner.X < LowerRightCorner.X)
			LowerRightCorner.X = other.LowerRightCorner.X;
		if (other.LowerRightCorner.Y < LowerRightCorner.Y)
			LowerRightCorner.Y = other.LowerRightCorner.Y;

		if (other.UpperLeftCorner.X > UpperLeftCorner.X)
			UpperLeftCorner.X = other.UpperLeftCorner.X;
		if (other.UpperLeftCorner.Y > UpperLeftCorner.Y)
			UpperLeftCorner.Y = other.UpperLeftCorner.Y;

		// correct possible invalid EzRect resulting from clipping
		if (UpperLeftCorner.Y > LowerRightCorner.Y)
			UpperLeftCorner.Y = LowerRightCorner.Y;
		if (UpperLeftCorner.X > LowerRightCorner.X)
			UpperLeftCorner.X = LowerRightCorner.X;
	}

	//! Moves this rectangle to fit inside another one.
	/** \return True on success, false if not possible */
	bool constrainTo(const EzRect& other)
	{
		if (other.getWidth() < getWidth() || other.getHeight() < getHeight())
			return false;

		float diff = other.LowerRightCorner.X - LowerRightCorner.X;
		if (diff < 0)
		{
			LowerRightCorner.X += diff;
			UpperLeftCorner.X  += diff;
		}

		diff = other.LowerRightCorner.Y - LowerRightCorner.Y;
		if (diff < 0)
		{
			LowerRightCorner.Y += diff;
			UpperLeftCorner.Y  += diff;
		}

		diff = UpperLeftCorner.X - other.UpperLeftCorner.X;
		if (diff < 0)
		{
			UpperLeftCorner.X  -= diff;
			LowerRightCorner.X -= diff;
		}

		diff = UpperLeftCorner.Y - other.UpperLeftCorner.Y;
		if (diff < 0)
		{
			UpperLeftCorner.Y  -= diff;
			LowerRightCorner.Y -= diff;
		}

		return true;
	}

	//! Returns width of rectangle.
	float getWidth() const
	{
		return LowerRightCorner.X - UpperLeftCorner.X;
	}

	//! Returns height of rectangle.
	float getHeight() const
	{
		return LowerRightCorner.Y - UpperLeftCorner.Y;
	}

	//! If the lower right corner of the EzRect is smaller then the upper left, the points are swapped.
	void repair()
	{
		if (LowerRightCorner.X < UpperLeftCorner.X)
		{
			float t = LowerRightCorner.X;
			LowerRightCorner.X = UpperLeftCorner.X;
			UpperLeftCorner.X = t;
		}

		if (LowerRightCorner.Y < UpperLeftCorner.Y)
		{
			float t = LowerRightCorner.Y;
			LowerRightCorner.Y = UpperLeftCorner.Y;
			UpperLeftCorner.Y = t;
		}
	}

	//! Returns if the EzRect is valid to draw.
	/** It would be invalid if the UpperLeftCorner is lower or more
	right than the LowerRightCorner. */
	bool isValid() const
	{
		return ((LowerRightCorner.X >= UpperLeftCorner.X) &&
			(LowerRightCorner.Y >= UpperLeftCorner.Y));
	}

	//! Returns the center of the rectangle
	EzPoint2 getCenter() const
	{
		return EzPoint2(
				(UpperLeftCorner.X + LowerRightCorner.X) / 2,
				(UpperLeftCorner.Y + LowerRightCorner.Y) / 2);
	}

	//! Returns the dimensions of the rectangle
	EzDimension2d getSize() const
	{
		return EzDimension2d((unsigned int) getWidth(), (unsigned int) getHeight());
	}


	//! Adds a point to the rectangle
	/** Cause the rectangle to grow bigger, if point is outside of
	the box
	\param p Point to add into the box. */
	void addInternalPoint(const EzPoint2& p)
	{
		addInternalPoint(p.X, p.Y);
	}

	//! Adds a point to the bounding rectangle
	/** Cause the rectangle to grow bigger, if point is outside of
	\param x X Coordinate of the point to add to this box.
	\param y Y Coordinate of the point to add to this box. */
	void addInternalPoint(float x, float y)
	{
		if (x>LowerRightCorner.X)
			LowerRightCorner.X = x;
		if (y>LowerRightCorner.Y)
			LowerRightCorner.Y = y;

		if (x<UpperLeftCorner.X)
			UpperLeftCorner.X = x;
		if (y<UpperLeftCorner.Y)
			UpperLeftCorner.Y = y;
	}

	EzPoint2 UpperLeftCorner;
	EzPoint2 LowerRightCorner;
};

#endif

