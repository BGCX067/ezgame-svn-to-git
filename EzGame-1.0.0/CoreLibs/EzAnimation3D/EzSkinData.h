#pragma once

#include "../EzMain3D/EzObject.h"
#include "EzSkinPartition.h"


namespace GameEasy
{


class EzSkinData : public EzObject
{
	EzDeclareRTTI;

public:
	class SkinWeight : public EzObject
	{
	public:

		SkinWeight() : m_usIdx(0), m_fWeight(0.0f) {}
	
		u16 m_usIdx;
		f32 m_fWeight;
	};

	/*! Skinning data component. */
	class SkinData : public EzObject
	{
	public:
		SkinData(){}

		EzTransform m_skinToBone;
		SkinWeight* m_pVertexWeights;
		u16			m_usVertexWeightCount;

	};

	EzSkinData(u16 nBoneCount, SkinData* pBoneData, const EzTransform& OverallTransform);
	~EzSkinData(void);

	const SkinData* getBoneData() 
	{ 
		return m_pBoneData; 
	}
	const u16& getBoneCount() const
	{ 
		return  m_uBoneCount;
	} 

    const EzTransform& getOverallTransform() const
	{
		return m_OverallTransform;
		
	}


protected:
	/*! This optionally links a SSkinPartition for hardware-acceleration information. */
	EzSkinPartition* m_pSkinPartition;

	SkinData*	m_pBoneData;
	u16			m_uBoneCount;

	EzTransform	m_OverallTransform;
};

}


