#pragma once

#include "../EzCore/EzTypes.h"
#include "../EzCore/EzEnumTypes.h"
#include "../EzRenderer/EzIndexBuffer.h"
#include "../EzRenderer/EzVertexBuffer.h"
#include "../EzRenderer/EzVertexDeclaration.h"
#include "../EzRenderer/EzVertexBufferBinding.h"


class EzGeometryData;
class EzSkinInstance;


class EzVertexData
{
public:
	EzVertexData(EzVertexDeclaration* pVertexDeclaration, EzVertexBufferBinding* pVertexBinding);
	~EzVertexData(void);
	const EPRIMITIVE_TYPE& getPrimitiveType();
	EzVertexDeclaration* getVertexDeclaration();
	EzVertexBufferBinding* getVertexBinding();
	EzVertexBuffer* getVertexBuffer(u16 usIdx);


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

inline EzVertexBuffer* EzVertexData::getVertexBuffer(u16 usIdx)
{
	EZASSERT(m_pVertexBinding);

	return m_pVertexBinding->getBuffer(usIdx);

}
