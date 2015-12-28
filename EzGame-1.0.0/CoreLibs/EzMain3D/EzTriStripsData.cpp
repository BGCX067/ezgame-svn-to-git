#include "EzTriStripsData.h"

EzImplementRTTI(EzTriStripsData, EzTriBasedGeomData);


EzTriStripsData::EzTriStripsData(EzPoint3* pVertices, EzPoint3* pNormal, EzPoint2* pTCoords, EzColor* pColor,
		unsigned short usVertexSize, unsigned short usTrianglesCount, unsigned short usStripCount, unsigned short* pusStripLengths, unsigned short* pusStripLists)
:EzTriBasedGeomData(EPT_TRIANGLE_STRIP, pVertices, pNormal, pTCoords, pColor, usVertexSize, usTrianglesCount * 3)
{
	m_usStripCount = usStripCount;
    m_pusStripLengths = pusStripLengths;
    m_pusStripLists = pusStripLists;
}

EzTriStripsData::~EzTriStripsData(void)
{
}

void EzTriStripsData::getIndexData(unsigned short* pIdxBuffer)
{
	for(unsigned int n = 0 ; n < getStripLengthSum(); n++)
	{
		pIdxBuffer[n] = getStripLists()[n];
	}
}

