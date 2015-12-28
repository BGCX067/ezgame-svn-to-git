#pragma once

#include "../EzCore/EzRTTI.h"
#include "../EzCore/EzTypes.h"
#include "../EzCore/EzDebugHelper.h"
#include "../EzCore/EzSystem.h"
#include "../EzCore/EzRefObject.h"


class Ez3DBuffer : public EzRefObject
{
	EzDeclareRootRTTI(Ez3DBuffer);

public:

	enum EUsage 
	{
		ESTATIC = 1,
		EDYNAMIC = 2,
		EWRITE_ONLY = 4,
		EDISCARDABLE = 8,
		ESTATIC_WRITE_ONLY = 5, 
		EDYNAMIC_WRITE_ONLY = 6,
		EDYNAMIC_WRITE_ONLY_DISCARDABLE = 14


	};


	enum ELockOptions
	{
		ENORMAL,
		EDISCARD,
		EREAD_ONLY,
		ENO_OVERWRITE

	};

	Ez3DBuffer(Ez3DBuffer::EUsage eUsage, 	bool bUseSystemMemory)
		: m_bIsLock(false), m_eUsage(eUsage), m_bSystemMemory(bUseSystemMemory), m_uiLockSize(0)
	{

	}
	~Ez3DBuffer(void)
	{

	}

	virtual void* lock(size_t offset, size_t length, ELockOptions eOptions);
	void* lock(ELockOptions eOptions);
	virtual void unlock(void);
	bool isLocked(void) const;

	void readData(u32 uiOffset, u32 uiLength, void* pDest);
	void writeData(u32 uiOffset, u32 uiLength, const void* pSource, bool bDiscardWholeBuffer = false);
	void copyData(Ez3DBuffer& srcBuffer, u32 uiSrcOffset, u32 uiDstOffset, u32 uiLength, bool bDiscardWholeBuffer = false);

protected:
	/// Internal implementation of lock()
	virtual void* lockImpl(u32 uiOffset, u32 uiLength, ELockOptions eOptions) = 0;
	/// Internal implementation of unlock()
	virtual void unlockImpl(void) = 0;

protected:
	bool		m_bIsLock;
	u32			m_uiSizeInBytes;
	EUsage		m_eUsage;
	u32			m_uiLockStart;
	u32			m_uiLockSize;
	bool		m_bSystemMemory;
};


inline bool Ez3DBuffer::isLocked(void) const
{
	return m_bIsLock;
}

inline void* Ez3DBuffer::lock(size_t offset, size_t length, ELockOptions eOptions)
{
	EZASSERT(!isLocked());

	void* ret;

	// Lock the real buffer if there is no shadow buffer 
	ret = lockImpl(offset, length, eOptions);
	m_bIsLock = true;
	m_uiLockStart = offset;
	m_uiLockSize = length;
	return ret;
}

inline void* Ez3DBuffer::lock(ELockOptions eOptions)
{
	return lock(0, m_uiSizeInBytes, eOptions);
}

inline void Ez3DBuffer::unlock(void)
{
	EZASSERT(isLocked());

	unlockImpl();
	m_bIsLock = false;

}

inline void Ez3DBuffer::readData(u32 uiOffset, u32 uiLength, void* pDest)
{
	void* pSrc = this->lock(uiOffset, uiLength, EREAD_ONLY);
	EzMemoryCopy(pDest, pSrc, uiLength);
	this->unlock();

}

inline void Ez3DBuffer::writeData(u32 uiOffset, u32 uiLength, const void* pSource, bool bDiscardWholeBuffer)
{
	void* pDst = lock(uiOffset, uiLength, bDiscardWholeBuffer ? EDISCARD : ENORMAL);
	EzMemoryCopy(pDst, pSource, uiLength);
	this->unlock();
}

inline void Ez3DBuffer::copyData(Ez3DBuffer& srcBuffer, u32 uiSrcOffset, u32 uiDstOffset, u32 uiLength, bool bDiscardWholeBuffer)
{
	const void *srcData = srcBuffer.lock(uiSrcOffset, uiLength, EREAD_ONLY);
	writeData(uiDstOffset, uiLength, srcData, bDiscardWholeBuffer);
	srcBuffer.unlock();
}

