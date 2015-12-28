#ifndef __EZVERTEXBUFFER_H_INCLUDE__
#define __EZVERTEXBUFFER_H_INCLUDE__

#include "Ez3DBuffer.h"


class EzVertexBuffer : public Ez3DBuffer
{
public:
	EzVertexBuffer(unsigned int uiStride, unsigned int uiNumVertices, Ez3DBuffer::EUsage eUsage, 	bool bUseSystemMemory);
	~EzVertexBuffer(void);


	const unsigned int& getStride(void) const;
	const unsigned int& getNumVertices(void) const;
	const unsigned int& getSourceNum(void) const;
	void setSourceNum(const unsigned short& usNum);


protected:
	unsigned int m_uiNumVertices;
	unsigned int m_uiStride;
	unsigned int 	m_uiSource;

};

inline void EzVertexBuffer::setSourceNum(const unsigned short& usNum)
{
	m_uiSource = usNum;
	
}


inline const unsigned int& EzVertexBuffer::getSourceNum(void) const
{
	return m_uiSource;
}

inline const unsigned int& EzVertexBuffer::getStride(void) const
{
	return m_uiStride;

}

inline const unsigned int& EzVertexBuffer::getNumVertices(void) const
{
	return m_uiNumVertices;
}

#endif

