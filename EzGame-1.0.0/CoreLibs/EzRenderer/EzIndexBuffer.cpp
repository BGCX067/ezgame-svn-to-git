#include "EzIndexBuffer.h"


EzImplementRTTI(EzIndexBuffer, Ez3DBuffer);

EzIndexBuffer::EzIndexBuffer(EIndexType eIndexType, u32 uiNumIndexes, Ez3DBuffer::EUsage eUsage, bool bUseSystemMemory)
: Ez3DBuffer(eUsage, bUseSystemMemory), m_eIndexType(eIndexType), m_uiNumIndexes(uiNumIndexes)
{
	switch (m_eIndexType)
	{
	case EIT_16BIT:
		m_uiIndexSize = sizeof(u16);
		break;
	case EIT_32BIT:
		m_uiIndexSize = sizeof(u32);
		break;
	
	}

	m_uiSizeInBytes = m_uiIndexSize * m_uiNumIndexes;
}


EzIndexBuffer::~EzIndexBuffer(void)
{
}
