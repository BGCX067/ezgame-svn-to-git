#pragma once


#include "EzGeometryData.h"
#include "../EzCore/EzTriangle.h"

EzSmartPointer(EzTriBasedGeomData);

class EzTriBasedGeomData : public EzGeometryData
{
	EzDeclareRTTI;
	EzDeclareStream;

public:
	~EzTriBasedGeomData(void);

	const unsigned short& getNumTrianglePoints() const;
	void setNumTrianglePoints(unsigned short usTriNum);
	virtual void getIndexData(unsigned short* pIdxBuffer) {};
	virtual void setTriangle(EzTriangle16* pTri) {};
	virtual EzTriangle16 getTriangle(unsigned int uiIdx)
	{
		EZASSERT(0);
		return EzTriangle16();
	}
	virtual unsigned short getNumIndexSize()
	{ 
		return m_usNumTrianglePoints; 
	}

protected:
	EzTriBasedGeomData(EPRIMITIVE_TYPE ePrimitiveType, EzPoint3* pVertices, EzPoint3* pNormal, EzPoint2* pTCoords, EzColor* pColor,
		unsigned short usVertexSize, unsigned short usNumTriSize);

	unsigned short m_usNumTrianglePoints;
};

inline 	void EzTriBasedGeomData::setNumTrianglePoints(unsigned short usTriNum)
{
	m_usNumTrianglePoints = usTriNum;

}

inline const unsigned short& EzTriBasedGeomData::getNumTrianglePoints() const
{ 
	return m_usNumTrianglePoints; 
}

