// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __EZ_DIMENSION2D_H_INCLUDED__
#define __EZ_DIMENSION2D_H_INCLUDED__

#include "EzCoreLibType.h"

//! Specifies a 2 dimensional size.
class EZCORE_ENTRY EzDimension2d
{
	public:
		//! Default constructor for empty dimension
		EzDimension2d() : Width(0), Height(0) {}
		//! Constructor with width and height
		EzDimension2d(const unsigned int& width, const unsigned int& height)
			: Width(width), Height(height) {}

		//! Equality operator
		bool operator==(const EzDimension2d& other) const
		{
			return Width == other.Width && Height == other.Height;
		}

		//! Inequality operator
		bool operator!=(const EzDimension2d& other) const
		{
			return ! (*this == other);
		}


		//! Set to new values
		EzDimension2d& set(const unsigned int& width, const unsigned int& height)
		{
			Width = width;
			Height = height;
			return *this;
		}

		//! Divide width and height by scalar
		EzDimension2d& operator/=(const float& scale)
		{
			Width /= (unsigned int) scale;
			Height /= (unsigned int) scale;
			return *this;
		}

		//! Divide width and height by scalar
		EzDimension2d operator/(const float& scale) const
		{
			return EzDimension2d((unsigned int)(Width/scale), (unsigned int)(Height/scale));
		}

		//! Multiply width and height by scalar
		EzDimension2d& operator*=(const float& scale)
		{
			Width *= (unsigned int) scale;
			Height *= (unsigned int) scale;
			return *this;
		}

		//! Multiply width and height by scalar
		EzDimension2d operator*(const float& scale) const
		{
			return EzDimension2d((unsigned int)(Width*scale), (unsigned int)(Height*scale));
		}

		//! Add two dimensions
		EzDimension2d& operator+=(const EzDimension2d& other)
		{
			Width *= other.Width;
			Height *= other.Height;
			return *this;
		}

		//! Add two dimensions
		EzDimension2d operator+(const EzDimension2d& other) const
		{
			return EzDimension2d(Width+other.Width, Height+other.Height);
		}

		//! Get area
		unsigned int getArea() const
		{
			return Width*Height;
		}

		//! Get the optimal size according to some properties
		/** This is a function often used for texture dimension
		calculations. The function returns the next larger or
		smaller dimension which is a power-of-two dimension
		(2^n,2^m) and/or square (Width=Height).
		\param requirePowerOfTwo Forces the result to use only
		powers of two as values.
		\param requireSquare Makes width==height in the result
		\param larger Choose whether the result is larger or
		smaller than the current dimension. If one dimension
		need not be changed it is kept with any value of larger.
		\return The optimal dimension under the given
		constraints. */
		EzDimension2d getOptimalSize(
				bool requirePowerOfTwo=true,
				bool requireSquare=false,
				bool larger=true) const
		{
			unsigned int i=1;
			unsigned int j=1;
			if (requirePowerOfTwo)
			{
				while (i<(unsigned int)Width)
					i<<=1;
				if (!larger && i!=1 && i!=(unsigned int)Width)
					i>>=1;
				while (j<(unsigned int)Height)
					j<<=1;
				if (!larger && j!=1 && j!=(unsigned int)Height)
					j>>=1;
			}
			else
			{
				i=(unsigned int)Width;
				j=(unsigned int)Height;
			}

			if (requireSquare)
			{
				if ((larger && (i>j)) || (!larger && (i<j)))
					j=i;
				else
					i=j;
			}
			return EzDimension2d(i, j);
		}

		//! Get the interpolated dimension
		/** \param other Other dimension to interpolate with.
		\param d Value between 0.0f and 1.0f.
		\return Interpolated dimension. */
		EzDimension2d getInterpolated(const EzDimension2d& other, float d) const
		{
			float inv = 1.0f - d;
			return EzDimension2d((unsigned int)(other.Width*inv + Width*d), (unsigned int)(other.Height*inv + Height*d));
		}


		//! Width of the dimension.
		unsigned int Width;
		//! Height of the dimension.
		unsigned int Height;
};

#endif

