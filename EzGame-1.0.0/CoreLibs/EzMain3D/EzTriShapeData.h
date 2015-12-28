#pragma once

#include "EzTriBasedGeomData.h"

EzSmartPointer(EzTriShapeData);

class EzTriShapeData : public EzTriBasedGeomData
{
	EzDeclareStream;
	EzDeclareRTTI;

public:
	EzTriShapeData(EzPoint3* pVertices, EzTriangle16*	pTriangles, EzPoint3* pNormal, EzPoint2* pTCoords, EzColor* pColor,
		unsigned short usVertexSize, unsigned short usNumTriSize);
	EzTriShapeData(void);
	~EzTriShapeData(void);
	

	virtual EzTriangle16 getTriangle(unsigned int uiIdx);
	virtual void setTriangle(EzTriangle16* pTri);
	virtual void getIndexData(unsigned short* pIdxBuffer); 
	virtual unsigned short getNumIndexSize();




protected:
	EzTriangle16*		m_pTriangles;
};

inline void EzTriShapeData::setTriangle(EzTriangle16* pTri)
{
	m_pTriangles = pTri;
}

inline EzTriangle16 EzTriShapeData::getTriangle(unsigned int uiIdx)
{
	return m_pTriangles[uiIdx];

}

inline unsigned short EzTriShapeData::getNumIndexSize()
{
	return m_usNumTrianglePoints * 3;
}


