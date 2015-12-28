#include "EzLineData.h"


EzImplementRTTI(EzLineData, EzGeometryData);


EzLineData::EzLineData(void)
: EzGeometryData(EPT_LINE_STRIP, NULL, NULL, NULL, NULL, 0), m_pbConnectFlag(false)
{
}

EzLineData::~EzLineData(void)
{
}

void EzLineData::getIndexData(unsigned short* pIdxBuffer)
{

	//if(!m_pHWIndexBuffer)
	//{
	//	unsigned short* data;

	//	data = pDriver->updateIndexHardwareBuffer(&m_pHWIndexBuffer, m_usVertexSize);

	//	bool* pbFlags = m_pbConnectFlag;
	//	unsigned short* pusConnTmp = (unsigned short*)data;
 //       unsigned short i;
 //       unsigned short uiMax = m_usVertexSize - 1;
 //       
	//	for (i = 0; i < uiMax; i++)
 //       {
	//		if(*pbFlags)
	//		{
	//			pusConnTmp[0] = i;
	//			pusConnTmp[1] = (i+1);
	//			pusConnTmp +=2;
	//			m_usIndexSize += 2;
	//		}
	//	
	//		pbFlags++;
 //       }

	//	if (*pbFlags)
 //       {
	//		pusConnTmp[0] = (unsigned short)uiMax;
 //           pusConnTmp[1] = (unsigned short)0;
 //           pusConnTmp +=2;
	//		m_usIndexSize += 2;
	//	}
 // 

	//	pDriver->endIndexHardwareBuffer(m_pHWIndexBuffer);
	//}

	//usIndexCount = m_usIndexSize;


}

