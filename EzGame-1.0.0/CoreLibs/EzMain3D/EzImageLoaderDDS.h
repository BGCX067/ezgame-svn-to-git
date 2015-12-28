// Copyright (C) 2002-2007 Thomas Ince
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_LOADER_DDS_H_INCLUDED__
#define __C_IMAGE_LOADER_DDS_H_INCLUDED__

#include "EzImageLoader.h"
#include <stdio.h>

class EzImageLoaderDDS : public EzImageLoader
{
public:
	//! constructor
	EzImageLoaderDDS();

	//! destructor
	virtual ~EzImageLoaderDDS();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	virtual bool isALoadableFileExtension(const char* fileName) const;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(EzFile* file) const;

	//! creates a surface from the file
	virtual EzImage* loadImage(EzFile* file) const;

private:
	struct S_PIXEL_FORMAT
	{
		unsigned int	size;
		unsigned int flags;
		unsigned int fourCC;
		unsigned int RGBBitCount;
		unsigned int	redBitMask;
		unsigned int greenBitMask;
		unsigned int blueBitMask;
		unsigned int	RGBAlphaMask;
	};

	struct S_CAPS2
	{
		unsigned int	caps1;
		unsigned int	caps2;
		unsigned int	reserved[2];
	};

	struct S_SURFACE_FORMAT_HEADER
	{
		unsigned int				size;
		unsigned int				flags;
		unsigned int				height;
		unsigned int				width;
		unsigned int				pitch;
		unsigned int				depth;
		unsigned int				mipMapCount;
		unsigned int				reserved1[11];
		S_PIXEL_FORMAT	pixelFormat;
		S_CAPS2			caps;
		unsigned int				reserved2;
	};

	struct S_R5G6B5
	{
		unsigned short b : 5;
		unsigned short g : 6;
		unsigned short r : 5;

		unsigned int toA8R8G8B8()
		{
			//video::EzColor tmp( 255,
			//	r * 0xFF / 0x1F,
			//	g * 0xFF / 0x3F,
			//	b * 0xFF / 0x1F );
			//return tmp.toA8R8G8B8();
			return 0xFF000000 | ( ( r * 0xFF ) / 0x1F ) | ( ( g * 0xFF ) / 0x3F ) | ( ( b * 0xFF ) / 0x1F );
		}

		bool operator > ( const S_R5G6B5& other ) const
		{
			//video::EzColor tmp1( 255,
			//	r * 0xFF / 0x1F,
			//	g * 0xFF / 0x3F,
			//	b * 0xFF / 0x1F );
			//video::EzColor tmp2( 255,
			//	other.r * 0xFF / 0x1F,
			//	other.g * 0xFF / 0x3F,
			//	other.b * 0xFF / 0x1F );
			//return tmp1.toA8R8G8B8() > tmp2.toA8R8G8B8();
			return (*(unsigned short*)this) > (*(unsigned short*)&other);
		}
	};
};





#endif // __C_IMAGE_LOADER_DDS_H_INCLUDED__


