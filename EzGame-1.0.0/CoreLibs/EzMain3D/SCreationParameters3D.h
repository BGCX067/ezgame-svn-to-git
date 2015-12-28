// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
//
// This file was modiffied by Neri Park
// - rename 
//
//



#ifndef __I_Neri3D_CREATION_PARAMETERS_H_INCLUDED__
#define __I_Neri3D_CREATION_PARAMETERS_H_INCLUDED__


#include "../CompileConfig/EzConfig.h"
#include "../core/EzCore.h"
#include "../core/EzEnumTypes.h"
#include "../core/SEvent.h"
#include "../core/EzApplication.h"


namespace GameEasy
{

	struct SCreationParameters3D
	{
		SCreationParameters3D() :
			DriverType(EDT_DIRECTX9),
			WindowSize(EzDimension2d(800, 600)),
			ucBits(16),
			ucZBufferBits(16),
			bFullscreen(false),
			bStencilbuffer(false),
			bVsync(false),
			bAntiAlias(false),
			bWithAlphaChannel(false),
			bHighPrecisionFPU(false),
			WindowID(0)
		{
		}

		SCreationParameters3D(const SCreationParameters3D& other) 
		{
			*this = other;
		}

		SCreationParameters3D& operator=(const SCreationParameters3D& other)
		{
			DriverType = other.DriverType;
			WindowSize = other.WindowSize;
			ucBits = other.ucBits;
			ucZBufferBits = other.ucZBufferBits;
			bFullscreen = other.bFullscreen;
			bStencilbuffer = other.bStencilbuffer;
			bVsync = other.bVsync;
			bAntiAlias = other.bAntiAlias;
			bWithAlphaChannel = other.bWithAlphaChannel;
			bHighPrecisionFPU = other.bHighPrecisionFPU;
			WindowID = other.WindowID;
			return *this;
		}


		eDRIVER_TYPE DriverType;

		EzDimension2d<int> WindowSize;

		unsigned char ucBits;

		unsigned char ucZBufferBits;

		bool bFullscreen;
		bool bStencilbuffer;
		bool bVsync;
		bool bAntiAlias;
		bool bWithAlphaChannel;
		bool bHighPrecisionFPU;
		
		void* WindowID;
	};


} // end namespace GameEasy


#endif