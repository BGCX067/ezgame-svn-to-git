// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "EzColorConverter.h"

//! converts a monochrome bitmap to A1R5G5B5 data
void EzColorConverter::convert1BitTo16Bit(const unsigned char* in, short* out, int width, int height, int linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (int y=0; y<height; ++y)
	{
		int shift = 7;
		if (flip)
			out -= width;

		for (int x=0; x<width; ++x)
		{
			out[x] = *in>>shift & 0x01 ? (short)0xffff : (short)0x8000;

			if ((--shift)<0) // 8 pixel done
			{
				shift=7;
				++in;
			}
		}

		if (shift != 7) // width did not fill last byte
			++in;

		if (!flip)
			out += width;
		in += linepad;
	}
}



//! converts a 4 bit palettized image to A1R5G5B5
void EzColorConverter::convert4BitTo16Bit(const unsigned char* in, short* out, int width, int height, const int* palette, int linepad, bool flip)
{
	if (!in || !out || !palette)
		return;

	if (flip)
		out += width*height;

	for (int y=0; y<height; ++y)
	{
		int shift = 4;
		if (flip)
			out -= width;

		for (int x=0; x<width; ++x)
		{
			out[x] = X8R8G8B8toA1R5G5B5(palette[(unsigned char)((*in >> shift) & 0xf)]);

			if (shift==0)
			{
				shift = 4;
				++in;
			}
			else
				shift = 0;
		}

		if (shift == 0) // odd width
			++in;

		if (!flip)
			out += width;
		in += linepad;
	}
}



//! converts a 8 bit palettized image into A1R5G5B5
void EzColorConverter::convert8BitTo16Bit(const unsigned char* in, short* out, int width, int height, const int* palette, int linepad, bool flip)
{
	if (!in || !out || !palette)
		return;

	if (flip)
		out += width * height;

	for (int y=0; y<height; ++y)
	{
		if (flip)
			out -= width; // one line back
		for (int x=0; x<width; ++x)
		{
			out[x] = X8R8G8B8toA1R5G5B5(palette[(unsigned char)(*in)]);
			++in;
		}
		if (!flip)
			out += width;
		in += linepad;
	}
}



//! converts 16bit data to 16bit data
void EzColorConverter::convert16BitTo16Bit(const short* in, short* out, int width, int height, int linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (int y=0; y<height; ++y)
	{
		if (flip)
			out -= width;
#ifdef __BIG_ENDIAN__
		for (int x=0; x<width; ++x)
			out[x]=os::Byteswap::byteswap(in[x]);
#else
		memcpy(out, in, width*sizeof(short));
#endif
		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}



//! copies R8G8B8 24bit data to 24bit data
void EzColorConverter::convert24BitTo24Bit(const unsigned char* in, unsigned char* out, int width, int height, int linepad, bool flip, bool bgr)
{
	if (!in || !out)
		return;

	const int lineWidth = 3 * width;
	if (flip)
		out += lineWidth * height;

	for (int y=0; y<height; ++y)
	{
		if (flip)
			out -= lineWidth;
		if (bgr)
		{
			for (int x=0; x<lineWidth; x+=3)
			{
				out[x+0] = in[x+2];
				out[x+1] = in[x+1];
				out[x+2] = in[x+0];
			}
		}
		else
		{
			memcpy(out,in,lineWidth);
		}
		if (!flip)
			out += lineWidth;
		in += lineWidth;
		in += linepad;
	}
}



//! Resizes the surface to a new size and converts it at the same time
//! to an A8R8G8B8 format, returning the pointer to the new buffer.
void EzColorConverter::convert16bitToA8R8G8B8andResize(const short* in, int* out, int newWidth, int newHeight, int currentWidth, int currentHeight)
{
	if (!newWidth || !newHeight)
		return;

	// note: this is very very slow. (i didn't want to write a fast version.
	// but hopefully, nobody wants to convert surfaces every frame.	

	float sourceXStep = (float)currentWidth / (float)newWidth;
	float sourceYStep = (float)currentHeight / (float)newHeight;
	float sy;
	int t;

	for (int x=0; x<newWidth; ++x)
	{
		sy = 0.0f;

		for (int y=0; y<newHeight; ++y)
		{
			t = in[(int)(((int)sy)*currentWidth + x*sourceXStep)];
			t = (((t >> 15)&0x1)<<31) |	(((t >> 10)&0x1F)<<19) |
				(((t >> 5)&0x1F)<<11) |	(t&0x1F)<<3;
			out[(int)(y*newWidth + x)] = t;

			sy+=sourceYStep;
		}
	}
}



//! copies X8R8G8B8 32 bit data
void EzColorConverter::convert32BitTo32Bit(const int* in, int* out, int width, int height, int linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (int y=0; y<height; ++y)
	{
		if (flip)
			out -= width;
#ifdef __BIG_ENDIAN__
		for (int x=0; x<width; ++x)
			out[x]=os::Byteswap::byteswap(in[x]);
#else
		memcpy(out, in, width*sizeof(int));
#endif
		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}



void EzColorConverter::convert_A1R5G5B5toR8G8B8(const void* sP, int sN, void* dP)
{
	unsigned short* sB = (unsigned short*)sP;
	unsigned char * dB = (unsigned char *)dP;

	for (int x = 0; x < sN; ++x)
	{
		dB[2] = (*sB & 0x7c00) >> 7;
		dB[1] = (*sB & 0x03e0) >> 2;
		dB[0] = (*sB & 0x1f) << 3;

		sB += 1;
		dB += 3;
	}
}

void EzColorConverter::convert_A1R5G5B5toB8G8R8(const void* sP, int sN, void* dP)
{
	unsigned short* sB = (unsigned short*)sP;
	unsigned char * dB = (unsigned char *)dP;

	for (int x = 0; x < sN; ++x)
	{
		dB[0] = (*sB & 0x7c00) >> 7;
		dB[1] = (*sB & 0x03e0) >> 2;
		dB[2] = (*sB & 0x1f) << 3;

		sB += 1;
		dB += 3;
	}
}

void EzColorConverter::convert_A1R5G5B5toA8R8G8B8(const void* sP, int sN, void* dP)
{
	unsigned short* sB = (unsigned short*)sP;
	unsigned int* dB = (unsigned int*)dP;

	for (int x = 0; x < sN; ++x)
		*dB++ = A1R5G5B5toA8R8G8B8(*sB++);
}

void EzColorConverter::convert_A1R5G5B5toA1R5G5B5(const void* sP, int sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

void EzColorConverter::convert_A1R5G5B5toR5G6B5(const void* sP, int sN, void* dP)
{
	unsigned short* sB = (unsigned short*)sP;
	unsigned short* dB = (unsigned short*)dP;

	for (int x = 0; x < sN; ++x)
		*dB++ = A1R5G5B5toR5G6B5(*sB++);
}

void EzColorConverter::convert_A8R8G8B8toR8G8B8(const void* sP, int sN, void* dP)
{
	unsigned char* sB = (unsigned char*)sP;
	unsigned char* dB = (unsigned char*)dP;

	for (int x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];

		sB += 4;
		dB += 3;
	}
}

void EzColorConverter::convert_A8R8G8B8toB8G8R8(const void* sP, int sN, void* dP)
{
	unsigned char* sB = (unsigned char*)sP;
	unsigned char* dB = (unsigned char*)dP;

	for (int x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[0];
		dB[1] = sB[1];
		dB[2] = sB[2];

		sB += 4;
		dB += 3;
	}
}

void EzColorConverter::convert_A8R8G8B8toA8R8G8B8(const void* sP, int sN, void* dP)
{
	memcpy(dP, sP, sN * 4);
}

void EzColorConverter::convert_A8R8G8B8toA1R5G5B5(const void* sP, int sN, void* dP)
{
	unsigned int* sB = (unsigned int*)sP;
	unsigned short* dB = (unsigned short*)dP;

	for (int x = 0; x < sN; ++x)
		*dB++ = A8R8G8B8toA1R5G5B5(*sB++);
}

void EzColorConverter::convert_A8R8G8B8toR5G6B5(const void* sP, int sN, void* dP)
{
	unsigned char * sB = (unsigned char *)sP;
	unsigned short* dB = (unsigned short*)dP;

	for (int x = 0; x < sN; ++x)
	{
		int r = sB[2] >> 3;
		int g = sB[1] >> 2;
		int b = sB[0] >> 3;

		dB[0] = (r << 11) | (g << 5) | (b);

		sB += 4;
		dB += 1;
	}
}

void EzColorConverter::convert_A8R8G8B8toR3G3B2(const void* sP, int sN, void* dP)
{
	unsigned char* sB = (unsigned char*)sP;
	unsigned char* dB = (unsigned char*)dP;

	for (int x = 0; x < sN; ++x)
	{
		unsigned char r = sB[2] & 0xe0;
		unsigned char g = (sB[1] & 0xe0) >> 3;
		unsigned char b = (sB[0] & 0xc0) >> 6;

		dB[0] = (r | g | b);

		sB += 4;
		dB += 1;
	}
}

void EzColorConverter::convert_R8G8B8toR8G8B8(const void* sP, int sN, void* dP)
{
	memcpy(dP, sP, sN * 3);
}

void EzColorConverter::convert_R8G8B8toA8R8G8B8(const void* sP, int sN, void* dP)
{
	unsigned char*  sB = (unsigned char* )sP;
	unsigned int* dB = (unsigned int*)dP;

	for (int x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[0]<<16) | (sB[1]<<8) | sB[2];

		sB += 3;
		++dB;
	}
}

void EzColorConverter::convert_R8G8B8toA1R5G5B5(const void* sP, int sN, void* dP)
{
	unsigned char * sB = (unsigned char *)sP;
	unsigned short* dB = (unsigned short*)dP;

	for (int x = 0; x < sN; ++x)
	{
		int r = sB[0] >> 3;
		int g = sB[1] >> 3;
		int b = sB[2] >> 3;

		dB[0] = (0x8000) | (r << 10) | (g << 5) | (b);

		sB += 3;
		dB += 1;
	}
}

void EzColorConverter::convert_R8G8B8toR5G6B5(const void* sP, int sN, void* dP)
{
	unsigned char * sB = (unsigned char *)sP;
	unsigned short* dB = (unsigned short*)dP;

	for (int x = 0; x < sN; ++x)
	{
		int r = sB[0] >> 3;
		int g = sB[1] >> 2;
		int b = sB[2] >> 3;

		dB[0] = (r << 11) | (g << 5) | (b);

		sB += 3;
		dB += 1;
	}
}

void EzColorConverter::convert_R5G6B5toR5G6B5(const void* sP, int sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

void EzColorConverter::convert_R5G6B5toR8G8B8(const void* sP, int sN, void* dP)
{
	unsigned short* sB = (unsigned short*)sP;
	unsigned char * dB = (unsigned char *)dP;

	for (int x = 0; x < sN; ++x)
	{
		dB[0] = (*sB & 0xf800) << 8;
		dB[1] = (*sB & 0x07e0) << 2;
		dB[2] = (*sB & 0x001f) << 3;

		sB += 4;
		dB += 3;
	}
}

void EzColorConverter::convert_R5G6B5toB8G8R8(const void* sP, int sN, void* dP)
{
	unsigned short* sB = (unsigned short*)sP;
	unsigned char * dB = (unsigned char *)dP;

	for (int x = 0; x < sN; ++x)
	{
		dB[2] = (*sB & 0xf800) << 8;
		dB[1] = (*sB & 0x07e0) << 2;
		dB[0] = (*sB & 0x001f) << 3;

		sB += 4;
		dB += 3;
	}
}

void EzColorConverter::convert_R5G6B5toA8R8G8B8(const void* sP, int sN, void* dP)
{
	unsigned short* sB = (unsigned short*)sP;
	unsigned int* dB = (unsigned int*)dP;

	for (int x = 0; x < sN; ++x)
		*dB++ = R5G6B5toA8R8G8B8(*sB++);
}

void EzColorConverter::convert_R5G6B5toA1R5G5B5(const void* sP, int sN, void* dP)
{
	unsigned short* sB = (unsigned short*)sP;
	unsigned short* dB = (unsigned short*)dP;

	for (int x = 0; x < sN; ++x)
		*dB++ = R5G6B5toA1R5G5B5(*sB++);
}


void EzColorConverter::convert_viaFormat(const void* sP, ECOLOR_FORMAT sF, int sN,
				void* dP, ECOLOR_FORMAT dF)
{
	switch (sF)
	{
		case ECF_A1R5G5B5:
			switch (dF)
			{
				case ECF_A1R5G5B5:
					convert_A1R5G5B5toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_A1R5G5B5toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_A1R5G5B5toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_A1R5G5B5toR8G8B8(sP, sN, dP);
				break;
			}
		break;
		case ECF_R5G6B5:
			switch (dF)
			{
				case ECF_A1R5G5B5:
					convert_R5G6B5toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_R5G6B5toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_R5G6B5toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_R5G6B5toR8G8B8(sP, sN, dP);
				break;
			}
		break;
		case ECF_A8R8G8B8:
			switch (dF)
			{
				case ECF_A1R5G5B5:
					convert_A8R8G8B8toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_A8R8G8B8toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_A8R8G8B8toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_A8R8G8B8toR8G8B8(sP, sN, dP);
				break;
			}
		break;
		case ECF_R8G8B8:
			switch (dF)
			{
				case ECF_A1R5G5B5:
					convert_R8G8B8toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_R8G8B8toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_R8G8B8toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_R8G8B8toR8G8B8(sP, sN, dP);
				break;
			}
		break;
	}
}



