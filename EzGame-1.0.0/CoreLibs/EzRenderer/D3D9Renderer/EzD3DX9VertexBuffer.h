#pragma once

#include "../EzVertexBuffer.h"

#include <d3d9.h>
#include <d3dx9.h>


class EzD3DX9VertexBuffer : public EzVertexBuffer
{
	EzDeclareRTTI;

public:
	EzD3DX9VertexBuffer(u32 uiVertexSize, u32 uiNumVertices, Ez3DBuffer::EUsage eUsage, LPDIRECT3DDEVICE9 pDev, bool bUseSystemMemory);
	~EzD3DX9VertexBuffer(void);

	LPDIRECT3DVERTEXBUFFER9 getD3DVertexBuffer();

protected:
	void* lockImpl(size_t offset, size_t length, Ez3DBuffer::ELockOptions eOptions);
	void unlockImpl(void);

protected:
	LPDIRECT3DVERTEXBUFFER9		m_lpD3DBuffer;
	D3DPOOL						m_D3DPool;
};

inline LPDIRECT3DVERTEXBUFFER9 EzD3DX9VertexBuffer::getD3DVertexBuffer()
{
	return m_lpD3DBuffer;
}
