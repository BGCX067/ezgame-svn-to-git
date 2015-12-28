#include "EzD3DX9VertexBuffer.h"
#include "EzD3DX9Mappings.h"


EzImplementRTTI(EzD3DX9VertexBuffer, EzVertexBuffer);

EzD3DX9VertexBuffer::EzD3DX9VertexBuffer(unsigned int uiVertexSize, unsigned int uiNumVertices, Ez3DBuffer::EUsage eUsage, LPDIRECT3DDEVICE9 pDev, bool bUseSystemMemory)
:EzVertexBuffer(uiVertexSize, uiNumVertices, eUsage, bUseSystemMemory)
{
	m_D3DPool = bUseSystemMemory ? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT;

	HRESULT hr = pDev->CreateVertexBuffer(
		static_cast<UINT>(m_uiSizeInBytes), 
		EzD3DX9Mappings::get(eUsage), 
		0, 
		m_D3DPool,
		&m_lpD3DBuffer,
		NULL);
	if (FAILED(hr))
	{
	//	String msg = DXGetErrorDescription9(hr);
	}
}

EzD3DX9VertexBuffer::~EzD3DX9VertexBuffer(void)
{
}


void* EzD3DX9VertexBuffer::lockImpl(unsigned int uiOffset, unsigned int uiLength, Ez3DBuffer::ELockOptions eOptions)
{
	void* pBuf;
	HRESULT hr = m_lpD3DBuffer->Lock(static_cast<UINT>(uiOffset), static_cast<UINT>(uiLength), &pBuf, EzD3DX9Mappings::get(eOptions, m_eUsage));

	if (FAILED(hr))
	{
	//	String msg = DXGetErrorDescription9(hr);
	}

	return pBuf;
}

void EzD3DX9VertexBuffer::unlockImpl(void)
{
	HRESULT hr = m_lpD3DBuffer->Unlock();
}
