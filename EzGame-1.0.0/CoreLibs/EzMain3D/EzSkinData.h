#pragma once

#include "EzObject.h"
#include "../EzCore/EzCore.h"


EzSmartPointer(EzSkinData);

class EzSkinData : public EzObject
{
	EzDeclareRTTI;

public:
	class SkinWeight : public EzObject
	{
	public:

		SkinWeight() : m_usIdx(0), m_fWeight(0.0f) {}
	
		unsigned short m_usIdx;
		float m_fWeight;
	};

	/*! Skinning data component. */
	class BoneData : public EzObject
	{
	public:
		BoneData(){}

		EzTransform m_skinToBone;
		SkinWeight* m_pVertexWeights;
		unsigned short			m_usVertexWeightCount;

	};

	EzSkinData(unsigned short nBoneCount, BoneData* pBoneData, const EzTransform& OverallTransform);
	~EzSkinData(void);

	BoneData* getBoneData() const
	{ 
		return m_pBoneData; 
	}
	const unsigned short& getBoneCount() const
	{ 
		return  m_uBoneCount;
	} 

    const EzTransform& getOverallTransform() const
	{
		return m_OverallTransform;
		
	}


protected:
	BoneData*	m_pBoneData;
	unsigned short			m_uBoneCount;

	EzTransform	m_OverallTransform;
};



