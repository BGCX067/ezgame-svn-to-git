// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "EzImageLoaderTGA.h"
#include "EzColorConverter.h"
#include "EzImage.h"


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool EzImageLoaderTGA::isALoadableFileExtension(const char* fileName) const
{
	return (strstr(fileName, ".tga") != 0) || (strstr(fileName, ".TGA") != 0);
}


//! loads a compressed tga.
unsigned char *EzImageLoaderTGA::loadCompressedImage(EzFile* file, const STGAHeader& header) const
{
	// This was written and sent in by Jon Pry, thank you very much!
	// I only changed the formatting a little bit.

	int bytesPerPixel = header.PixelDepth/8;
	int imageSize =  header.ImageHeight * header.ImageWidth * bytesPerPixel;
	unsigned char* data = new unsigned char[imageSize];
	int currentByte = 0;

	while(currentByte < imageSize)
	{
		unsigned char chunkheader = 0;
		file->read(&chunkheader, sizeof(unsigned char)); // Read The Chunk's Header

		if(chunkheader < 128) // If The Chunk Is A 'RAW' Chunk
		{
			chunkheader++; // Add 1 To The Value To Get Total Number Of Raw Pixels

			file->read(&data[currentByte], bytesPerPixel * chunkheader);
			currentByte += bytesPerPixel * chunkheader;
		}
		else
		{
			// thnx to neojzs for some fixes with this code

			// If It's An RLE Header
			chunkheader -= 127; // Subtract 127 To Get Rid Of The ID Bit

			int dataOffset = currentByte;
			file->read(&data[dataOffset], bytesPerPixel);

			currentByte += bytesPerPixel;

			for(int counter = 1; counter < chunkheader; counter++)
			{
				for(int elementCounter=0; elementCounter < bytesPerPixel; elementCounter++)
					data[currentByte + elementCounter] = data[dataOffset + elementCounter];

				currentByte += bytesPerPixel;
			}
		}
	}

	return data;
}



//! returns true if the file maybe is able to be loaded by this class
bool EzImageLoaderTGA::isALoadableFileFormat(EzFile* file) const
{
	if (!file)
		return false;

	STGAFooter footer;
	memset(&footer, 0, sizeof(STGAFooter));
	file->seek(file->getSize()-sizeof(STGAFooter));
	file->read(&footer, sizeof(STGAFooter));
	return (!strcmp(footer.Signature,"TRUEVISION-XFILE.")); // very old tgas are refused.
}



//! creates a surface from the file
EzImage* EzImageLoaderTGA::loadImage(EzFile* file) const
{
	STGAHeader header;
	unsigned char* colorMap = 0;

	file->read(&header, sizeof(STGAHeader));

#ifdef __BIG_ENDIAN__
	header.ColorMapLength = os::Byteswap::byteswap(header.ColorMapLength);
	header.ImageWidth = os::Byteswap::byteswap(header.ImageWidth);
	header.ImageHeight = os::Byteswap::byteswap(header.ImageHeight);
#endif

	// skip image identification field
	if (header.IdLength)
		file->seek(header.IdLength, true);

	if (header.ColorMapType)
	{
		// read color map
		colorMap = new unsigned char[header.ColorMapEntrySize/8 * header.ColorMapLength];
		file->read(colorMap,header.ColorMapEntrySize/8 * header.ColorMapLength);
	}

	// read image

	unsigned char* data = 0;

	if (header.ImageType == 2)
	{
		const int imageSize = header.ImageHeight * header.ImageWidth * header.PixelDepth/8;
		data = new unsigned char[imageSize];
	  	file->read(data, imageSize);
	}
	else
	if(header.ImageType == 10)
		data = loadCompressedImage(file, header);
	else
	{
		//os::Printer::log("Unsupported TGA file type", file->getFileName(), ELL_ERROR);
		if (colorMap)
			delete [] colorMap;
		return 0;
	}

	EzImage* image = 0;

	switch(header.PixelDepth)
	{
	case 16:
		{
			image = EzNew EzImage(ECF_A1R5G5B5,
				EzDimension2d(header.ImageWidth, header.ImageHeight));
			if (image)
				EzColorConverter::convert16BitTo16Bit((short*)data,
					(short*)image->lock(), header.ImageWidth,	header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0);
		}
		break;
	case 24:
		{
			image = EzNew EzImage(ECF_R8G8B8,
				EzDimension2d(header.ImageWidth, header.ImageHeight));
			if (image)
				EzColorConverter::convert24BitTo24Bit(
					(unsigned char*)data, (unsigned char*)image->lock(), header.ImageWidth, header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0, true);
		}
		break;
	case 32:
		{
			image = EzNew EzImage(ECF_A8R8G8B8,
				EzDimension2d(header.ImageWidth, header.ImageHeight));
			if (image)
				EzColorConverter::convert32BitTo32Bit((int*)data,
					(int*)image->lock(), header.ImageWidth, header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0);
		}
		break;
	default:
		//os::Printer::log("Unsupported TGA format", file->getFileName(), ELL_ERROR);
		break;
	}
	if (image)
		image->unlock();
	delete [] data;
	if (colorMap)
		delete [] colorMap;

	return image;
}



//! creates a loader which is able to load tgas
EzImageLoader* createImageLoaderTGA()
{
	return EzNew EzImageLoaderTGA();
}


