#pragma once
#include "Ez3DBuffer.h"


EzSmartPointer(EzIndexBuffer);

class EzIndexBuffer : public Ez3DBuffer
{
	EzDeclareRTTI;

public:
	enum EIndexType 
	{
		EIT_16BIT,
		EIT_32BIT
	};

public:

	EzIndexBuffer(EIndexType eIndexType, unsigned int uiNumIndexes, Ez3DBuffer::EUsage eUsage, bool bUseSystemMemory);
	~EzIndexBuffer(void);

	const EIndexType& getType(void) const;
	const unsigned int& getNumIndexes(void) const;
	const unsigned int& getIndexSize(void) const;

protected:
	virtual void* lockImpl(unsigned int uiOffset, unsigned int uiLength, ELockOptions eOptions)
	{
		return NULL;
	}
	virtual void unlockImpl(void)
	{

	}

protected:
	EIndexType m_eIndexType;
	unsigned int m_uiNumIndexes;
	unsigned int m_uiIndexSize;


};



inline const EzIndexBuffer::EIndexType& EzIndexBuffer::getType(void) const 
{ 
	return m_eIndexType; 
}

inline const unsigned int& EzIndexBuffer::getNumIndexes(void) const 
{
	return m_uiNumIndexes; 
}

inline const unsigned int& EzIndexBuffer::getIndexSize(void) const 
{ 
	return m_uiIndexSize; 
}


