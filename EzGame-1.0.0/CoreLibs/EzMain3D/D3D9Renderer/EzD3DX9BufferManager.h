#pragma once

#include "../Ez3DBufferManager.h"
#include <d3d9.h>
#include <d3dx9.h>


class EzD3DX9VertexDeclaration;


class EzD3DX9BufferManager : public Ez3DBufferManager
{
public:
	EzD3DX9BufferManager(IDirect3DDevice9* device);
	~EzD3DX9BufferManager(void);

	virtual EzVertexBuffer* createVertexBuffer(unsigned int uiStride, unsigned int uiNumVerts, Ez3DBuffer::EUsage eUsage,  bool bUseSystemMemory);
	virtual EzIndexBuffer* createIndexBuffer(EzIndexBuffer::EIndexType eType, unsigned int uiNumIndexes, Ez3DBuffer::EUsage eUsage,  bool bUseSystemMemory);
	virtual EzVertexDeclaration* createVertexDeclaration(void);
	
protected:


	IDirect3DDevice9*	m_pID3DDevice;
};

