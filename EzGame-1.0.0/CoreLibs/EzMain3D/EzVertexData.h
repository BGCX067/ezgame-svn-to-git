#pragma once

#include "../EzCore/EzTypes.h"
#include "../EzCore/EzEnumTypes.h"
#include "EzIndexBuffer.h"
#include "EzVertexBuffer.h"
#include "EzVertexDeclaration.h"
#include "EzVertexBufferBinding.h"


class EzGeometryData;
class EzSkinInstance;


class EzVertexData : public EzMemObject
{
public:
	EzVertexData(EzVertexDeclaration* pVertexDeclaration, EzVertexBufferBinding* pVertexBinding);
	~EzVertexData(void);
	const EPRIMITIVE_TYPE& getPrimitiveType();
	EzVertexDeclaration* getVertexDeclaration();
	EzVertexBufferBinding* getVertexBinding();
	EzVertexBuffer* getVertexBuffer(unsigned short usIdx);


protected:
	EzVertexDeclaration*	m_pVertexDeclaration;
	EzVertexBufferBinding*	m_pVertexBinding;

};



inline EzVertexDeclaration* EzVertexData::getVertexDeclaration()
{
	return m_pVertexDeclaration;
}

inline EzVertexBufferBinding* EzVertexData::getVertexBinding()
{
	return m_pVertexBinding;
}

inline EzVertexBuffer* EzVertexData::getVertexBuffer(unsigned short usIdx)
{
	EZASSERT(m_pVertexBinding);

	return m_pVertexBinding->getBuffer(usIdx);

}
