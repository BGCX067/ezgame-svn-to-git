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

	EzIndexBuffer(EIndexType eIndexType, u32 uiNumIndexes, Ez3DBuffer::EUsage eUsage, bool bUseSystemMemory);
	~EzIndexBuffer(void);

	const EIndexType& getType(void) const;
	const u32& getNumIndexes(void) const;
	const u32& getIndexSize(void) const;

protected:
	virtual void* lockImpl(u32 uiOffset, u32 uiLength, ELockOptions eOptions)
	{
		return NULL;
	}
	virtual void unlockImpl(void)
	{

	}

protected:
	EIndexType m_eIndexType;
	u32 m_uiNumIndexes;
	u32 m_uiIndexSize;


};



inline const EzIndexBuffer::EIndexType& EzIndexBuffer::getType(void) const 
{ 
	return m_eIndexType; 
}

inline const u32& EzIndexBuffer::getNumIndexes(void) const 
{
	return m_uiNumIndexes; 
}

inline const u32& EzIndexBuffer::getIndexSize(void) const 
{ 
	return m_uiIndexSize; 
}


