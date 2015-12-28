#include "EzD3DX9IndexBuffer.h"
#include "EzD3DX9Mappings.h"
#include "../../EzCore/EzString.h"

//#include <DxErr9.h>


EzImplementRTTI(EzD3DX9IndexBuffer, EzIndexBuffer);

EzD3DX9IndexBuffer::EzD3DX9IndexBuffer(EzIndexBuffer::EIndexType eIndexType, unsigned int uiNumIndexes, Ez3DBuffer::EUsage eUsage,
 LPDIRECT3DDEVICE9 pDev, bool bUseSystemMemory)
:EzIndexBuffer(eIndexType, uiNumIndexes, eUsage, bUseSystemMemory)
{


	m_D3DPool = bUseSystemMemory? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT;


	HRESULT hr = pDev->CreateIndexBuffer(
		static_cast<UINT>(m_uiSizeInBytes),
		EzD3DX9Mappings::get(m_eUsage),
		EzD3DX9Mappings::get(m_eIndexType),
		m_D3DPool,
		&m_lpD3DBuffer,
		NULL
		);

	if (FAILED(hr))
	{
		//EzStringc msg = DXGetErrorDescription9(hr);
	}
}

EzD3DX9IndexBuffer::~EzD3DX9IndexBuffer(void)
{
}


void* EzD3DX9IndexBuffer::lockImpl(unsigned int uiOffset, unsigned int uiLength, Ez3DBuffer::ELockOptions eOptions)
{
	void* pBuf;
	HRESULT hr = m_lpD3DBuffer->Lock(static_cast<UINT>(uiOffset), static_cast<UINT>(uiLength), 	&pBuf,
		EzD3DX9Mappings::get(eOptions, m_eUsage));

	if (FAILED(hr))
	{
//		String msg = DXGetErrorDescription9(hr);
	}

	return pBuf;

}

void EzD3DX9IndexBuffer::unlockImpl(void)
{
	HRESULT hr = m_lpD3DBuffer->Unlock();

}

