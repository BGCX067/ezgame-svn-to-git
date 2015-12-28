#pragma once

#include "../EzCore/EzCore.h"
#include "EzObject.h"
#include "EzVideoDriver.h"
#include "EzBound.h"
#include "EzRendererData.h"


class EzSkinInstance;

EzSmartPointer(EzGeometryData);

class EzGeometryData : public EzObject
{
	EzDeclareRTTI;
	EzDeclareStream;

public:
	EzGeometryData(EPRIMITIVE_TYPE ePrimitiveType, EzPoint3* pVertices, EzPoint3* pNormal, EzPoint2* pTCoords, EzColor* pColor,
		unsigned int uiNumVertex);
	~EzGeometryData(void);


	virtual void getIndexData(unsigned short* pIdxBuffer) = 0;
	
	const EPRIMITIVE_TYPE& getPrimitiveType();
	const EzBound& getBound() const;
	const EzPoint3* getVertices();
	const EzPoint3* getVertexNormals();
	const EzColor* getVertexColors();
	const EzPoint2* getTexCoords();	

	void updateBound();


    void createUVSet(bool bInitialize, unsigned short usSets);

	const unsigned short& getNumUVSets() const;
	void setNumUVSets(unsigned short usNum);
	EzPoint2* getUVSet(unsigned short usSet);
	const EzPoint2* getUVSet(unsigned short usSet) const;
	void addUVSet(EzPoint2* pSet, short uvChannelID);
	short getUVSetIndex(short uvID);

	void setNumVertices(unsigned int uiNumVert);
	void setVertices(EzPoint3* pVert);
	void setNormals(EzPoint3* pNorms);
	void setVertexColors(EzColor* pColor);
	inline void setHasVertexColors(bool bHasVertexColor) { m_bHasVertexColor = bHasVertexColor; }
	inline bool HasVertexColors() { return m_bHasVertexColor; }
	

	inline unsigned short getRevisionID() const
	{
		return (m_usDirtyFlags & DIRTY_MASK);
	}
	
	inline void markAsChanged(unsigned short usFlags)
	{
		//EZASSERT(GetConsistency() != STATIC);
		m_usDirtyFlags |= (usFlags & DIRTY_MASK);
	}
	
	inline void clearRevisionID()
	{
		m_usDirtyFlags &= ~DIRTY_MASK;
	}

	const unsigned int& getNumVertex() const
	{
		return m_uiNumVertex; 
	}

	
	enum
	{
		VERTEX_MASK     = 0x0001,
		NORMAL_MASK     = 0x0002,
		COLOR_MASK      = 0x0004,
		TEXTURE_MASK    = 0x0008,
		DIRTY_MASK      = 0x0FFF
	};

	void indexSort(unsigned int*& puiIndex0, unsigned int*& puiIndex1, unsigned int uiNumVertices);
	int vertexCompare(unsigned int v0, unsigned int v1, unsigned short usVertexType);
		
protected:
	void indexHeapsort(unsigned int *puiIndex, unsigned short usVertexType, int max);
	void heapsink(unsigned int* puiIndex, int element, unsigned short usVertexType, int max);

	mutable unsigned int					m_uiNumVertex;
	mutable unsigned short					m_usNumUVSets;

	EzBound						m_Bound;
	EPRIMITIVE_TYPE				m_ePrimitiveType;

	EzPoint3*					m_pVertices;
	EzPoint3*					m_pNormals;  

	bool						m_bHasVertexColor; // spareing memory
	EzColor*					m_pColors;	   
	EzPoint2*					m_pTCoords;
	short*						m_pUVChannelID;
	unsigned short							m_usDirtyFlags;
};


inline EzGeometryData::EzGeometryData(EPRIMITIVE_TYPE ePrimitiveType, EzPoint3* pVertices, EzPoint3* pNormal, EzPoint2* pTCoords, EzColor* pColor,
									  unsigned int uiNumVertex)
: m_pVertices(pVertices), m_uiNumVertex(uiNumVertex), m_pNormals(pNormal), m_pColors(pColor), m_pTCoords(pTCoords),
  m_usDirtyFlags(0), m_ePrimitiveType(ePrimitiveType), m_usNumUVSets(0), m_bHasVertexColor(false), m_pUVChannelID(NULL)
 
{
	
}

inline void EzGeometryData::updateBound()
{
	if(m_pVertices != 0)
	{
		m_Bound.computeFromData(m_uiNumVertex, m_pVertices);
	}
}

inline EzGeometryData::~EzGeometryData(void)
{
}

inline const EzPoint3* EzGeometryData::getVertices()
{ 
	return m_pVertices;
}

inline const EzBound& EzGeometryData::getBound() const
{ 
	return m_Bound;
}

inline const EzPoint2* EzGeometryData::getTexCoords()
{
	return m_pTCoords;
}

inline const EzColor* EzGeometryData::getVertexColors()
{	
	return m_pColors;
}

inline const EzPoint3* EzGeometryData::getVertexNormals()
{
	return m_pNormals;
}

inline const EPRIMITIVE_TYPE& EzGeometryData::getPrimitiveType()
{
	return m_ePrimitiveType;
}

inline const unsigned short& EzGeometryData::getNumUVSets() const
{
	return m_usNumUVSets;
}

inline void EzGeometryData::setNumUVSets(unsigned short usNum)
{
	m_usNumUVSets = usNum;
}

inline void EzGeometryData::setVertices(EzPoint3* pVert)
{
	m_pVertices = pVert;
}

inline void EzGeometryData::setNumVertices(unsigned int uiNumVert)
{
	m_uiNumVertex = uiNumVert;
}

inline void EzGeometryData::setNormals(EzPoint3* pNorms)
{
	m_pNormals = pNorms;

}


inline void EzGeometryData::setVertexColors(EzColor* pColor)
{
	m_pColors = pColor;
}

