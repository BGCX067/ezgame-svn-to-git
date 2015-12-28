#include "EzTriBasedGeom.h"


EzImplementRTTI(EzTriBasedGeom, EzGeometry);

EzTriBasedGeom::EzTriBasedGeom(void)
{
}

EzTriBasedGeom::~EzTriBasedGeom(void)
{
}

void EzTriBasedGeom::loadBinary(EzStream& Stream)
{
	EzGeometry::loadBinary(Stream);

}


void EzTriBasedGeom::saveBinary(EzStream& Stream)
{
	EzGeometry::saveBinary(Stream);

}

void EzTriBasedGeom::copyMember(EzTriBasedGeom* pDest)
{
	EzCopyMember(EzGeometry);
}


