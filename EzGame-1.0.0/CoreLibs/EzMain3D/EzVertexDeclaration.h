#ifndef __EZVERTEXDECLARATION_H_INCLUDE__
#define __EZVERTEXDECLARATION_H_INCLUDE__

#include "EzVertexElement.h"
#include "../EzCore/EzArray.h"
#include "../EzCore/EzMap.h"
#include "../EzCore/EzRefObject.h"



class EzVertexDeclaration : public EzRefObject
{
	EzDeclareRootRTTI(EzVertexDeclaration);

public:

	typedef EzArray<EzVertexElement> EzVertexElementArray;
	typedef EzMap<unsigned short, unsigned short>			 EzVertexSourceMap;

	EzVertexDeclaration(void);
	~EzVertexDeclaration(void);

	unsigned short getMaxSource(void) const;
	unsigned int getElementCount(void);
	const EzVertexElementArray& getElements(void) const;
	const EzVertexElement* getElement(unsigned int uiElem_index);

	EzVertexSourceMap& getSourceMap();

	virtual const EzVertexElement& addElement(unsigned short usSource, unsigned int uiOffset, EzVertexElement::EVertexElementType eType,
		EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex = 0);

	virtual void removeElement(unsigned int uiElem_index);
	virtual void removeElement(EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex = 0);
	virtual void removeAllElements(void);

	virtual void modifyElement(unsigned int uiElem_index, unsigned short usSource, unsigned int uiOffset, EzVertexElement::EVertexElementType theType,
		EzVertexElement::EVertexElementSemantic semantic, u8 uIndex = 0);

	const EzVertexElement* findElementBySemantic(EzVertexElement::EVertexElementSemantic  eSemantic, unsigned short usIndex = 0);
	EzVertexElementArray findElementsBySource(unsigned short usSource);
	unsigned int getStride(unsigned short usSource);

	bool operator== (const EzVertexDeclaration& rhs) const;
	bool operator!= (const EzVertexDeclaration& rhs) const;

	void sort();

protected:
	EzVertexElementArray	m_aVertexElement;
	EzVertexSourceMap		m_mapVertexSource;
};

inline EzVertexDeclaration::EzVertexSourceMap& EzVertexDeclaration::getSourceMap()
{
	return m_mapVertexSource;
}

inline void EzVertexDeclaration::sort()
{
	m_aVertexElement.sort();
}

inline unsigned int EzVertexDeclaration::getElementCount(void) 
{ 
	return m_aVertexElement.size(); 
}

inline const EzVertexDeclaration::EzVertexElementArray& EzVertexDeclaration::getElements(void) const
{
	return m_aVertexElement;
}

inline const EzVertexElement* EzVertexDeclaration::getElement(unsigned int uiElem_index)
{
	EZASSERT(uiElem_index < m_aVertexElement.size());

	return &m_aVertexElement[uiElem_index];

}

inline bool EzVertexDeclaration::operator==(const EzVertexDeclaration& rhs) const
{
	if (m_aVertexElement.size() != rhs.m_aVertexElement.size())
		return false;

	unsigned int uiMaxCount = m_aVertexElement.size();

	for(unsigned int uiIdx = 0; uiIdx < uiMaxCount; ++uiIdx)
	{
		if(!(m_aVertexElement[uiIdx] == rhs.m_aVertexElement[uiIdx]))
		{
			return false;
		}
	}

	return true;
}
inline bool EzVertexDeclaration::operator!=(const EzVertexDeclaration& rhs) const
{
	return !(*this == rhs);
}


#endif
