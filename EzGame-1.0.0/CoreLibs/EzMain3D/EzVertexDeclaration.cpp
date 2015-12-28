#include "EzVertexDeclaration.h"


EzImplementRootRTTI(EzVertexDeclaration);

EzVertexDeclaration::EzVertexDeclaration(void)
{
}

EzVertexDeclaration::~EzVertexDeclaration(void)
{
}

unsigned short EzVertexDeclaration::getMaxSource(void) const
{

	unsigned short usRet = 0;

	for(unsigned int uiIdx = 0 ; uiIdx < m_aVertexElement.size(); ++uiIdx)
	{
		if(m_aVertexElement[uiIdx].getSource() > usRet)
		{
			usRet = m_aVertexElement[uiIdx].getSource();
		}

	}

	return usRet;
}

const EzVertexElement& EzVertexDeclaration::addElement(unsigned short usSource, unsigned int uiOffset, EzVertexElement::EVertexElementType eType, 
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
	for(unsigned int uiIdx = 0 ; uiIdx < m_aVertexElement.size(); ++uiIdx)
	{
		if(m_aVertexElement[uiIdx].getSemantic() == eSemantic && m_aVertexElement[uiIdx].getIndex() == index)
		{
			return &m_aVertexElement[uiIdx];
		}

	}

	return NULL;

}

EzVertexDeclaration::EzVertexElementArray EzVertexDeclaration::findElementsBySource(unsigned short usSource)
{
	EzVertexElementArray resultArray;

	for(unsigned int uiIdx = 0 ; uiIdx < m_aVertexElement.size(); ++uiIdx)
	{
		if(m_aVertexElement[uiIdx].getSource() == usSource)
		{
			resultArray.push_back(m_aVertexElement[uiIdx]);
		}

	}

	return resultArray;
}

unsigned int EzVertexDeclaration::getStride(unsigned short usSource)
{
	unsigned int uiStride = 0;

	for(unsigned int uiIdx = 0 ; uiIdx < m_aVertexElement.size(); ++uiIdx)
	{
		if(m_aVertexElement[uiIdx].getSource() == usSource)
		{
			uiStride += m_aVertexElement[uiIdx].getSize();
		}

	}

	return uiStride;

}



void EzVertexDeclaration::modifyElement(unsigned int uiElem_index, unsigned short usSource, unsigned int uiOffset, EzVertexElement::EVertexElementType eType,
				   EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex)
{

}

void EzVertexDeclaration::removeElement(unsigned int uiElem_index)
{

}

void EzVertexDeclaration::removeElement(EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex)
{

}

void EzVertexDeclaration::removeAllElements(void)
{

}

