#include "EzVertexBuffer.h"


EzVertexBuffer::EzVertexBuffer(unsigned int uiStride, unsigned int uiNumVertices, Ez3DBuffer::EUsage eUsage, 	bool bUseSystemMemory)
:Ez3DBuffer(eUsage, bUseSystemMemory), m_uiStride(uiStride), m_uiNumVertices(uiNumVertices), m_uiSource(0)
{
	// Calculate the size of the vertices
	m_uiSizeInBytes = uiStride * uiNumVertices;
}

EzVertexBuffer::~EzVertexBuffer(void)
{
}
