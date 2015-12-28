#include "EzTriShapeData.h"
#include "EzStream.h"


EzImplementRTTI(EzTriShapeData, EzTriBasedGeomData);
EzImplementCreateObject(EzTriShapeData);

EzTriShapeData::EzTriShapeData(EzPoint3* pVertices, EzTriangle16* pTriangles, EzPoint3* pNormal, EzPoint2* pTCoords, EzColor* pColor,
		unsigned short usVertexSize, unsigned short usNumTriSize)
: EzTriBasedGeomData(EPT_TRIANGLE_LIST, pVertices, pNormal, pTCoords, pColor, usVertexSize, usNumTriSize), m_pTriangles(pTriangles)
{
}

EzTriShapeData::~EzTriShapeData(void)
{
	if(m_pTriangles)
	{
		EzDelete [] m_pTriangles;
	}
}

EzTriShapeData::EzTriShapeData(void)
: EzTriBasedGeomData(EPT_TRIANGLE_LIST, NULL, NULL, NULL, NULL, 0, 0), m_pTriangles(NULL)
{

}

void EzTriShapeData::loadBinary(EzStream& Stream)
{
	EzTriBasedGeomData::loadBinary(Stream);

	EzStreamLoad<unsigned short>(Stream, m_usNumTrianglePoints);

	if(m_usNumTrianglePoints > 0)
	{
		m_pTriangles = EzNew EzTriangle16[m_usNumTrianglePoints];

		for(unsigned short us = 0 ; us < m_usNumTrianglePoints; us++)
		{
			EzStreamLoad<unsigned short>(Stream, m_pTriangles[us].v1);
			EzStreamLoad<unsigned short>(Stream, m_pTriangles[us].v2);
			EzStreamLoad<unsigned short>(Stream, m_pTriangles[us].v3);
		}
	}

}

void EzTriShapeData::saveBinary(EzStream& Stream)
{
	EzTriBasedGeomData::saveBinary(Stream);

	EzStreamWrite<unsigned short>(Stream, m_usNumTrianglePoints);

	for(unsigned short us = 0 ; us < m_usNumTrianglePoints; us++)
	{
		EzStreamWrite<unsigned short>(Stream, m_pTriangles[us].v1);
		EzStreamWrite<unsigned short>(Stream, m_pTriangles[us].v2);
		EzStreamWrite<unsigned short>(Stream, m_pTriangles[us].v3);
	}
}


void EzTriShapeData::getIndexData(unsigned short* pIdxBuffer)
{
	unsigned short usIdxCount = 0;

	for(unsigned short us = 0 ; us < m_usNumTrianglePoints; us++)
	{
		pIdxBuffer[usIdxCount++] = m_pTriangles[us].v1;
		pIdxBuffer[usIdxCount++] = m_pTriangles[us].v2;
		pIdxBuffer[usIdxCount++] = m_pTriangles[us].v3;
	}
}

