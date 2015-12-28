#include "EzIndexBuffer.h"


EzImplementRTTI(EzIndexBuffer, Ez3DBuffer);

EzIndexBuffer::EzIndexBuffer(EIndexType eIndexType, unsigned int uiNumIndexes, Ez3DBuffer::EUsage eUsage, bool bUseSystemMemory)
: Ez3DBuffer(eUsage, bUseSystemMemory), m_eIndexType(eIndexType), m_uiNumIndexes(uiNumIndexes)
{
	switch (m_eIndexType)
	{
	case EIT_16BIT:
		m_uiIndexSize = sizeof(unsigned short);
		break;
	case EIT_32BIT:
		m_uiIndexSize = sizeof(unsigned int);
		break;
	
	}

	m_uiSizeInBytes = m_uiIndexSize * m_uiNumIndexes;
}


EzIndexBuffer::~EzIndexBuffer(void)
{
}
