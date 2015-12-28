#pragma once

#include "../EzIndexBuffer.h"

#include <d3d9.h>
#include <d3dx9.h>


EzSmartPointer(EzD3DX9IndexBuffer);

class EzD3DX9IndexBuffer : public EzIndexBuffer
{
	EzDeclareRTTI;

public:
	EzD3DX9IndexBuffer(EzIndexBuffer::EIndexType eIndexType, unsigned int uiNumIndexes, Ez3DBuffer::EUsage eUsage, LPDIRECT3DDEVICE9 pDev, bool bUseSystemMemory);
	~EzD3DX9IndexBuffer(void);

	
	LPDIRECT3DINDEXBUFFER9 getD3DIndexBuffer(void);

protected:
	LPDIRECT3DINDEXBUFFER9 m_lpD3DBuffer;
	D3DPOOL m_D3DPool;
	
	virtual void* lockImpl(unsigned int uiOffset, unsigned int uiLength, Ez3DBuffer::ELockOptions eOptions);
	virtual void unlockImpl(void);
};

inline LPDIRECT3DINDEXBUFFER9 EzD3DX9IndexBuffer::getD3DIndexBuffer(void) 
{ 
	return m_lpD3DBuffer; 
}

