#include "EzGeometryData.h"
#include "EzSkinInstance.h"
#include "../EzCore/EzSystem.h"
#include "EzStream.h"



EzImplementRTTI(EzGeometryData, EzObject);

int floatCompare(float* pf0, float* pf1, unsigned int uiNum)
{
	for (unsigned int ui = 0; ui < uiNum; ui++)
	{
		if (pf0[ui] < pf1[ui])
			return -1;
		if (pf0[ui] > pf1[ui])
			return 1;
	}

	return 0;
}

void EzGeometryData::loadBinary(EzStream& Stream)
{
	EzObject::loadBinary(Stream);

	EzStreamLoad<unsigned int>(Stream, m_uiNumVertex);
	EzStreamLoad<unsigned short>(Stream, m_usNumUVSets);

	EzStreamLoadBound(Stream, m_Bound);

	unsigned char isVertices;
	EzStreamLoad<unsigned char>(Stream, isVertices);

	if(isVertices)
	{
		m_pVertices = EzNew  EzPoint3[m_uiNumVertex];
		EzStreamLoadArray<EzPoint3>(Stream, m_pVertices, m_uiNumVertex);
	}

	unsigned char isNomals;
	EzStreamLoad<unsigned char>(Stream, isNomals);

	if(isNomals)
	{
		m_pNormals = EzNew  EzPoint3[m_uiNumVertex];
		EzStreamLoadArray<EzPoint3>(Stream, m_pNormals, m_uiNumVertex);

	}

	unsigned char isVertexColors;
	unsigned char isVertexAllWhite;

	EzStreamLoad<unsigned char>(Stream, isVertexColors);
	EzStreamLoad<unsigned char>(Stream, isVertexAllWhite);

	setHasVertexColors(isVertexColors ? true : false);

	if(isVertexColors)
	{
		if(!isVertexAllWhite)
		{
			m_pColors = EzNew  EzColor[m_uiNumVertex];
			EzStreamLoadArray<EzColor>(Stream, m_pColors, m_uiNumVertex);
		}

	}

	if(m_usNumUVSets > 0)
	{
		unsigned int uiTotalSize  = m_usNumUVSets * m_uiNumVertex;
		m_pTCoords = EzNew EzPoint2[uiTotalSize];
		EzStreamLoadArray<EzPoint2>(Stream, m_pTCoords, uiTotalSize);
	}

}


void EzGeometryData::saveBinary(EzStream& Stream)
{
	EzObject::saveBinary(Stream);

	EzStreamWrite<unsigned int>(Stream, m_uiNumVertex);
	EzStreamWrite<unsigned short>(Stream, m_usNumUVSets);

	EzStreamWriteBound(Stream, m_Bound);


	if(m_pVertices)
	{
		EzStreamWrite<unsigned char>(Stream, 1);
		EzStreamWriteArray<EzPoint3>(Stream, m_pVertices, m_uiNumVertex);

	}
	else
	{
		EzStreamWrite<unsigned char>(Stream, 0);
	}

	if(m_pNormals)
	{
		EzStreamWrite<unsigned char>(Stream, 1);
		EzStreamWriteArray<EzPoint3>(Stream, m_pNormals, m_uiNumVertex);
	}
	else
	{
		EzStreamWrite<unsigned char>(Stream, 0);
	}

	EzStreamWrite<unsigned char>(Stream, HasVertexColors() ? 1 : 0);

	if(m_pColors)
	{
		EzStreamWrite<unsigned char>(Stream, 0);
		EzStreamWriteArray<EzColor>(Stream, m_pColors, m_uiNumVertex);
	}
	else
	{
		EzStreamWrite<unsigned char>(Stream, 1);	// All White Colors;

	}

	if(m_usNumUVSets > 0)
	{
		unsigned int uiTotalSize  = m_usNumUVSets * m_uiNumVertex;
		EzStreamWriteArray<EzPoint2>	(Stream, m_pTCoords, uiTotalSize);
	}
}


void EzGeometryData::indexSort(unsigned int*& puiIndex0, unsigned int*& puiIndex1, unsigned int uiNumVertices)
{
	puiIndex0 = EzAlloc(unsigned int, uiNumVertices);
	puiIndex1 = EzAlloc(unsigned int, uiNumVertices);

	unsigned int ui;

	for(ui = 0; ui < uiNumVertices; ++ui)
	{
		puiIndex0[ui] = ui;
	}

	indexHeapsort(puiIndex0, VERTEX_MASK, uiNumVertices);

	for(ui = 0; ui < uiNumVertices; ++ui)
	{
		puiIndex1[puiIndex0[ui]] = ui;
	}
}


void EzGeometryData::indexHeapsort(unsigned int *puiIndex, unsigned short usVertexType, int max)
{
	// for EzHeapsink we pretent this is not c++, where
	// arrays start with index 0. So we decrease the array pointer,
	// the maximum always +2 and the element always +1
	unsigned int* virtualArray = puiIndex - 1;
	int virtualSize = max + 2;
	int i;

	// build heap

	for (i=((max-1)/2); i>=0; --i)
		heapsink(virtualArray, i+1, usVertexType, virtualSize-1);

	// sort array
	for (i=max-1; i>=0; --i)
	{
		unsigned int t = puiIndex[0];
		puiIndex[0] = puiIndex[i];
		puiIndex[i] = t;
		heapsink(virtualArray, 1, usVertexType, i + 1);
	}
}


void EzGeometryData::heapsink(unsigned int* puiIndex, int element, unsigned short usVertexType, int max)
{
	while ((element<<1) < max) // there is a left child
	{
		int j = (element<<1);

		if (j+1 < max && vertexCompare(puiIndex[j], puiIndex[j+1], usVertexType) > 0)
			j = j+1; // take right child

		if (vertexCompare(puiIndex[element], puiIndex[j], usVertexType) > 0)
		{
			unsigned int t = puiIndex[j]; // swap elements
			puiIndex[j] = puiIndex[element];
			puiIndex[element] = t;
			element = j;
		}
		else
			return;
	}
}

int EzGeometryData::vertexCompare(unsigned int v0, unsigned int v1, unsigned short usVertexType)
{
	const EzPoint3* pVertics = getVertices();
	
	if(usVertexType & VERTEX_MASK)
	{
		return floatCompare((float*) &pVertics[v0], (float*) &pVertics[v1], 3);

	}
	//const EzPoint3* getVertexNormals();
	//const EzColor* getVertexColors();
	//const EzPoint2* getTexCoords();	

	//printf("11");
	return 0;

}


void EzGeometryData::createUVSet(bool bInitialize, unsigned short usSets)
{

	if(m_pTCoords && usSets != m_usNumUVSets)
	{
		EzDelete [] m_pTCoords;

		EzFree(m_pUVChannelID);

		m_pTCoords = NULL;
		m_pUVChannelID = NULL;
	}

	unsigned int uiTotalSize  = usSets * m_uiNumVertex;
	
	if(!m_pTCoords)
	{
		m_pTCoords = EzNew EzPoint2[uiTotalSize];

		m_pUVChannelID = EzAlloc(short, usSets);
		memset(m_pUVChannelID, -1, sizeof(short) * usSets);

		m_usNumUVSets = usSets;
	}

	if(bInitialize)
		memset(m_pTCoords, 0, sizeof(m_pTCoords[0]) * uiTotalSize);
}

EzPoint2* EzGeometryData::getUVSet(unsigned short usSet)
{
	if (!m_pTCoords || usSet >= m_usNumUVSets)
		return 0;

	return &m_pTCoords[m_uiNumVertex * usSet];
}

const EzPoint2* EzGeometryData::getUVSet(unsigned short usSet) const
{
	if (!m_pTCoords || usSet >= m_usNumUVSets)
		return 0;

	return &m_pTCoords[m_uiNumVertex * usSet];
}

short EzGeometryData::getUVSetIndex(short uvID)
{
	if(!m_pUVChannelID)
	{
		return -1;
	}

	for(short i = 0; i < m_usNumUVSets; ++i)
	{
		if(m_pUVChannelID[i] == uvID)
		{
			return i;
		}
	}

	return -1;
}

void EzGeometryData::addUVSet(EzPoint2* pSet, short uvChannelID)
{
	unsigned int uiIndex;
	unsigned short usNumTextureSets = m_usNumUVSets;

	if (!m_pTCoords)
	{
		m_pTCoords = EzNew EzPoint2[m_uiNumVertex];
		usNumTextureSets = 1;
		for (uiIndex = 0; uiIndex < m_uiNumVertex; uiIndex++)
			m_pTCoords[uiIndex] = pSet[uiIndex];

		m_pUVChannelID = EzAlloc(short, 1);

		*m_pUVChannelID = uvChannelID;
	}
	else
	{
		// reallocate storage for the old and new texture coordinates
		unsigned int uiBase = m_uiNumVertex * usNumTextureSets;
		EzPoint2* pTCoords = EzNew EzPoint2[uiBase + m_uiNumVertex];

		for (uiIndex = 0; uiIndex < uiBase; uiIndex++)
			pTCoords[uiIndex] = m_pTCoords[uiIndex];

		EzDelete[] m_pTCoords;
		m_pTCoords = pTCoords;

		for (uiIndex = 0; uiIndex < m_uiNumVertex; uiIndex++)
		{
			m_pTCoords[uiBase+uiIndex] = pSet[uiIndex];
		}


		short* pUVChannelID = EzAlloc(short , usNumTextureSets + 1);

		for (uiIndex = 0; uiIndex < usNumTextureSets; uiIndex++)
			pUVChannelID[uiIndex] = m_pUVChannelID[uiIndex];

		pUVChannelID[uiIndex] = uvChannelID;
		
		EzFree(m_pUVChannelID);

		m_pUVChannelID = pUVChannelID;

		usNumTextureSets++;
	}

	m_usNumUVSets = usNumTextureSets;
}
