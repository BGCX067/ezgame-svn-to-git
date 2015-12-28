// Copyright (C) 2002-2008 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "EzImage.h"
#include "EzColorConverter.h"



// some 2D Defines
struct AbsRectangle
{
	s32 x0;
	s32 y0;
	s32 x1;
	s32 y1;
};

struct SBlitJob
{
	AbsRectangle Dest;
	AbsRectangle Source;

	u32 argb;

	void * src;
	void * dst;

	s32 width;
	s32 height;

	u32 srcPitch;
	u32 dstPitch;

	u32 srcPixelMul;
	u32 dstPixelMul;
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
	dest.x0 = s32_max( a.x0, b.x0 );
	dest.y0 = s32_max( a.y0, b.y0 );
	dest.x1 = s32_min( a.x1, b.x1 );
	dest.y1 = s32_min( a.y1, b.y1 );
}

inline bool isValid (const AbsRectangle& a)
{
	return a.x0 < a.x1 && a.y0 < a.y1;
}

//! a more useful memset for pixel
inline void memset32 ( void * dest, const u32 value, u32 bytesize )
{
	u32 * d = (u32*) dest;

	u32 i;

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
REALINLINE void memcpy32_small ( void * dest, const void *source, u32 bytesize )
{
	u32 c = bytesize >> 2;

	do
	{
		((u32*) dest ) [ c-1 ] = ((u32*) source) [ c-1 ];
	} while ( --c );

}



// integer log2 of a f32 ieee 754. TODO: non ieee f32ing point
static inline s32 s32_log2_f32( f32 f)
{
	u32 x = IR ( f );

	return ((x & 0x7F800000) >> 23) - 127;
}

static inline s32 s32_log2_s32(u32 x)
{
	return s32_log2_f32( (f32) x);
}

static inline s32 s32_abs(s32 x)
{
	s32 b = x >> 31;
	return (x ^ b ) - b;
}


//! conditional set based on mask and arithmetic shift
REALINLINE u32 if_mask_a_else_b ( const u32 mask, const u32 a, const u32 b )
{
	return ( mask & ( a ^ b ) ) ^ b;
}

// ------------------ Video---------------------------------------
/*!
	Pixel = dest * ( 1 - alpha ) + source * alpha
	alpha [0;256]
*/
REALINLINE u32 PixelBlend32 ( const u32 c2, const u32 c1, u32 alpha )
{
	u32 srcRB = c1 & 0x00FF00FF;
	u32 srcXG = c1 & 0x0000FF00;

	u32 dstRB = c2 & 0x00FF00FF;
	u32 dstXG = c2 & 0x0000FF00;


	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

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
inline u16 PixelBlend16 ( const u16 c2, const u32 c1, const u32 alpha )
{
	u32 srcRB = c1 & 0x7C1F;
	u32 srcXG = c1 & 0x03E0;

	u32 dstRB = c2 & 0x7C1F;
	u32 dstXG = c2 & 0x03E0;


	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 5;
	xg >>= 5;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x7C1F;
	xg &= 0x03E0;

	return (u16)(rb | xg);
}

/*!
	Scale Color by (1/value)
	value 0 - 256 ( alpha )
*/
inline u32 PixelLerp32 ( const u32 source, const u32 value )
{
	u32 srcRB = source & 0x00FF00FF;
	u32 srcXG = (source & 0xFF00FF00) >> 8;

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
inline u32 extractAlpha ( const u32 c )
{
	return ( c >> 24 ) + ( c >> 31 );
}

/*
	Pixel = c0 * (c1/31). c0 Alpha retain
*/
inline u16 PixelMul16 ( const u16 c0, const u16 c1)
{
	return (u16)((( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 ) |
			(( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 ) |
			(( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5 ) |
			(c0 & 0x8000));
}

/*
	Pixel = c0 * (c1/31). 
*/
inline u16 PixelMul16_2 ( u16 c0, u16 c1)
{
	return	(u16)(( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 |
			( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 |
			( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5  |
			( c0 & c1 & 0x8000));
}

/*
	Pixel = c0 * (c1/255). c0 Alpha Retain
*/
REALINLINE u32 PixelMul32 ( const u32 c0, const u32 c1)
{
	return	(c0 & 0xFF000000) |
			(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
			(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
			(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
}

/*
	Pixel = c0 * (c1/255). 
*/
REALINLINE u32 PixelMul32_2 ( const u32 c0, const u32 c1)
{
	return	(( ( (c0 & 0xFF000000) >> 16 ) * ( (c1 & 0xFF000000) >> 16 ) ) & 0xFF000000 ) |
			(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
			(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
			(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
}

/*
	Pixel = clamp ( c0 + c1, 0, 255 )
*/
REALINLINE u32 PixelAdd32 ( const u32 c2, const u32 c1)
{
	u32 sum = ( c2 & 0x00FFFFFF )  + ( c1 & 0x00FFFFFF );
	u32 low_bits = ( c2 ^ c1 ) & 0x00010101;
	s32 carries  = ( sum - low_bits ) & 0x01010100;
	u32 modulo = sum - carries;
	u32 clamp = carries - ( carries >> 8 );
	return modulo | clamp;
}



// 1 - Bit Alpha Blending
inline u16 PixelBlend16 ( const u16 c2, const u16 c1 )
{
	u16 c = c1 & 0x8000;
	
	c >>= 15;
	c += 0x7fff;
	
	c &= c2;
	c |= c1;
	
	return c;
}

// 1 - Bit Alpha Blending 16Bit SIMD
inline u32 PixelBlend16_simd ( const u32 c2, const u32 c1 )
{
	u32 c = c1 & 0x80008000;
	
	c >>= 15;
	c += 0x7fff7fff;
	
	c &= c2;
	c |= c1;
	
	return c;
}


/*!
	Pixel = dest * ( 1 - SourceAlpha ) + source * SourceAlpha
*/
inline u32 PixelBlend32 ( const u32 c2, const u32 c1 )
{
	// alpha test
	u32 alpha = c1 & 0xFF000000;

	if ( 0 == alpha )
		return c2;

	if ( 0xFF000000 == alpha )
	{
		return c1;
	}

	alpha >>= 24;

	// add highbit alpha, if ( alpha > 127 ) alpha += 1;
	alpha += ( alpha >> 7);

	u32 srcRB = c1 & 0x00FF00FF;
	u32 srcXG = c1 & 0x0000FF00;

	u32 dstRB = c2 & 0x00FF00FF;
	u32 dstXG = c2 & 0x0000FF00;


	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

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

	const u32 widthPitch = job->width * job->dstPixelMul;
	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		memcpy( dst, src, widthPitch );

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (void*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
*/
static void executeBlit_TextureCopy_32_to_16( const SBlitJob * job )
{
	const u32 *src = static_cast<const u32*>(job->src);
	u16 *dst = static_cast<u16*>(job->dst);

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			//16 bit Blitter depends on pre-multiplied color
			const u32 s = PixelLerp32( src[dx] | 0xFF000000, extractAlpha( src[dx] ) );
			dst[dx] = A8R8G8B8toA1R5G5B5( s );
		}

		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_TextureCopy_24_to_16( const SBlitJob * job )
{
	const void *src = (void*) job->src;
	u16 *dst = (u16*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		u8 * s = (u8*) src;

		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = RGB16(s[0], s[1], s[2]);
			s += 3;
		}

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
*/
static void executeBlit_TextureCopy_16_to_32( const SBlitJob * job )
{
	const u16 *src = (u16*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = A1R5G5B5toA8R8G8B8( src[dx] );
		}

		src = (u16*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

static void executeBlit_TextureCopy_16_to_24( const SBlitJob * job )
{
	const u16 *src = (u16*) job->src;
	u8 *dst = (u8*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			u32 colour = A1R5G5B5toA8R8G8B8( src[dx] );
			u8 * writeTo = &dst[dx * 3];
			*writeTo++ = (colour >> 16)& 0xFF;
			*writeTo++ = (colour >> 8) & 0xFF;
			*writeTo++ = colour & 0xFF;
		}

		src = (u16*) ( (u8*) (src) + job->srcPitch );
		dst += job->dstPitch;
	}
}


/*!
*/
static void executeBlit_TextureCopy_24_to_32( const SBlitJob * job )
{
	void *src = (void*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		u8 * s = (u8*) src;

		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = 0xFF000000 | s[0] << 16 | s[1] << 8 | s[2];
			s += 3;
		}

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

static void executeBlit_TextureCopy_32_to_24( const SBlitJob * job )
{
	const u32 * src = (u32*) job->src;
	u8 * dst = (u8*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			u8 * writeTo = &dst[dx * 3];
			*writeTo++ = (src[dx] >> 16)& 0xFF;
			*writeTo++ = (src[dx] >> 8) & 0xFF;
			*writeTo++ = src[dx] & 0xFF;
		}

		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst += job->dstPitch ;
	}
	
}




/*!
*/
static void executeBlit_TextureBlend_16_to_16( const SBlitJob * job )
{
	u32 dx;
	s32 dy;

	u32 *src = (u32*) job->src;
	u32 *dst = (u32*) job->dst;


	const u32 rdx = job->width >> 1;
	const u32 off = if_c_a_else_b( job->width & 1 ,job->width - 1, 0 );


	if ( 0 == off )
	{
		for ( dy = 0; dy != job->height; ++dy )
		{
			for ( dx = 0; dx != rdx; ++dx )
			{
				dst[dx] = PixelBlend16_simd( dst[dx], src[dx] );
			}

			src = (u32*) ( (u8*) (src) + job->srcPitch );
			dst = (u32*) ( (u8*) (dst) + job->dstPitch );
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

			((u16*) dst)[off] = PixelBlend16( ((u16*) dst)[off], ((u16*) src)[off] );

			src = (u32*) ( (u8*) (src) + job->srcPitch );
			dst = (u32*) ( (u8*) (dst) + job->dstPitch );
		}

	}
}

/*!
*/
static void executeBlit_TextureBlend_32_to_32( const SBlitJob * job )
{
	u32 *src = (u32*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32( dst[dx], src[dx] );
		}
		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_TextureBlendColor_16_to_16( const SBlitJob * job )
{
	u16 *src = (u16*) job->src;
	u16 *dst = (u16*) job->dst;

	u16 blend = A8R8G8B8toA1R5G5B5 ( job->argb );
	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend16( dst[dx], PixelMul16_2( src[dx], blend ) );
		}
		src = (u16*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
*/
static void executeBlit_TextureBlendColor_32_to_32( const SBlitJob * job )
{
	u32 *src = (u32*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32( dst[dx], PixelMul32_2( src[dx], job->argb ) );
		}
		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_Color_16_to_16( const SBlitJob * job )
{
	u16 *dst = (u16*) job->dst;

	u16 c0 = A8R8G8B8toA1R5G5B5( job->argb );
	u32 c = c0 | c0 << 16;

	if ( 0 == (job->srcPitch & 3 ) )
	{
		for ( s32 dy = 0; dy != job->height; ++dy )
		{
			memset32( dst, c, job->srcPitch );
			dst = (u16*) ( (u8*) (dst) + job->dstPitch );
		}
	}
	else
	{
		s32 dx = job->width - 1;

		for ( s32 dy = 0; dy != job->height; ++dy )
		{
			memset32( dst, c, job->srcPitch );
			dst[dx] = c0;
			dst = (u16*) ( (u8*) (dst) + job->dstPitch );
		}

	}
}

/*!
*/
static void executeBlit_Color_32_to_32( const SBlitJob * job )
{
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		memset32( dst, job->argb, job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_ColorAlpha_16_to_16( const SBlitJob * job )
{
	u16 *dst = (u16*) job->dst;

	const u32 alpha = extractAlpha( job->argb ) >> 3;
	const u32 src = A8R8G8B8toA1R5G5B5( job->argb );

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend16( dst[dx], src, alpha );
		}
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_ColorAlpha_32_to_32( const SBlitJob * job )
{
	u32 *dst = (u32*) job->dst;

	const u32 alpha = extractAlpha( job->argb );
	const u32 src = job->argb;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32( dst[dx], src, alpha );
		}
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
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
static s32 Blit(eBlitter operation,
		EzImage * dest,
		const EzRect<s32> *destClipping,
		const EzPosition2d<s32> *destPos,
		EzImage * const source,
		const EzRect<s32> *sourceClipping,
		u32 argb)
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
		sourceClip.x0 = sourceClipping->UpperLeftCorner.X;
		sourceClip.y0 = sourceClipping->UpperLeftCorner.Y;
		sourceClip.x1 = sourceClipping->LowerRightCorner.X;
		sourceClip.y1 = sourceClipping->LowerRightCorner.Y;
	}
	else
	{
		sourceClip.x0 = 0;
		sourceClip.y0 = 0;
		sourceClip.x1 = source ? source->getDimension().Width : 0;
		sourceClip.y1 = source ? source->getDimension().Height : 0;
	}

	if ( destClipping )
	{
		destClip.x0 = destClipping->UpperLeftCorner.X;
		destClip.y0 = destClipping->UpperLeftCorner.Y;
		destClip.x1 = destClipping->LowerRightCorner.X;
		destClip.y1 = destClipping->LowerRightCorner.Y;
	}
	else
	{
		destClip.x0 = 0;
		destClip.y0 = 0;
		destClip.x1 = dest ? dest->getDimension().Width : 0;
		destClip.y1 = dest ? dest->getDimension().Height : 0;
	}

	v.x0 = destPos ? destPos->X : 0;
	v.y0 = destPos ? destPos->Y : 0;
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
		job.src = (void*) ( (u8*) source->lock() + ( job.Source.y0 * job.srcPitch ) + ( job.Source.x0 * job.srcPixelMul ) );
	}
	else
	{
		// use srcPitch for color operation on dest
		job.srcPitch = job.width * dest->getBytesPerPixel();
	}

	job.dstPitch = dest->getPitch();
	job.dstPixelMul = dest->getBytesPerPixel();
	job.dst = (void*) ( (u8*) dest->lock() + ( job.Dest.y0 * job.dstPitch ) + ( job.Dest.x0 * job.dstPixelMul ) );

	blitter( &job );

	if ( source )
		source->unlock();

	if ( dest )
		dest->unlock();

	return 1;
}


EzImplementRootRTTI(EzImage);

//! constructor
EzImage::EzImage(ECOLOR_FORMAT format, const EzDimension2d<s32>& size )
:Data(0), Size(size), Format(format), DeleteMemory(true), Depth(1), IsCompressed(false), MipMapData(0)
{
	initData();
}

//! constructor for empty 3D texture
EzImage::EzImage(ECOLOR_FORMAT format, const EzDimension2d< s32 >& size, const s32 & iDepth)
: Format(format), Size(EzDimension2d< s32 >(size.Width, size.Height)), Data(0), Depth(iDepth), IsCompressed(false), MipMapData(0)
{
	initData();
}

//! constructor 3d volume images
EzImage::EzImage(ECOLOR_FORMAT format, const EzDimension2d< s32 >& size, const s32 & iDepth, void* data, bool ownForeignMemory)
: Format(format), Size(EzDimension2d< s32 >( size.Width, size.Height )), Data(0), Depth(iDepth), IsCompressed(false), MipMapData(0)
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
		memcpy( Data, data, Size.Height * Size.Width * Depth * BytesPerPixel );
	}
}


//! constructor
EzImage::EzImage(ECOLOR_FORMAT format, const EzDimension2d<s32>& size, void* data,
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
		memcpy(Data, data, Size.Height * Pitch);
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
	Pitch = BytesPerPixel * Size.Width;

	if (!Data)
		Data = new s8[Size.Height * Pitch];
}


//! destructor
EzImage::~EzImage()
{
	if ( DeleteMemory )
	{
		delete [] (s8*)Data;

		if( MipMapData )
			delete [] MipMapData;
	}
}


//! Returns width and height of image data.
const EzDimension2d<s32>& EzImage::getDimension() const
{
	return Size;
}



//! Returns bits per pixel.
u32 EzImage::getBitsPerPixel() const
{
	return BitsPerPixel;
}


//! Returns bytes per pixel
u32 EzImage::getBytesPerPixel() const
{
	return BytesPerPixel;
}



//! Returns image data size in bytes
u32 EzImage::getImageDataSizeInBytes() const
{
	return Pitch * Size.Height;
}



//! Returns image data size in pixels
u32 EzImage::getImageDataSizeInPixels() const
{
	return Size.Width * Size.Height;
}



//! returns mask for red value of a pixel
u32 EzImage::getRedMask() const
{
	return RedMask;
}



//! returns mask for green value of a pixel
u32 EzImage::getGreenMask() const
{
	return GreenMask;
}



//! returns mask for blue value of a pixel
u32 EzImage::getBlueMask() const
{
	return BlueMask;
}



//! returns mask for alpha value of a pixel
u32 EzImage::getAlphaMask() const
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


u32 EzImage::getBitsPerPixelFromFormat(ECOLOR_FORMAT format)
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
void EzImage::setPixel(u32 x, u32 y, const SColor &color )
{
	if (x >= (u32)Size.Width || y >= (u32)Size.Height)
		return;

	switch(Format)
	{
		case ECF_A1R5G5B5:
		{
			u16 * dest = (u16*) ((u8*) Data + ( y * Pitch ) + ( x << 1 ));
			*dest = A8R8G8B8toA1R5G5B5( color.color );
		} break;

		case ECF_R5G6B5:
		{
			u16 * dest = (u16*) ((u8*) Data + ( y * Pitch ) + ( x << 1 ));
			*dest = A8R8G8B8toR5G6B5( color.color );
		} break;

		case ECF_R8G8B8:
		{
			u8* dest = (u8*) Data + ( y * Pitch ) + ( x * 3 );
			dest[0] = (u8)color.getRed();
			dest[1] = (u8)color.getGreen();
			dest[2] = (u8)color.getBlue();
		} break;

		case ECF_A8R8G8B8:
		{
			u32 * dest = (u32*) ((u8*) Data + ( y * Pitch ) + ( x << 2 ));
			*dest = color.color;
		} break;

		// jay adds from irrspintz
		case ECF_A4R4G4B4:
		{
			//u16* dest = (u16*) ((u8*) Data + ( y * Pitch ) + ( x << 1 ));
			//*dest = A8R8G8B8toA4R4G4B4( color.color );
			break;
		}

		case ECF_X8R8G8B8:
		{
			u32 * dest = (u32*) ((u8*) Data + ( y * Pitch ) + ( x << 2 ));
			*dest = color.color;
		} break;
	}
}


//! returns a pixel
SColor EzImage::getPixel(u32 x, u32 y) const
{
	if( IsCompressed )
	{
		//os::Printer::log("EzImage::Format is Compressed", ELL_ERROR);
		return SColor(0);
	}

	if (x >= (u32)Size.Width || y >= (u32)Size.Height)
		return SColor(0);

	switch(Format)
	{
	case ECF_A1R5G5B5:
		return A1R5G5B5toA8R8G8B8(((u16*)Data)[y*Size.Width + x]);
	case ECF_R5G6B5:
		return R5G6B5toA8R8G8B8(((u16*)Data)[y*Size.Width + x]);
	case ECF_A8R8G8B8:
		return ((u32*)Data)[y*Size.Width + x];
	case ECF_R8G8B8:
		{
			u8* p = &((u8*)Data)[(y*3)*Size.Width + (x*3)];
			return SColor(255,p[0],p[1],p[2]);
		}

	//jay adds from irrspintz
	case ECF_X8R8G8B8:
		return X8R8G8B8toA8R8G8B8(((u32*)Data)[y*Size.Width + x]);
	case ECF_I8:
		{
			u8 tmp = ((u8*)Data)[ y * Size.Width + x ];
			return SColor( tmp, tmp, tmp, tmp );
		}
	case ECF_I16:
		{
			u16 tmp = ((u16*)Data)[ y * Size.Width + x ];
			return SColor( tmp, tmp, tmp, tmp );
		}	
	case ECF_I32:
		{
			u32 tmp = ((u32*)Data)[ y * Size.Width + x ];
			return SColor( tmp, tmp, tmp, tmp );
		}
	}

	return SColor(0);
}

//! returns a pixel from a volume texture
SColor EzImage::getPixel(s32 x, s32 y, s32 z) const
{
	if( IsCompressed )
	{
		//os::Printer::log("EzImage::Format is Compressed", ELL_ERROR);
		return SColor(0);
	}


	if(x < 0 || y < 0 || x >= Size.Width || y >= Size.Height)
		return SColor(0);

	if( Depth == 1 )
		z = 0;

	switch(Format)
	{
		case ECF_A1R5G5B5:
			return A1R5G5B5toA8R8G8B8( ((s16*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ] );
		case ECF_R5G6B5:
			return R5G6B5toA8R8G8B8( ((s16*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ] );
		case ECF_A8R8G8B8:
			return ((s32*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ];
		case ECF_R8G8B8:
		{
			u8* p = &((u8*)Data)[ ( z * 3 * Size.Width * Size.Height ) + ( y * 3 * Size.Width ) + ( x * 3 ) ];
			return SColor( 255, p[0], p[1], p[2] );
		}
		case ECF_I8:
		{
			u8 tmp = ((u8*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ];
			return SColor( tmp, tmp, tmp, tmp );
		}
		case ECF_I16:
		{
			u16 tmp = ((u16*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ];
			return SColor( tmp, tmp, tmp, tmp );
		}
		case ECF_I32:
		{
			u32 tmp = ((u32*)Data)[ ( z * Size.Width * Size.Height ) + ( y * Size.Width ) + ( x ) ];
			return SColor( tmp, tmp, tmp, tmp );
		}
	}

	return SColor(0);
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
	EzDimension2d< s32 > currentLevelSize = Size;
	u32 offset = 0;
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
void EzImage::copyToScaling(void* target, s32 width, s32 height, ECOLOR_FORMAT format, u32 pitch)
{
	if (!target || !width || !height)
		return;

	const u32 bpp=getBitsPerPixelFromFormat(format)/8;
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
			u8* tgtpos = (u8*) target;
			u8* dstpos = (u8*) Data;
			const u32 bwidth = width*bpp;
			for (s32 y=0; y<height; ++y)
			{
				memcpy(target, Data, height*pitch);
				memset(tgtpos+width, 0, pitch-bwidth);
				tgtpos += pitch;
				dstpos += Pitch;
			}
			return;
		}
	}

	const f32 sourceXStep = (f32)Size.Width / (f32)width;
	const f32 sourceYStep = (f32)Size.Height / (f32)height;
	s32 yval=0, syval=0;
	f32 sy = 0.0f;
	for (s32 y=0; y<height; ++y)
	{
		f32 sx = 0.0f;
		for (s32 x=0; x<width; ++x)
		{
			EzColorConverter::convert_viaFormat(((u8*)Data)+ syval + ((s32)sx)*BytesPerPixel, Format, 1, ((u8*)target)+ yval + (x*bpp), format);
			sx+=sourceXStep;
		}
		sy+=sourceYStep;
		syval=((s32)sy)*Pitch;
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

	const EzDimension2d<s32>& targetSize = target->getDimension();

	if (targetSize==Size)
	{
		copyTo(target);
		return;
	}

	copyToScaling(target->lock(), targetSize.Width, targetSize.Height, target->getColorFormat());
	target->unlock();
}


//! copies this surface into another
void EzImage::copyTo(EzImage* target, const EzPosition2d<s32>& pos)
{
	Blit(BLITTER_TEXTURE, target, 0, &pos, this, 0, 0);
}


//! copies this surface into another
void EzImage::copyTo(EzImage* target, const EzPosition2d<s32>& pos, const EzRect<s32>& sourceRect, const EzRect<s32>* clipRect)
{
	Blit(BLITTER_TEXTURE, target, clipRect, &pos, this, &sourceRect, 0);
}



//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
void EzImage::copyToWithAlpha(EzImage* target, const EzPosition2d<s32>& pos, const EzRect<s32>& sourceRect, const SColor &color, const EzRect<s32>* clipRect)
{
	// color blend only necessary on not full spectrum aka. color.color != 0xFFFFFFFF
	Blit(color.color == 0xFFFFFFFF ? BLITTER_TEXTURE_ALPHA_BLEND: BLITTER_TEXTURE_ALPHA_COLOR_BLEND,
			target, clipRect, &pos, this, &sourceRect, color.color);
}
