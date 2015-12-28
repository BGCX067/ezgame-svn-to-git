#pragma once

#include "EzTriBasedGeomData.h"

EzSmartPointer(EzTriStripsData);

class EzTriStripsData : public EzTriBasedGeomData
{

	EzDeclareRTTI;

public:
	EzTriStripsData(EzPoint3* pVertices, EzPoint3* pNormal, EzPoint2* pTCoords, EzColor* pColor,
		unsigned short usVertexSize, unsigned short usTrianglesCount, unsigned short usStripCount, unsigned short* pusStripLengths, unsigned short* pusStripLists);
	~EzTriStripsData(void);

	virtual void getIndexData(unsigned short* pIdxBuffer);
	
    inline unsigned short* getStripLengths() const
	{
		return m_pusStripLengths;
	}
    
	inline unsigned short* getStripLists() const
	{
		return m_pusStripLists;
	}

	inline unsigned short getStripCount() const
	{
		return m_usStripCount;
	}

	const unsigned short getStripLengthSum() const
	{
		return m_usNumTrianglePoints + 2 * m_usStripCount;
	}

	virtual EzTriangle16 getTriangle(unsigned int uiIdx);



protected:




	/*! Number of triangles. */
	unsigned short m_usStripCount;
	/*! The number of points in each triangle strip. */
	mutable unsigned short* m_pusStripLengths;
	unsigned short* m_pusStripLists;
};


inline EzTriangle16 EzTriStripsData::getTriangle(unsigned int uiIdx)
{
	unsigned short usTriangles;
	unsigned short usStrip = 0;
	EzTriangle16 ret;

	// const function so we need a local copy.
	unsigned short* pusStripLists = m_pusStripLists;

	while (uiIdx >= (usTriangles = m_pusStripLengths[usStrip] - 2))
	{
		uiIdx -= usTriangles;
		pusStripLists += m_pusStripLengths[usStrip++];
	}

	if (uiIdx & 1)
	{
		ret.v1 = pusStripLists[uiIdx + 1];
		ret.v2 = pusStripLists[uiIdx];
	}
	else
	{
		ret.v1 = pusStripLists[uiIdx];
		ret.v2 = pusStripLists[uiIdx + 1];
	}	

	ret.v3 = pusStripLists[uiIdx + 2];

	return ret;
}
