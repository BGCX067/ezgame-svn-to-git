#include "EzSkinData.h"


EzImplementRTTI(EzSkinData, EzObject);

EzSkinData::EzSkinData(unsigned short nBoneCount, BoneData* pBoneData, const EzTransform& RootParentToSkin)
:m_OverallTransform(RootParentToSkin), m_pBoneData(pBoneData), m_uBoneCount(nBoneCount)
{
}

EzSkinData::~EzSkinData(void)
{
	EzDelete m_pBoneData;
}


