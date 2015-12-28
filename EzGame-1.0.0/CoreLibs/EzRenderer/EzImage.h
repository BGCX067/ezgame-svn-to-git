// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_H_INCLUDED__
#define __C_IMAGE_H_INCLUDED__


#include "../EzCore/EzCore.h"
#include "../EzCore/EzRTTI.h"
#include "../EzCore/EzRefObject.h"


//! EzImage implementation with a lot of special image operations for
//! 16 bit A1R5G5B5/32 Bit A8R8G8B8 images, which are used by the SoftwareDevice.
class EzImage : public EzRefObject
{
	EzDeclareRootRTTI(EzImage);

public:

	//! constructor from another image with format change
	EzImage(ECOLOR_FORMAT format, EzImage* imageToCopy);

	//! constructor for empty 3D texture
	EzImage(ECOLOR_FORMAT format, const EzDimension2d& size);

	//! constructor from raw image data
	//! \param useForeignMemory: If true, the image will use the data pointer
	//! directly and own it from now on, which means it will also try to delete [] the
	//! data when the image will be destructed. If false, the memory will by copied.
	EzImage(ECOLOR_FORMAT format, const EzDimension2d& size,
		void* data, bool ownForeignMemory=true, bool deleteMemory = true);

	//! constructor 3d volume images
	EzImage(ECOLOR_FORMAT format, const EzDimension2d& size, const int & iDepth,
		void* data, bool ownForeignMemory=true);

	//! constructor for empty image
	EzImage(ECOLOR_FORMAT format, const EzDimension2d& size, const int & iDepth);

	//! destructor
	virtual ~EzImage();

	//! Lock function.
	virtual void* lock()
	{
		return Data;
	}

	//! Unlock function.
	virtual void unlock() {}

	//! Returns width and height of image data.
	virtual const EzDimension2d& getDimension() const;

	//! Returns bits per pixel.
	virtual unsigned int getBitsPerPixel() const;

	//! Returns bytes per pixel
	virtual unsigned int getBytesPerPixel() const;

	//! Returns image data size in bytes
	virtual unsigned int getImageDataSizeInBytes() const;

	//! Returns image data size in pixels
	virtual unsigned int getImageDataSizeInPixels() const;

	//! returns mask for red value of a pixel
	virtual unsigned int getRedMask() const;

	//! returns mask for green value of a pixel
	virtual unsigned int getGreenMask() const;

	//! returns mask for blue value of a pixel
	virtual unsigned int getBlueMask() const;

	//! returns mask for alpha value of a pixel
	virtual unsigned int getAlphaMask() const;

	//! returns a pixel
	virtual SColor getPixel(unsigned int x, unsigned int y) const;

	//! returns a pixel from a volume texture
	SColor getPixel(int x, int y, int z) const;

	//! sets a pixel
	virtual void setPixel(unsigned int x, unsigned int y, const SColor &color );

	//! returns the color format
	virtual ECOLOR_FORMAT getColorFormat() const;

	//! returns pitch of image
	virtual unsigned int getPitch() const { return Pitch; }

	unsigned int getBitsPerPixelFromFormat(ECOLOR_FORMAT format);


	//! generates mipmaps for the image
	virtual void generateMipMaps(bool force=true);

	//! Returns whether or not this image has mipmap data generated
	virtual bool hasMipMaps() { return ( MipLevelInfo.size() > 0 ); }

	//! Returns the number of mip levels this image has.
	virtual unsigned int getMipLevels() { return MipLevelInfo.size(); }

	//! Returns the offset in bytes to the start of data for a mipLevel
	virtual unsigned int getMipLevelDataOffset( const unsigned int& mipLevel )
	{
		if( mipLevel > 0 && mipLevel <= MipLevelInfo.size() )
		{
			return MipLevelInfo[mipLevel-1].Offset;
		}

		return 0;
	}

	//! Returns the pitch of the data for the requested mipLevel.
	virtual unsigned int getMipLevelPitch( const unsigned int& mipLevel )
	{
		if( mipLevel > 0 && mipLevel <= MipLevelInfo.size() )
		{
			return MipLevelInfo[mipLevel-1].Pitch;
		}

		return 0;
	}


	virtual void copyToScaling(void* target, int width, int height, ECOLOR_FORMAT format, int pitch=0);

	//! copies this surface into another, scaling it to fit.
	virtual void copyToScaling(EzImage* target);

	//! copies this surface into another
	virtual void copyTo(EzImage* target, const EzPosition2d& pos=EzPosition2d(0,0));

	//! copies this surface into another
	virtual void copyTo(EzImage* target, const EzPosition2d& pos, const EzRect& sourceRect, const EzRect* clipRect=0);

	//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
	virtual void copyToWithAlpha(EzImage* target, const EzPosition2d& pos,
			const EzRect& sourceRect, const SColor &color,
			const EzRect* clipRect = 0);

private:

	//! assumes format and size has been set and creates the rest
	void initData();

	void setBitMasks();

	inline SColor getPixelBox ( int x, int y, int fx, int fy, int bias ) const;

	void* Data;
	char* MipMapData;
	EzDimension2d Size;
	int Depth;
	int BitsPerPixel;
	int BytesPerPixel;
	unsigned int Pitch;
	ECOLOR_FORMAT Format;

	unsigned int RedMask;
	unsigned int GreenMask;
	unsigned intBlueMask;
	unsigned int AlphaMask;

	bool DeleteMemory;
	bool IsCompressed;

	struct SMipMapLevel
	{
		EzDimension2d Size;
		unsigned int Pitch;
		unsigned int Offset;
		unsigned int Bytes;
	};
	EzArray< SMipMapLevel > MipLevelInfo;


};


#endif

