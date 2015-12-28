#include "EzVertexBufferBinding.h"
#include "EzVertexBuffer.h"



EzVertexBufferBinding::EzVertexBufferBinding(void)
{
}

EzVertexBufferBinding::~EzVertexBufferBinding(void)
{
	unsetAllBindings();
}


void EzVertexBufferBinding::setBinding(u16 uiIndex, EzVertexBuffer* pVertexBuffer)
{
	pVertexBuffer->setSourceNum(uiIndex);
	m_aVertexBinding.push_back(pVertexBuffer);
}

void EzVertexBufferBinding::unsetBinding(u16 uiIndex)
{
	for(u32 n = 0; n < m_aVertexBinding.size(); ++n)
	{
		if(m_aVertexBinding[n]->getSourceNum() == uiIndex)
		{
			m_aVertexBinding.erase(uiIndex);
			return;
		}
	}
}

void EzVertexBufferBinding::unsetAllBindings()
{
    m_aVertexBinding.clear();
}

const EzVertexBufferBinding::EzVertexBufferBindingArray& EzVertexBufferBinding::getBindings(void)
{
	return m_aVertexBinding;
}


EzVertexBuffer* EzVertexBufferBinding::getBuffer(u16 uiIndex)
{
	for(u32 n = 0; n < m_aVertexBinding.size(); ++n)
	{
		if(m_aVertexBinding[n]->getSourceNum() == uiIndex)
		{
			return m_aVertexBinding[n];
		}
	}

	return NULL;
}

