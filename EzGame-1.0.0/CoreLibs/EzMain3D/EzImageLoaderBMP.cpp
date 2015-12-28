// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "EzImageLoaderBMP.h"
#include "EzImage.h"
#include "EzColorConverter.h"


//! constructor
EzImageLoaderBMP::EzImageLoaderBMP()
{
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool EzImageLoaderBMP::isALoadableFileExtension(const char* fileName) const
{
	return strstr(fileName, ".bmp") != 0;
}


//! returns true if the file maybe is able to be loaded by this class
bool EzImageLoaderBMP::isALoadableFileFormat(EzFile* file) const
{
	unsigned short headerID;
	file->read(&headerID, sizeof(unsigned short));

#ifdef __BIG_ENDIAN__
	headerID = os::Byteswap::byteswap(headerID);
#endif

	return headerID == 0x4d42;
}


void EzImageLoaderBMP::decompress8BitRLE(unsigned char*& bmpData, int size, int width, int height, int pitch) const
{
	unsigned char* p = bmpData;
	unsigned char* newBmp = new unsigned char[(width+pitch)*height];
	unsigned char* d = newBmp;
	unsigned char* destEnd = newBmp + (width+pitch)*height;
	int line = 0;

	while (bmpData - p < size && d < destEnd)
	{
		if (*p == 0)
		{
			++p;

			switch(*p)
			{
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line*(width+pitch));
				break;
			case 1: // end of bmp
				delete [] bmpData;
				bmpData = newBmp;
				return;
			case 2:
				++p; d +=(unsigned char)*p;  // delta
				++p; d += ((unsigned char)*p)*(width+pitch);
				++p;
				break;
			default:
				{
					// absolute mode
					int count = (unsigned char)*p; ++p;
					int readAdditional = ((2-(count%2))%2);
					int i;

					for (i=0; i<count; ++i)
					{
						*d = *p;
						++p;
						++d;
					}

					for (i=0; i<readAdditional; ++i)
						++p;
				}
			}
		}
		else
		{
			int count = (unsigned char)*p; ++p;
			unsigned char color = *p; ++p;
			for (int i=0; i<count; ++i)
			{
				*d = color;
				++d;
			}
		}
	}

	delete [] bmpData;
	bmpData = newBmp;
}


void EzImageLoaderBMP::decompress4BitRLE(unsigned char*& bmpData, int size, int width, int height, int pitch) const
{
	int lineWidth = (width+1)/2+pitch;
	unsigned char* p = bmpData;
	unsigned char* newBmp = new unsigned char[lineWidth*height];
	unsigned char* d = newBmp;
	unsigned char* destEnd = newBmp + lineWidth*height;
	int line = 0;
	int shift = 4;

	while (bmpData - p < size && d < destEnd)
	{
		if (*p == 0)
		{
			++p;

			switch(*p)
			{
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line*lineWidth);
				shift = 4;
				break;
			case 1: // end of bmp
				delete [] bmpData;
				bmpData = newBmp;
				return;
			case 2:
				{
					++p;
					int x = (unsigned char)*p; ++p;
					int y = (unsigned char)*p; ++p;
					d += x/2 + y*lineWidth;
					shift = x%2==0 ? 4 : 0;
				}
				break;
			default:
				{
					// absolute mode
					int count = (unsigned char)*p; ++p;
					int readAdditional = ((2-((count)%2))%2);
					int readShift = 4;
					int i;

					for (i=0; i<count; ++i)
					{
						int color = (((unsigned char)*p) >> readShift) & 0x0f;
						readShift -= 4;
						if (readShift < 0)
						{
							++*p;
							readShift = 4;
						}

						unsigned char mask = 0x0f << shift;
						*d = (*d & (~mask)) | ((color << shift) & mask);

						shift -= 4;
						if (shift < 0)
						{
							shift = 4;
							++d;
						}

					}

					for (i=0; i<readAdditional; ++i)
						++p;
				}
			}
		}
		else
		{
			int count = (unsigned char)*p; ++p;
			int color1 = (unsigned char)*p; color1 = color1 & 0x0f;
			int color2 = (unsigned char)*p; color2 = (color2 >> 4) & 0x0f;
			++p;

			for (int i=0; i<count; ++i)
			{
				unsigned char mask = 0x0f << shift;
				unsigned char toSet = (shift==0 ? color1 : color2) << shift;
				*d = (*d & (~mask)) | (toSet & mask);

				shift -= 4;
				if (shift < 0)
				{
					shift = 4;
					++d;
				}
			}
		}
	}

	delete [] bmpData;
	bmpData = newBmp;
}



//! creates a surface from the file
EzImage* EzImageLoaderBMP::loadImage(EzFile* file) const
{
	SBMPHeader header;

	file->read(&header, sizeof(header));

#ifdef __BIG_ENDIAN__
	//header.Id = os::Byteswap::byteswap(header.Id);
	//header.FileSize = os::Byteswap::byteswap(header.FileSize);
	//header.BitmapDataOffset = os::Byteswap::byteswap(header.BitmapDataOffset);
	//header.BitmapHeaderSize = os::Byteswap::byteswap(header.BitmapHeaderSize);
	//header.Width = os::Byteswap::byteswap(header.Width);
	//header.Height = os::Byteswap::byteswap(header.Height);
	//header.Planes = os::Byteswap::byteswap(header.Planes);
	//header.BPP = os::Byteswap::byteswap(header.BPP);
	//header.Compression = os::Byteswap::byteswap(header.Compression);
	//header.BitmapDataSize = os::Byteswap::byteswap(header.BitmapDataSize);
	//header.PixelPerMeterX = os::Byteswap::byteswap(header.PixelPerMeterX);
	//header.PixelPerMeterY = os::Byteswap::byteswap(header.PixelPerMeterY);
	//header.Colors = os::Byteswap::byteswap(header.Colors);
	//header.ImportantColors = os::Byteswap::byteswap(header.ImportantColors);
#endif

	int pitch = 0;

	//! return if the header is false

	if (header.Id != 0x4d42)
		return 0;

	if (header.Compression > 2) // we'll only handle RLE-Compression
	{
		//os::Printer::log("Compression mode not supported.", ELL_ERROR);
		return 0;
	}

	// adjust bitmap data size to dword boundary
	header.BitmapDataSize += (4-(header.BitmapDataSize%4))%4;

	// read palette

	long pos = file->getPos();
	int paletteSize = (header.BitmapDataOffset - pos) / 4;

	int* paletteData = 0;
	if (paletteSize)
	{
		paletteData = new int[paletteSize];
		file->read(paletteData, paletteSize * sizeof(int));
#ifdef __BIG_ENDIAN__
		//for (int i=0; i<paletteSize; ++i)
		//	paletteData[i] = os::Byteswap::byteswap(paletteData[i]);
#endif
	}

	// read image data

	if (!header.BitmapDataSize)
	{
		// okay, lets guess the size
		// some tools simply don't set it
		header.BitmapDataSize = static_cast<unsigned int>(file->getSize()) - header.BitmapDataOffset;
	}

	file->seek(header.BitmapDataOffset);

	float t = (header.Width) * (header.BPP / 8.0f);
	int widthInBytes = (int)t;
	t -= widthInBytes;
	if (t!=0.0f)
		++widthInBytes;

	int lineData = widthInBytes + ((4-(widthInBytes%4)))%4;
	pitch = lineData - widthInBytes;

	unsigned char* bmpData = new unsigned char[header.BitmapDataSize];
	file->read(bmpData, header.BitmapDataSize);

	// decompress data if needed
	switch(header.Compression)
	{
	case 1: // 8 bit rle
		decompress8BitRLE(bmpData, header.BitmapDataSize, header.Width, header.Height, pitch);
		break;
	case 2: // 4 bit rle
		decompress4BitRLE(bmpData, header.BitmapDataSize, header.Width, header.Height, pitch);
		break;
	}

	// create surface

	EzImage* image = 0;
	switch(header.BPP)
	{
	case 1:
		image = EzNew EzImage(ECF_A1R5G5B5, EzDimension2d(header.Width, header.Height));
		if (image)
			EzColorConverter::convert1BitTo16Bit(bmpData, (short*)image->lock(), header.Width, header.Height, pitch, true);
		break;
	case 4:
		image = EzNew EzImage(ECF_A1R5G5B5, EzDimension2d(header.Width, header.Height));
		if (image)
			EzColorConverter::convert4BitTo16Bit(bmpData, (short*)image->lock(), header.Width, header.Height, paletteData, pitch, true);
		break;
	case 8:
		image = EzNew EzImage(ECF_A1R5G5B5, EzDimension2d(header.Width, header.Height));
		if (image)
			EzColorConverter::convert8BitTo16Bit(bmpData, (short*)image->lock(), header.Width, header.Height, paletteData, pitch, true);
		break;
	case 16:
		image = EzNew EzImage(ECF_A1R5G5B5, EzDimension2d(header.Width, header.Height));
		if (image)
			EzColorConverter::convert16BitTo16Bit((short*)bmpData, (short*)image->lock(), header.Width, header.Height, pitch, true);
		break;
	case 24:
		image = EzNew EzImage(ECF_R8G8B8, EzDimension2d(header.Width, header.Height));
		if (image)
			EzColorConverter::convert24BitTo24Bit(bmpData, (unsigned char*)image->lock(), header.Width, header.Height, pitch, true, true);
		break;
	case 32: // thx to Reinhard Ostermeier
		image = EzNew EzImage(ECF_A8R8G8B8, EzDimension2d(header.Width, header.Height));
		if (image)
			EzColorConverter::convert32BitTo32Bit((int*)bmpData, (int*)image->lock(), header.Width, header.Height, pitch, true);
		break;
	};
	if (image)
		image->unlock();

	// clean up

	delete [] paletteData;
	delete [] bmpData;

	return image;
}


//! creates a loader which is able to load windows bitmaps
EzImageLoader* createImageLoaderBMP()
{
	return EzNew EzImageLoaderBMP();
}



