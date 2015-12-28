#include "EzSkinData.h"

namespace GameEasy
{

EzImplementRTTI(EzSkinData, EzObject);

EzSkinData::EzSkinData(u16 nBoneCount, SkinData* pBoneData, const EzTransform& RootParentToSkin)
: m_pSkinPartition(0), m_OverallTransform(RootParentToSkin), m_pBoneData(pBoneData), m_uBoneCount(nBoneCount)
{
}

EzSkinData::~EzSkinData(void)
{
	EzDelete m_pBoneData;
}

//void EzSkinData::setBoneWeights( u16 bone_index, const EzArray<SSkinWeight> & weights, EzVector3df center, f32 radius ) 
//{
//	if ( bone_index > m_aBoneList.size() ) 
//	{
//		//os::Printer::log("Error: The specified bone index was larger than the number of bones in this EzSkinData.", ELL_ERROR);
//	}
//
//	HasVertexWeights = true;
//	m_aBoneList[bone_index].vertexWeights = weights;
//	m_aBoneList[bone_index].boundingSphereOffset = center;
//	m_aBoneList[bone_index].boundingSphereRadius = radius;
//}

//void EzSkinData::setBoneScale( u16 bone_index, f32 scale )
//{
//	m_aBoneList[bone_index].scale = scale;
//
//}
//
//void EzSkinData::setBoneTranslation( u16 bone_index, EzVector3df translation )
//{
//	m_aBoneList[bone_index].translation = translation;
//
//}
//
//void EzSkinData::setBoneRotation( u16 bone_index, matrix3 rotation)
//{
//	m_aBoneList[bone_index].rotation = rotation;
//
//}



}
