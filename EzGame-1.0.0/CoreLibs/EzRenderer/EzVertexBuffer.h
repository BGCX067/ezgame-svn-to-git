#ifndef __EZVERTEXBUFFER_H_INCLUDE__
#define __EZVERTEXBUFFER_H_INCLUDE__

#include "Ez3DBuffer.h"


class EzVertexBuffer : public Ez3DBuffer
{
public:
	EzVertexBuffer(u32 uiStride, u32 uiNumVertices, Ez3DBuffer::EUsage eUsage, 	bool bUseSystemMemory);
	~EzVertexBuffer(void);


	const u32& getStride(void) const;
	const u32& getNumVertices(void) const;
	const u32& getSourceNum(void) const;
	void setSourceNum(const u16& usNum);


protected:
	u32 m_uiNumVertices;
	u32 m_uiStride;
	u32 	m_uiSource;

};

inline void EzVertexBuffer::setSourceNum(const u16& usNum)
{
	m_uiSource = usNum;
	
}


inline const u32& EzVertexBuffer::getSourceNum(void) const
{
	return m_uiSource;
}

inline const u32& EzVertexBuffer::getStride(void) const
{
	return m_uiStride;

}

inline const u32& EzVertexBuffer::getNumVertices(void) const
{
	return m_uiNumVertices;
}

#endif

