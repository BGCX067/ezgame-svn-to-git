// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __EZ_COLOR_H_INCLUDED__
#define __EZ_COLOR_H_INCLUDED__

#include "EzMath.h"
#include "EzMemObject.h"


//! Creates a 16 bit A1R5G5B5 color
inline unsigned short RGBA16(unsigned int r, unsigned int g, unsigned int b, unsigned int a=0xFF)
{
	return (unsigned short)((a & 0x80) << 8 |
		(r & 0xF8) << 7 |
		(g & 0xF8) << 2 |
		(b & 0xF8) >> 3);
}


//! Creates a 16 bit A1R5G5B5 color
inline unsigned short RGB16(unsigned int r, unsigned int g, unsigned int b)
{
	return RGBA16(r,g,b);
}


//! Creates a 16bit A1R5G5B5 color, based on 16bit input values
inline unsigned short RGB16from16(unsigned short r, unsigned short g, unsigned short b)
{
	return (0x8000 |
			(r & 0x1F) << 10 |
			(g & 0x1F) << 5  |
			(b & 0x1F));
}


////! Converts a 32bit (X8R8G8B8) color to a 16bit A1R5G5B5 color
//inline unsigned short X8R8G8B8toA1R5G5B5(unsigned int color)
//{
//	return (unsigned short)(0x8000 |
//		( color & 0x00F80000) >> 9 |
//		( color & 0x0000F800) >> 6 |
//		( color & 0x000000F8) >> 3);
//}


////! Converts a 32bit (A8R8G8B8) color to a 16bit A1R5G5B5 color
//inline unsigned short A8R8G8B8toA1R5G5B5(unsigned int color)
//{
//	return (unsigned short)(( color & 0x80000000) >> 16|
//		( color & 0x00F80000) >> 9 |
//		( color & 0x0000F800) >> 6 |
//		( color & 0x000000F8) >> 3);
//}




////! Convert A8R8G8B8 Color from A1R5G5B5 color
///** build a nicer 32bit Color by extending dest lower bits with source high bits. */
//inline unsigned int A1R5G5B5toA8R8G8B8(unsigned short color)
//{
//	return ( (( -( (int) color & 0x00008000 ) >> (int) 31 ) & 0xFF000000 ) |
//			(( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4) |
//			(( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1) |
//			(( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2)
//			);
//}


////! Returns A8R8G8B8 Color from R5G6B5 color
//inline unsigned int R5G6B5toA8R8G8B8(unsigned short color)
//{
//	return 0xFF000000 |
//		((color & 0xF800) << 8)|
//		((color & 0x07E0) << 5)|
//		((color & 0x001F) << 3);
//}


////! Returns A1R5G5B5 Color from R5G6B5 color
//inline unsigned short R5G6B5toA1R5G5B5(unsigned short color)
//{
//	return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
//}


////! Returns R5G6B5 Color from A1R5G5B5 color
//inline unsigned short A1R5G5B5toR5G6B5(unsigned short color)
//{
//	return (((color & 0x7FE0) << 1) | (color & 0x1F));
//}


/*********************************************************************************************************************/
/**                                                                                                                 **/
/*                                      Start Color Conversion Functions                                             */
/**                                                                                                                 **/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/***                                               To A8B8G8R8                                                     ***/
/*********************************************************************************************************************/



//! Converts a 32-bit A8R8G8B8 color to a 32-bit A8B8G8R8 color
inline unsigned int A8R8G8B8toA8B8G8R8( const unsigned int& color )
{
	return ( color & 0xFF000000 ) | ( (color & 0xFF ) << 16 ) | ( color & 0xFF00 ) | ( ( color & 0xFF0000 ) >> 16 );
}

//! Converts a 16-bit A1R5G5B5) color to a 32-bit A8B8G8R8 color
inline unsigned int A1R5G5B5toA8B8G8R8( const unsigned int& color )
{
	// alpha value is first set to -65535 or 0, then shifted right
	// to introduce sign bits and and'ed with the desired 8bit value.
	return	( ( -( (int) color & 0x00008000 ) >> (int) 31 ) & 0xFF000000 ) |
		( ( color & 0x7C00 ) >> 7 ) |
		( ( color & 0x3E0 ) << 6 ) |
		( ( color & 0x1F ) << 19 );
}

//! Converts a 16-bit R5G6B5 color to a 32-bit A8B8G8R8 color
inline unsigned int R5G6B5toA8B8G8R8( const unsigned short& color )
{
	return 0xFF000000 |
		( ( color & 0x1F ) << 19 ) |
		( ( color & 0x7E0 ) << 5 ) |
		( ( color & 0xF800 ) >> 8 );
}

//! Converts a 16-bit A4R4G4B4 color to a 32-bit A8B8G8R8 color
inline unsigned int A4R4G4B4toA8B8G8R8( const unsigned short& color )
{
	return ( ( color & 0xF000 ) << 16 ) | ( ( color & 0xF000 ) << 12 ) |
		( ( color & 0xF00 ) >> 8 ) | ( ( color & 0xF00 ) >> 4 ) |
		( ( color & 0xF0 ) << 8 ) | ( ( color & 0xF0 ) << 4 ) |
		( ( color & 0xF ) << 20 ) | ( ( color & 0xF ) << 16 );
}

//! Converts a 32-bit X8R8G8B8 color to a 32-bit A8B8G8R8 color
inline unsigned int X8R8G8B8toA8B8G8R8( const unsigned int& color )
{
	return 0xFF000000 | 
		( ( color & 0xFF ) << 16 ) |
		( color & 0xFF00 ) |
		( ( color & 0xFF0000 ) >> 16 );
}

/*********************************************************************************************************************/
/***                                               To A1R5G5B5                                                     ***/
/*********************************************************************************************************************/

//! Converts a 32-bit X8R8G8B8 color to a 16-bit A1R5G5B5 color
inline unsigned short X8R8G8B8toA1R5G5B5( const unsigned int& color )
{
	return ( 0x8000 |
		( color & 0x00F80000 ) >> 9 |
		( color & 0x0000F800 ) >> 6 |
		( color & 0x000000F8 ) >> 3 );
}

//! Converts a 32-bit A8R8G8B8 color to a 16 A1R5G5B5 color
inline unsigned short A8R8G8B8toA1R5G5B5( const unsigned int& color )
{
	return ( ( color & 0x80000000 ) >> 16 |
		( color & 0x00F80000 ) >> 9 |
		( color & 0x0000F800 ) >> 6 |
		( color & 0x000000F8 ) >> 3 );
}

//! Converts a 16-bit R5G6B5 color to a 16-bit A1R5G5B5 color
inline unsigned short R5G6B5toA1R5G5B5( const unsigned short& color )
{
	return 0x8000 | ( ( ( color & 0xFFC0 ) >> 1 ) | ( color & 0x1F ) );
}

/*********************************************************************************************************************/
/***                                               To A8R8G8B8                                                     ***/
/*********************************************************************************************************************/
//! Converts a 32-bit (X8R8G8B8) color to a 32-bit (A8R8G8B8) color
inline unsigned int X8R8G8B8toA8R8G8B8( const unsigned int& color )
{
	return 0xFF000000 | color;
}
//! Converts a 16-bit A1R5G5B5 color to a 32-bit A8R8G8B8 color
inline unsigned int A1R5G5B5toA8R8G8B8( const unsigned short& color )
{
	// alpha value is first set to -65535 or 0, then shifted right
	// to introduce sign bits and and'ed with the desired 8bit value.
	return	( ( -( (int) color & 0x00008000 ) >> (int) 31 ) & 0xFF000000 |
			( color & 0x00007C00 ) << 9 |
			( color & 0x000003E0 ) << 6 |
			( color & 0x0000001F ) << 3 );
}

//! Converts a 16-bit R5G6B5 color to a 32-bit A8R8G8B8 color
inline unsigned int R5G6B5toA8R8G8B8( const unsigned short& color )
{
	return 0xFF000000 |
		( ( color & 0xF800 ) << 8 ) |
		( ( color & 0x07E0 ) << 5 ) |
		( ( color & 0x001F ) << 3 );
}

//! Converts a 16-bit A4R4G4B4 color to a 32-bit A8R8G8B8 color
inline unsigned int A4R4G4B4toA8R8G8B8( const unsigned short& color )
{
	return ( ( color & 0xF000 ) << 16 ) | ( ( color & 0xF000 ) << 12 ) |
		( ( color & 0xF00 ) << 12 ) | ( ( color & 0xF00 ) << 8 ) |
		( ( color & 0xF0 ) << 8 ) | ( ( color & 0xF0 ) << 4 ) |
		( ( color & 0xF ) << 4 ) | ( color & 0xF );
}

//! Converts a 32-bit R8G8B8A8 color to a 32-bit A8R8G8B8 color
inline unsigned int R8G8B8A8toA8R8G8B8( const unsigned int& color )
{
	return ( color & 0xff ) | // alpha
		( ( color >> 24 ) & 0xff ) | // red
		( ( color >> 16 ) & 0xff ) | // green
		( ( color >> 8 ) & 0xff ); // blue
}

/*********************************************************************************************************************/
/***                                               To R8G8B8A8                                                     ***/
/*********************************************************************************************************************/

//! Converts a 32-bit A8R8G8B8 color to a 32-bit R8G8B8A8 color
inline unsigned int A8R8G8B8toR8G8B8A8( const unsigned int& color )
{
	return ( ( color >> 16 ) & 0xff ) | // red
		( ( color >> 8 ) & 0xff ) | // green
		( color & 0xff ) | // blue
		( ( color >> 24 ) & 0xff ); // alpha
}

/*********************************************************************************************************************/
/***                                                To R5G6B5                                                      ***/
/*********************************************************************************************************************/

//! Converts a 16-bit A1R5G5B5 color to a 16-bit R5G6B5 color
inline unsigned short A1R5G5B5toR5G6B5( const unsigned short& color )
{
	return ( ( ( color & 0x7FE0 ) << 1 ) | ( color & 0x1F ) );
}

//! Converts a 32bit (A8R8G8B8) color to a 16bit R5G6B5 color
inline unsigned short A8R8G8B8toR5G6B5(unsigned int color)
{
	return (unsigned short)(( color & 0x00F80000) >> 8 |
		( color & 0x0000FC00) >> 5 |
		( color & 0x000000F8) >> 3);
}

/*********************************************************************************************************************/
/**                                                                                                                 **/
/*                                       End Color Conversion Functions                                              */
/**                                                                                                                 **/
/*********************************************************************************************************************/



//! Returns the alpha component from A1R5G5B5 color
/** In Irrlicht, alpha refers to opacity. 
\return The alpha value of the color. 0 is transparent, 1 is opaque. */
inline unsigned int getAlpha(unsigned short color)
{
	return ((color >> 15)&0x1);
}


//! Returns the red component from A1R5G5B5 color.
/** Shift left by 3 to get 8 bit value. */
inline unsigned int getRed(unsigned short color)
{
	return ((color >> 10)&0x1F);
}


//! Returns the green component from A1R5G5B5 color
/** Shift left by 3 to get 8 bit value. */
inline unsigned int getGreen(unsigned short color)
{
	return ((color >> 5)&0x1F);
}


//! Returns the blue component from A1R5G5B5 color
/** Shift left by 3 to get 8 bit value. */
inline unsigned int getBlue(unsigned short color)
{
	return (color & 0x1F);
}


//! Returns the average from a 16 bit A1R5G5B5 color
inline int getAverage(short color)
{
	return ((getRed(color)<<3) + (getGreen(color)<<3) + (getBlue(color)<<3)) / 3;
}



//! Class representing a color with four floats.
/** The color values for red, green, blue
and alpha are each stored in a 32 bit floating point variable.
So all four values may be between 0.0f and 1.0f.
Another, faster way to define colors is using the class EzColor, which
stores the color values in a single 32 bit integer.
*/
class EzColor : public EzMemObject
{
	public:

		//! Constructor of the Color. Does nothing.
		/** The color value is not initialized to save time. */
		EzColor() {}

		//! Constructs the color from 4 values representing the alpha, red, green and blue component.
		/** Must be values between 0 and 255. */
		EzColor (unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255 )
			: color(((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)) {}

		//EzColor Constructs the color from a 32 bit value. Could be another color.
		EzColor(unsigned int clr)
			: color(clr) {}

		//! Returns the alpha component of the color.
		/** The alpha component defines how opaque a color is. 
		\return The alpha value of the color. 0 is fully transparent, 255 is fully opaque. */
		unsigned int getAlpha() const { return color>>24; }

		//! Returns the red component of the color.
		/** \return Value between 0 and 255, specifying how red the color is.
		0 means no red, 255 means full red. */
		unsigned int getRed() const { return (color>>16) & 0xff; }

		//! Returns the green component of the color.
		/** \return Value between 0 and 255, specifying how green the color is.
		0 means no green, 255 means full green. */
		unsigned int getGreen() const { return (color>>8) & 0xff; }

		//! Returns the blue component of the color.
		/** \return Value between 0 and 255, specifying how blue the color is.
		0 means no blue, 255 means full blue. */
		unsigned int getBlue() const { return color & 0xff; }

		//! Returns the luminance of the color.
		float getLuminance() const
		{
			return 0.3f*getRed() + 0.59f*getGreen() + 0.11f*getBlue();
		}

		//! Returns the average intensity of the color.
		unsigned int getAverage() const
		{
			return ( getRed() + getGreen() + getBlue() ) / 3;
		}

		//! Sets the alpha component of the Color.
		/** The alpha component defines how transparent a color should be.
		\param a The alpha value of the color. 0 is fully transparent, 255 is fully opaque. */
		void setAlpha(unsigned int a) { color = ((a & 0xff)<<24) | (color & 0x00ffffff); }

		//! Sets the red component of the Color.
		/** \param r: Has to be a value between 0 and 255.
		0 means no red, 255 means full red. */
		void setRed(unsigned int r) { color = ((r & 0xff)<<16) | (color & 0xff00ffff); }

		//! Sets the green component of the Color.
		/** \param g: Has to be a value between 0 and 255.
		0 means no green, 255 means full green. */
		void setGreen(unsigned int g) { color = ((g & 0xff)<<8) | (color & 0xffff00ff); }

		//! Sets the blue component of the Color.
		/** \param b: Has to be a value between 0 and 255.
		0 means no blue, 255 means full blue. */
		void setBlue(unsigned int b) { color = (b & 0xff) | (color & 0xffffff00); }

		//! Calculates a 16 bit A1R5G5B5 value of this color.
		/** \return 16 bit A1R5G5B5 value of this color. */
		unsigned short toA1R5G5B5() const { return A8R8G8B8toA1R5G5B5(color); }


		//! Converts color to OpenGL color format
		/** From ARGB to RGBA in 4 byte components for endian aware
		passing to OpenGL
		\param dest: address where the 4x8 bit OpenGL color is stored. */
		void toOpenGLColor(unsigned char* dest) const
		{
			*dest =   (unsigned char)getRed();
			*++dest = (unsigned char)getGreen();
			*++dest = (unsigned char)getBlue();
			*++dest = (unsigned char)getAlpha();
		}

		//! Sets all four components of the color at once.
		/** Constructs the color from 4 values representing the alpha,
		red, green and blue components of the color. Must be values
		between 0 and 255.
		\param a: Alpha component of the color. The alpha component
		defines how transparent a color should be. Has to be a value
		between 0 and 255. 255 means not transparent (opaque), 0 means
		fully transparent.
		\param r: Sets the red component of the Color. Has to be a
		value between 0 and 255. 0 means no red, 255 means full red.
		\param g: Sets the green component of the Color. Has to be a
		value between 0 and 255. 0 means no green, 255 means full
		green.
		\param b: Sets the blue component of the Color. Has to be a
		value between 0 and 255. 0 means no blue, 255 means full blue. */
		void set(unsigned int a, unsigned int r, unsigned int g, unsigned int b)
		{
			color = (((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff));
		}
		void set(unsigned int col) { color = col; }

		//! Compares the color to another color.
		/** \return True if the colors are the same, and false if not. */
		bool operator==(const EzColor& other) const { return other.color == color; }

		//! Compares the color to another color.
		/** \return True if the colors are different, and false if they are the same. */
		bool operator!=(const EzColor& other) const { return other.color != color; }

		//! comparison operator
		/** \return True if this color is smaller than the other one */
		bool operator<(const EzColor& other) const { return (color < other.color); }

		//! Adds two colors, result is clamped to 0..255 values
		/** \param other Color to add to this color
		\return Addition of the two colors, clamped to 0..255 values */
		EzColor operator+(const EzColor& other) const
		{
			return EzColor(min_(getAlpha() + other.getAlpha(), 255u),
					min_(getRed() + other.getRed(), 255u),
					min_(getGreen() + other.getGreen(), 255u),
					min_(getBlue() + other.getBlue(), 255u));
		}

		//! Interpolates the color with a f32 value to another color
		/** \param other: Other color
		\param d: value between 0.0f and 1.0f
		\return Interpolated color. */
		EzColor getInterpolated(const EzColor &other, float d) const
		{
			d = clamp(d, 0.f, 1.f);
			const float inv = 1.0f - d;
			return EzColor((unsigned int)(other.getAlpha()*inv + getAlpha()*d),
				(unsigned int)(other.getRed()*inv + getRed()*d),
				(unsigned int)(other.getGreen()*inv + getGreen()*d),
				(unsigned int)(other.getBlue()*inv + getBlue()*d));
		}

		//! Returns interpolated color. ( quadratic )
		/** \param c1: first color to interpolate with
		\param c2: second color to interpolate with
		\param d: value between 0.0f and 1.0f. */
		EzColor getInterpolated_quadratic(const EzColor& c1, const EzColor& c2, float d) const
		{
			// this*(1-d)*(1-d) + 2 * c1 * (1-d) + c2 * d * d;
			d = clamp(d, 0.f, 1.f);
			const float inv = 1.f - d;
			const float mul0 = inv * inv;
			const float mul1 = 2.f * d * inv;
			const float mul2 = d * d;

			return EzColor(
					clamp( floor32(
							getAlpha() * mul0 + c1.getAlpha() * mul1 + c2.getAlpha() * mul2 ), 0, 255 ),
					clamp( floor32(
							getRed()   * mul0 + c1.getRed()   * mul1 + c2.getRed()   * mul2 ), 0, 255 ),
					clamp ( floor32(
							getGreen() * mul0 + c1.getGreen() * mul1 + c2.getGreen() * mul2 ), 0, 255 ),
					clamp ( floor32(
							getBlue()  * mul0 + c1.getBlue()  * mul1 + c2.getBlue()  * mul2 ), 0, 255 ));
		}

		//! color in A8R8G8B8 Format
		unsigned int color;

};

#endif

