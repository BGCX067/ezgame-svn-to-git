#include "../EzRenderer/D3D9Renderer/EzD3DX9Driver.h"
#include "../EzRenderer/EzTextureSource.h"
#include "EzDevice3DWin32.h"
#include "../EzRenderer/EzRender.h"
#include "../EzApplication/EzLogReceiver.h"


EzImplementRTTI(EzDevice3DWin32, EzDeviceWin32);

EzDevice3DWin32::EzDevice3DWin32(const SCreationParameters3D& parameters)
: EzDeviceWin32(parameters), m_pVideoDriver(0), m_pRender(0)
{
}

EzDevice3DWin32::~EzDevice3DWin32(void)
{
	delete m_pRender;
}

void EzDevice3DWin32::createDriver(HWND hWnd, const SCreationParameters3D& parameters)
{
	switch(parameters.DriverType)
	{
	case EDT_DIRECTX9:
		m_pVideoDriver = createDirectX9Driver(parameters.WindowSize, hWnd, parameters.ucBits, parameters.bFullscreen, parameters.bStencilbuffer, 
			false, parameters.bHighPrecisionFPU, parameters.bVsync, 
			parameters.eMultiSampleType, parameters.bMuiltView);

		if (!m_pVideoDriver)
		{
			log(0, "Could not create DIRECT3D9 Driver.");
			return;
		}

		m_pRender = EzNew EzRender(m_pVideoDriver);

		break;

	default:
		log(0, "Unable to create video driver of unknown type.");
		return;
		break;
	}


	EzTextureSource::get().setVideoDriver(m_pVideoDriver);
}


EZ_API EzDevice* EZCALLCONV create3DDeviceEx(const SCreationParameters3D& parameters)
{
	EzDevice3DWin32* pDevice3D = EzNew EzDevice3DWin32(parameters);

	pDevice3D->createDriver(pDevice3D->getWinHandle(), parameters);

	return pDevice3D;
}

