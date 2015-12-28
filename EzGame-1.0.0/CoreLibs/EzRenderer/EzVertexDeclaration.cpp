#include "EzVertexDeclaration.h"


EzImplementRootRTTI(EzVertexDeclaration);

EzVertexDeclaration::EzVertexDeclaration(void)
{
}

EzVertexDeclaration::~EzVertexDeclaration(void)
{
}

u16 EzVertexDeclaration::getMaxSource(void) const
{

	u16 usRet = 0;

	for(u32 uiIdx = 0 ; uiIdx < m_aVertexElement.size(); ++uiIdx)
	{
		if(m_aVertexElement[uiIdx].getSource() > usRet)
		{
			usRet = m_aVertexElement[uiIdx].getSource();
		}

	}

	return usRet;
}

const EzVertexElement& EzVertexDeclaration::addElement(u16 usSource, u32 uiOffset, EzVertexElement::EVertexElementType eType, 
	EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex)
{
	if (eType == EzVertexElement::EVET_COLOUR)
	{
		eType = EzVertexElement::getBestColourVertexElementType();
	}
	m_aVertexElement.push_back(EzVertexElement(usSource, uiOffset, eType, eSemantic, uIndex));

	return m_aVertexElement.getLast();

}

const EzVertexElement* EzVertexDeclaration::findElementBySemantic(EzVertexElement::EVertexElementSemantic  eSemantic, unsigned short index)
{
	for(u32 uiIdx = 0 ; uiIdx < m_aVertexElement.size(); ++uiIdx)
	{
		if(m_aVertexElement[uiIdx].getSemantic() == eSemantic && m_aVertexElement[uiIdx].getIndex() == index)
		{
			return &m_aVertexElement[uiIdx];
		}

	}

	return NULL;

}

EzVertexDeclaration::EzVertexElementArray EzVertexDeclaration::findElementsBySource(u16 usSource)
{
	EzVertexElementArray resultArray;

	for(u32 uiIdx = 0 ; uiIdx < m_aVertexElement.size(); ++uiIdx)
	{
		if(m_aVertexElement[uiIdx].getSource() == usSource)
		{
			resultArray.push_back(m_aVertexElement[uiIdx]);
		}

	}

	return resultArray;
}

u32 EzVertexDeclaration::getStride(u16 usSource)
{
	u32 uiStride = 0;

	for(u32 uiIdx = 0 ; uiIdx < m_aVertexElement.size(); ++uiIdx)
	{
		if(m_aVertexElement[uiIdx].getSource() == usSource)
		{
			uiStride += m_aVertexElement[uiIdx].getSize();
		}

	}

	return uiStride;

}



void EzVertexDeclaration::modifyElement(u32 uiElem_index, u16 usSource, u32 uiOffset, EzVertexElement::EVertexElementType eType,
				   EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex)
{

}

void EzVertexDeclaration::removeElement(u32 uiElem_index)
{

}

void EzVertexDeclaration::removeElement(EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex)
{

}

void EzVertexDeclaration::removeAllElements(void)
{

}

