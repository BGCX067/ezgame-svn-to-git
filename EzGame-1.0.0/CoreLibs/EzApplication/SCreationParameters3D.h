// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
//
// This file was modiffied by Neri Park
// - rename 
//
//
#ifndef __I_SCREATION_PARAMETERS_H_INCLUDED__
#define __I_SCREATION_PARAMETERS_H_INCLUDED__


#include "../CompileConfig/EzConfig.h"
#include "../EzCore/EzCore.h"
#include "../EzCore/EzEnumTypes.h"
#include "SEvent.h"


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
		eMultiSampleType(EMULTISAMPLE_NONE),
		bWithAlphaChannel(false),
		bHighPrecisionFPU(false),
		WindowID(0),
		bMuiltView(false)
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
		eMultiSampleType = other.eMultiSampleType;
		bWithAlphaChannel = other.bWithAlphaChannel;
		bHighPrecisionFPU = other.bHighPrecisionFPU;
		bMuiltView = other.bMuiltView;
		WindowID = other.WindowID;
		return *this;
	}


	EDRIVER_TYPE DriverType;

	EzDimension2d  WindowSize;

	u8 ucBits;

	u8 ucZBufferBits;

	bool bFullscreen;
	bool bStencilbuffer;
	bool bVsync;
	EMULTISAMPLE_TYPE eMultiSampleType;
	bool bWithAlphaChannel;
	bool bHighPrecisionFPU;
	bool bMuiltView;

	void* WindowID;
};


#endif