#include "../../EzCore/EzSystem.h"
#include "../../EzCore/EzCoreUtility.h"
#include "../../EzMain3D/EzDirectionalLight.h"
#include "../../EzMain3D/EzRendererData.h"
#include "../../EzMain3D/EzGeometryData.h"
#include "../../EzMain3D/EzSkinInstance.h"
#include "../../EzMain3D/EzTexturingProperty.h"
#include "../../EzMain3D/EzMaterialProperty.h"
#include "../../EzMain3D/EzTriBasedGeomData.h"
#include "../../EzMain3D/EzGeometry.h"
#include "../../EzMain3D/EzShaderFactory.h"

#include "../EzRenderer.h"
#include "../Ez3DBufferManager.h"
#include "../EzVertexBufferBinding.h"
#include "../EzVertexData.h"
#include "../EzIndexData.h"


#include "EzD3DX9Driver.h"
#include "EzD3DX9Texture.h"
#include "EzD3DX9MultiViewRenderer.h"
#include "EzD3DX9IndexBuffer.h"
#include "EzD3DX9VertexBuffer.h"
#include "EzD3DX9VertexDeclaration.h"
#include "EzD3DX9GPUProgramManager.h"
#include "EzD3DX9HardwareSkinning.h"
#include "EzD3DX9Mappings.h"
#include "EzD3DX9DefaultShader.h"

#include "../EzGPUProgram.h"





#if( D3DX_SDK_VERSION > 23 )
	D3DXFilterTextureFunction				pD3DXFilterTextureFunction = NULL;
	D3DXLoadSurfaceFromMemoryFunction		pD3DXLoadSurfaceFromMemoryFunction = NULL;
	D3DXAssembleShaderFunction				pD3DXAssembleShaderFunction = NULL;
	D3DXAssembleShaderFromFileFunction		pD3DXAssembleShaderFromFileFunction = NULL;
	D3DXCompileShaderFunction				pD3DXCompileShaderFunction = NULL;
	D3DXCompileShaderFromFileFunction		pD3DXCompileShaderFromFileFunction = NULL;
	D3DXGetShaderConstantTableFunction		pD3DXGetShaderConstantTableFunction = NULL;
#endif

HRESULT WINAPI stubD3DXFilterTexture( LPDIRECT3DBASETEXTURE9 pBaseTexture, CONST PALETTEENTRY* pPalette, UINT SrcLevel, DWORD Filter )
{
#if( D3DX_SDK_VERSION < 24 ) 
	return D3DXFilterTexture( pBaseTexture, pPalette, SrcLevel, Filter );
#else
	if( pD3DXFilterTextureFunction )
		return (*pD3DXFilterTextureFunction)( pBaseTexture, pPalette, SrcLevel, Filter );
#endif // D3DX_SDK_VERSION < 24

	return 0;
}

HRESULT WINAPI stubD3DXLoadSurfaceFromMemory( LPDIRECT3DSURFACE9 pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect,
	LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey )
{
#if( D3DX_SDK_VERSION < 24 ) 
	return D3DXLoadSurfaceFromMemory( pDestSurface, pDestPalette, pDestRect, pSrcMemory, SrcFormat, SrcPitch, pSrcPalette, pSrcRect, Filter, ColorKey );
#else
	if( pD3DXLoadSurfaceFromMemoryFunction )
		return (*pD3DXLoadSurfaceFromMemoryFunction)( pDestSurface, pDestPalette, pDestRect, pSrcMemory, SrcFormat, SrcPitch, pSrcPalette, pSrcRect, Filter, ColorKey );
#endif // D3DX_SDK_VERSION < 24

	return 0;
}

HRESULT stubD3DXAssembleShader( LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude,
	DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs )
{
#if( D3DX_SDK_VERSION < 24 ) 
	return D3DXAssembleShader( pSrcData, SrcDataLen, pDefines, pInclude, Flags, ppShader, ppErrorMsgs );
#else
	if( pD3DXAssembleShaderFunction )
		return (*pD3DXAssembleShaderFunction)(pSrcData, SrcDataLen, pDefines, pInclude, Flags, ppShader, ppErrorMsgs);
#endif // D3DX_SDK_VERSION < 24

	return 0;
}

HRESULT stubD3DXAssembleShaderFromFile( LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE  pInclude, DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs )
{
#if( D3DX_SDK_VERSION < 24 ) 
	return D3DXAssembleShaderFromFile( pSrcFile, pDefines, pInclude, Flags, ppShader, ppErrorMsgs );
#else
	if( pD3DXAssembleShaderFromFileFunction )
		return (*pD3DXAssembleShaderFromFileFunction)( pSrcFile, pDefines, pInclude, Flags, ppShader, ppErrorMsgs );
#endif // D3DX_SDK_VERSION < 24

	return 0;
}

HRESULT stubD3DXCompileShader( LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile,
	DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable )
{
#if( D3DX_SDK_VERSION < 24 ) 
	return D3DXCompileShader( pSrcData, SrcDataLen, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable );
#else
	if( pD3DXCompileShaderFunction )
		return (*pD3DXCompileShaderFunction)( pSrcData, SrcDataLen, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable );
#endif // D3DX_SDK_VERSION < 24
	return 0;
}

HRESULT stubD3DXCompileShaderFromFile( LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile,
	DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable )
{
#if( D3DX_SDK_VERSION < 24 ) 
	return D3DXCompileShaderFromFile(pSrcFile, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable);
#else
	if( pD3DXCompileShaderFromFileFunction )
		return (*pD3DXCompileShaderFromFileFunction)( pSrcFile, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable );
#endif // D3DX_SDK_VERSION < 24

	return 0;
}

HRESULT stubD3DXGetShaderConstantTable( const DWORD* pFunction, LPD3DXCONSTANTTABLE* ppConstantTable )
{
#if( D3DX_SDK_VERSION < 24 ) 
	return D3DXGetShaderConstantTable( pFunction, ppConstantTable );
#else
	if( pD3DXGetShaderConstantTableFunction )
		return (*pD3DXGetShaderConstantTableFunction)( pFunction, ppConstantTable );
#endif // D3DX_SDK_VERSION < 24

	return 0;
}


EzImplementRTTI(EzD3DX9Driver, EzVideoDriver);


EzD3DX9Driver::EzD3DX9Driver(void)
: m_pID3D(0), m_pID3DDevice(0), m_eCurrentRenderMode(ERND_NONE), m_uiLastSetLight(0), m_fMaxLightDistance(sqrtf(FLT_MAX)), m_pSceneSourceRect(NULL)
  , m_pMultiViewRenderer(0), m_uiVertexBlendMatricesCount(0), m_pBufferManager(0), m_uiLastVertexSourceCount(0), 
  m_pGpuProgramManager(NULL), m_bTransformation3DChanged(true)
{

	for(unsigned int n = 0; n < NUM_TEXTURE_SUPPORT; n++)
		m_bTextureFormatSupport[n] = false;

	for (unsigned int i=0; i<4; ++i)
	{
		m_pCurrentTexture[i] = 0;
		m_bLastTextureMipMapsAvailable[i] = false;
	}


	// init direct 3d is done in the factory function

#if( D3DX_SDK_VERSION > 23 )
		// try to load dll
		EzStringc strDllName = "d3dx9_";
		strDllName += (int)D3DX_SDK_VERSION;
		strDllName += ".dll";

		HMODULE hMod = LoadLibrary( strDllName.c_str() );
		if( hMod )
		{
			pD3DXFilterTextureFunction = (D3DXFilterTextureFunction)GetProcAddress( hMod, "D3DXFilterTexture" );
			pD3DXLoadSurfaceFromMemoryFunction = (D3DXLoadSurfaceFromMemoryFunction)GetProcAddress( hMod, "D3DXLoadSurfaceFromMemory" );
			pD3DXAssembleShaderFunction = (D3DXAssembleShaderFunction)GetProcAddress( hMod, "D3DXAssembleShader" );
			pD3DXAssembleShaderFromFileFunction = (D3DXAssembleShaderFromFileFunction)GetProcAddress( hMod, "D3DXAssembleShaderFromFile" );
			pD3DXCompileShaderFunction = (D3DXCompileShaderFunction)GetProcAddress( hMod, "D3DXCompileShader" );
			pD3DXCompileShaderFromFileFunction = (D3DXCompileShaderFromFileFunction)GetProcAddress( hMod, "D3DXCompileShaderFromFile" );
			pD3DXGetShaderConstantTableFunction = (D3DXGetShaderConstantTableFunction)GetProcAddress( hMod, "D3DXGetShaderConstantTable" );
		}
#endif

}


EzD3DX9Driver::~EzD3DX9Driver(void)
{
	if(m_pMultiViewRenderer)
		EzDelete m_pMultiViewRenderer;

	if(m_pBufferManager)
		EzDelete m_pBufferManager;

	if(m_pGpuProgramManager)
		EzDelete m_pGpuProgramManager;
}

//! applications must call this method before performing any rendering. returns false if failed.
bool EzD3DX9Driver::beginScene(bool backBuffer, bool zBuffer, const EzColor& color)
{
	EzVideoDriver::beginScene(backBuffer, zBuffer, color);

	HRESULT hr;
	//if (DeviceLost)
	//{
	//	if (FAILED(hr = pID3DDevice->TestCooperativeLevel()))
	//	{
	//		if (hr == D3DERR_DEVICELOST)
	//		{
	//			Sleep(100);
	//			hr = pID3DDevice->TestCooperativeLevel();
	//			if (hr == D3DERR_DEVICELOST)
	//				return false;
	//		}

	//		if ((hr == D3DERR_DEVICENOTRESET) && !reset())
	//			return false;
	//	}
	//}
	
	DWORD dwFlags = 0;

	if (backBuffer)
		dwFlags |= D3DCLEAR_TARGET;

	if (zBuffer)
		dwFlags |= D3DCLEAR_ZBUFFER;

	if (m_bStencilBuffer)
		dwFlags |= D3DCLEAR_STENCIL;

	hr = m_pID3DDevice->Clear( 0, NULL, dwFlags, color.color, 1.0, 0);
	if (FAILED(hr))
	{
		EZLOG("DIRECT3D9 clear failed.");
	}

	hr = m_pID3DDevice->BeginScene();
	if (FAILED(hr))
	{
//		log(0, "DIRECT3D9 begin scene failed.");
		return false;
	}


	return true;
}


bool EzD3DX9Driver::setResize(const EzDimension2d& size, void* WindowId)
{
	//if (!m_pID3DDevice)
	//	return false;

	//if ( size.Width <= 0 || size.Height <= 0 ) 
	//	return false; 

	//D3DPRESENT_PARAMETERS PresentParameters = m_D3DPresent; 

	//PresentParameters.BackBufferWidth = size.Width; 
	//PresentParameters.BackBufferHeight = size.Height; 

	//LPDIRECT3DSWAPCHAIN9* lpSwapChain;
	//LPDIRECT3DSWAPCHAIN9 lpNewSwapChain;

	//lpSwapChain = getSwapChain(WindowId);

	//if(lpSwapChain)
	//{
	//	LPDIRECT3DSURFACE9 lpBackBuffer = NULL;
	//	LRESULT result = m_pID3DDevice->CreateAdditionalSwapChain( &PresentParameters, &lpNewSwapChain ); 
	//	if ( result != D3D_OK ) 
	//		return false;

	//	lpSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &lpBackBuffer);

	//	lpSwapChain->Release(); 
	//	lpBackBuffer->Release();

	//	lpSwapChain = lpNewSwapChain;

	//}

	return true; 
}

// set Lighting
void EzD3DX9Driver::setLighting(bool bLight)
{
	if (bLight)
		m_pID3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	else
		m_pID3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}

// set material 
void EzD3DX9Driver::setShadingMode(ESHADE_MODE eShading, bool bResetAllRenderstates)
{
	static ESHADE_MODE eLastShading;

	if (bResetAllRenderstates || eLastShading != eShading)
	{
		switch(eShading)
		{
		case ESHADE_FLAT:
			m_pID3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
			break;

		case ESHADE_GOURAUD:
			m_pID3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
			break;

		case ESHADE_PHONG:
			m_pID3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_PHONG);
			break;
		}
	}

	eLastShading = eShading;
}

// 
void EzD3DX9Driver::setProperty(EzProperty* pPropert)
{

	switch(pPropert->type())
	{
	case EzProperty::MATERIAL:
		setMaterial(EzStaticCast(EzMaterialProperty, pPropert));
		break;

	//case EzProperty::TEXTURE:
	//	m_pTextureStage->texturePipeline(EzStaticCast(EzTexturingProperty, pPropert));

	//	//setTexture(EzStaticCast(EzTexturingProperty, pPropert));

	//	break;

	}
}

// set material 
void EzD3DX9Driver::setMaterial(EzMaterialProperty* pMat)
{
	if(pMat == NULL)
	{
		D3DMATERIAL9 mat;
		ZeroMemory(&mat, sizeof(D3DMATERIAL9));
		m_pID3DDevice->SetMaterial(&mat);

		m_uiMatLastRevisionID = UINT_MAX;
	}
	else
	{
		if(m_uiMatLastRevisionID != pMat->getRevisonID())
		{
			D3DMATERIAL9 mat;

			mat.Diffuse = EzD3DX9Mappings::get(pMat->getDiffuseColor());
			mat.Ambient = EzD3DX9Mappings::get(pMat->getAmbientColor());
			mat.Specular = EzD3DX9Mappings::get(pMat->getSpecularColor());
			mat.Emissive = EzD3DX9Mappings::get(pMat->getEmissiveColor());
			mat.Power = pMat->getGlossiness();

			m_pID3DDevice->SetMaterial(&mat);

			m_uiMatLastRevisionID = pMat->getRevisonID();

		}
	}
}

//! applications must call this method after performing any rendering. returns false if failed.
bool EzD3DX9Driver::endScene()
{
	EzVideoDriver::endScene();
	
	HRESULT hr = m_pID3DDevice->EndScene();
	if (FAILED(hr))
	{
		EZASSERT(!"DIRECT3D9 end scene failed.");
		return false;
	}

	RECT* pSrcRct = 0;

	if ( m_pSceneSourceRect )
	{
		pSrcRct = m_pSceneSourceRect;
	}

	if(m_pMultiViewRenderer)
	{
		m_pMultiViewRenderer->Present();
	}
	else
	{
		hr = m_pID3DDevice->Present(pSrcRct, NULL, (HWND)m_WindowId, NULL);
	}

	if (SUCCEEDED(hr))
		return true;

	//	if (hr == D3DERR_DEVICELOST)
	//	{
	//		DeviceLost = true;
	//		os::Printer::log("Present failed", "DIRECT3D9 device lost.", ELL_WARNING);
	//	}
	//#ifdef D3DERR_DEVICEREMOVED
	//	else if (hr == D3DERR_DEVICEREMOVED)
	//	{
	//		os::Printer::log("Present failed", "Device removed.", ELL_WARNING);
	//	}
	//#endif
	//	else if (hr == D3DERR_INVALIDCALL)
	//	{
	//		os::Printer::log("Present failed", "Invalid Call", ELL_WARNING);
	//	}
	//	else
	//		os::Printer::log("DIRECT3D9 present failed.", ELL_WARNING);


	return false;
}

// draw function
////////////////////////////////////////////

//! Draws a 3d line.
void EzD3DX9Driver::draw3DLine(const EzPoint3& start,	const EzPoint3& end, EzColor color)
{
	//setVertexShader(EVT_STANDARD);
	//setRenderStates3DMode();
	//S3DVertex v[2];
	//v[0].Color = color;
	//v[1].Color = color;
	//v[0].Pos = start;
	//v[1].Pos = end;

	//m_pID3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(S3DVertex));
}

// draw function end
///////////////////////////////////////////



// enable anti alias if possible and desired
bool EzD3DX9Driver::setMultiSample(EMULTISAMPLE_TYPE eMultiSampleType)
{
	DWORD dwQualityLevels = 0;

	if (SUCCEEDED(m_pID3D->CheckDeviceMultiSampleType(m_uiAdapter,
		m_devtype, m_D3DPresent.BackBufferFormat, !m_bFullscreen,
		EzD3DX9Mappings::get(eMultiSampleType), &dwQualityLevels)))
	{
		// enable multi sampling
		m_D3DPresent.MultiSampleType		= EzD3DX9Mappings::get(eMultiSampleType);
		m_D3DPresent.MultiSampleQuality		= dwQualityLevels-1;
		m_D3DPresent.SwapEffect				= D3DSWAPEFFECT_DISCARD;
		return true;
	}
	else
	{
//		log(EZ_WARN_NOT_SUPPORT_MULTISAMPLE, "Not Support this MultiSampleTyped.(%d)",(unsigned int) eMultiSampleType);
		return false;
	}

}


//! initialises the Direct3D API
bool EzD3DX9Driver::initDriver(const EzDimension2d& screenSize,	HWND hWnd, unsigned int uiBits, bool bFullScreen, bool bPureSoftware,
		bool bHighPrecisionFPU, bool bVsync, EMULTISAMPLE_TYPE eMultiSampleType, bool bMuiltView)
{
	HRESULT hr;

	m_WindowId = (void*) hWnd;
	m_bFullscreen = bFullScreen;
	m_CurrentDepthBufferSize = screenSize;

	if (!m_pID3D)
	{
		m_hD3DLibrary = LoadLibrary( "d3d9.dll" );

		if (!m_hD3DLibrary)
		{
//			log(EZ_ERROR_D3D_DLL, "Error, could not load d3d9.dll.");
			return false;
		}

		typedef IDirect3D9 * (__stdcall *D3DCREATETYPE)(UINT);
		D3DCREATETYPE d3dCreate = (D3DCREATETYPE) GetProcAddress(m_hD3DLibrary, "Direct3DCreate9");

		if (!d3dCreate)
		{
//			log(EZ_ERROR_D3D_DLL, "Error, could not get proc adress of Direct3DCreate9.");
			return false;
		}

		//just like pID3D = Direct3DCreate9(D3D_SDK_VERSION);
		m_pID3D = (*d3dCreate)(D3D_SDK_VERSION);

		if (!m_pID3D)
		{
//			log(EZ_ERROR_INITIALIZING_D3D, "Error, initializing D3D.");
			return false;
		}
	}

	// print device information
	D3DADAPTER_IDENTIFIER9 dai;
	if (!FAILED(m_pID3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &dai)))
	{
		int nProduct = HIWORD(dai.DriverVersion.HighPart);
		int nVersion = LOWORD(dai.DriverVersion.HighPart);
		int nSubVersion = HIWORD(dai.DriverVersion.LowPart);
		int nBuild = LOWORD(dai.DriverVersion.LowPart);

		EZLOG("%s %s %d.%d.%d.%d", dai.Description, dai.Driver, nProduct, nVersion,
			nSubVersion, nBuild);

		// Assign vendor name based on vendor id.
		switch(dai.VendorId)
		{
			case 0x1002 : m_vendorName = "ATI Technologies Inc."; break;
			case 0x10DE : m_vendorName = "NVIDIA Corporation"; break;
			case 0x102B : m_vendorName = "Matrox Electronic Systems Ltd."; break;
			case 0x121A : m_vendorName = "3dfx Interactive Inc"; break;
			case 0x5333 : m_vendorName = "S3 Graphics Co., Ltd."; break;
			case 0x8086 : m_vendorName = "Intel Corporation"; break;
			default: m_vendorName = "Unknown"; break;
		}
	}


	D3DDISPLAYMODE d3ddm;
	hr = m_pID3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(hr))
	{
		EZASSERT(!"Error: Could not get Adapter Display mode.");
		return false;
	}

	ZeroMemory(&m_D3DPresent, sizeof(m_D3DPresent));

	m_D3DPresent.BackBufferCount		= 1;
	m_D3DPresent.EnableAutoDepthStencil	= TRUE;
	if (bVsync)
		m_D3DPresent.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else
		m_D3DPresent.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (bFullScreen)
	{
		m_D3DPresent.BackBufferWidth = screenSize.Width;
		m_D3DPresent.BackBufferHeight = screenSize.Height;
		// request 32bit mode if user specified 32 bit, added by Thomas Stuefe
		if (uiBits == 32)
			m_D3DPresent.BackBufferFormat = D3DFMT_X8R8G8B8;
		else
			m_D3DPresent.BackBufferFormat = D3DFMT_R5G6B5;

		m_D3DPresent.SwapEffect	= D3DSWAPEFFECT_FLIP;
		m_D3DPresent.Windowed	= FALSE;
		m_D3DPresent.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	}
	else
	{
		m_D3DPresent.BackBufferWidth = screenSize.Width;
		m_D3DPresent.BackBufferHeight = screenSize.Height;
		m_D3DPresent.BackBufferFormat	= d3ddm.Format;
		m_D3DPresent.SwapEffect		= D3DSWAPEFFECT_COPY;
		m_D3DPresent.Windowed		= TRUE;
	}

	m_uiAdapter = D3DADAPTER_DEFAULT;
	m_devtype = D3DDEVTYPE_HAL;

	#if defined(_USE_NVIDIA_PERFHUD_)
	for (UINT adapter_i = 0; adapter_i < m_pID3D->GetAdapterCount(); ++adapter_i)
	{
		D3DADAPTER_IDENTIFIER9 identifier;
		m_pID3D->GetAdapterIdentifier(adapter_i,0,&identifier);
	
		if (strstr(identifier.Description,"PerfHUD") != 0)
		{
			m_uiAdapter = adapter_i;
			m_devtype = D3DDEVTYPE_REF;
			break;
		}
	}
	#endif

	bool bAntiAlias = false;

	// set MuiltSample for AntiAlias.
	for(unsigned int ui = (unsigned int) eMultiSampleType; ui > 0; --ui)
	{
		if(setMultiSample((EMULTISAMPLE_TYPE) ui))
		{
			bAntiAlias = true;
			break;
		}
	}

	// check stencil buffer compatibility
	if (m_bStencilBuffer)
	{
		m_D3DPresent.AutoDepthStencilFormat = D3DFMT_D24S8;
		if(FAILED(m_pID3D->CheckDeviceFormat(m_uiAdapter, m_devtype,
			m_D3DPresent.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE, m_D3DPresent.AutoDepthStencilFormat)))
		{
			m_D3DPresent.AutoDepthStencilFormat = D3DFMT_D24X4S4;
			if(FAILED(m_pID3D->CheckDeviceFormat(m_uiAdapter, m_devtype,
				m_D3DPresent.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, m_D3DPresent.AutoDepthStencilFormat)))
			{
				m_D3DPresent.AutoDepthStencilFormat = D3DFMT_D15S1;
				if(FAILED(m_pID3D->CheckDeviceFormat(m_uiAdapter, m_devtype,
					m_D3DPresent.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
					D3DRTYPE_SURFACE, m_D3DPresent.AutoDepthStencilFormat)))
				{
					EZLOG("Device does not support stencilbuffer, disabling stencil buffer.");
					m_bStencilBuffer = false;
				}
			}
		}
		else
		if(FAILED(m_pID3D->CheckDepthStencilMatch(m_uiAdapter, m_devtype,
			m_D3DPresent.BackBufferFormat, m_D3DPresent.BackBufferFormat, m_D3DPresent.AutoDepthStencilFormat)))
		{
			EZLOG("Depth-stencil format is not compatible with display format, disabling stencil buffer.");
			m_bStencilBuffer = false;
		}
	}
	// do not use else here to cope with flag change in previous block
	if (!m_bStencilBuffer)
	{
		m_D3DPresent.AutoDepthStencilFormat = D3DFMT_D32;
		if(FAILED(m_pID3D->CheckDeviceFormat(m_uiAdapter, m_devtype,
			m_D3DPresent.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE, m_D3DPresent.AutoDepthStencilFormat)))
		{
			m_D3DPresent.AutoDepthStencilFormat = D3DFMT_D24X8;
			if(FAILED(m_pID3D->CheckDeviceFormat(m_uiAdapter, m_devtype,
				m_D3DPresent.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, m_D3DPresent.AutoDepthStencilFormat)))
			{
				m_D3DPresent.AutoDepthStencilFormat = D3DFMT_D16;
				if(FAILED(m_pID3D->CheckDeviceFormat(m_uiAdapter, m_devtype,
					m_D3DPresent.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
					D3DRTYPE_SURFACE, m_D3DPresent.AutoDepthStencilFormat)))
				{
					EZASSERT(!"Device does not support required depth buffer.");
					return false;
				}
			}
		}
	}

	// create device

	DWORD fpuPrecision = bHighPrecisionFPU ? D3DCREATE_FPU_PRESERVE : 0;
	if (bPureSoftware)
	{
		hr = m_pID3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
				fpuPrecision | D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_D3DPresent, &m_pID3DDevice);

		if (FAILED(hr))
		{
			EZASSERT(!"Was not able to create Direct3D9 device.");
			return false;
		}
	}
	else
	{
		hr = m_pID3D->CreateDevice(m_uiAdapter, m_devtype, hWnd,
				fpuPrecision | D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_D3DPresent, &m_pID3DDevice);

		if(FAILED(hr))
			hr = m_pID3D->CreateDevice(m_uiAdapter, m_devtype, hWnd,
					fpuPrecision | D3DCREATE_MIXED_VERTEXPROCESSING , &m_D3DPresent, &m_pID3DDevice);

		if(FAILED(hr))
			hr = m_pID3D->CreateDevice(m_uiAdapter, m_devtype, hWnd,
					fpuPrecision | D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_D3DPresent, &m_pID3DDevice);

		if (FAILED(hr))
		{
			EZASSERT(!"Was not able to create Direct3D9 device.");
			return false;
		}
	}



	// enable antialiasing
	if (bAntiAlias)
		m_pID3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

	// check driver compatibility
	checkCompatibility(m_devtype);

	// disable stencilbuffer if necessary
	if (m_bStencilBuffer &&
		(!(m_D3DCaps.StencilCaps & D3DSTENCILCAPS_DECRSAT) ||
		!(m_D3DCaps.StencilCaps & D3DSTENCILCAPS_INCRSAT) ||
		!(m_D3DCaps.StencilCaps & D3DSTENCILCAPS_KEEP)))
	{
		EZLOG("Device not able to use stencil buffer, disabling stencil buffer.");
		m_bStencilBuffer = false;
	}


	//// set fog mode
	//setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// set exposed data
	m_ExposedData.D3D9.D3D9 = m_pID3D;
	m_ExposedData.D3D9.D3DDev9 = m_pID3DDevice;
	m_ExposedData.D3D9.HWnd = hWnd;

	m_bResetRenderStates = true;


	setAmbientLight(EzColor());

	//
	//MaxTextureUnits = min_((unsigned int)Caps.MaxSimultaneousTextures, MATERIAL_MAX_TEXTURES);
	//MaxUserClipPlanes = (unsigned int)Caps.MaxUserClipPlanes;

	//// set the renderstates
//	setRenderStates3DMode();

	// set maximal anisotropy
	m_pID3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, min(16ul, m_D3DCaps.MaxAnisotropy));
	m_pID3DDevice->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, min(16ul, m_D3DCaps.MaxAnisotropy));
	m_pID3DDevice->SetSamplerState(2, D3DSAMP_MAXANISOTROPY, min(16ul, m_D3DCaps.MaxAnisotropy));
	m_pID3DDevice->SetSamplerState(3, D3DSAMP_MAXANISOTROPY, min(16ul, m_D3DCaps.MaxAnisotropy));

	// store the screen's depth buffer
	m_aDepthBuffers.push_back(new SDepthSurface());
	m_pID3DDevice->GetDepthStencilSurface(&(m_aDepthBuffers[0]->Surface));
	m_aDepthBuffers[0]->Size= screenSize;

	m_D3DColorFormat = D3DFMT_A8R8G8B8;
	IDirect3DSurface9* bb=0;
	if (SUCCEEDED(m_pID3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb)))
	{
		D3DSURFACE_DESC desc;
		bb->GetDesc(&desc);
		m_D3DColorFormat = desc.Format;

		if (m_D3DColorFormat == D3DFMT_X8R8G8B8)
			m_D3DColorFormat = D3DFMT_A8R8G8B8;

		bb->Release();
	}



	// create Buffer Manager
	m_pBufferManager = EzExternalNew EzD3DX9BufferManager(m_pID3DDevice);

	// creat GpuProgram Manager 
	m_pGpuProgramManager = EzExternalNew EzD3DX9GPUProgramManager(m_pID3DDevice);

	
//	m_pTextureStage = EzNew EzD3DX9TextureStage(m_pID3DDevice, m_D3DCaps.MaxTextureBlendStages);

	// create multiView Renderer (add Swapchain)
	if(bMuiltView)
		m_pMultiViewRenderer = EzNew EzD3DX9MultiViewRenderer(m_pID3DDevice, m_D3DPresent);

	m_eColorFormat = EzD3DX9Mappings::get(m_D3DColorFormat);

	//EzRegisterShader(EzD3DX9DefaultShader);




	// init texture SamplerState
	//STextureSamplerState TexSampler;

	//setTextSamplerState(0, TexSampler, true);

	// so far so good.
	return true;
}


// check a driver of compatibility
void EzD3DX9Driver::checkCompatibility(D3DDEVTYPE devtype)
{
	m_pID3DDevice->GetDeviceCaps(&m_D3DCaps);

//	log(EZ_DIRVER_INFORMATION, "Supported Vertex Shader Version { %d.%d }", D3DSHADER_VERSION_MAJOR( m_D3DCaps.VertexShaderVersion ),
	//	D3DSHADER_VERSION_MINOR( m_D3DCaps.VertexShaderVersion ) );
	//log(EZ_DIRVER_INFORMATION, "Supported Pixel Shader Version { %d.%d }", D3DSHADER_VERSION_MAJOR( m_D3DCaps.PixelShaderVersion ),
	//	D3DSHADER_VERSION_MINOR( m_D3DCaps.PixelShaderVersion ) );


	//Check Hardware skinning 
	m_uiVertexBlendMatricesCount = m_D3DCaps.MaxVertexBlendMatrices;

	m_uiMaxTextureBlendStages = m_D3DCaps.MaxTextureBlendStages;

	if(m_uiVertexBlendMatricesCount >= HW_BONE_LIMIT)
		setHardwareSkinning(true);

	//m_uiHWBones = m_kD3DCaps9.MaxVertexBlendMatrices;

	//// Software vertex processing allows 4 "HW" bones
	//if (m_bSWVertexCapable && !m_bSWVertexSwitchable)
	//	m_uiHWBones = 4;

	//if (m_uiHWBones >= HW_BONE_LIMIT)
	//	m_uiRendFlags |= CAPS_HARDWARESKINNING;


	// Check for texture support
	if( SUCCEEDED( m_pID3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, devtype, m_D3DPresent.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT1 ) ) )
		m_bTextureFormatSupport[COMPRESSED_TEXTURESUPPORT] = true;
	
	if( SUCCEEDED( m_pID3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, devtype, m_D3DPresent.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_A1R5G5B5 ) ) )
		m_bTextureFormatSupport[A1R5G5G5_TEXTURESUPPORT] = true;
	
	if( SUCCEEDED( m_pID3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, devtype, m_D3DPresent.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_R5G6B5 ) ) )
		m_bTextureFormatSupport[R5G6B5_TEXTURESUPPORT] = true;
	
	if( SUCCEEDED( m_pID3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, devtype, m_D3DPresent.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_A4R4G4B4 ) ) )
		m_bTextureFormatSupport[A4R4G4B4_TEXTURESUPPORT] = true;
	
	if( SUCCEEDED( m_pID3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, devtype, m_D3DPresent.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_X8R8G8B8 ) ) )
		m_bTextureFormatSupport[X8R8G8B8_TEXTURESUPPORT] = true;
	
	if( SUCCEEDED( m_pID3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, devtype, m_D3DPresent.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8 ) ) )
		m_bTextureFormatSupport[A8R8G8B8_TEXTURESUPPORT] = true;
	
	if( SUCCEEDED( m_pID3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, devtype, m_D3DPresent.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_L8 ) ) )
		m_bTextureFormatSupport[L8_TEXTURESUPPORT] = true;




}


//! sets the current Texture
bool EzD3DX9Driver::setTexture(unsigned int eMap, EzTexture* pSource)
{
	if (!pSource)
	{
		m_pID3DDevice->SetTexture(eMap, 0);
		m_pID3DDevice->SetTextureStageState( eMap, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	}
	else
	{
		m_pID3DDevice->SetTexture(eMap, ((const EzD3DX9Texture*)pSource)->getDX9Texture());
	}
	return true;
}


//! sets transformation
void EzD3DX9Driver::setTransform(ETRANSFORMATION_STATE state,const EzMatrix4& mat)
{
	m_bTransformation3DChanged = true;

	switch(state)
	{
	case ETS_VIEW:
		m_pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)mat.pointer()));
		break;
	case ETS_WORLD:
		m_pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)mat.pointer()));
		break;
	case ETS_PROJECTION:
		m_pID3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)((void*)mat.pointer()));
		break;
	case ETS_TEXTURE_0:
	case ETS_TEXTURE_1:
	case ETS_TEXTURE_2:
	case ETS_TEXTURE_3:
		m_pID3DDevice->SetTextureStageState( state - ETS_TEXTURE_0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		m_pID3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+ ( state - ETS_TEXTURE_0 )),
			(D3DMATRIX*)((void*)mat.pointer()));
		break;
	case ETS_COUNT:
		break;
	}

	m_Matrices[state] = mat;
}


//! draws a vertex primitive EzList
void EzD3DX9Driver::drawVertexPrimitive(EPRIMITIVE_TYPE eType, unsigned int uiVertexStart, unsigned int uiIndexNum, unsigned int uiIndexStart)
{
	EzVideoDriver::drawVertexPrimitive(eType, uiVertexStart, uiIndexNum, uiIndexStart);

	if (!uiIndexNum)
		return;

	D3DPRIMITIVETYPE primType = D3DPT_TRIANGLELIST;
	unsigned int primCount = 0;
	switch(eType)
	{
	case EPT_POINTS:
		primType = D3DPT_POINTLIST;
		primCount = (unsigned int) uiIndexNum;
		break;

	case EPT_LINES:
		primType = D3DPT_LINELIST;
		primCount = (unsigned int) uiIndexNum / 2;
		break;

	case EPT_LINE_STRIP:
		primType = D3DPT_LINESTRIP;
		primCount = (unsigned int) uiIndexNum - 1;
		break;

	case EPT_TRIANGLE_LIST:
		primType = D3DPT_TRIANGLELIST;
		primCount = (unsigned int) uiIndexNum / 3;
		break;

	case EPT_TRIANGLE_STRIP:
		primType = D3DPT_TRIANGLESTRIP;
		primCount = (unsigned int) uiIndexNum - 2;
		break;

	case EPT_TRIANGLE_FAN:
		primType = D3DPT_TRIANGLEFAN;
		primCount = (unsigned int) uiIndexNum - 2;
		break;
	}

	HRESULT hr;

	// do indexed draw operation
	hr = m_pID3DDevice->DrawIndexedPrimitive(
		primType, 
		static_cast<INT>(uiVertexStart), 
		0, 
		static_cast<UINT>(uiIndexNum), 
		static_cast<UINT>(uiIndexStart), 
		static_cast<UINT>(primCount)
		);

	if (FAILED(hr))
	{
//		log(0, "DrawIndexedPrimitive fail");
	}
}



EzVertexData* EzD3DX9Driver::createVertexData(EzTriBasedGeomData* pGemData, EzHardwareSkinPartition::Partition* pPartition, 
	unsigned int& uiPosOffset, unsigned int& uiWeightOffset, unsigned int& uiBoneIndexOffset, unsigned int& uiNormOffset, unsigned int& uiColorOffset, unsigned int& uiTexOffset)
{
	unsigned short usStreamCount = 0;
	EzD3DX9VertexDeclaration* pVertexDec;
	static EzColor whiteVertexColor(255, 255, 255, 255);

	pVertexDec = EzStaticCast(EzD3DX9VertexDeclaration, generateVertexStream(pGemData, pPartition, uiPosOffset, uiWeightOffset,
		uiBoneIndexOffset, uiNormOffset, uiColorOffset,	uiTexOffset));

	EzVertexBufferBinding* pVertexBinding = createVertexBufferBinding();


	// Create Stream Source 0
	EzD3DX9VertexBuffer* pVertexBuffer = EzStaticCast(EzD3DX9VertexBuffer, 
		createVertexBuffer(pVertexDec->getStride(usStreamCount), pGemData->getNumVertex(), Ez3DBuffer::ESTATIC));

	pVertexBinding->setBinding(usStreamCount, pVertexBuffer);

	usStreamCount++;

	if(uiColorOffset != UINT_MAX)
	{
		// Create Stream Source 0
		EzD3DX9VertexBuffer* pVertexColor = EzStaticCast(EzD3DX9VertexBuffer, 
			createVertexBuffer(pVertexDec->getStride(usStreamCount), pGemData->getNumVertex(), Ez3DBuffer::ESTATIC));

		pVertexBinding->setBinding(usStreamCount, pVertexColor);


		u8* pLockVertexColor;

		pLockVertexColor = (u8*) pVertexColor->lock(Ez3DBuffer::ENORMAL);

		for(unsigned int ui = 0; ui < pVertexColor->getNumVertices(); ++ui, pLockVertexColor += pVertexColor->getStride())
		{
			if(pGemData->getVertexColors())
			{
				EzMemoryCopy(&pLockVertexColor[uiColorOffset], &pGemData->getVertexColors()[ui], sizeof(EzColor));
			}
			else
			{
				EzMemoryCopy(&pLockVertexColor[uiColorOffset], &whiteVertexColor, sizeof(EzColor));
			}
		}
	}

	if(uiTexOffset != UINT_MAX)
	{
		u8* pTextPosLock;
		EzD3DX9VertexBuffer* pTexPostionStream;

		EzPoint2* pTest;

		for(unsigned int n = 0; n < pGemData->getNumUVSets(); ++n)
		{
			usStreamCount++;
			
			pTexPostionStream = EzStaticCast(EzD3DX9VertexBuffer, 
				createVertexBuffer(pVertexDec->getStride(usStreamCount), pGemData->getNumVertex(), Ez3DBuffer::ESTATIC));

			pVertexBinding->setBinding(usStreamCount, pTexPostionStream);

			pTextPosLock = (u8*) pTexPostionStream->lock(Ez3DBuffer::ENORMAL);



			for(unsigned int ui = 0; ui < pTexPostionStream->getNumVertices(); ++ui, pTextPosLock += pTexPostionStream->getStride())
			{
				pTest = (EzPoint2*) &pGemData->getUVSet(n)[ui];

				EzMemoryCopy(&pTextPosLock[uiTexOffset + (sizeof(EzPoint2) * n)], &pGemData->getUVSet(n)[ui], sizeof(EzPoint2));
			}

			pTexPostionStream->unlock();

		}
	}

	return EzNew EzVertexData(pVertexDec, pVertexBinding);
}


void EzD3DX9Driver::skinningDeformSW(EzGeometryData* pGemData, EzRendererData* pRendererData, EzSkinInstance* pSkinInstance)
{

	EzPoint3 *pDestVertices;
	EzPoint3 *pDestNomals;
	EzD3DX9VertexDeclaration* pVertexDec;
	u8* pLockVertex;
	const EzVertexElement* pElementTemp;

	EzVertexData* pVertexData = pRendererData->getVertexData();
	EzVertexBufferBinding* pVertexBufferBinding = pVertexData->getVertexBinding();

	pVertexDec = EzStaticCast(EzD3DX9VertexDeclaration, pVertexData->getVertexDeclaration());
	EzD3DX9VertexBuffer* pVertexBuffer0 = (EzD3DX9VertexBuffer*) pVertexData->getVertexBuffer(0);


	pLockVertex = (u8*) pVertexBuffer0->lock(Ez3DBuffer::EDISCARD);

	if(pGemData->getVertices())
	{
		pElementTemp = pVertexDec->findElementBySemantic(EzVertexElement::EVES_POSITION);
		EZASSERT(pElementTemp);
		pDestVertices = (EzPoint3*) &pLockVertex[pElementTemp->getOffset()];
	}

	if(pGemData->getVertexNormals())
	{
		pElementTemp = pVertexDec->findElementBySemantic(EzVertexElement::EVES_NORMAL);
					
		EZASSERT(pElementTemp);
		pDestNomals = (EzPoint3*)&pLockVertex[pElementTemp->getOffset()];
	}


	pSkinInstance->deform(pGemData->getVertices(), pGemData->getVertexNormals(), pVertexBuffer0->getNumVertices(),
		pDestVertices, pDestNomals, pVertexBuffer0->getStride());

	pVertexBuffer0->unlock();

}

void EzD3DX9Driver::prePareRenderPass(EzGeometry* pGeometry)
{
	//EzSkinInstance* pSkinInstance = pGeometry->getSkinInstance();
	//EzRenderPass *pPass = pGeometry->getRenderPass();

	//if(pSkinInstance)
	//{
	//	EzHardwareSkinPartition* pHWSkinPartition = pSkinInstance->getHWSkinPartition();

	//	if(pHWSkinPartition && isHardwareSkinning())
	//	{
	//		if(!pPass->getVertexProgram())
	//		{
	//			pPass->setVertexProgram("HardwareSkinning");
	//		}

	//		for(unsigned int ui = 0; ui < pHWSkinPartition->getPartitionCount(); ++ui)
	//		{
	//			pSkinInstance->calculateBoneMatrices(ui);
	//		}
	//	}
	//}
}


void EzD3DX9Driver::prePareRanderTexture(EzGeometry* pGeometry)
{
	//EzSkinInstance* pSkinInstance = pGeometry->getSkinInstance();
	//EzRenderPass *pPass = pGeometry->getRenderPass();

	//EzTexturingProperty* pTextureProperty;
	//unsigned short usTextureCount;


	//pTextureProperty = pGeometry->getTextureProperty();

	//if(pTextureProperty == NULL)
	//	return;

	//usTextureCount = pTextureProperty->getTextureCount();

	//if(getTextureBlendStagesCount() < usTextureCount)
	//{
	//	printf("1111");
	//}

	//EzD3DX9TextureStage* pD3DTextureStage = EzNew EzD3DX9TextureStage(m_pID3DDevice);

	//EzTexture* pTexture = pTextureProperty->getTextureRanderData(EzTexturingProperty::BASE_MAP);

	//pD3DTextureStage->setStage(0);
	//pD3DTextureStage->setTexture(pTexture);


	//pPass->setTextureStage(0, pD3DTextureStage);

}


void EzD3DX9Driver::render(EzGeometry* pGeometry)
{
	EzRendererData* pRendererData = m_pRenderer->prepareGeometryForRendering(pGeometry, pGeometry->getSkinInstance());

	switch(pRendererData->getAnimationType())
	{
	case EzRendererData::E_SW_SKINNING:
		skinningDeformSW(pGeometry->getData(), pRendererData, pGeometry->getSkinInstance());
		break;
	case EzRendererData::E_HW_SKINNING:
		prePareRenderPass(pGeometry);
		break;
	}

	m_pRenderer->setupRenderingPass(pGeometry, pRendererData);


	prePareRanderTexture(pGeometry);

//	m_pEntity->addGeometry(pGeometry);
}


//! Draw hardware buffer
//void EzD3DX9Driver::drawBuffer(EzRendererData* pRendererData)
//{
//	while(pRendererData)
//	{
//		EzVertexData* pVertexData = pRendererData->getVertexData();
//		
//		EzD3DX9VertexDeclaration* pVertexDeclaration = EzStaticCast(EzD3DX9VertexDeclaration, pVertexData->getVertexDeclaration());
//
//		EzGPUProgram* pVertexProgram = pPass->getVertexProgram();
//
//		if(pVertexProgram)
//		{
//			const  EzHardwareSkinPartition::Partition* pPartition = pRendererData->getHardwareSkinPartition();
//
//			if(pPartition && isHardwareSkinning())
//			{
//				EzGpuProgramParameters* pParam = pPass->getVertexProgramParameters();
//
//				for(unsigned int i = 0; i < pPartition->m_uiNumMatrices; ++i)
//				{
//					pParam->setShaderConstant(7 + i * 3, &pPartition->m_pfBoneMatrices[4 * 3 * i], 3);
//				}
//
//				bindGpuProgramParameters(pVertexProgram->getType(), pParam);
//
//			}
//		}
//
//		setVertexDeclaration(pVertexDeclaration);
//		setMeshBufferDataBinding(pVertexData);
//
//		EzIndexData* pIndexData = pRendererData->getIndexData();
//		unsigned int uiIndexStart = 0;
//
//		if(pIndexData)
//		{
//			EzD3DX9IndexBuffer* pIndexBuffer = EzStaticCast(EzD3DX9IndexBuffer, pIndexData->getIndexBuffer());
//			uiIndexStart = pIndexBuffer->getNumIndexes();
//			m_pID3DDevice->SetIndices((IDirect3DIndexBuffer9*) pIndexBuffer->getD3DIndexBuffer());
//		}
//		else
//		{
//			m_pID3DDevice->SetIndices(NULL);
//		}
//
//		drawVertexPrimitive(pRendererData->getPrimitiveType(), 0, uiIndexStart, 0);
//
//		pRendererData = pRendererData->getNextRendererData();
//	}
//
//}

void EzD3DX9Driver::drawBuffer(EzRendererData* pRendererData)
{
	EzIndexData* pIndexData = pRendererData->getIndexData();
	unsigned int uiIndexStart = 0;

	if(pIndexData)
	{
		EzD3DX9IndexBuffer* pIndexBuffer = EzStaticCast(EzD3DX9IndexBuffer, pIndexData->getIndexBuffer());
		uiIndexStart = pIndexBuffer->getNumIndexes();
		m_pID3DDevice->SetIndices((IDirect3DIndexBuffer9*) pIndexBuffer->getD3DIndexBuffer());
	}
	else
	{
		m_pID3DDevice->SetIndices(NULL);
	}

	drawVertexPrimitive(pRendererData->getPrimitiveType(), 0, uiIndexStart, 0);

}



//! sets the needed renderstates
void EzD3DX9Driver::setRenderStates2DMode(bool bAlpha, bool bTexture, bool bAlphaChannel)
{
	if (m_eCurrentRenderMode != ERND_2D || m_bTransformation3DChanged)
	{
		// unset last 3d material
		if (m_eCurrentRenderMode != ERND_2D)
		{
			//if (static_cast<unsigned int>(LastMaterial.MaterialType) < MaterialRenderers.size())
			//	MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();
			//setBasicRenderStates(SMaterial(), SMaterial(), true);

			// everything that is wrongly set by SMaterial default
			m_pID3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			m_pID3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

			m_pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			m_pID3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			m_pID3DDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
			m_pID3DDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			m_pID3DDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
			m_pID3DDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			m_pID3DDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );

			m_pID3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

			m_pID3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
			m_pID3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
			//m_pID3DDevice->SetTransform( D3DTS_TEXTURE0, &UnitMatrixD3D9 );
		}

		//m_pID3DDevice->SetTransform(D3DTS_VIEW, &UnitMatrixD3D9);
		//m_pID3DDevice->SetTransform(D3DTS_WORLD, &UnitMatrixD3D9);
		//m_pID3DDevice->SetTransform(D3DTS_PROJECTION, &UnitMatrixD3D9);

		m_bTransformation3DChanged = false;
	}

	if (bTexture)
	{
		m_pID3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m_pID3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		m_pID3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

		if (bAlphaChannel)
		{
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

			if (bAlpha)
			{
				m_pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
				m_pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				m_pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
			}
			else
			{
				m_pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
				m_pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			}

			m_pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		else
		{
			if (bAlpha)
			{
				m_pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
				m_pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				m_pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				m_pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			}
			else
			{
				m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
				m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				m_pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
				m_pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			}
		}
	}
	else
	{
		if (bAlpha)
		{
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			m_pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		else
		{
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			m_pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
	}

	m_eCurrentRenderMode = ERND_2D;
}

//! returns a device dependent texture from a software surface (EzImage)
EzTexture* EzD3DX9Driver::createDeviceDependentTexture(EzImage* surface, const c8* name)
{
	return EzNew EzD3DX9Texture(surface, this, m_uiTextureCreationFlags, name);
}

//! queries the features of the driver, returns true if feature is available
bool EzD3DX9Driver::queryFeature(EVIDEO_DRIVER_FEATURE feature) const
{
	if (!m_bFeatureEnabled[feature])
		return false;

	switch (feature)
	{
	case EVDF_MULTITEXTURE:
	case EVDF_BILINEAR_FILTER:
		return true;
	case EVDF_RENDER_TO_TARGET:
		return m_D3DCaps.NumSimultaneousRTs > 0;
	case EVDF_HARDWARE_TL:
		return (m_D3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
	case EVDF_MIP_MAP:
		return (m_D3DCaps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) != 0;
	case EVDF_MIP_MAP_AUTO_UPDATE:
		// always return false because a lot of drivers claim they do
		// this but actually don't do this at all.
		return false; //(Caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) != 0;
	case EVDF_STENCIL_BUFFER:
		return m_bStencilBuffer && m_D3DCaps.StencilCaps;
	case EVDF_VERTEX_SHADER_1_1:
		return m_D3DCaps.VertexShaderVersion >= D3DVS_VERSION(1,1);
	case EVDF_VERTEX_SHADER_2_0:
		return m_D3DCaps.VertexShaderVersion >= D3DVS_VERSION(2,0);
	case EVDF_VERTEX_SHADER_3_0:
		return m_D3DCaps.VertexShaderVersion >= D3DVS_VERSION(3,0);
	case EVDF_PIXEL_SHADER_1_1:
		return m_D3DCaps.PixelShaderVersion >= D3DPS_VERSION(1,1);
	case EVDF_PIXEL_SHADER_1_2:
		return m_D3DCaps.PixelShaderVersion >= D3DPS_VERSION(1,2);
	case EVDF_PIXEL_SHADER_1_3:
		return m_D3DCaps.PixelShaderVersion >= D3DPS_VERSION(1,3);
	case EVDF_PIXEL_SHADER_1_4:
		return m_D3DCaps.PixelShaderVersion >= D3DPS_VERSION(1,4);
	case EVDF_PIXEL_SHADER_2_0:
		return m_D3DCaps.PixelShaderVersion >= D3DPS_VERSION(2,0);
	case EVDF_PIXEL_SHADER_3_0:
		return m_D3DCaps.PixelShaderVersion >= D3DPS_VERSION(3,0);
	case EVDF_HLSL:
		return m_D3DCaps.VertexShaderVersion >= D3DVS_VERSION(1,1);
	case EVDF_TEXTURE_NSQUARE:
		return (m_D3DCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) == 0;
	case EVDF_TEXTURE_NPOT:
		return (m_D3DCaps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0;
	case EVDF_COMPRESSED_TEXTURES:
		return m_bTextureFormatSupport[COMPRESSED_TEXTURESUPPORT];
	case EVDF_A1R5G5G5_TEXTURES:
		return m_bTextureFormatSupport[A1R5G5G5_TEXTURESUPPORT];
	case EVDF_R5G6B5_TEXTURES:
		return m_bTextureFormatSupport[R5G6B5_TEXTURESUPPORT];
	case EVDF_A4R4G4B4_TEXTURES:
		return m_bTextureFormatSupport[A4R4G4B4_TEXTURESUPPORT];
	case EVDF_X8R8G8B8_TEXTURES:
		return m_bTextureFormatSupport[X8R8G8B8_TEXTURESUPPORT];
	case EVDF_A8R8G8B8_TEXTURES:
		return m_bTextureFormatSupport[A8R8G8B8_TEXTURESUPPORT];
	case EVDF_L8_TEXTURES:
		return m_bTextureFormatSupport[L8_TEXTURESUPPORT];

	default:
		return false;
	};
}


//! Enables or disables a texture creation flag.
void EzD3DX9Driver::setTextureCreationFlag(eTEXTURE_CREATION_FLAG flag,
		bool enabled)
{
	if (flag == ETCF_CREATE_MIP_MAPS && !queryFeature(EVDF_MIP_MAP))
		enabled = false;

	EzVideoDriver::setTextureCreationFlag(flag, enabled);
}


//! returns the maximal amount of dynamic lights the device can handle
unsigned int EzD3DX9Driver::getMaximalDynamicLightAmount() const
{
	return m_D3DCaps.MaxActiveLights;
}


//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void EzD3DX9Driver::setAmbientLight(const EzColor& color)
{
	m_AmbientLight = color;
	D3DCOLOR col = color.color;
	m_pID3DDevice->SetRenderState(D3DRS_AMBIENT, col);
}


//! deletes all dynamic lights there are
void EzD3DX9Driver::clearDynamicLights()
{
	for (unsigned int i=0; i<m_uiLastSetLight+1; ++i)
		m_pID3DDevice->LightEnable(i, false);

	m_uiLastSetLight = -1;
}


//! adds a EzD3DX9Driver light
void EzD3DX9Driver::setDynamicLight(const unsigned int uindex, EzLight* pLight)
{
	if (m_uiLastSetLight == m_D3DCaps.MaxActiveLights - 1)
		return;

	D3DLIGHT9 light;

	switch (pLight->getEffectType())
	{
	case EzDynamicEffect::POINT_LIGHT:
		light.Type = D3DLIGHT_POINT;
	break;
	case EzDynamicEffect::SPOT_LIGHT:
		light.Type = D3DLIGHT_SPOT;
	break;
	case EzDynamicEffect::DIR_LIGHT:
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Direction = *(D3DVECTOR*)((void*)(&((EzDirectionalLight*) EzStaticCast(EzDirectionalLight, pLight))->getDirection()));
	break;
	}

	//light.Position = *(D3DVECTOR*)((void*)(&dl.vPosition));


	//light.Range = min_(dl.fRadius, m_fMaxLightDistance);
	//light.Falloff = dl.fFalloff;

	light.Diffuse = *(D3DCOLORVALUE*)((void*)(&pLight->getDiffuseColor()));
	light.Specular = *(D3DCOLORVALUE*)((void*)(&pLight->getSpecularColor()));
	light.Ambient = *(D3DCOLORVALUE*)((void*)(&pLight->getAmbientColor()));

	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;


	//light.Attenuation0 = dl.vAttenuation.X;
	//light.Attenuation1 = dl.vAttenuation.Y;
	//light.Attenuation2 = dl.vAttenuation.Z;

	//light.Theta = dl.fInnerCone * 2.0f * DEGTORAD;
	//light.Phi = dl.fOuterCone * 2.0f * DEGTORAD;

	++m_uiLastSetLight;
	m_pID3DDevice->SetLight(m_uiLastSetLight, &light);
	m_pID3DDevice->LightEnable(m_uiLastSetLight, true);
}


void EzD3DX9Driver::setEffectState(EzDynamicEffectState* pEffectState)
{

	//if(m_lastDynamicEffectState == pEffectState)
	//	return;

	clearDynamicLights();

	if(pEffectState)
	{
		EzDynEffectStateIter LightPos = pEffectState->getLightHeadPos();
		EzLight* pLight;

		while(LightPos)
		{
			pLight = pEffectState->getNextLight(LightPos);

			setDynamicLight(pLight->getIndex(), pLight);
		}
	}

	m_lastDynamicEffectState = pEffectState;
	
}

bool EzD3DX9Driver::setResize(void* WindowId, int iWidth, int iHigth)
{
	EZASSERT(m_pMultiViewRenderer);
	return m_pMultiViewRenderer->setResize(WindowId, iWidth, iHigth);

}

void EzD3DX9Driver::setTargetWindow(void* WindowID)
{
	EZASSERT(m_pMultiViewRenderer);
	m_pMultiViewRenderer->setTargetWindow(WindowID);

}

bool EzD3DX9Driver::createMultiRenderer(void* WindowID, int iWidth, int iHigth)
{
	EZASSERT(m_pMultiViewRenderer);
	return m_pMultiViewRenderer->createMultiRenderer(WindowID, iWidth, iHigth);
}


bool EzD3DX9Driver::setMeshBufferDataBinding(EzVertexData* pVertexData)
{
	HRESULT hr;

	EzVertexBufferBinding* pVertexBufferBinding = pVertexData->getVertexBinding();

	EzVertexBufferBinding::EzVertexBufferBindingArray MapSource = pVertexBufferBinding->getBindings(); 
	

	//// TODO: attempt to detect duplicates
	unsigned short uiSource = 0;
	EzD3DX9VertexBuffer* pVertexBuffer;

	for(uiSource = 0; uiSource < MapSource.size(); ++uiSource)
	{
		if(uiSource < MapSource[uiSource]->getSourceNum())
		{
			hr = m_pID3DDevice->SetStreamSource(static_cast<UINT>(uiSource), NULL, 0, 0);
			uiSource++;

			if (FAILED(hr))
			{
//				log(EZ_ERROR_UNABLE_D3D9STREAM_SOURCE_RESET, "Unable to reset unused D3D9 stream source");
				return false;
			}

			continue;
		}

		pVertexBuffer = EzStaticCast(EzD3DX9VertexBuffer, MapSource[uiSource]);

		hr = m_pID3DDevice->SetStreamSource(static_cast<UINT>(uiSource), 
			pVertexBuffer->getD3DVertexBuffer(),
			0, // no stream offset, this is handled in _render instead
			static_cast<UINT>(pVertexBuffer->getStride()) // stride
			);

		if (FAILED(hr))
		{
//			log(EZ_ERROR_UNABLE_D3D9STREAM_SOURCE_BIND,"Unable to set D3D9 stream source for buffer binding");
			return false;
		}
	
	}

	// Unbind any unused sources
	for (unsigned short unused = uiSource; unused < m_uiLastVertexSourceCount; ++unused)
	{

		hr = m_pID3DDevice->SetStreamSource(static_cast<UINT>(unused), NULL, 0, 0);
		if (FAILED(hr))
		{
//			log(EZ_ERROR_UNABLE_D3D9STREAM_SOURCE_RESET, "Unable to reset unused D3D9 stream source");
			return false;
		}
	}

	// When Call the next Unbind any sources
	m_uiLastVertexSourceCount = uiSource;
	return true;

}

void EzD3DX9Driver::bindGpuProgram(EzGPUProgram* prg)
{
	HRESULT hr;
	switch (prg->getType())
	{
	case EGP_VERTEX_PROGRAM:
		hr = m_pID3DDevice->SetVertexShader(
			static_cast<EzD3DX9GPUVertexProgram*>(prg)->getVertexShader());
		if (FAILED(hr))
		{
//			log(EZ_WARN_UNABLE_CALLING_VERTEXSHADER, "Error calling SetVertexShader");
			//OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error calling SetVertexShader", "D3D9RenderSystem::bindGpuProgram");
		}
		break;
	case EGP_FRAGMENT_PROGRAM:
		//hr = m_pID3DDevice->SetPixelShader(
		//	static_cast<D3D9GpuFragmentProgram*>(prg)->getPixelShader());
		//if (FAILED(hr))
		//{
		////	OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error calling SetPixelShader", "D3D9RenderSystem::bindGpuProgram");
		//}
		break;
	};

	//RenderSystem::bindGpuProgram(prg);

}


void EzD3DX9Driver::bindGpuProgramParameters(EGPUProgramType etype, EzGpuProgramParameters* pParams)
{
	HRESULT hr;
	EzGpuLogicalIndexUseMap* pFloatLogical = pParams->getFloatLogicalToPhysical();
	EzGpuLogicalIndexUseMap* pIntLogical = pParams->getIntLogicalToPhysical();

	switch(etype)
	{
	case EGP_VERTEX_PROGRAM:
		{
			EzGpuLogicalIndexUseMap::Iterator *it = &pFloatLogical->getIterator();

			for(; !it->atEnd(); (*it)++)
			{
				unsigned int uiLogicalIndx = (*it)->getKey();
				const float* pFloat = pParams->getFloatPointer((*it)->getValue().uiPhysicalIndex);
				unsigned int uiSlotCount = (*it)->getValue().uiCurrentSize / 4;
				EZASSERT((*it)->getValue().uiCurrentSize % 4 == 0);

				if (FAILED(hr = m_pID3DDevice->SetVertexShaderConstantF(uiLogicalIndx, pFloat, uiSlotCount)))
				{
//					log(EZ_WARN_UNABLE_UPLOAD_VERTEXSHADER_PARAM, "Unable to upload vertex shader float parameters");
				}
			}
		}
	}
}

void EzD3DX9Driver::release()
{
	EZASSERT(m_spVideoDriver);
	EzDelete (EzD3DX9Driver*) m_spVideoDriver;

}


//! creates a video driver
EzVideoDriver* createDirectX9Driver(const EzDimension2d& screenSize,	HWND window, unsigned int uiBits, bool bFullscreen, bool bStencilbuffer,
	bool bPureSoftware, bool bHighPrecisionFPU,	bool bVsync, EMULTISAMPLE_TYPE eMultiSampleType, bool bMuiltView)
{
	EzD3DX9Driver* dx9 = EzNew EzD3DX9Driver();

	if (!dx9->initDriver(screenSize, window, uiBits, bFullscreen, bPureSoftware, bHighPrecisionFPU, bVsync, eMultiSampleType, bMuiltView))
	{
		dx9 = 0;
	}

	return dx9;
}


