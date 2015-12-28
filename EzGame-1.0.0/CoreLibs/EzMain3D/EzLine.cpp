#include "EzRenderer.h"

#include "EzLine.h"
#include "EzLineData.h"



EzImplementRTTI(EzLine, EzGeometry);
EzImplementClon(EzLine);

EzLine::EzLine(void)
{

}

EzLine::EzLine(EzPoint3* pVertices, unsigned short uiVertexCount, EzColor* pColor, bool* pbConnectFlag)
{

	//EzLineData* pNeri3DLineData = new EzLineData;

	//pNeri3DLineData->setVertices(pVertices, uiVertexCount);
	//pNeri3DLineData->setVertexColor(pColor);
	//pNeri3DLineData->setConnectFlag(pbConnectFlag);

	//setData(pNeri3DLineData);
}

EzLine::~EzLine(void)
{
}

void EzLine::copyMember(EzLine* pDest)
{
	EzCopyMember(EzGeometry);

}


void EzLine::render(EzVideoDriver* pDriver)
{


//	m_pGeometryData->render(pDriver, m_pSkinInstance);

}

