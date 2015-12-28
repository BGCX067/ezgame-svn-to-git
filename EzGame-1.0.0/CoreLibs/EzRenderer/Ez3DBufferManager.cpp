#include "Ez3DBufferManager.h"
#include "EzVertexBufferBinding.h"
#include "../EzMain3D/EzGeometryData.h"


template<> 
Ez3DBufferManager* EzSingleton<Ez3DBufferManager>::ms_pSingleton = 0;

Ez3DBufferManager::Ez3DBufferManager(void)
{
}

Ez3DBufferManager::~Ez3DBufferManager(void)
{
}

EzVertexBufferBinding* Ez3DBufferManager::createVertexBufferBinding()
{
	return EzNew EzVertexBufferBinding;

}

EzVertexDeclaration* Ez3DBufferManager::generateVertexStream(EzGeometryData* pGemData, EzHardwareSkinPartition::Partition* pPartition, 
  u32& uiPosOffset, u32& uiWeightOffset, u32& uiBoneIndexOffset, u32& uiNormOffset,
  u32& uiColorOffset, u32& uiTexOffset)
{

	u16 usStreamCount = 0;
	u32 uiOffset = 0;
	EzVertexDeclaration* pVertexDec;

	uiPosOffset = UINT_MAX;
	uiWeightOffset= UINT_MAX;
	uiBoneIndexOffset= UINT_MAX;
	uiNormOffset= UINT_MAX;
	uiColorOffset= UINT_MAX;
	uiTexOffset= UINT_MAX;




	pVertexDec = createVertexDeclaration();

	if(pGemData->getVertices())
	{
		pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_FLOAT3, EzVertexElement::EVES_POSITION);
		uiPosOffset = uiOffset;
		uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT3);

		if(pPartition)
		{
			switch(pPartition->m_usWeightsPerVertexCount)
			{
			case 1:
				pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_FLOAT1, EzVertexElement::EVES_BLEND_WEIGHTS);
				uiWeightOffset = uiOffset;
				uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT1);

				pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_FLOAT1, EzVertexElement::EVES_BLEND_INDICES);
				uiBoneIndexOffset = uiOffset;
				uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT1);
				break;

			case 2:
				pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_FLOAT2, EzVertexElement::EVES_BLEND_WEIGHTS);
				uiWeightOffset = uiOffset;
				uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT2);

				pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_FLOAT2, EzVertexElement::EVES_BLEND_INDICES);
				uiBoneIndexOffset = uiOffset;
				uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT2);
				break;

			case 3:
				pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_FLOAT3, EzVertexElement::EVES_BLEND_WEIGHTS);
				uiWeightOffset = uiOffset;
				uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT3);

				pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_FLOAT3, EzVertexElement::EVES_BLEND_INDICES);
				uiBoneIndexOffset = uiOffset;
				uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT3);
				break;

			case 4:
				pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_FLOAT4, EzVertexElement::EVES_BLEND_WEIGHTS);
				uiWeightOffset = uiOffset;
				uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT4);

				pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_FLOAT4, EzVertexElement::EVES_BLEND_INDICES);
				uiBoneIndexOffset = uiOffset;
				uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT4);
				break;
			}
		}
	}

	if(pGemData->getVertexNormals())
	{
		pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_FLOAT3, EzVertexElement::EVES_NORMAL);
		uiNormOffset = uiOffset;
		uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT3);
	}

	uiOffset = 0;
	usStreamCount++;

	if(pGemData->HasVertexColors())
	{
		pVertexDec->addElement(usStreamCount, uiOffset, EzVertexElement::EVET_COLOUR, EzVertexElement::EVES_DIFFUSE);
		uiColorOffset = uiOffset;
		uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_COLOUR);
	}

	usStreamCount++;
	uiOffset = 0;


	if(pGemData->getTexCoords())
	{
		uiTexOffset = uiOffset;

		for(u8 n = 0 ; n < pGemData->getNumUVSets(); ++n)
		{
			pVertexDec->addElement(usStreamCount + n, uiOffset, EzVertexElement::EVET_FLOAT2, EzVertexElement::EVES_TEXTURE_COORDINATES, n);
			uiOffset += EzVertexElement::getTypeSize(EzVertexElement::EVET_FLOAT2);
		}
	}

	pVertexDec->sort();

	return pVertexDec;

}
