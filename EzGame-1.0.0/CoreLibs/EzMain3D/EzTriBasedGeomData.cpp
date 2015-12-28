#include "EzTriBasedGeomData.h"


EzImplementRTTI(EzTriBasedGeomData, EzGeometryData);

EzTriBasedGeomData::EzTriBasedGeomData(EPRIMITIVE_TYPE ePrimitiveType, EzPoint3* pVertices, EzPoint3* pNormal, EzPoint2* pTCoords, EzColor* pColor,
									   unsigned short usVertexSize, unsigned short usNumTriSize)
:EzGeometryData(ePrimitiveType, pVertices, pNormal, pTCoords, pColor, usVertexSize), m_usNumTrianglePoints(usNumTriSize)

{
}

EzTriBasedGeomData::~EzTriBasedGeomData(void)
{
}

void EzTriBasedGeomData::loadBinary(EzStream& Stream)
{
	EzGeometryData::loadBinary(Stream);

}


void EzTriBasedGeomData::saveBinary(EzStream& Stream)
{
	EzGeometryData::saveBinary(Stream);

}
