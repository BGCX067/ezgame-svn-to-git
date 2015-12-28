#ifndef __EZVERTEXDECLARATION_H_INCLUDE__
#define __EZVERTEXDECLARATION_H_INCLUDE__

#include "EzVertexElement.h"
#include "../EzCore/EzArray.h"
#include "../EzCore/EzMap.h"
#include "../EzCore/EzRTTI.h"



class EzVertexDeclaration
{
	EzDeclareRootRTTI(EzVertexDeclaration);

public:

	typedef EzArray<EzVertexElement> EzVertexElementArray;
	typedef EzMap<u16, u16>			 EzVertexSourceMap;

	EzVertexDeclaration(void);
	~EzVertexDeclaration(void);

	u16 getMaxSource(void) const;
	u32 getElementCount(void);
	const EzVertexElementArray& getElements(void) const;
	const EzVertexElement* getElement(u32 uiElem_index);

	EzVertexSourceMap& getSourceMap();

	virtual const EzVertexElement& addElement(u16 usSource, u32 uiOffset, EzVertexElement::EVertexElementType eType,
		EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex = 0);

	virtual void removeElement(u32 uiElem_index);
	virtual void removeElement(EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex = 0);
	virtual void removeAllElements(void);

	virtual void modifyElement(u32 uiElem_index, u16 usSource, u32 uiOffset, EzVertexElement::EVertexElementType theType,
		EzVertexElement::EVertexElementSemantic semantic, u8 uIndex = 0);

	const EzVertexElement* findElementBySemantic(EzVertexElement::EVertexElementSemantic  eSemantic, u16 usIndex = 0);
	EzVertexElementArray findElementsBySource(u16 usSource);
	u32 getStride(u16 usSource);

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

inline u32 EzVertexDeclaration::getElementCount(void) 
{ 
	return m_aVertexElement.size(); 
}

inline const EzVertexDeclaration::EzVertexElementArray& EzVertexDeclaration::getElements(void) const
{
	return m_aVertexElement;
}

inline const EzVertexElement* EzVertexDeclaration::getElement(u32 uiElem_index)
{
	EZASSERT(uiElem_index < m_aVertexElement.size());

	return &m_aVertexElement[uiElem_index];

}

inline bool EzVertexDeclaration::operator==(const EzVertexDeclaration& rhs) const
{
	if (m_aVertexElement.size() != rhs.m_aVertexElement.size())
		return false;

	u32 uiMaxCount = m_aVertexElement.size();

	for(u32 uiIdx = 0; uiIdx < uiMaxCount; ++uiIdx)
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
