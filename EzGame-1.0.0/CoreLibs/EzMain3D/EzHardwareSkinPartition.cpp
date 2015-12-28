#include "../EzCore/EzMemoryDefines.h"
#include "EzHardwareSkinPartition.h"
#include "EzSkinInstance.h"

EzImplementRTTI(EzHardwareSkinPartition, EzObject);

EzHardwareSkinPartition::Partition::Partition()
: m_pusBones(NULL), m_pfWeights(NULL), m_pusVertexMap(NULL), m_pucBonePalette(NULL), m_pusTriList(NULL), m_pusStripLengths(NULL),
m_usVertexCount(0), m_usTriangleCount(0), m_usBoneCount(0), m_usStripCount(0), m_usWeightsPerVertexCount(0), m_pRendererData(NULL), 
 m_pfBoneMatrices(NULL), m_uiNumMatrices(0), m_uiBoneMatricesAllocatedSize(0)
{

}

EzHardwareSkinPartition::Partition::~Partition()
{
	EzFree(m_pusBones);
	EzFree(m_pfWeights);
	EzFree(m_pusVertexMap);
	EzFree(m_pucBonePalette);
	EzFree(m_pusTriList);
	EzFree(m_pusStripLengths);

	EzAlignedFree(m_pfBoneMatrices);

}

void EzHardwareSkinPartition::calculateBoneMatrices(const unsigned int& uiIndex, EzSkinInstance* pSkinInstance)
{
	EZASSERT(uiIndex < m_uiPartitionCount);

	unsigned short usBoneCount = m_pPartitions[uiIndex].m_usBoneCount;
	unsigned short usBoneMatrixRegisters = m_pPartitions[uiIndex].m_usWeightsPerVertexCount;
	float* pfBoneMatrices =  m_pPartitions[uiIndex].m_pfBoneMatrices;

	unsigned int uiNeededSize = usBoneCount * usBoneMatrixRegisters * 4 * 4;

	if(uiNeededSize > m_pPartitions[uiIndex].m_uiBoneMatricesAllocatedSize)
	{
		EzAlignedFree(pfBoneMatrices);

		pfBoneMatrices = EzAlignedAlloc(float, usBoneCount * usBoneMatrixRegisters * 4, 16);
		m_pPartitions[uiIndex].m_uiBoneMatricesAllocatedSize = uiNeededSize;
		m_pPartitions[uiIndex].m_uiNumMatrices = usBoneCount;
		m_pPartitions[uiIndex].m_pfBoneMatrices = pfBoneMatrices;

	}

	EzTransform WorldToSkin, WorldFromSkinFromWorld;

	pSkinInstance->getWorldToSkinTransform(WorldToSkin);

	unsigned short i;

	EzAVObject*const* ppBones = pSkinInstance->getBones();
	EzSkinData* pSkinData =pSkinInstance->getSkinData();
	const EzSkinData::BoneData* pBoneData = pSkinData->getBoneData();
	unsigned char uBoneIndex;


	for(i = 0; i < usBoneCount; ++i)
	{
		EzTransform XForm;

		uBoneIndex = m_pPartitions[uiIndex].m_pucBonePalette[i];

		XForm = WorldToSkin * ppBones[uBoneIndex]->getWorldTransform() * pBoneData[uBoneIndex].m_skinToBone;

		unsigned int uiIndex = 4 * 3 * i;

		pfBoneMatrices[uiIndex + 0] = XForm.getToMatrixRotation()[0] * XForm.getScale();
		pfBoneMatrices[uiIndex + 1] = XForm.getToMatrixRotation()[3] * XForm.getScale();
		pfBoneMatrices[uiIndex + 2] = XForm.getToMatrixRotation()[6] * XForm.getScale();
		pfBoneMatrices[uiIndex + 4] = XForm.getToMatrixRotation()[1] * XForm.getScale();
		pfBoneMatrices[uiIndex + 5] = XForm.getToMatrixRotation()[4] * XForm.getScale();
		pfBoneMatrices[uiIndex + 6] = XForm.getToMatrixRotation()[7] * XForm.getScale();
		pfBoneMatrices[uiIndex + 8] = XForm.getToMatrixRotation()[2] * XForm.getScale();
		pfBoneMatrices[uiIndex + 9] = XForm.getToMatrixRotation()[5] * XForm.getScale();
		pfBoneMatrices[uiIndex +10] = XForm.getToMatrixRotation()[8] * XForm.getScale();
		pfBoneMatrices[uiIndex + 3] = XForm.getTranslation().X;
		pfBoneMatrices[uiIndex + 7] = XForm.getTranslation().Y;
		pfBoneMatrices[uiIndex + 11] = XForm.getTranslation().Z;

	}

}

