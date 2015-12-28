#include "EzTriShape.h"
#include "EzNode.h"
#include "EzRenderer.h"


EzImplementRTTI(EzTriShape, EzTriBasedGeom);
EzImplementClon(EzTriShape);
EzImplementCreateObject(EzTriShape);

EzTriShape::EzTriShape(void)
{
}

EzTriShape::~EzTriShape(void)
{
}

void EzTriShape::copyMember(EzTriShape* pDest)
{
	EzCopyMember(EzTriBasedGeom);

}

void EzTriShape::loadBinary(EzStream& Stream)
{
	EzTriBasedGeom::loadBinary(Stream);

}

void EzTriShape::saveBinary(EzStream& Stream)
{
	EzTriBasedGeom::saveBinary(Stream);
}

void EzTriShape::render(EzRenderer* pRenderer)
{
	EzGeometry::render(pRenderer);
	pRenderer->renderShape(this);
}

