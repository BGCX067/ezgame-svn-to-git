#include "EzD3DX9BufferManager.h"
#include "EzD3DX9VertexDeclaration.h"
#include "EzD3DX9VertexBuffer.h"
#include "EzD3DX9IndexBuffer.h"
#include "EzD3DX9VertexDeclaration.h"

#include "../../EzMain3D/EzGeometryData.h"
#include "../../EzMain3D/EzSkinInstance.h"


EzD3DX9BufferManager::EzD3DX9BufferManager(IDirect3DDevice9* pDevice)
:m_pID3DDevice(pDevice)
{
}

EzD3DX9BufferManager::~EzD3DX9BufferManager(void)
{
}

EzVertexBuffer* EzD3DX9BufferManager::createVertexBuffer(unsigned int uiStride, unsigned int uiNumVerts, Ez3DBuffer::EUsage eUsage, bool bUseSystemMemory)
{
	return (EzVertexBuffer*)EzNew EzD3DX9VertexBuffer(uiStride, uiNumVerts, eUsage, m_pID3DDevice, bUseSystemMemory);
}

EzIndexBuffer* EzD3DX9BufferManager::createIndexBuffer(EzIndexBuffer::EIndexType eType, unsigned int uiNumIndexes, Ez3DBuffer::EUsage eUsage,  bool bUseSystemMemory)
{
	return (EzIndexBuffer*)EzNew EzD3DX9IndexBuffer(eType, uiNumIndexes, eUsage, m_pID3DDevice, bUseSystemMemory);
}

EzVertexDeclaration* EzD3DX9BufferManager::createVertexDeclaration(void)
{
	return (EzVertexDeclaration*)EzNew EzD3DX9VertexDeclaration(m_pID3DDevice);
}
