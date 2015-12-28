// Copyright (C) 2002-2008 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "EzImage.h"
#include "EzColorConverter.h"



// some 2D Defines
struct AbsRectangle
{
	int x0;
	int y0;
	int x1;
	int y1;
};

struct SBlitJob
{
	AbsRectangle Dest;
	AbsRectangle Source;

	unsigned int argb;

	void * src;
	void * dst;

	int width;
	int height;

	unsigned int srcPitch;
	unsigned int dstPitch;

	unsigned int srcPixelMul;
	unsigned int dstPixelMul;
};

// Blitter Operation
enum eBlitter
{
	BLITTER_INVALID = 0,
	BLITTER_COLOR,
	BLITTER_COLOR_ALPHA,
	BLITTER_TEXTURE,
	BLITTER_TEXTURE_ALPHA_BLEND,
	BLITTER_TEXTURE_ALPHA_COLOR_BLEND
};

typedef void (*tExecuteBlit) ( const SBlitJob * job );



// ----------------------- Generic ----------------------------------

inline void intersect ( AbsRectangle &dest, const AbsRectangle& a, const AbsRectangle& b)
{
	dest.x0 = int_max( a.x0, b.x0 );
	dest.y0 = int_max( a.y0, b.y0 );
	dest.x1 = int_min( a.x1, b.x1 );
	dest.y1 = int_min( a.y1, b.y1 );
}

inline bool isValid (const AbsRectangle& a)
{
	return a.x0 < a.x1 && a.y0 < a.y1;
}

//! a more useful memset for pixel
inline void memset32 ( void * dest, const unsigned int value, unsigned int bytesize )
{
	unsigned int * d = (unsigned int*) dest;

	unsigned int i;

	i = bytesize >> ( 2 + 3 );
	while( i )
	{
		d[0] = value;
		d[1] = value;
		d[2] = value;
		d[3] = value;

		d[4] = value;
		d[5] = value;
		d[6] = value;
		d[7] = value;

		d += 8;
		i -= 1;
	}

	i = (bytesize >> 2 ) & 7;
	while( i )
	{
		d[0] = value;
		d += 1;
		i -= 1;
	}

}

/*
	use biased loop counter
	--> 0 byte copy is forbidden
*/
REALINLINE void memcpy32_small ( void * dest, const void *source, unsigned int bytesize )
{
	unsigned int c = bytesize >> 2;

	do
	{
		((unsigned int*) dest ) [ c-1 ] = ((unsigned int*) source) [ c-1 ];
	} while ( --c );

}



// integer log2 of a float ieee 754. TODO: non ieee floating point
static inline int int_log2_float( float f)
{
	unsigned int x = IR ( f );

	return ((x & 0x7F800000) >> 23) - 127;
}

static inline int int_log2_int(unsigned int x)
{
	return int_log2_float( (float) x);
}

static inline int int_abs(int x)
{
	int b = x >> 31;
	return (x ^ b ) - b;
}


//! conditional set based on mask and arithmetic shift
REALINLINE unsigned int if_mask_a_else_b ( const unsigned int mask, const unsigned int a, const unsigned int b )
{
	return ( mask & ( a ^ b ) ) ^ b;
}

// ------------------ Video---------------------------------------
/*!
	Pixel = dest * ( 1 - alpha ) + source * alpha
	alpha [0;256]
*/
REALINLINE unsigned int PixelBlend32 ( const unsigned int c2, const unsigned int c1, unsigned int alpha )
{
	unsigned int srcRB = c1 & 0x00FF00FF;
	unsigned int srcXG = c1 & 0x0000FF00;

	unsigned int dstRB = c2 & 0x00FF00FF;
	unsigned int dstXG = c2 & 0x0000FF00;


	unsigned int rb = srcRB - dstRB;
	unsigned int xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 8;
	xg >>= 8;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x00FF00FF;
	xg &= 0x0000FF00;

	return rb | xg; 
}

/*!
	Pixel = dest * ( 1 - alpha ) + source * alpha
	alpha [0;32]
*/
inline unsigned short PixelBlend16 ( const unsigned short c2, const unsigned int c1, const unsigned int alpha )
{
	unsigned int srcRB = c1 & 0x7C1F;
	unsigned int srcXG = c1 & 0x03E0;

	unsigned int dstRB = c2 & 0x7C1F;
	unsigned int dstXG = c2 & 0x03E0;


	unsigned int rb = srcRB - dstRB;
	unsigned int xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 5;
	xg >>= 5;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x7C1F;
	xg &= 0x03E0;

	return (unsigned short)(rb | xg);
}

/*!
	Scale Color by (1/value)
	value 0 - 256 ( alpha )
*/
inline unsigned int PixelLerp32 ( const unsigned int source, const unsigned int value )
{
	unsigned int srcRB = source & 0x00FF00FF;
	unsigned int srcXG = (source & 0xFF00FF00) >> 8;

	srcRB *= value;
	srcXG *= value;

	srcRB >>= 8;
	//srcXG >>= 8;

	srcXG &= 0xFF00FF00;
	srcRB &= 0x00FF00FF;

	return srcRB | srcXG; 
}

/*
	return alpha in [0;256] Granularity
	 add highbit alpha ( alpha > 127 ? + 1 )
*/
inline unsigned int extractAlpha ( const unsigned int c )
{
	return ( c >> 24 ) + ( c >> 31 );
}

/*
	Pixel = c0 * (c1/31). c0 Alpha retain
*/
inline unsigned short PixelMul16 ( const unsigned short c0, const unsigned short c1)
{
	return (unsigned short)((( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 ) |
			(( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 ) |
			(( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5 ) |
			(c0 & 0x8000));
}

/*
	Pixel = c0 * (c1/31). 
*/
inline unsigned short PixelMul16_2 ( unsigned short c0, unsigned short c1)
{
	return	(unsigned short)(( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 |
			( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 |
			( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5  |
			( c0 & c1 & 0x8000));
}

/*
	Pixel = c0 * (c1/255). c0 Alpha Retain
*/
REALINLINE unsigned int PixelMul32 ( const unsigned int c0, const unsigned int c1)
{
	return	(c0 & 0xFF000000) |
			(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
			(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
			(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
}

/*
	Pixel = c0 * (c1/255). 
*/
REALINLINE unsigned int PixelMul32_2 ( const unsigned int c0, const unsigned int c1)
{
	return	(( ( (c0 & 0xFF000000) >> 16 ) * ( (c1 & 0xFF000000) >> 16 ) ) & 0xFF000000 ) |
			(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
			(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
			(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
}

/*
	Pixel = clamp ( c0 + c1, 0, 255 )
*/
REALINLINE unsigned int PixelAdd32 ( const unsigned int c2, const unsigned int c1)
{
	unsigned int sum = ( c2 & 0x00FFFFFF )  + ( c1 & 0x00FFFFFF );
	unsigned int low_bits = ( c2 ^ c1 ) & 0x00010101;
	int carries  = ( sum - low_bits ) & 0x01010100;
	unsigned int modulo = sum - carries;
	unsigned int clamp = carries - ( carries >> 8 );
	return modulo | clamp;
}



// 1 - Bit Alpha Blending
inline unsigned short PixelBlend16 ( const unsigned short c2, const unsigned short c1 )
{
	unsigned short c = c1 & 0x8000;
	
	c >>= 15;
	c += 0x7fff;
	
	c &= c2;
	c |= c1;
	
	return c;
}

// 1 - Bit Alpha Blending 16Bit SIMD
inline unsigned int PixelBlend16_simd ( const unsigned int c2, const unsigned int c1 )
{
	unsigned int c = c1 & 0x80008000;
	
	c >>= 15;
	c += 0x7fff7fff;
	
	c &= c2;
	c |= c1;
	
	return c;
}


/*!
	Pixel = dest * ( 1 - SourceAlpha ) + source * SourceAlpha
*/
inline unsigned int PixelBlend32 ( const unsigned int c2, const unsigned int c1 )
{
	// alpha test
	unsigned int alpha = c1 & 0xFF000000;

	if ( 0 == alpha )
		return c2;

	if ( 0xFF000000 == alpha )
	{
		return c1;
	}

	alpha >>= 24;

	// add highbit alpha, if ( alpha > 127 ) alpha += 1;
	alpha += ( alpha >> 7);

	unsigned int srcRB = c1 & 0x00FF00FF;
	unsigned int srcXG = c1 & 0x0000FF00;

	unsigned int dstRB = c2 & 0x00FF00FF;
	unsigned int dstXG = c2 & 0x0000FF00;


	unsigned int rb = srcRB - dstRB;
	unsigned int xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 8;
	xg >>= 8;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x00FF00FF;
	xg &= 0x0000FF00;

	return rb | xg; 
}



/*!
*/
static void executeBlit_TextureCopy_x_to_x( const SBlitJob * job )
{
	const void *src = (void*) job->src;
	void *dst = (void*) job->dst;

	const unsigned int widthPitch = job->width * job->dstPixelMul;
	for ( int dy = 0; dy != job->height; ++dy )
	{
		memcpy( dst, src, widthPitch );

		src = (void*) ( (unsigned char*) (src) + job->srcPitch );
		dst = (void*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}


/*!
*/
static void executeBlit_TextureCopy_32_to_16( const SBlitJob * job )
{
	const unsigned int *src = static_cast<const unsigned int*>(job->src);
	unsigned short *dst = static_cast<unsigned short*>(job->dst);

	for ( int dy = 0; dy != job->height; ++dy )
	{
		for ( int dx = 0; dx != job->width; ++dx )
		{
			//16 bit Blitter depends on pre-multiplied color
			const unsigned int s = PixelLerp32( src[dx] | 0xFF000000, extractAlpha( src[dx] ) );
			dst[dx] = A8R8G8B8toA1R5G5B5( s );
		}

		src = (unsigned int*) ( (unsigned char*) (src) + job->srcPitch );
		dst = (unsigned short*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_TextureCopy_24_to_16( const SBlitJob * job )
{
	const void *src = (void*) job->src;
	unsigned short *dst = (unsigned short*) job->dst;

	for ( int dy = 0; dy != job->height; ++dy )
	{
		unsigned char * s = (unsigned char*) src;

		for ( int dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = RGB16(s[0], s[1], s[2]);
			s += 3;
		}

		src = (void*) ( (unsigned char*) (src) + job->srcPitch );
		dst = (unsigned short*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}


/*!
*/
static void executeBlit_TextureCopy_16_to_32( const SBlitJob * job )
{
	const unsigned short *src = (unsigned short*) job->src;
	unsigned int *dst = (unsigned int*) job->dst;

	for ( int dy = 0; dy != job->height; ++dy )
	{
		for ( int dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = A1R5G5B5toA8R8G8B8( src[dx] );
		}

		src = (unsigned short*) ( (unsigned char*) (src) + job->srcPitch );
		dst = (unsigned int*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}

static void executeBlit_TextureCopy_16_to_24( const SBlitJob * job )
{
	const unsigned short *src = (unsigned short*) job->src;
	unsigned char *dst = (unsigned char*) job->dst;

	for ( int dy = 0; dy != job->height; ++dy )
	{
		for ( int dx = 0; dx != job->width; ++dx )
		{
			unsigned int colour = A1R5G5B5toA8R8G8B8( src[dx] );
			unsigned char * writeTo = &dst[dx * 3];
			*writeTo++ = (colour >> 16)& 0xFF;
			*writeTo++ = (colour >> 8) & 0xFF;
			*writeTo++ = colour & 0xFF;
		}

		src = (unsigned short*) ( (unsigned char*) (src) + job->srcPitch );
		dst += job->dstPitch;
	}
}


/*!
*/
static void executeBlit_TextureCopy_24_to_32( const SBlitJob * job )
{
	void *src = (void*) job->src;
	unsigned int *dst = (unsigned int*) job->dst;

	for ( int dy = 0; dy != job->height; ++dy )
	{
		unsigned char * s = (unsigned char*) src;

		for ( int dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = 0xFF000000 | s[0] << 16 | s[1] << 8 | s[2];
			s += 3;
		}

		src = (void*) ( (unsigned char*) (src) + job->srcPitch );
		dst = (unsigned int*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}

static void executeBlit_TextureCopy_32_to_24( const SBlitJob * job )
{
	const unsigned int * src = (unsigned int*) job->src;
	unsigned char * dst = (unsigned char*) job->dst;

	for ( int dy = 0; dy != job->height; ++dy )
	{
		for ( int dx = 0; dx != job->width; ++dx )
		{
			unsigned char * writeTo = &dst[dx * 3];
			*writeTo++ = (src[dx] >> 16)& 0xFF;
			*writeTo++ = (src[dx] >> 8) & 0xFF;
			*writeTo++ = src[dx] & 0xFF;
		}

		src = (unsigned int*) ( (unsigned char*) (src) + job->srcPitch );
		dst += job->dstPitch ;
	}
	
}




/*!
*/
static void executeBlit_TextureBlend_16_to_16( const SBlitJob * job )
{
	unsigned int dx;
	int dy;

	unsigned int *src = (unsigned int*) job->src;
	unsigned int *dst = (unsigned int*) job->dst;


	const unsigned int rdx = job->width >> 1;
	const unsigned int off = if_c_a_else_b( job->width & 1 ,job->width - 1, 0 );


	if ( 0 == off )
	{
		for ( dy = 0; dy != job->height; ++dy )
		{
			for ( dx = 0; dx != rdx; ++dx )
			{
				dst[dx] = PixelBlend16_simd( dst[dx], src[dx] );
			}

			src = (unsigned int*) ( (unsigned char*) (src) + job->srcPitch );
			dst = (unsigned int*) ( (unsigned char*) (dst) + job->dstPitch );
		}

	}
	else
	{
		for ( dy = 0; dy != job->height; ++dy )
		{
			for ( dx = 0; dx != rdx; ++dx )
			{
				dst[dx] = PixelBlend16_simd( dst[dx], src[dx] );
			}

			((unsigned short*) dst)[off] = PixelBlend16( ((unsigned short*) dst)[off], ((unsigned short*) src)[off] );

			src = (unsigned int*) ( (unsigned char*) (src) + job->srcPitch );
			dst = (unsigned int*) ( (unsigned char*) (dst) + job->dstPitch );
		}

	}
}

/*!
*/
static void executeBlit_TextureBlend_32_to_32( const SBlitJob * job )
{
	unsigned int *src = (unsigned int*) job->src;
	unsigned int *dst = (unsigned int*) job->dst;

	for ( int dy = 0; dy != job->height; ++dy )
	{
		for ( int dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32( dst[dx], src[dx] );
		}
		src = (unsigned int*) ( (unsigned char*) (src) + job->srcPitch );
		dst = (unsigned int*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_TextureBlendColor_16_to_16( const SBlitJob * job )
{
	unsigned short *src = (unsigned short*) job->src;
	unsigned short *dst = (unsigned short*) job->dst;

	unsigned short blend = A8R8G8B8toA1R5G5B5 ( job->argb );
	for ( int dy = 0; dy != job->height; ++dy )
	{
		for ( int dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend16( dst[dx], PixelMul16_2( src[dx], blend ) );
		}
		src = (unsigned short*) ( (unsigned char*) (src) + job->srcPitch );
		dst = (unsigned short*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}


/*!
*/
static void executeBlit_TextureBlendColor_32_to_32( const SBlitJob * job )
{
	unsigned int *src = (unsigned int*) job->src;
	unsigned int *dst = (unsigned int*) job->dst;

	for ( int dy = 0; dy != job->height; ++dy )
	{
		for ( int dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32( dst[dx], PixelMul32_2( src[dx], job->argb ) );
		}
		src = (unsigned int*) ( (unsigned char*) (src) + job->srcPitch );
		dst = (unsigned int*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_Color_16_to_16( const SBlitJob * job )
{
	unsigned short *dst = (unsigned short*) job->dst;

	unsigned short c0 = A8R8G8B8toA1R5G5B5( job->argb );
	unsigned int c = c0 | c0 << 16;

	if ( 0 == (job->srcPitch & 3 ) )
	{
		for ( int dy = 0; dy != job->height; ++dy )
		{
			memset32( dst, c, job->srcPitch );
			dst = (unsigned short*) ( (unsigned char*) (dst) + job->dstPitch );
		}
	}
	else
	{
		int dx = job->width - 1;

		for ( int dy = 0; dy != job->height; ++dy )
		{
			memset32( dst, c, job->srcPitch );
			dst[dx] = c0;
			dst = (unsigned short*) ( (unsigned char*) (dst) + job->dstPitch );
		}

	}
}

/*!
*/
static void executeBlit_Color_32_to_32( const SBlitJob * job )
{
	unsigned int *dst = (unsigned int*) job->dst;

	for ( int dy = 0; dy != job->height; ++dy )
	{
		memset32( dst, job->argb, job->srcPitch );
		dst = (unsigned int*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_ColorAlpha_16_to_16( const SBlitJob * job )
{
	unsigned short *dst = (unsigned short*) job->dst;

	const unsigned int alpha = extractAlpha( job->argb ) >> 3;
	const unsigned int src = A8R8G8B8toA1R5G5B5( job->argb );

	for ( int dy = 0; dy != job->height; ++dy )
	{
		for ( int dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend16( dst[dx], src, alpha );
		}
		dst = (unsigned short*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_ColorAlpha_32_to_32( const SBlitJob * job )
{
	unsigned int *dst = (unsigned int*) job->dst;

	const unsigned int alpha = extractAlpha( job->argb );
	const unsigned int src = job->argb;

	for ( int dy = 0; dy != job->height; ++dy )
	{
		for ( int dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32( dst[dx], src, alpha );
		}
		dst = (unsigned int*) ( (unsigned char*) (dst) + job->dstPitch );
	}
}

/*!
*/
static tExecuteBlit getBlitter( eBlitter operation,const EzImage * dest,const EzImage * source )
{
	ECOLOR_FORMAT sourceFormat = (ECOLOR_FORMAT) -1;
	ECOLOR_FORMAT destFormat = (ECOLOR_FORMAT) -1;

	if ( source )
		sourceFormat = source->getColorFormat();

	if ( dest )
		destFormat = dest->getColorFormat();

	switch ( operation )
	{
		case BLITTER_TEXTURE:
		{
			if ( sourceFormat == destFormat )
				return executeBlit_TextureCopy_x_to_x;

			if ( destFormat == ECF_A1R5G5B5 && sourceFormat == ECF_A8R8G8B8 )
				return executeBlit_TextureCopy_32_to_16;

			if ( destFormat == ECF_A1R5G5B5 && sourceFormat == ECF_R8G8B8 )
				return executeBlit_TextureCopy_24_to_16;

			if ( destFormat == ECF_A8R8G8B8 && sourceFormat == ECF_A1R5G5B5 )
				return executeBlit_TextureCopy_16_to_32;

			if ( destFormat == ECF_R8G8B8 && sourceFormat == ECF_A1R5G5B5 )
				return executeBlit_TextureCopy_16_to_24;

			if ( destFormat == ECF_A8R8G8B8 && sourceFormat == ECF_R8G8B8 )
				return executeBlit_TextureCopy_24_to_32;

			if ( destFormat == ECF_R8G8B8 && sourceFormat == ECF_A8R8G8B8 )
				return executeBlit_TextureCopy_32_to_24;

		} break;

		case BLITTER_TEXTURE_ALPHA_BLEND:
		{
			if ( destFormat == ECF_A1R5G5B5 && sourceFormat == ECF_A1R5G5B5 )
				return executeBlit_TextureBlend_16_to_16;

			if ( destFormat == ECF_A8R8G8B8 && sourceFormat == ECF_A8R8G8B8 )
				return executeBlit_TextureBlend_32_to_32;

		} break;

		case BLITTER_TEXTURE_ALPHA_COLOR_BLEND:
		{
			if ( destFormat == ECF_A1R5G5B5 && sourceFormat == ECF_A1R5G5B5 )
				return executeBlit_TextureBlendColor_16_to_16;

			if ( destFormat == ECF_A8R8G8B8 && sourceFormat == ECF_A8R8G8B8 )
				return executeBlit_TextureBlendColor_32_to_32;
		} break;

		case BLITTER_COLOR:
		{
			if ( destFormat == ECF_A1R5G5B5 )
				return executeBlit_Color_16_to_16;

			if ( destFormat == ECF_A8R8G8B8 )
				return executeBlit_Color_32_to_32;
		} break;

		case BLITTER_COLOR_ALPHA:
		{
			if ( destFormat == ECF_A1R5G5B5 )
				return executeBlit_ColorAlpha_16_to_16;

			if ( destFormat == ECF_A8R8G8B8 )
				return executeBlit_ColorAlpha_32_to_32;

		} break;

		case BLITTER_INVALID:
		break;
	}
/*
	char buf[64];
	sprintf( buf, "Blit: %d %d->%d unsupported",operation,sourceFormat,destFormat );
	os::Printer::log(buf );
*/
	return 0;

}




/*!
	a generic 2D Blitter
*/
static int Blit(eBlitter operation,
		EzImage * dest,
		const EzRect *destClipping,
		const EzDimension2d *destPos,
		EzImage * const source,
		const EzRect*sourceClipping,
		unsigned int argb)
{
	tExecuteBlit blitter = getBlitter( operation, dest, source );
	if ( 0 == blitter )
	{
		return 0;
	}

	// Clipping
	AbsRectangle sourceClip;
	AbsRectangle destClip;
	AbsRectangle v;

	SBlitJob job;

	if ( sourceClipping )
	{
		sourceClip.x0 = (int) sourceClipping->UpperLeftCorner.X;
		sourceClip.y0 = (int)sourceClipping->UpperLeftCorner.Y;
		sourceClip.x1 = (int)sourceClipping->LowerRightCorner.X;
		sourceClip.y1 = (int)sourceClipping->LowerRightCorner.Y;
	}
	else
	{
		sourceClip.x0 = 0;
		sourceClip.y0 = 0;
		sourceClip.x1 = source ? (int)source->getDimension().Width : 0;
		sourceClip.y1 = source ? (int)source->getDimension().Height : 0;
	}

	if ( destClipping )
	{
		destClip.x0 = (int) destClipping->UpperLeftCorner.X;
		destClip.y0 = (int) destClipping->UpperLeftCorner.Y;
		destClip.x1 = (int) destClipping->LowerRightCorner.X;
		destClip.y1 = (int) destClipping->LowerRightCorner.Y;
	}
	else
	{
		destClip.x0 = 0;
		destClip.y0 = 0;
		destClip.x1 = dest ? (int) dest->getDimension().Width : 0;
		destClip.y1 = dest ? (int) dest->getDimension().Height : 0;
	}

	v.x0 = destPos ? (int) destPos->Width : 0;
	v.y0 = destPos ? (int) destPos->Height : 0;
	v.x1 = v.x0 + ( sourceClip.x1 - sourceClip.x0 );
	v.y1 = v.y0 + ( sourceClip.y1 - sourceClip.y0 );

	intersect( job.Dest, destClip, v );
	if ( !isValid( job.Dest ) )
		return 0;

	job.width = job.Dest.x1 - job.Dest.x0;
	job.height = job.Dest.y1 - job.Dest.y0;


	job.Source.x0 = sourceClip.x0 + ( job.Dest.x0 - v.x0 );
	job.Source.x1 = job.Source.x0 + job.width;

	job.Source.y0 = sourceClip.y0 + ( job.Dest.y0 - v.y0 );
	job.Source.y1 = job.Source.y0 + job.height;

	job.argb = argb;

	if ( source )
	{
		job.srcPitch = source->getPitch();
		job.srcPixelMul = source->getBytesPerPixel();
		job.src = (void*) ( (unsigned char*) source->lock() + ( job.Source.y0 * job.srcPitch ) + ( job.Source.x0 * job.srcPixelMul ) );
	}
	else
	{
		// use srcPitch for color operation on dest
		job.srcPitch = job.width * dest->getBytesPerPixel();
	}

	job.dstPitch = dest->getPitch();
	job.dstPixelMul = dest->getBytesPerPixel();
	job.dst = (void*) ( (unsigned char*) dest->lock() + ( job.Dest.y0 * job.dstPitch ) + ( job.Dest.x0 * job.dstPixelMul ) );

	blitter( &job );

	if ( source )
		source->unlock();

	if ( dest )
		dest->unlock();

	return 1;
}


EzImplementRootRTTI(EzImage);

//! constructor
EzImage::EzImage(ECOLOR_FORMAT format, const EzDimension2d& size )
:Data(0), Size(size), Format(format), DeleteMemory(true), Depth(1), IsCompressed(false), MipMapData(0)
{
	initData();
}

//! constructor for empty 3D texture
EzImage::EzImage(ECOLOR_FORMAT format, const EzDimension2d& size, const int & iDepth)
: Format(format), Size(EzDimension2d(size.Width, size.Height)), Data(0), Depth(iDepth), IsCompressed(false), MipMapData(0)
{
	initData();
}

//! constructor 3d volume images
EzImage::EzImage(ECOLOR_FORMAT format, const EzDimension2d& size, const int & iDepth, void* data, bool ownForeignMemory)
: Format(format), Size(EzDimension2d( size.Width, size.Height )), Data(0), Depth(iDepth), IsCompressed(false), MipMapData(0)
{
	if( ownForeignMemory )
	{
		Data = (void*)0xbadf00d;
		initData();
		Data = data;
	}
	else
	{
		Data = 0;
		initData();
		memcpy( Data, data, (int)(Size.Height * Size.Width) * Depth * BytesPerPixel );
	}
}


//! constructor
EzImage::EzImage(ECOLOR_FORMAT format, const EzDimension2d& size, void* data,
			bool ownForeignMemory, bool deleteForeignMemory)
: Data(0), Size(size), Format(format), DeleteMemory(deleteForeignMemory), Depth(1), IsCompressed(false), MipMapData(0)
{
	if (ownForeignMemory)
	{
		Data = (void*)0xbadf00d;
		initData();
		Data = data;
	}
	else
	{
		Data = 0;
		initData();
		memcpy(Data, data, (int) (Size.Height) * Pitch);
	}
}


//! constructor
EzImage::EzImage(ECOLOR_FORMAT format, EzImage* imageToCopy)
: Data(0), Format(format), DeleteMemory(true), Depth(1), IsCompressed(false), MipMapData(0)
{
	if (!imageToCopy)
		return;

	Size = imageToCopy->getDimension();
	initData();

	// now copy data from other image

	Blit ( BLITTER_TEXTURE, this, 0, 0, imageToCopy, 0,0 );
}




//! assumes format and size has been set and creates the rest
void EzImage::initData()
{
	setBitMasks();
	BitsPerPixel = getBitsPerPixelFromFormat(Format);
	BytesPerPixel = BitsPerPixel / 8;

	// Pitch should be aligned...
	Pitch = BytesPerPixel * (int) (Size.Width);

	if (!Data)
		Data = new char [(int) (Size.Height) * Pitch];
}


//! destructor
EzImage::~EzImage()
{
	if ( DeleteMemory )
	{
		delete [] (char*)Data;

		if( MipMapData )
			delete [] MipMapData;
	}
}


//! Returns width and height of image data.
const EzDimension2d& EzImage::getDimension() const
{
	return Size;
}



//! Returns bits per pixel.
unsigned int EzImage::getBitsPerPixel() const
{
	return BitsPerPixel;
}


//! Returns bytes per pixel
unsigned int EzImage::getBytesPerPixel() const
{
	return BytesPerPixel;
}



//! Returns image data size in bytes
unsigned int EzImage::getImageDataSizeInBytes() const
{
	return Pitch * (int) (Size.Height);
}



//! Returns image data size in pixels
unsigned int EzImage::getImageDataSizeInPixels() const
{
	return (int) (Size.Width * Size.Height);
}



//! returns mask for red value of a pixel
unsigned int EzImage::getRedMask() const
{
	return RedMask;
}



//! returns mask for green value of a pixel
unsigned int EzImage::getGreenMask() const
{
	return GreenMask;
}



//! returns mask for blue value of a pixel
unsigned int EzImage::getBlueMask() const
{
	return (int) BlueMask;
}



//! returns mask for alpha value of a pixel
unsigned int EzImage::getAlphaMask() const
{
	return AlphaMask;
}


void EzImage::setBitMasks()
{
	switch(Format)
	{
	case ECF_A1R5G5B5:
		AlphaMask = 0x1<<15;
		RedMask = 0x1F<<10;
		GreenMask = 0x1F<<5;
		BlueMask = 0x1F;
	break;
	case ECF_R5G6B5:
		AlphaMask = 0x0;
		RedMask = 0x1F<<11;
		GreenMask = 0x3F<<5;
		BlueMask = 0x1F;
	break;
	case ECF_R8G8B8:
		AlphaMask = 0x0;
		RedMask   = 0x00FF0000;
		GreenMask = 0x0000FF00;
		BlueMask  = 0x000000FF;
	break;
	case ECF_A8R8G8B8:
		AlphaMask = 0xFF000000;
		RedMask   = 0x00FF0000;
		GreenMask = 0x0000FF00;
		BlueMask  = 0x000000FF;
	break;

// jay adds

	case ECF_A4R4G4B4:
		AlphaMask = 0xF000;
		RedMask = 0xF00;
		GreenMask = 0xF0;
		BlueMask = 0xF;
	break;
	case ECF_X8R8G8B8:
		AlphaMask = 0x0;
		RedMask = 0xFF<<16;
		GreenMask = 0xFF<<8;
		BlueMask = 0xFF;
	break;
	case ECF_A8B8G8R8:
		AlphaMask = 0xFF<<24;
		BlueMask = 0xFF<<16;
		GreenMask = 0xFF<<8;
		RedMask = 0xFF;
	break;
	case ECF_L8:
		AlphaMask = RedMask = GreenMask = BlueMask = 0;
	break;
	case ECF_DXT1:
		AlphaMask = RedMask = GreenMask = BlueMask = 0;
	break;
	case ECF_DXT2:
	case ECF_DXT3:
	case ECF_DXT4:
	case ECF_DXT5:
		AlphaMask = RedMask = GreenMask = BlueMask = 0;
	break;
	case ECF_I8:
		AlphaMask = GreenMask = BlueMask = 0;
		RedMask = 0xFF;
	break;
	case ECF_I16:
		AlphaMask = GreenMask = BlueMask = 0;
		RedMask = 0xFFFF;
	break;
	case ECF_I32:
		AlphaMask = GreenMask = BlueMask = 0;
		RedMask = 0xFFFFFFFF;
	break;
	case ECF_F32:
		AlphaMask = RedMask = GreenMask = BlueMask = 0;
	break;

	}
}


unsigned int EzImage::getBitsPerPixelFromFormat(ECOLOR_FORMAT format)
{
	switch(format)
	{
	case ECF_A1R5G5B5:
		return 16;
	case ECF_R5G6B5:
		return 16;
	case ECF_R8G8B8:
		return 24;
	case ECF_A8R8G8B8:
		return 32;

// jay adds from irrspintz
	case ECF_A4R4G4B4:
		return 16;

	case ECF_X8R8G8B8:
		return 32;

	case ECF_A8B8G8R8:
		return 32;

	case ECF_L8:
		return 8;
	case ECF_DXT1:
		IsCompressed = true;
		return 16;
	case ECF_DXT2:
	case ECF_DXT3:
	case ECF_DXT4:
	case ECF_DXT5:
		IsCompressed = true;
		return 32;
	case ECF_I8:
		return 8;
	case ECF_I16:
		return 16;
	case ECF_I32:
		return 32;
	case ECF_F32:
		return 32;
	}

	return 0;
}

//! sets a pixel
void EzImage::setPixel(unsigned int x, unsigned int y, const EzColor &color )
{
	if (x >= (unsigned int)Size.Width || y >= (unsigned int)Size.Height)
		return;

	switch(Format)
	{
		case ECF_A1R5G5B5:
		{
			unsigned short * dest = (unsigned short*) ((unsigned char*) Data + ( y * Pitch ) + ( x << 1 ));
			*dest = A8R8G8B8toA1R5G5B5( color.color );
		} break;

		case ECF_R5G6B5:
		{
			unsigned short * dest = (unsigned short*) ((unsigned char*) Data + ( y * Pitch ) + ( x << 1 ));
			*dest = A8R8G8B8toR5G6B5( color.color );
		} break;

		case ECF_R8G8B8:
		{
			unsigned char* dest = (unsigned char*) Data + ( y * Pitch ) + ( x * 3 );
			dest[0] = (unsigned char)color.getRed();
			dest[1] = (unsigned char)color.getGreen();
			dest[2] = (unsigned char)color.getBlue();
		} break;

		case ECF_A8R8G8B8:
		{
			unsigned int * dest = (unsigned int*) ((unsigned char*) Data + ( y * Pitch ) + ( x << 2 ));
			*dest = color.color;
		} break;

		// jay adds from irrspintz
		case ECF_A4R4G4B4:
		{
			//unsigned short* dest = (unsigned short*) ((unsigned char*) Data + ( y * Pitch ) + ( x << 1 ));
			//*dest = A8R8G8B8toA4R4G4B4( color.color );
			break;
		}

		case ECF_X8R8G8B8:
		{
			unsigned int * dest = (unsigned int*) ((unsigned char*) Data + ( y * Pitch ) + ( x << 2 ));
			*dest = color.color;
		} break;
	}
}


//! returns a pixel
EzColor EzImage::getPixel(unsigned int x, unsigned int y) const
{
	if( IsCompressed )
	{
		//os::Printer::log("EzImage::Format is Compressed", ELL_ERROR);
		return EzColor(0);
	}

	if (x >= (unsigned int)Size.Width || y >= (unsigned int)Size.Height)
		return EzColor(0);

	switch(Format)
	{
	case ECF_A1R5G5B5:
		return A1R5G5B5toA8R8G8B8(((unsigned short*)Data)[y*Size.Width + x]);
	case ECF_R5G6B5:
		return R5G6B5toA8R8G8B8(((unsigned short*)Data)[y*Size.Width + x]);
	case ECF_A8R8G8B8:
		return ((unsigned int*)Data)[y*Size.Width + x];
	case ECF_R8G8B8:
		{
			unsigned char* p = &((unsigned char*)Data)[(y*3)*Size.Width + (x*3)];
			return EzColor(255,p[0],p[1],p[2]);
		}

	//jay adds from irrspintz
	case ECF_X8R8G8B8:
		return X8R8G8B8toA8R8G8B8(((unsigned int*)Data)[y*Size.Width + x]);
	case ECF_I8:
		{
			unsigned char tmp = ((unsigned char*)Data)[ y * Size.Width + x ];
			return EzColor( tmp, tmp, tmp, tmp );
		}
	case ECF_I16:
		{
			unsigned short tmp = ((unsigned short*)Data)[ y * Size.Width + x ];
			return EzColor( tmp, tmp, tmp, tmp );
		}	
	case ECF_I32:
		{
			unsigned int tmp = ((unsigned int*)Data)[ y * Size.Width + x ];
			return EzColor( tmp, tmp, tmp, tmp );
		}
	}

	return EzColor(0);
}

//! returns a pixel from a volume texture
EzColor EzImage::getPixel(unsigned int x, unsigned int y, unsigned int z) const
{
	if( IsCompressed )
	{
		//os::Printer::log("EzImage::Format is Compressed", ELL_ERROR);
		return EzColor(0);
	}


	if(x >= Size.Width || y >= Size.Height)
		return EzColor(0);

	if( Depth == 1 )
		z = 0;

	switch(Format)
	{
		case ECF_A1R5G5B5:
			return A1R5G5B5toA8R8G8B8( ((short*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ] );
		case ECF_R5G6B5:
			return R5G6B5toA8R8G8B8( ((short*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ] );
		case ECF_A8R8G8B8:
			return ((int*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ];
		case ECF_R8G8B8:
		{
			unsigned char* p = &((unsigned char*)Data)[ ( z * 3 * Size.Width * Size.Height ) + ( y * 3 * Size.Width ) + ( x * 3 ) ];
			return EzColor( 255, p[0], p[1], p[2] );
		}
		case ECF_I8:
		{
			unsigned char tmp = ((unsigned char*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ];
			return EzColor( tmp, tmp, tmp, tmp );
		}
		case ECF_I16:
		{
			unsigned short tmp = ((unsigned short*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ];
			return EzColor( tmp, tmp, tmp, tmp );
		}
		case ECF_I32:
		{
			unsigned int tmp = ((unsigned int*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ];
			return EzColor( tmp, tmp, tmp, tmp );
		}
	}

	return EzColor(0);
}


//! returns the color format
ECOLOR_FORMAT EzImage::getColorFormat() const
{
	return Format;
}


//! generates mipmaps for the image, only support 2d image data at the moment
void EzImage::generateMipMaps( bool force )
{
	// determine number of mipmap chains and size in bytes of each, to allocate memory
	bool run = true;		
	MipLevelInfo.clear();
	EzDimension2d currentLevelSize = Size;
	unsigned int offset = 0;
	while( run )
	{
		if( currentLevelSize.Width > 1 || currentLevelSize.Height > 1 )
		{
			SMipMapLevel level;

			if( currentLevelSize.Width > 1 )
			{
				if( currentLevelSize.Width % 2 == 1 )
					currentLevelSize.Width = currentLevelSize.Width / 2 + 1;
				else
					currentLevelSize.Width = currentLevelSize.Width / 2;
			}
			else
			{
				currentLevelSize.Width = 1;
			}
			if( currentLevelSize.Height > 1 )
			{
				if( currentLevelSize.Height % 2 == 1 )
					currentLevelSize.Height = currentLevelSize.Height / 2 + 1;
				else
					currentLevelSize.Height = currentLevelSize.Height / 2;
			}
			else
			{
				currentLevelSize.Height = 1;
			}

			level.Size = currentLevelSize;
			level.Pitch = BytesPerPixel * level.Size.Width;
			level.Bytes = level.Pitch * level.Size.Height;
			level.Offset = offset;
			offset += level.Bytes;
			MipLevelInfo.push_back( level );
		}
		else
		{
			run = false;
		}
	}
}

//! copies this surface into another, scaling it to the target image size
// note: this is very very slow. (i didn't want to write a fast version.
// but hopefully, nobody wants to scale surfaces every frame.
void EzImage::copyToScaling(void* target, int width, int height, ECOLOR_FORMAT format, unsigned int pitch)
{
	if (!target || !width || !height)
		return;

	const unsigned int bpp=getBitsPerPixelFromFormat(format)/8;
	if (0==pitch)
		pitch = width*bpp;

	if (Format==format && Size.Width==width && Size.Height==height)
	{
		if (pitch==Pitch)
		{
			memcpy(target, Data, height*pitch);
			return;
		}
		else
		{
			unsigned char* tgtpos = (unsigned char*) target;
			unsigned char* dstpos = (unsigned char*) Data;
			const unsigned int bwidth = width*bpp;
			for (int y=0; y<height; ++y)
			{
				memcpy(target, Data, height*pitch);
				memset(tgtpos+width, 0, pitch-bwidth);
				tgtpos += pitch;
				dstpos += Pitch;
			}
			return;
		}
	}

	const float sourceXStep = (float)Size.Width / (float)width;
	const float sourceYStep = (float)Size.Height / (float)height;
	int yval=0, syval=0;
	float sy = 0.0f;
	for (int y=0; y<height; ++y)
	{
		float sx = 0.0f;
		for (int x=0; x<width; ++x)
		{
			EzColorConverter::convert_viaFormat(((unsigned char*)Data)+ syval + ((int)sx)*BytesPerPixel, Format, 1, ((unsigned char*)target)+ yval + (x*bpp), format);
			sx+=sourceXStep;
		}
		sy+=sourceYStep;
		syval=((int)sy)*Pitch;
		yval+=pitch;
	}
}

//! copies this surface into another, scaling it to the target image size
// note: this is very very slow. (i didn't want to write a fast version.
// but hopefully, nobody wants to scale surfaces every frame.
void EzImage::copyToScaling(EzImage* target)
{
	if (!target)
		return;

	const EzDimension2d& targetSize = target->getDimension();

	if (targetSize==Size)
	{
		copyTo(target);
		return;
	}

	copyToScaling(target->lock(), targetSize.Width, targetSize.Height, target->getColorFormat());
	target->unlock();
}


//! copies this surface into another
void EzImage::copyTo(EzImage* target, const EzDimension2d& pos)
{
	Blit(BLITTER_TEXTURE, target, 0, &pos, this, 0, 0);
}


//! copies this surface into another
void EzImage::copyTo(EzImage* target, const EzDimension2d& pos, const EzRect& sourceRect, const EzRect* clipRect)
{
	Blit(BLITTER_TEXTURE, target, clipRect, &pos, this, &sourceRect, 0);
}



//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
void EzImage::copyToWithAlpha(EzImage* target, const EzDimension2d& pos, const EzRect& sourceRect, const EzColor &color, const EzRect* clipRect)
{
	// color blend only necessary on not full spectrum aka. color.color != 0xFFFFFFFF
	Blit(color.color == 0xFFFFFFFF ? BLITTER_TEXTURE_ALPHA_BLEND: BLITTER_TEXTURE_ALPHA_COLOR_BLEND,
			target, clipRect, &pos, this, &sourceRect, color.color);
}
