// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __EZDIRECTX9_TEXTURE_H_INCLUDED__
#define __EZDIRECTX9_TEXTURE_H_INCLUDED__


#include "../EzTexture.h"
#include "../EzImage.h"
#include <d3d9.h>


class EzD3DX9Driver;
// forward declaration for RTT depth buffer handling
struct SDepthSurface;
/*!
	interface for a Video Driver dependent Texture.
*/
class EzD3DX9Texture : public EzTexture
{

	EzDeclareRTTI;

public:

	//! constructor
	EzD3DX9Texture(EzImage* image, EzD3DX9Driver* driver,
		u32 flags, const char* name);

	//! rendertarget constructor
	EzD3DX9Texture(EzD3DX9Driver* driver, const EzDimension2d<s32>& size, const char* name);

	//! destructor
	virtual ~EzD3DX9Texture();

	//! lock function
	virtual void* lock(bool readOnly = false);

	//! unlock function
	virtual void unlock();

	//! returns driver type of texture (=the driver, who created the texture)
	virtual EDRIVER_TYPE getDriverType() const;

	//! returns the DIRECT3D9 Texture
	IDirect3DBaseTexture9* getDX9Texture() const;

	//! returns if texture has mipmap levels
	bool hasMipMaps() const;

	//! Regenerates the mip EzMap levels of the texture. Useful after locking and
	//! modifying the texture
	virtual void regenerateMipMapLevels();

	//! returns if it is a render target
	virtual bool isRenderTarget() const;

	//! Returns pointer to the render target surface
	IDirect3DSurface9* getRenderTargetSurface();

private:
	friend class EzD3DX9Driver;

	void createRenderTarget();

	//! returns the size of a texture which would be the optimize size for rendering it
	inline s32 getTextureSizeFromSurfaceSize(s32 size) const;

	//! creates the hardware texture
	bool createTexture(u32 flags, EzImage * image);

	//! copies the image to the texture
	bool copyTexture(EzImage * image);

	//! Helper function for mipmap generation.
	bool createMipMaps(u32 level=1);

	//! Helper function for mipmap generation.
	void copy16BitMipMap(char* src, char* tgt,
		s32 width, s32 height,  s32 pitchsrc, s32 pitchtgt) const;

	//! Helper function for mipmap generation.
	void copy32BitMipMap(char* src, char* tgt,
		s32 width, s32 height,  s32 pitchsrc, s32 pitchtgt) const;

	//! set Pitch based on the d3d format
	void setPitch(D3DFORMAT d3dformat);

	IDirect3DDevice9* Device;
	IDirect3DTexture9* Texture;
	IDirect3DSurface9* RTTSurface;
	EzD3DX9Driver* Driver;
	SDepthSurface* DepthSurface;

	bool SurfaceHasSameSize;

	bool HasMipMaps;
	bool HardwareMipMaps;
	bool IsRenderTarget;
	bool IsCompressedData;
};


#endif 




