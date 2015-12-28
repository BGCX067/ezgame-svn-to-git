// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_LOADER_TGA_H_INCLUDED__
#define __C_IMAGE_LOADER_TGA_H_INCLUDED__


#include "EzImageLoader.h"
#include <stdio.h>



// byte-align structures
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__) 
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	define PACK_STRUCT
#endif

	// these structs are also used in the TGA writer
	struct STGAHeader{
		unsigned char IdLength;
		unsigned char ColorMapType;
		unsigned char ImageType;
		unsigned char FirstEntryIndex[2];
		unsigned short ColorMapLength;
		unsigned char ColorMapEntrySize;
		unsigned char XOrigin[2];
		unsigned char YOrigin[2];
		unsigned short ImageWidth;
		unsigned short ImageHeight;
		unsigned char PixelDepth;
		unsigned char ImageDescriptor;
	} PACK_STRUCT;

	struct STGAFooter
	{
		unsigned int ExtensionOffset;
		unsigned int DeveloperOffset;
		char  Signature[18];
	} PACK_STRUCT;

// Default alignment
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__) 
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

/*!
	Surface Loader for targa images
*/

class EzImageLoaderTGA : public EzImageLoader
{
public:

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	virtual bool isALoadableFileExtension(const char* fileName) const;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(EzFile* file) const;

	//! creates a surface from the file
	virtual EzImage* loadImage(EzFile* file) const;

private:

	//! loads a compressed tga. Was written and sent in by Jon Pry, thank you very much!
	unsigned char* loadCompressedImage(EzFile* file, const STGAHeader& header) const;
};


#endif

