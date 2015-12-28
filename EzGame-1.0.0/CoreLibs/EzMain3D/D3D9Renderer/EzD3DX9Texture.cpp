// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h


#include "../EzImage.h"
#include "EzD3DX9Texture.h"
#include "EzD3DX9Driver.h"
#include "EzD3DX9Mappings.h"
#include <d3dx9tex.h>


#ifdef _USE_D3DXFilterTexture_
#pragma comment(lib, "d3dx9.lib")
#endif


EzImplementRTTI(EzD3DX9Texture, EzTexture);

//! rendertarget constructor
EzD3DX9Texture::EzD3DX9Texture(EzD3DX9Driver* driver, const EzDimension2d& size, const char* name)
: EzTexture(name, size), Texture(0), RTTSurface(0), Driver(driver), DepthSurface(0),
	HasMipMaps(false), HardwareMipMaps(false), IsRenderTarget(true), SurfaceHasSameSize(false), IsCompressedData(false)
{
	Device=driver->getExposedVideoData().D3D9.D3DDev9;
	if (Device)
		Device->AddRef();

	createRenderTarget();
}


//! constructor
EzD3DX9Texture::EzD3DX9Texture(EzImage* image, EzD3DX9Driver* driver,
					   unsigned int flags, const char* name)
: EzTexture(name), Texture(0), RTTSurface(0), Driver(driver), DepthSurface(0),
HasMipMaps(false), HardwareMipMaps(false), IsRenderTarget(false), SurfaceHasSameSize(false), IsCompressedData(false)
{


	const bool generateMipLevels = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);

	Device=driver->getExposedVideoData().D3D9.D3DDev9;
	if (Device)
		Device->AddRef();

	if (image)
	{
		if (createTexture(flags, image))
		{
			if (copyTexture(image) && generateMipLevels)
			{
				// create mip maps.
				#ifdef _USE_D3DXFilterTexture_
					// The D3DXFilterTexture function seems to get linked wrong when
					// compiling with both D3D8 and 9, causing it not to work in the D3D9 device.
					// So mipmapgeneration is replaced with my own bad generation
					HRESULT hr  = D3DXFilterTexture(Texture, NULL, D3DX_DEFAULT, D3DX_DEFAULT);
					if (FAILED(hr))
//						os::Printer::log("Could not create direct3d mip EzMap levels.", ELL_WARNING);
					else
						HasMipMaps = true;
				#else
					HRESULT hr  = stubD3DXFilterTexture( Texture, NULL, D3DX_DEFAULT, D3DX_DEFAULT );
					if( FAILED( hr ) )
					{
//						os::Printer::log("Could not create direct3d mip EzMap levels.", ELL_WARNING);
					}
					else
					{
						HasMipMaps = true;
					}

				//	createMipMaps();
				//	HasMipMaps = true;
				#endif
			}
		}
//		else
////			os::Printer::log("Could not create DIRECT3D9 Texture.", ELL_WARNING);
	}
}


//! destructor
EzD3DX9Texture::~EzD3DX9Texture()
{
	if (Texture)
		Texture->Release();

	if (RTTSurface)
		RTTSurface->Release();

	//// if this texture was the last one using the depth buffer
	//// we can release the surface. We only use the value of the pointer
	//// hence it is safe to use the dropped pointer...
	//if (DepthSurface)
	//	if (DepthSurface->drop())
	//		Driver->removeDepthSurface(DepthSurface);

	if (Device)
		Device->Release();
}


void EzD3DX9Texture::createRenderTarget()
{
	//// are texture size restrictions there ?
	//if(!Driver->queryFeature(EVDF_TEXTURE_NPOT))
	//{
	//	TextureSize.Width = getTextureSizeFromSurfaceSize(TextureSize.Width);
	//	TextureSize.Height = getTextureSizeFromSurfaceSize(TextureSize.Height);
	//	if (TextureSize != ImageSize)
	//		os::Printer::log("RenderTarget size has to be a power of two", ELL_INFORMATION);
	//}

	//// get irrlicht format from backbuffer
	//ColorFormat = Driver->getColorFormat();
	//D3DFORMAT d3dformat = Driver->getD3DColorFormat();
	//setPitch(d3dformat);

	//// create texture
	//HRESULT hr;

	//hr = Device->CreateTexture(
	//	TextureSize.Width,
	//	TextureSize.Height,
	//	1, // mip EzMap level count, we don't want mipmaps here
	//	D3DUSAGE_RENDERTARGET,
	//	d3dformat,
	//	D3DPOOL_DEFAULT,
	//	&Texture,
	//	NULL);

	//if (FAILED(hr))
	//{
	//	if (D3DERR_INVALIDCALL == hr)
	//		os::Printer::log("Could not create render target texture", "Invalid Call");
	//	else
	//	if (D3DERR_OUTOFVIDEOMEMORY == hr)
	//		os::Printer::log("Could not create render target texture", "Out of Video Memory");
	//	else
	//	if (E_OUTOFMEMORY == hr)
	//		os::Printer::log("Could not create render target texture", "Out of Memory");
	//	else
	//		os::Printer::log("Could not create render target texture");
	//}
}


bool EzD3DX9Texture::createMipMaps(unsigned int level)
{
//	if (level==0)
//		return true;
//
//	if (HardwareMipMaps && Texture)
//	{
//		// generate mipmaps in hardware
//		Texture->GenerateMipSubLevels();
//		return true;
//	}
//	// os::Printer::log("manual mipmap");
//
//	IDirect3DSurface9* upperSurface = 0;
//	IDirect3DSurface9* lowerSurface = 0;
//
//	// get upper level
//	HRESULT hr = Texture->GetSurfaceLevel(level-1, &upperSurface);
//	if (FAILED(hr) || !upperSurface)
//	{
////		os::Printer::log("Could not get upper surface level for mip EzMap generation", ELL_WARNING);
//		return false;
//	}
//
//	// get lower level
//	hr = Texture->GetSurfaceLevel(level, &lowerSurface);
//	if (FAILED(hr) || !lowerSurface)
//	{
//	//	os::Printer::log("Could not get lower surface level for mip EzMap generation", ELL_WARNING);
//		upperSurface->Release();
//		return false;
//	}
//
//	D3DSURFACE_DESC upperDesc, lowerDesc;
//	upperSurface->GetDesc(&upperDesc);
//	lowerSurface->GetDesc(&lowerDesc);
//
//	D3DLOCKED_RECT upperlr;
//	D3DLOCKED_RECT lowerlr;
//
//	// lock upper surface
//	if (FAILED(upperSurface->LockRect(&upperlr, NULL, 0)))
//	{
//		upperSurface->Release();
//		lowerSurface->Release();
//	//	os::Printer::log("Could not lock upper texture for mip EzMap generation", ELL_WARNING);
//		return false;
//	}
//
//	// lock lower surface
//	if (FAILED(lowerSurface->LockRect(&lowerlr, NULL, 0)))
//	{
//		upperSurface->UnlockRect();
//		upperSurface->Release();
//		lowerSurface->Release();
//	//	os::Printer::log("Could not lock lower texture for mip EzMap generation", ELL_WARNING);
//		return false;
//	}
//
//	if (upperDesc.Format != lowerDesc.Format)
//	{
//	//	os::Printer::log("Cannot copy mip maps with different formats.", ELL_WARNING);
//	}
//	else
//	{
//		if ((upperDesc.Format == D3DFMT_A1R5G5B5) || (upperDesc.Format == D3DFMT_R5G6B5))
//			copy16BitMipMap((char*)upperlr.pBits, (char*)lowerlr.pBits,
//					lowerDesc.Width, lowerDesc.Height,
//					upperlr.Pitch, lowerlr.Pitch);
//		else
//		if (upperDesc.Format == D3DFMT_A8R8G8B8)
//			copy32BitMipMap((char*)upperlr.pBits, (char*)lowerlr.pBits,
//					lowerDesc.Width, lowerDesc.Height,
//					upperlr.Pitch, lowerlr.Pitch);
//		else
//		//	os::Printer::log("Unsupported mipmap format, cannot copy.", ELL_WARNING);
//	}
//
//	bool result=true;
//	// unlock
//	if (FAILED(upperSurface->UnlockRect()))
//		result=false;
//	if (FAILED(lowerSurface->UnlockRect()))
//		result=false;
//
//	// release
//	upperSurface->Release();
//	lowerSurface->Release();
//
//	if (!result || (upperDesc.Width <= 3 && upperDesc.Height <= 3))
//		return result; // stop generating levels
//
//	// generate next level
//	return createMipMaps(level+1);

	return true;
}


//! creates the hardware texture
bool EzD3DX9Texture::createTexture(unsigned int flags, EzImage * image)
{
	EzDimension2d optSize;
	m_ImageSize = image->getDimension();

	if (Driver->queryFeature(EVDF_TEXTURE_NPOT))
		optSize=m_ImageSize;
	else
	{
		optSize.Width = getTextureSizeFromSurfaceSize(m_ImageSize.Width);
		optSize.Height = getTextureSizeFromSurfaceSize(m_ImageSize.Height);
	}

	HRESULT hr;

	SurfaceHasSameSize = m_ImageSize == optSize;

	D3DFORMAT format = D3DFMT_A1R5G5B5;

	switch(image->getColorFormat())
	{
	case ECF_A1R5G5B5:
		format = D3DFMT_A1R5G5B5;
		break;
	case ECF_A8R8G8B8:
		format = D3DFMT_A8R8G8B8;
		break;
	case ECF_X8R8G8B8:
		format = D3DFMT_X8R8G8B8;
		break;
	case ECF_A4R4G4B4:
		format = D3DFMT_A4R4G4B4;
		break;
	case ECF_R5G6B5:
		format = D3DFMT_R5G6B5;
		break;
	case ECF_R8G8B8:
	// devices do no support 24 bit color formats, only 16 or 24, convert to a 32 bit format
		format = D3DFMT_A8R8G8B8;
		break;
	case ECF_L8:
		format = D3DFMT_L8;
		break;
	case ECF_DXT1:
		format = D3DFMT_DXT1;
		IsCompressedData = true;
		break;
	case ECF_DXT2:
		format = D3DFMT_DXT2;
		IsCompressedData = true;
		break;
	case ECF_DXT3:
		format = D3DFMT_DXT3;
		IsCompressedData = true;
		break;
	case ECF_DXT4:
		format = D3DFMT_DXT4;
		IsCompressedData = true;
		break;
	case ECF_DXT5:
		format = D3DFMT_DXT5;
		IsCompressedData = true;
		break;
	default:
		return false;
	}
	
	if (Driver->getTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL))
	{
		if (format == D3DFMT_A8R8G8B8)
			format = D3DFMT_R8G8B8;
		else if (format == D3DFMT_A1R5G5B5)
			format = D3DFMT_R5G6B5;
	}

	const bool mipmaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);

	DWORD usage = 0;

	// This enables hardware mip EzMap generation.
	if (mipmaps && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
	{
		LPDIRECT3D9 intf = Driver->getExposedVideoData().D3D9.D3D9;
		D3DDISPLAYMODE d3ddm;
		intf->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

		if (D3D_OK==intf->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_AUTOGENMIPMAP,D3DRTYPE_TEXTURE,format))
		{
			usage = D3DUSAGE_AUTOGENMIPMAP;
			HardwareMipMaps = true;
		}
	}

	hr = Device->CreateTexture(optSize.Width, optSize.Height,
		mipmaps ? 0 : 1, // number of mipmaplevels (0 = automatic all)
		usage, // usage
		format, D3DPOOL_MANAGED , &Texture, NULL);

	if (FAILED(hr))
	{
		// try brute force 16 bit
		HardwareMipMaps = false;
		if (format == D3DFMT_A8R8G8B8)
			format = D3DFMT_A1R5G5B5;
		else if (format == D3DFMT_R8G8B8)
			format = D3DFMT_R5G6B5;
		else
			return false;

		hr = Device->CreateTexture(optSize.Width, optSize.Height,
			mipmaps ? 0 : 1, // number of mipmaplevels (0 = automatic all)
			0, format, D3DPOOL_MANAGED, &Texture, NULL);
	}

	m_eColorFormat = EzD3DX9Mappings::get(format);

	return (SUCCEEDED(hr));
}


//! copies the image to the texture
bool EzD3DX9Texture::copyTexture(EzImage * image)
{

	if( Texture && image )
	{
		D3DSURFACE_DESC desc;
		Texture->GetLevelDesc( 0, &desc );

		m_TextureSize.Width = desc.Width;
		m_TextureSize.Height = desc.Height;

		EzImage* tmpImage = image;
		bool deleteTmpImage = false;

		// devices do no support 24 bit color formats, only 16 or 24, convert to a 32 bit format
		if( m_eColorFormat == ECF_R8G8B8 || ( IsCompressedData && !Driver->queryFeature( EVDF_COMPRESSED_TEXTURES ) ) )
		{
			tmpImage = EzNew EzImage( ECF_A8R8G8B8, image );
			deleteTmpImage = true;
			m_eColorFormat = ECF_A8R8G8B8;
		}

		// Check that image and texture sizes match, if not, copy data to texture size 
		if( !SurfaceHasSameSize )
		{
			EzImage* oldImage = tmpImage;
			tmpImage = EzNew EzImage( m_eColorFormat, m_TextureSize );
			((EzImage*)oldImage)->copyToScaling( (EzImage*)tmpImage );
			
			if( deleteTmpImage )
				oldImage->release();
			
			deleteTmpImage = true;
		}

		m_iPitch = tmpImage->getPitch();

		IDirect3DSurface9* pSurface = 0;
		HRESULT hr = Texture->GetSurfaceLevel( 0, &pSurface );
		if( FAILED( hr ) )
		{
//			os::Printer::log("Could not GetSurfaceLevel DIRECT3D9 Texture.", ELL_ERROR);
			return false;
		}

		RECT srcRect;
		srcRect.top = srcRect.left = 0;
		srcRect.right = m_TextureSize.Width;
		srcRect.bottom = m_TextureSize.Height;
		
		if( FAILED( stubD3DXLoadSurfaceFromMemory( pSurface, NULL, NULL, tmpImage->lock(), EzD3DX9Mappings::get(m_eColorFormat), m_iPitch, NULL, &srcRect, D3DX_DEFAULT, 0 ) ) )
		{
			// Maybe, if this fails, and the surfaces are not the same size and NPOT_TEXTURES is not supported, resize the surface to be power of two
	//		os::Printer::log( "Could not Load Surface From Memory.", ELL_ERROR );
			return false;
		}

		tmpImage->unlock();
	
		if( deleteTmpImage )
			tmpImage->release();
	}

	return true;
}


//! lock function
void* EzD3DX9Texture::lock(bool readOnly)
{
	if (!Texture)
		return 0;

	HRESULT hr;
	D3DLOCKED_RECT EzRect;
	if(!IsRenderTarget)
	{
		hr = Texture->LockRect(0, &EzRect, 0, readOnly?D3DLOCK_READONLY:0);
		if (FAILED(hr))
		{
		//	os::Printer::log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
			return 0;
		}

		return EzRect.pBits;
	}
	else
	{
		D3DSURFACE_DESC desc;
		Texture->GetLevelDesc(0, &desc);
		if (!RTTSurface)
		{
			hr = Device->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &RTTSurface, 0);
			if (FAILED(hr))
			{
		//		os::Printer::log("Could not lock DIRECT3D9 Texture", "Offscreen surface creation failed.", ELL_ERROR);
				return 0;
			}
		}

		IDirect3DSurface9 *surface = 0;
		hr = Texture->GetSurfaceLevel(0, &surface);
		if (FAILED(hr))
		{
	//		os::Printer::log("Could not lock DIRECT3D9 Texture", "Could not get surface.", ELL_ERROR);
			return 0;
		}
		hr = Device->GetRenderTargetData(surface, RTTSurface);
		surface->Release();
		if(FAILED(hr))
		{
//			os::Printer::log("Could not lock DIRECT3D9 Texture", "Data copy failed.", ELL_ERROR);
			return 0;
		}
		hr = RTTSurface->LockRect(&EzRect, 0, readOnly?D3DLOCK_READONLY:0);
		if(FAILED(hr))
		{
		//	os::Printer::log("Could not lock DIRECT3D9 Texture", "LockRect failed.", ELL_ERROR);
			return 0;
		}
		return EzRect.pBits;
	}
}


//! unlock function
void EzD3DX9Texture::unlock()
{
	if (!Texture)
		return;

	if (!IsRenderTarget)
		Texture->UnlockRect(0);
	else if (RTTSurface)
		RTTSurface->UnlockRect();
}




//! returns the size of a texture which would be the optimize size for rendering it
inline int EzD3DX9Texture::getTextureSizeFromSurfaceSize(int size) const
{
	int ts = 0x01;

	while(ts < size)
		ts <<= 1;

	return ts;
}


//! returns the DIRECT3D9 Texture
IDirect3DBaseTexture9* EzD3DX9Texture::getDX9Texture() const
{
	return Texture;
}


//! returns if texture has mipmap levels
bool EzD3DX9Texture::hasMipMaps() const
{
	return HasMipMaps;
}


void EzD3DX9Texture::copy16BitMipMap(char* src, char* tgt,
				   int width, int height,
				   int pitchsrc, int pitchtgt) const
{
	for (int y=0; y<height; ++y)
	{
		for (int x=0; x<width; ++x)
		{
			unsigned int a=0, r=0, g=0, b=0;

			for (int dy=0; dy<2; ++dy)
			{
				const int tgy = (y*2)+dy;
				for (int dx=0; dx<2; ++dx)
				{
					const int tgx = (x*2)+dx;

					EzColor c;
					if (m_eColorFormat == ECF_A1R5G5B5)
						c = A1R5G5B5toA8R8G8B8(*(unsigned short*)(&src[(tgx*2)+(tgy*pitchsrc)]));
					else
						c = R5G6B5toA8R8G8B8(*(unsigned short*)(&src[(tgx*2)+(tgy*pitchsrc)]));

					a += c.getAlpha();
					r += c.getRed();
					g += c.getGreen();
					b += c.getBlue();
				}
			}

			a /= 4;
			r /= 4;
			g /= 4;
			b /= 4;

			unsigned short c;
			if (m_eColorFormat == ECF_A1R5G5B5)
				c = RGBA16(r,g,b,a);
			else
				c = A8R8G8B8toR5G6B5(EzColor(a,r,g,b).color);
			*(unsigned short*)(&tgt[(x*2)+(y*pitchtgt)]) = c;
		}
	}
}


void EzD3DX9Texture::copy32BitMipMap(char* src, char* tgt,
				   int width, int height,
				   int pitchsrc, int pitchtgt) const
{
	for (int y=0; y<height; ++y)
	{
		for (int x=0; x<width; ++x)
		{
			unsigned int a=0, r=0, g=0, b=0;
			EzColor c;

			for (int dy=0; dy<2; ++dy)
			{
				const int tgy = (y*2)+dy;
				for (int dx=0; dx<2; ++dx)
				{
					const int tgx = (x*2)+dx;

					c = *(unsigned int*)(&src[(tgx*4)+(tgy*pitchsrc)]);

					a += c.getAlpha();
					r += c.getRed();
					g += c.getGreen();
					b += c.getBlue();
				}
			}

			a /= 4;
			r /= 4;
			g /= 4;
			b /= 4;

			c.set(a, r, g, b);
			*(unsigned int*)(&tgt[(x*4)+(y*pitchtgt)]) = c.color;
		}
	}
}


//! Regenerates the mip EzMap levels of the texture. Useful after locking and
//! modifying the texture
void EzD3DX9Texture::regenerateMipMapLevels()
{

	//if( HasMipMaps )
	//{
	//	HRESULT hr  = Driver->stubD3DXFilterTexture( Texture, NULL, D3DX_DEFAULT, D3DX_DEFAULT );
	//	if( FAILED( hr ) )
	//		os::Printer::log("Could not create direct3d mip EzMap levels.", ELL_WARNING);
	//	else
	//		HasMipMaps = true;
	//}

	//if (HasMipMaps)
	//	createMipMaps();
}


//! returns if it is a render target
bool EzD3DX9Texture::isRenderTarget() const
{
	return IsRenderTarget;
}


//! Returns pointer to the render target surface
IDirect3DSurface9* EzD3DX9Texture::getRenderTargetSurface()
{
	if (!IsRenderTarget)
		return 0;

	IDirect3DSurface9 *pRTTSurface = 0;
	if (Texture)
		Texture->GetSurfaceLevel(0, &pRTTSurface);

	if (pRTTSurface)
		pRTTSurface->Release();

	return pRTTSurface;
}


void EzD3DX9Texture::setPitch(D3DFORMAT d3dformat)
{
	switch(d3dformat)
	{
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		m_iPitch = m_TextureSize.Width * 2;
	break;
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		m_iPitch = m_TextureSize.Width * 4;
	break;
	case D3DFMT_R5G6B5:
		m_iPitch = m_TextureSize.Width * 2;
	break;
	case D3DFMT_R8G8B8:
		m_iPitch = m_TextureSize.Width * 3;
	break;
	default:
		m_iPitch = 0;
	};
}


