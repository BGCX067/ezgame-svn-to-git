#ifndef __EZD3DX9MULTIVIEWRENDERER_H_INCLUDED__
#define __EZD3DX9MULTIVIEWRENDERER_H_INCLUDED__
#include <d3d9.h>
#include <d3dx9.h>

#include "../../EzCore/EzCore.h"
#include "../../EzCore/EzRTTI.h"
#include "../../EzCore/EzRefObject.h"


class EzD3DX9MultiViewRenderer : public EzRefObject
{
public:
	EzD3DX9MultiViewRenderer(IDirect3DDevice9* pID3DDevice, D3DPRESENT_PARAMETERS	PresentParameters);
	~EzD3DX9MultiViewRenderer(void);

	virtual bool createMultiRenderer(void* WindowID, s32 iWidth, s32 iHigth);
	virtual void setTargetWindow(void* WindowId);

	inline void Present()
	{
		m_lpCurrentlpSwapChain->Present(NULL, NULL, (HWND) m_hCurrentWinID, NULL, 0);
	}

	bool setResize(void* WindowId, s32 iWidth, s32 iHigth);

	
protected:
	struct SwapChainData
	{
		LPDIRECT3DSURFACE9		lpBackBuffer;
		LPDIRECT3DSWAPCHAIN9	lpSwapChain;	
	};


	LPDIRECT3DSURFACE9				m_lpRenderTargetBackBuffer;

	D3DPRESENT_PARAMETERS			m_D3DPresent; 
	IDirect3DDevice9*				m_pID3DDevice;

	EzMap<void*, SwapChainData*>	m_mapSwapChain;

	void*							m_hCurrentWinID;
	LPDIRECT3DSURFACE9				m_lpCurrentBackBuffer;
	LPDIRECT3DSWAPCHAIN9			m_lpCurrentlpSwapChain;	

};


#endif