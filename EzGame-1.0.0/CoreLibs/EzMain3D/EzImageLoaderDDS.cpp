// Copyright (C) 2002-2007 Thomas Ince
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "EzImageLoaderDDS.h"
#include "EzImage.h"

#define DDS_MAGIC			0x20534444

//  S_SURFACE_FORMAT_HEADER.flags
#define DDSD_CAPS			0x00000001
#define DDSD_HEIGHT			0x00000002
#define DDSD_WIDTH			0x00000004
#define DDSD_PITCH			0x00000008
#define DDSD_PIXELFORMAT	0x00001000
#define DDSD_MIPMAPCOUNT	0x00020000
#define DDSD_LINEARSIZE		0x00080000
#define DDSD_DEPTH			0x00800000

//  S_SURFACE_FORMAT_HEADER.S_PIXEL_FORMAT.flags
#define DDPF_ALPHAPIXELS	0x00000001
#define DDPF_ALPHA			0x00000002
#define DDPF_FOURCC			0x00000004
#define DDPF_RGB			0x00000040
#define DDPF_COMPRESSED		0x00000080
#define DDPF_LUMINANCE		0x00020000

//  S_SURFACE_FORMAT_HEADER.caps.caps1
#define DDSCAPS1_COMPLEX	0x00000008
#define DDSCAPS1_TEXTURE	0x00001000
#define DDSCAPS1_MIPMAP		0x00400000

//  S_SURFACE_FORMAT_HEADER.caps.caps2
#define DDSCAPS2_CUBEMAP            0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000
#define DDSCAPS2_VOLUME             0x00200000

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
	((int)(unsigned char)(ch0) | ((int)(unsigned char)(ch1) << 8) | \
	((int)(unsigned char)(ch2) << 16) | ((int)(unsigned char)(ch3) << 24 ))
#endif

#define D3DFMT_R8G8B8               20
#define D3DFMT_A8R8G8B8             21
#define D3DFMT_X8R8G8B8             22
#define D3DFMT_R5G6B5               23
#define D3DFMT_A1R5G5B5             25
#define D3DFMT_A4R4G4B4             26
#define D3DFMT_A8B8G8R8             32
#define D3DFMT_X8B8G8R8             33
#define D3DFMT_DXT1                 MAKEFOURCC('D', 'X', 'T', '1')
#define D3DFMT_DXT2                 MAKEFOURCC('D', 'X', 'T', '2')
#define D3DFMT_DXT3                 MAKEFOURCC('D', 'X', 'T', '3')
#define D3DFMT_DXT4                 MAKEFOURCC('D', 'X', 'T', '4')
#define D3DFMT_DXT5                 MAKEFOURCC('D', 'X', 'T', '5')

//! constructor
EzImageLoaderDDS::EzImageLoaderDDS()
{
}

//! destructor
EzImageLoaderDDS::~EzImageLoaderDDS()
{
}

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool EzImageLoaderDDS::isALoadableFileExtension(const char* fileName) const
{
	return strstr(fileName, ".dds") != 0;
}

//! returns true if the file maybe is able to be loaded by this class
bool EzImageLoaderDDS::isALoadableFileFormat(EzFile* file) const
{
	char magicWord[4];
	file->read( &magicWord, 4 );
	return (magicWord[0] == 'D' && magicWord[1] == 'D' && magicWord[2] == 'S' );
}

//! creates a surface from the file
EzImage* EzImageLoaderDDS::loadImage(EzFile* file) const
{
	EzImage* image = NULL;
	S_SURFACE_FORMAT_HEADER	header;

	file->seek( 4 );
	file->read( &header, sizeof( S_SURFACE_FORMAT_HEADER ) );

	if( header.size == 124 && ( header.flags & DDSD_PIXELFORMAT ) && ( header.flags & DDSD_CAPS ) )
	{
		// determine if texture is 3d( has depth )
		bool is3D = header.depth > 0 && ( header.flags & DDSD_DEPTH );

		if( !is3D )
			header.depth = 1;

		// determine if texture has alpha
		bool usingAlpha = header.pixelFormat.flags & DDPF_ALPHAPIXELS;

		// color format to create image
		ECOLOR_FORMAT format = ECF_UNKNOWN;

		unsigned int dataSize = 0;

		if( header.pixelFormat.flags & DDPF_RGB )
		{
			unsigned int byteCount = header.pixelFormat.RGBBitCount / 8;
		
			if( header.flags & DDSD_PITCH )
				dataSize = header.pitch * header.height * header.depth * ( header.pixelFormat.RGBBitCount / 8 );
			else
				dataSize = header.width * header.height * header.depth * ( header.pixelFormat.RGBBitCount / 8 );

			unsigned char* data = new unsigned char[dataSize];
			file->read( data, dataSize );
			
			// has an alpha mask
			switch( header.pixelFormat.RGBBitCount )
			{
				case 16:
				{
					if( usingAlpha )
					{
						if( header.pixelFormat.RGBAlphaMask == 0x8000 )
						{
							format = ECF_A1R5G5B5;
						//	os::Printer::print( "DDS : ECF_A1R5G5B5 format" );
						}
						else if( header.pixelFormat.RGBAlphaMask == 0xf000 )
						{
							format = ECF_A4R4G4B4;
						//	os::Printer::print( "DDS : ECF_A4R4G4B4 format" );
						}
						else
						{
						//	os::Printer::print( "DDS : Unsupported 16 bit format with alpha" );
						}
					}
					else
					{
						if( header.pixelFormat.redBitMask == 0xf800 )
						{
							format = ECF_R5G6B5;
						//	os::Printer::print( "DDS : ECF_R5G6B5 format" );
						}
					}
					break;
				}
				case 24:
				{
					if( usingAlpha )
					{
					//	os::Printer::print( "DDS : Unsupported 24 bit format with alpha" );
					}
					else
					{
						if( header.pixelFormat.redBitMask & 0xff0000 )
						{
							format = ECF_R8G8B8;
						//	os::Printer::print( "DDS : ECF_R8G8B8 format" );
						}
					}
					break;
				}
				case 32:
				{
					if( usingAlpha )
					{
						if( header.pixelFormat.redBitMask & 0xff0000 )
						{
							format = ECF_A8R8G8B8;
						//	os::Printer::print( "DDS : ECF_A8R8G8B8 format" );
						}
						else if( header.pixelFormat.redBitMask & 0xff )
						{
							format = ECF_A8R8G8B8;
						//	os::Printer::print( "DDS : ECF_A8R8G8B8 format" );

							// convert data from A8B8G8R8 to A8R8G8B8
							unsigned char tmp = 0;
							for( unsigned int x=0; x<dataSize; x+=4 )
							{
								tmp = data[x];
								data[x] = data[x+2];
								data[x+2] = tmp;
							}
						}
						else
						{
						//	os::Printer::print( "DDS : Unsupported 32 bit alpha format" );
						}
					}
					else
					{
						if( header.pixelFormat.redBitMask & 0xff0000 )
						{
							format = ECF_X8R8G8B8;
						//	os::Printer::print( "DDS : ECF_X8R8G8B8 format" );
						}
						else if( header.pixelFormat.redBitMask & 0xff )
						{
							format = ECF_X8R8G8B8;
						//	os::Printer::print( "DDS : ECF_X8R8G8B8 format" );

							// convert data from X8B8G8R8 to X8R8G8B8
							unsigned char tmp = 0;
							for( unsigned int x=0; x<dataSize; x+=4 )
							{
								data[x+3] = 255;
								tmp = data[x];
								data[x] = data[x+2];
								data[x+2] = tmp;
							}
						}
					}
					break;
				}
			}

			if( format != ECF_UNKNOWN )
			{
				if( is3D )
					image = EzNew EzImage( format, EzDimension2d( header.width, header.height ), header.depth , data );
				else
					image = EzNew EzImage( format, EzDimension2d( header.width, header.height ), data );
			}
		}
		else if( header.pixelFormat.flags & DDPF_FOURCC )
		{
			switch( header.pixelFormat.fourCC )
			{
				case D3DFMT_DXT1:
				{
					dataSize = ( header.width / 4 ) * ( header.height / 4 ) * 8;
					format = ECF_DXT1;
				//	os::Printer::print( "DDS : ECF_DXT1 format" );
					break;
				}
				case D3DFMT_DXT2:
				case D3DFMT_DXT3:
				{
					dataSize = ( header.width / 4 ) * ( header.height / 4 ) * 16;
					format = ECF_DXT3;
				//	os::Printer::print( "DDS : ECF_DXT3 format" );
					break;
				}
				case D3DFMT_DXT4:
				case D3DFMT_DXT5:
				{
					dataSize = ( header.width / 4 ) * ( header.height / 4 ) * 16;
					format = ECF_DXT5;
				//	os::Printer::print( "DDS : ECF_DXT5 format" );
					break;
				}
			}

			if( format != ECF_UNKNOWN )
			{
				unsigned char* data = new unsigned char[ dataSize ];
				file->read( data, dataSize );

				if( is3D )
					image = EzNew EzImage( format, EzDimension2d( header.width, header.height), header.depth, data );
				else
					image = EzNew EzImage( format, EzDimension2d( header.width, header.height ), data );
			}
		}
		else if( header.pixelFormat.flags & DDPF_LUMINANCE ) // 8-bit luminance [ D3DFMT_L8 ]
		{
			format = ECF_L8;
			dataSize = header.width * header.height * header.depth;

			if( format != ECF_UNKNOWN )
			{
				unsigned char* data = new unsigned char[ dataSize ];
				file->read( data, dataSize );

				if( is3D )
					image = EzNew EzImage( format, EzDimension2d( header.width, header.height), header.depth, data );
				else
					image = EzNew EzImage( format, EzDimension2d( header.width, header.height ), data );
			}
		}
		else
		{
			//os::Printer::print( "UNKNOWN DDS FORMAT TEXTURE" );
		}
	}

	return image;
}

//! creates a loader which is able to load DDS images
EzImageLoader* createImageLoaderDDS()
{
	return EzNew EzImageLoaderDDS();
}



