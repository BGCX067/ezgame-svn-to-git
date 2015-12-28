#include "EzD3DX9MultiViewRenderer.h"

EzD3DX9MultiViewRenderer::EzD3DX9MultiViewRenderer(IDirect3DDevice9* pID3DDevice, D3DPRESENT_PARAMETERS	PresentParameters)
: m_pID3DDevice(pID3DDevice), m_D3DPresent(PresentParameters)
{
}

EzD3DX9MultiViewRenderer::~EzD3DX9MultiViewRenderer(void)
{
}

bool EzD3DX9MultiViewRenderer::createMultiRenderer(void* WindowID, int iWidth, int iHigth)
{

	HRESULT hr;
	LPDIRECT3DSWAPCHAIN9	lpSwapChain = NULL;
	LPDIRECT3DSURFACE9		lpBackBuffer = NULL;
	D3DPRESENT_PARAMETERS	PresentParameters = m_D3DPresent; 

	PresentParameters.BackBufferWidth = iWidth; 
	PresentParameters.BackBufferHeight = iHigth; 


	hr = m_pID3DDevice->CreateAdditionalSwapChain(&PresentParameters, &lpSwapChain);

	if (FAILED(hr))
	{
		return false;
	}

	lpSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &lpBackBuffer);

	SwapChainData* pChainData = (SwapChainData*) EzMalloc(sizeof(SwapChainData));

	pChainData->lpBackBuffer = lpBackBuffer;
	pChainData->lpSwapChain = lpSwapChain;

	m_mapSwapChain.insert(WindowID, pChainData);

	return true;
}

bool EzD3DX9MultiViewRenderer::setResize(void* WindowId, int iWidth, int iHigth)
{
	EzMap<void*, SwapChainData*>::Node* pFindItem;

	pFindItem = m_mapSwapChain.find(WindowId);

	if(pFindItem)
	{
		SwapChainData* pData = pFindItem->getValue();

		HRESULT hr;
		LPDIRECT3DSWAPCHAIN9	lpSwapChain = NULL;
		LPDIRECT3DSURFACE9		lpBackBuffer = NULL;
		D3DPRESENT_PARAMETERS	PresentParameters = m_D3DPresent; 
		bool bReset = false;

		PresentParameters.BackBufferWidth = iWidth; 
		PresentParameters.BackBufferHeight = iHigth; 

		hr = m_pID3DDevice->CreateAdditionalSwapChain(&PresentParameters, &lpSwapChain);

		if (FAILED(hr))
		{
			return false;
		}

		lpSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &lpBackBuffer);

		pData->lpBackBuffer->Release();
		pData->lpSwapChain->Release();

		pData->lpBackBuffer = lpBackBuffer;
		pData->lpSwapChain = lpSwapChain;

		return true;
	}

	return false;
}

void EzD3DX9MultiViewRenderer::setTargetWindow(void* WindowId)
{
	EzMap<void*, SwapChainData*>::Node* pFindItem;

	pFindItem = m_mapSwapChain.find(WindowId);

	if(pFindItem)
	{
		SwapChainData* pData = pFindItem->getValue();

		m_hCurrentWinID = WindowId;
		m_lpCurrentBackBuffer = pData->lpBackBuffer;
		m_lpCurrentlpSwapChain = pData->lpSwapChain;	

		LPDIRECT3DSURFACE9 pBackBuffer = NULL;
		m_pID3DDevice->SetRenderTarget( 0, m_lpCurrentBackBuffer );
	}
	else
	{
		m_hCurrentWinID = 0;
		m_lpCurrentBackBuffer = NULL;
		m_lpCurrentlpSwapChain = NULL;
	}
}
