#include "EzTexture.h"
#include "EzTriStrips.h"


EzImplementRTTI(EzTriStrips, EzTriBasedGeom);
EzImplementClon(EzTriStrips);

EzTriStrips::EzTriStrips(void)
{
}

EzTriStrips::~EzTriStrips(void)
{
}

void EzTriStrips::copyMember(EzTriStrips* pDest)
{
	EzCopyMember(EzTriBasedGeom);

}

//void EzTriStrips::render(EzVideoDriver* pDriver)
//{
//	void* pVerticsBuffer;
//	unsigned short uiVertexCount;
//
//	void* pIndexBuffer;
//	unsigned short uiIndexCount;
//
//
//	EzTriBasedGeom::preRender(pDriver, &pVerticsBuffer, uiVertexCount, &pIndexBuffer, uiIndexCount);
//}


