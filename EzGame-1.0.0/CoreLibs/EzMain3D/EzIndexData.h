#pragma once


#include "EzIndexBuffer.h"


class EzIndexData : public EzMemObject
{
public:
	EzIndexData(EzIndexBuffer* pIndexBuffer);
	~EzIndexData(void);

	EzIndexBuffer* getIndexBuffer();

protected:
	EzIndexBufferPtr	m_spIndexBuffer;
	
};

inline EzIndexBuffer* EzIndexData::getIndexBuffer()
{
	return m_spIndexBuffer;
}

