#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#include "../../EzCore/EzCore.h"
#include "../../EzMain3D/EzHardwareSkinPartition.h"
#include "../EzVideoDriver.h"
#include "EzD3DX9BufferManager.h"
#include "EzD3DX9VertexDeclaration.h"
#include "EzD3DX9Mappings.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9shader.h>



#define  HW_BONE_LIMIT							4



class EzTriBasedGeomData;
class EzVertexData;
class EzD3DX9GPUProgramManager;
class EzD3DX9MultiViewRenderer;
class EzGPUProgram;
class EzMaterialProperty;
class EzProperty;


struct SDepthSurface 
{
	SDepthSurface() : Surface(0) {}
	~SDepthSurface()
	{
		if (Surface)
			Surface->Release();
	}

	IDirect3DSurface9*	Surface;
	dimension2di		Size;
};



#if( D3DX_SDK_VERSION < 24 )
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "dxerr9.lib")

#else
// Because Irrlicht needs to be able to start up even without installed d3d dlls, it needs to load external d3d dlls manually. examples for the dlls are:
// SDK				DLL Name		D3DX_SDK_VERSION
// -------------------------------------------------
// Summer 2004:     no dll				22
// December 2004:	no dll				23
// February 2005:	d3dx9_24.dll		24
// April 2005:		d3dx9_25.dll		25
// June 2005:		d3dx9_26.dll		26
// August 2005:		d3dx9_27.dll		27 
// October 2005,		
// December 2005:	d3dx9_28.dll		28
// February 2006:	d3dx9_29.dll		29
// April 2006:		d3dx9_30.dll		30
// August 2006:		d3dx9_31.dll		31
// December 2006:	d3dx9_32.dll		32
// February 2007:	d3dx9_33.dll		33

HRESULT WINAPI stubD3DXFilterTexture( LPDIRECT3DBASETEXTURE9 pBaseTexture, CONST PALETTEENTRY* pPalette, UINT SrcLevel, DWORD Filter );
HRESULT WINAPI stubD3DXLoadSurfaceFromMemory( LPDIRECT3DSURFACE9 pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect,
											 LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey );
HRESULT stubD3DXAssembleShader(LPCSTR pSrcData,  UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs);
HRESULT stubD3DXAssembleShaderFromFile( LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE  pInclude, DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs );
HRESULT stubD3DXCompileShader(LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, DWORD Flags,
							  LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable);
HRESULT stubD3DXCompileShaderFromFile(LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, DWORD Flags,
									  LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable);
HRESULT stubD3DXGetShaderConstantTable( const DWORD* pFunction, LPD3DXCONSTANTTABLE* ppConstantTable );
#endif


#if( D3DX_SDK_VERSION > 23 )

	typedef HRESULT (WINAPI* D3DXFilterTextureFunction)( LPDIRECT3DBASETEXTURE9 pBaseTexture, CONST PALETTEENTRY* pPalette, UINT SrcLevel, DWORD Filter );
	typedef HRESULT (WINAPI* D3DXLoadSurfaceFromMemoryFunction)( LPDIRECT3DSURFACE9 pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect,
		LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey );
	typedef HRESULT (WINAPI* D3DXAssembleShaderFunction)( LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, 
		LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs );
	typedef HRESULT (WINAPI* D3DXAssembleShaderFromFileFunction)( LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXBUFFER* ppShader,
		LPD3DXBUFFER* ppErrorMsgs );
	typedef HRESULT (WINAPI* D3DXCompileShaderFunction)( LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pFunctionName,
		LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable );
	typedef HRESULT (WINAPI* D3DXCompileShaderFromFileFunction)( LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pFunctionName,
		LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable );
	typedef HRESULT (WINAPI* D3DXGetShaderConstantTableFunction)( const DWORD* pFunction, LPD3DXCONSTANTTABLE* ppConstantTable );

#endif



class EzD3DX9Driver : public EzVideoDriver
{
	EzDeclareRTTI;
	EzDeclareFlags(u32);

public:
	enum
	{
		// indicates renderer supports hardware skinning
		HARDWARESKINNING           = 0x00000001,
	};


	EzD3DX9Driver(void);
	~EzD3DX9Driver(void);

	//draw funtion start
	/////////////////
	virtual void draw3DLine(const EzPoint3& start, const EzPoint3& end, SColor color);
	//draw funtion end
	//////////////////

	bool initDriver(const EzDimension2d<s32>& screenSize,	HWND hWnd, u32 uiBits, bool bFullScreen, bool bPureSoftware,
		bool bHighPrecisionFPU, bool bVsync, EMULTISAMPLE_TYPE eMultiSampleType, bool bMuiltView);

	virtual bool setMultiSample(EMULTISAMPLE_TYPE eMultiSampleType);

	bool setResize(const EzDimension2d<s32>& size, void* WindowId);

	virtual void setTargetWindow(void* WindowID);
	virtual bool setResize(void* WindowId, s32 iWidth, s32 iHigth);
	virtual bool createMultiRenderer(void* WindowID, s32 iWidth, s32 iHigth);
	virtual bool beginScene(bool backBuffer = true, bool zBuffer = true, SColor color = SColor(255, 0, 0, 0));
	virtual bool endScene();
	virtual void render(EzGeometry* pGeometry);
	virtual	EzVertexData* createVertexData(EzTriBasedGeomData* pGemData, EzHardwareSkinPartition::Partition* pPartition, 
		u32& uiPosOffset, u32& uiWeightOffset, u32& uiBoneIndexOffset, u32& uiNormOffset, u32& uiColorOffset, u32& uiTexOffset);



	

	virtual void drawBuffer(EzRendererData* pRendererData);
	virtual void setTransform(ETRANSFORMATION_STATE state,const EzMatrix4& mat);
	virtual void setEffectState(EzDynamicEffectState* pEffectState);
	virtual bool queryFeature(EVIDEO_DRIVER_FEATURE feature) const;


	virtual EzTexture* createDeviceDependentTexture(EzImage* surface, const c8* name);


	virtual void setTextureCreationFlag(eTEXTURE_CREATION_FLAG flag, bool enabled);
	virtual void setAmbientLight(const EzColor& color);
	virtual u32 getMaximalDynamicLightAmount() const;


	bool isHardwareSkinning();

	virtual bool setTexture(u32 eMap, EzTextureDesc* pTexDesc);
	virtual void setLighting(bool bLight);
	virtual	bool setVertexDeclaration(EzVertexDeclaration* decl);
	virtual bool setMeshBufferDataBinding(EzVertexData* pVertexData);


protected:
	void skinningDeformSW(EzGeometryData* pGemData, EzRendererData* pRendererData, EzSkinInstance* pSkinInstance);

	virtual void setDynamicLight(const u32 uindex, EzLight* pLight);
	virtual void clearDynamicLights();
	virtual void drawVertexPrimitive(EPRIMITIVE_TYPE eType, u32 vertexStart, u32 vertexCount, u32 indexStart);

	void bindGpuProgram(EzGPUProgram* prg);
	void bindGpuProgramParameters(EGPUProgramType etype, EzGpuProgramParameters* pParams);

	void prePareRanderTexture(EzGeometry* pGeometry);
	void prePareRenderPass(EzGeometry* pGeometry);


	void setProperty(EzProperty* pPropert);
	virtual void setMaterial(EzMaterialProperty* pMat);
	void setShadingMode(ESHADE_MODE eShading, bool bResetAllRenderstates = false);
	
	
private:

	void setHardwareSkinning(bool bVal);
	void setRenderStates2DMode(bool bAlpha, bool bTexture, bool bAlphaChannel);

	void checkCompatibility(D3DDEVTYPE devtype);


	UINT		m_uiAdapter;
	D3DDEVTYPE  m_devtype;


	EzMatrix4 m_Matrices[ETS_COUNT]; // matrizes of the 3d mode we need to restore when we switch back from the 2d mode.

	
	enum
    {
        COMPRESSED_TEXTURESUPPORT,
		A1R5G5G5_TEXTURESUPPORT,
		R5G6B5_TEXTURESUPPORT,
		A4R4G4B4_TEXTURESUPPORT,
		X8R8G8B8_TEXTURESUPPORT,
		A8R8G8B8_TEXTURESUPPORT,
		L8_TEXTURESUPPORT,
        NUM_TEXTURE_SUPPORT  
    };

   
	bool m_bTextureFormatSupport[NUM_TEXTURE_SUPPORT];



	enum eRENDER_MODE
	{
		ERND_NONE = 0,	// no render state has been set yet.
		ERND_2D,			// 2d drawing rendermode
		ERND_3D,			// 3d rendering mode
		ERND_STENCIL_FILL, // stencil fill mode
		ERND_SHADOW_VOLUME_ZFAIL, // stencil volume draw mode
		ERND_SHADOW_VOLUME_ZPASS // stencil volume draw mode
	};

	EzArray<SDepthSurface*> m_aDepthBuffers;

	u32								m_uiVertexBlendMatricesCount;


	const EzTexture*				m_pCurrentTexture[4];	
	bool  m_bLastTextureMipMapsAvailable[4];
	

	bool						m_bResetRenderStates; // bool to make all renderstates be reseted if set.
	eRENDER_MODE				m_eCurrentRenderMode;


	bool						m_bTransformation3DChanged;


	EVERTEX_TYPE				m_eLastVertexType;
	u32							m_uiLastSetLight;
	u16							m_uiLastVertexSourceCount;

	
	HINSTANCE					m_hD3DLibrary;
	EzDimension2d<s32>			m_CurrentDepthBufferSize;


	D3DPRESENT_PARAMETERS			m_D3DPresent;
	IDirect3D9*						m_pID3D;
	IDirect3DDevice9*				m_pID3DDevice;
	D3DCAPS9						m_D3DCaps;

	EzStringc						m_vendorName;

	RECT*							m_pSceneSourceRect;

	void*						m_WindowId;  // it is temp because need beginScene() function


	ECOLOR_FORMAT				m_eColorFormat;
	D3DFORMAT					m_D3DColorFormat;


	f32							m_fMaxLightDistance;
	EzColor						m_AmbientLight;


	EzD3DX9MultiViewRenderer*   m_pMultiViewRenderer;

	EzD3DX9BufferManager*		m_pBufferManager;
	EzD3DX9GPUProgramManager*   m_pGpuProgramManager;
};


inline void EzD3DX9Driver::setHardwareSkinning(bool bVal)
{
	setBit(bVal, HARDWARESKINNING);
}

inline bool EzD3DX9Driver::isHardwareSkinning()
{
	return getBit(HARDWARESKINNING);
}

inline bool EzD3DX9Driver::setVertexDeclaration(EzVertexDeclaration* decl)
{
	HRESULT hr;

	EzD3DX9VertexDeclaration* d3ddecl = 
		static_cast<EzD3DX9VertexDeclaration*>(decl);

	if (FAILED(hr = m_pID3DDevice->SetVertexDeclaration(d3ddecl->getD3DVertexDeclaration())))
	{
	//	error(0x0001, "Unable to set D3D9 vertex declaration");
		return false;
	}

	return true;
}



EzVideoDriver* createDirectX9Driver(const EzDimension2d<s32>& screenSize,	HWND window, u32 uiBits, bool bFullscreen, bool bStencilbuffer,
		bool bPureSoftware, bool bHighPrecisionFPU,	bool bVsync, EMULTISAMPLE_TYPE eMultiSampleType, bool bMuiltView);




