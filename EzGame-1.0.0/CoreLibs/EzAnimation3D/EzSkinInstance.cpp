#include "EzSkinInstance.h"
#include "../EzMain3D/EzNode.h"
#include "../core/EzCore.h"
#include "../core/EzMeasurePerformance.h"
#include "../core/EzDebugConsole.h"

namespace GameEasy
{

EzImplementRTTI(EzSkinInstance, EzObject);

EzSkinInstance::EzSkinInstance(EzNode** ppBones, u16 usBoneCount)
	: m_pSkinData(0), SSkinPartition(0), m_pSkeletonRoot(0), m_ppBones(ppBones), m_usBoneCount(usBoneCount)
{
}

EzSkinInstance::~EzSkinInstance(void)
{
}
u16 EzSkinInstance::getBoneCount() const
{
	return m_usBoneCount;

}

void EzSkinInstance::setBones(u16 idx, EzNode* pBone)
{
	m_ppBones[idx] = pBone;
}

EzNode*const* EzSkinInstance::getBones() const
{
	return m_ppBones;
}
	
EzNode* EzSkinInstance::getSkeletonRoot() const
{
	return m_pSkeletonRoot;

}
const EzSkinData* EzSkinInstance::getSkinData() const
{
	return m_pSkinData;

}
const EzSkinPartition* EzSkinInstance::getSkinPartition() const
{
	return SSkinPartition;

}
void EzSkinInstance::setSkinData( EzSkinData* n )
{
	m_pSkinData = n;
}

void EzSkinInstance::setSkinPartition( EzSkinPartition * n )
{
	SSkinPartition = n;

}

void EzSkinInstance::setSkeletonRoot(EzNode* root)
{
	m_pSkeletonRoot = root;
}

//---------------------------------------------------------------------------
void EzSkinInstance::deform(EzVector3df *pSrcVertices, EzVector3df *pSrcNormal, u32 vertexCount,
	EzVector3df *pDestVertices, EzVector3df *pDestNormal, u32 uiStride)
{
	EZMEASURE_PERFORMANCE_SCOPETIMER(SKINNINGTIME);

    u16 i, j;
    
	EzTransform WorldToSkin, WorldToRootParent;
	m_pSkeletonRoot->getWorldTransform().getInvert(WorldToRootParent);
	WorldToSkin = m_pSkinData->getOverallTransform() * WorldToRootParent;
	const EzSkinData::SkinData* pBoneData = m_pSkinData->getBoneData();
	u16 usBoneCount = m_pSkinData->getBoneCount();

	if(pSrcNormal == NULL)
	{

	}
	else if(pSrcNormal != NULL)
	{
		EzVector3df* pvV = pDestVertices;
		EzVector3df* pvN = pDestNormal;

		for(i = 0; i < vertexCount; ++i)
		{
			(*pvV).X = 0;
			(*pvV).Y = 0;
			(*pvV).Z = 0;

			pvV = (EzVector3df*) ((u32) pvV + uiStride);

			(*pvN).X = 0;
			(*pvN).Y = 0;
			(*pvN).Z = 0;

			pvN = (EzVector3df*) ((u32) pvN + uiStride);
		}

		
		for(i = 0; i < usBoneCount; ++i)
		{
			u16 usVertexWeightCount = pBoneData[i].m_usVertexWeightCount;
			EzTransform XForm;

			XForm = WorldToSkin * m_ppBones[i]->getWorldTransform() * pBoneData[i].m_skinToBone;

			matrix3 ScaledRot = XForm.getToMatrixRotation() * XForm.getScale();

			const EzSkinData::SkinWeight* pSkinWeightData = pBoneData[i].m_pVertexWeights;

			for(j = 0; j < usVertexWeightCount; ++j)
			{
				u16 usIdx = pSkinWeightData[j].m_usIdx;
				f32 fWeight = pSkinWeightData[j].m_fWeight;
				EZASSERT(usIdx < vertexCount)

				u32 uiOffset = usIdx * uiStride;

				pvV = (EzVector3df*) ((u32) pDestVertices + uiOffset);
				EzVector3df vResultVertex;
				EzMatrix3TimesPointPlusPoint(vResultVertex, ScaledRot, pSrcVertices[usIdx], XForm.getTranslation());

				pvN = (EzVector3df*) ((u32) pDestNormal + uiOffset);
				EzVector3df vResultNormal;
				EzMatrix3TimesPoint(vResultNormal, XForm.getToMatrixRotation(), pSrcNormal[usIdx]);

				*pvV += fWeight * vResultVertex;
				*pvN += fWeight * vResultNormal;
			}

		}

		//unitizeVectors(pDestNormal, vertexCount, uiStride);

	}

	//const EzArray<SSkinData> BoneData = m_pSkinData->getBoneData();

	//for(int n = 0; n < m_aBones.size(); ++n)
	//{
	//	u16 uiBoneVerts = BoneData[n].vertexWeights.size();
 //       transform kXform;

 //       kXform = kWorldToSkin * m_aBones[n]->getwGetWorldTransform() *
 //               pkBoneData[i].m_kSkinToBone;
 //           
 //           const NiSkinData::BoneVertData* pkBoneVertData =
 //               pkBoneData[i].m_pkBoneVertData;
 //           
 //           NiMatrix3 kScaledRotate = kXform.m_Rotate * kXform.m_fScale;


	//	
	//	for(u16 j = 0; j < uiBoneVerts; ++j)
	//	{
	//		u16 usIdx = BoneData[n].vertexWeights[j].usIdx;
 //           const f32 fWeight = BoneData[n].vertexWeights[j].fWeight;

 //           pV =(EzVector3df *) ((u32) pVertices + usIdx * uiStride);

	//		pV->X = 0;
	//		pV->Y = 0;
	//		pV->Z = 0;
	//	}

	//}





//    CTransform WorldToSkin, WorldToRootParent;
//    m_pkRootParent->GetWorldTransform().Invert(kWorldToRootParent);
//    kWorldToSkin = m_spSkinData->GetRootParentToSkin() * kWorldToRootParent;
//    const NiSkinData::BoneData* pkBoneData = m_spSkinData->GetBoneData();
//    unsigned int uiBones = m_spSkinData->GetBoneCount();
//
//    if (pkSrcNormal == NULL || pkDstNormal == NULL)
//    {
//        // Positions only
//        NiPoint3* pkV;
//
//        if (uiStride == sizeof(pkDstVertex[0]))
//        {
//            memset(pkDstVertex, 0, sizeof(pkDstVertex[0]) * uiVertices);
//        }
//        else
//        {
//            pkV = pkDstVertex;
//
//            for (i = 0; i < uiVertices; i++)
//            {
//                *pkV = NiPoint3::ZERO;
//                pkV = (NiPoint3*) ((unsigned int) pkV + uiStride);
//            }
//        }
//        
//        for (i = 0; i < uiBones; i++)
//        {
//            unsigned int uiBoneVerts = pkBoneData[i].m_usVerts;
//            NiTransform kXform;
//            
//            kXform = kWorldToSkin * m_ppkBones[i]->GetWorldTransform() *
//                pkBoneData[i].m_kSkinToBone;
//            
//            NiMatrix3 kScaledRotate = kXform.m_Rotate * kXform.m_fScale;
//
//            const NiSkinData::BoneVertData* pkBoneVertData =
//                pkBoneData[i].m_pkBoneVertData;
//            
//            for (j = 0; j < uiBoneVerts; j++)
//            {
//                unsigned short usVert = pkBoneVertData[j].m_usVert;
//                NIASSERT(usVert < uiVertices);
//                const float fWeight = (pkBoneVertData[j].m_fWeight);
//                pkV = (NiPoint3*) ((unsigned int) pkDstVertex + usVert *
//                    uiStride);
//
//                NiPoint3 kVertex;
//                NiMatrixTimesPointPlusPoint(kVertex, kScaledRotate,
//                    (pkSrcVertex[usVert]), kXform.m_Translate);
//
//                // The following assembly is to perform:
//                // *pkV += fWeight * kVertex;
//                NiPoint3* pkVertex = &kVertex;
//
//__asm
//{
//    mov eax, pkV;
//    mov ecx, pkVertex;
//
//    fld DWORD PTR [ecx];        v.x
//    fmul fWeight;               w*v.x
//
//    fld DWORD PTR [ecx+4];      v.y w*v.x
//    fmul fWeight;               w*v.y w*v.x
//    fxch;                       w*v.x w*v.y
//    fadd DWORD PTR [eax];       x+w*v.x w*v.y
//
//    fld DWORD PTR [ecx+8];      v.z x+w*v.x w*v.y
//    fmul fWeight;               w*v.z x+w*v.x w*v.y
//    fxch ST(2);                 w*v.y x+w*v.x w*v.z
//    fadd DWORD PTR [eax+4];     y+w*v.y x+w*v.x w*v.z
//    fxch;                       x+w*v.x y+w*v.y w*v.z
//    fstp DWORD PTR [eax];       y+w*v.y w*v.z
//    fxch;                       w*v.z y+w*v.y
//    fadd DWORD PTR [eax+8];     z+w*v.z y+w*v.y
//    fxch;                       y+w*v.y z+w*v.z
//    fstp DWORD PTR [eax+4];     z+w*v.z
//    fstp DWORD PTR [eax+8];     
//}
//
//            }
//        }
//    }
//    else if (pkDstBinormal == NULL || pkDstTangent == NULL)
//    {
//        // Positions and normals only
//
//        NiPoint3* pkV;
//        NiPoint3* pkN;
//
//        if (uiStride == sizeof(pkDstVertex[0]))
//        {
//            memset(pkDstVertex, 0, sizeof(pkDstVertex[0]) * uiVertices);
//            memset(pkDstNormal, 0, sizeof(pkDstNormal[0]) * uiVertices);
//        }
//        else
//        {
//            pkV = pkDstVertex;
//            pkN = pkDstNormal;
//
//            for (i = 0; i < uiVertices; i++)
//            {
//                *pkV = NiPoint3::ZERO;
//                pkV = (NiPoint3*) ((unsigned int) pkV + uiStride);
//                *pkN = NiPoint3::ZERO;
//                pkN = (NiPoint3*) ((unsigned int) pkN + uiStride);
//            }
//        }
//        
//        for (i = 0; i < uiBones; i++)
//        {
//            unsigned int uiBoneVerts = pkBoneData[i].m_usVerts;
//            NiTransform kXform;
//            
//            kXform = kWorldToSkin * m_ppkBones[i]->GetWorldTransform() *
//                pkBoneData[i].m_kSkinToBone;
//            
//            const NiSkinData::BoneVertData* pkBoneVertData =
//                pkBoneData[i].m_pkBoneVertData;
//            
//            NiMatrix3 kScaledRotate = kXform.m_Rotate * kXform.m_fScale;
//
//            for (j = 0; j < uiBoneVerts; j++)
//            {
//                unsigned short usVert = pkBoneVertData[j].m_usVert;
//                NIASSERT(usVert < uiVertices);
//                float fWeight = pkBoneVertData[j].m_fWeight;
//             
//                unsigned int uiOffset = usVert * uiStride;
//                pkV = (NiPoint3*) ((unsigned int) pkDstVertex + uiOffset);
//                NiPoint3 kVertex;
//                NiMatrixTimesPointPlusPoint(kVertex, kScaledRotate,
//                    (pkSrcVertex[usVert]), kXform.m_Translate);
//                pkN = (NiPoint3*) ((unsigned int) pkDstNormal + uiOffset);
//                NiPoint3 kNormal;
//                NiMatrixTimesPoint(kNormal, kXform.m_Rotate, 
//                    (pkSrcNormal[usVert]));
//                
//                *pkV += fWeight * kVertex;
//                *pkN += fWeight * kNormal;
//            }
//        }
//        
//        NiPoint3::UnitizeVectors(pkDstNormal, uiVertices, uiStride);
//    }
//    else
//    {
//        // Positions, normals, binormals, and tangents
//
//        const NiPoint3* pkSrcBinormal = pkSrcNormal + uiVertices;
//        const NiPoint3* pkSrcTangent = pkSrcBinormal + uiVertices;
//
//        NiPoint3* pkV;
//        NiPoint3* pkN;
//        NiPoint3* pkB;
//        NiPoint3* pkT;
//
//        if (uiStride == sizeof(pkDstVertex[0]))
//        {
//            memset(pkDstVertex, 0, sizeof(pkDstVertex[0]) * uiVertices);
//            memset(pkDstNormal, 0, sizeof(pkDstNormal[0]) * uiVertices);
//            memset(pkDstBinormal, 0, sizeof(pkDstBinormal[0]) * uiVertices);
//            memset(pkDstTangent, 0, sizeof(pkDstTangent[0]) * uiVertices);
//        }
//        else
//        {
//            pkV = pkDstVertex;
//            pkN = pkDstNormal;
//            pkB = pkDstBinormal;
//            pkT = pkDstTangent;
//
//            for (i = 0; i < uiVertices; i++)
//            {
//                *pkV = NiPoint3::ZERO;
//                pkV = (NiPoint3*) ((unsigned int) pkV + uiStride);
//                *pkN = NiPoint3::ZERO;
//                pkN = (NiPoint3*) ((unsigned int) pkN + uiStride);
//                *pkB = NiPoint3::ZERO;
//                pkB = (NiPoint3*) ((unsigned int) pkB + uiStride);
//                *pkT = NiPoint3::ZERO;
//                pkT = (NiPoint3*) ((unsigned int) pkT + uiStride);
//            }
//        }
//        
//        for (i = 0; i < uiBones; i++)
//        {
//            unsigned int uiBoneVerts = pkBoneData[i].m_usVerts;
//            NiTransform kXform;
//            
//            kXform = kWorldToSkin * m_ppkBones[i]->GetWorldTransform() *
//                pkBoneData[i].m_kSkinToBone;
//            
//            const NiSkinData::BoneVertData* pkBoneVertData =
//                pkBoneData[i].m_pkBoneVertData;
//            
//            NiMatrix3 kScaledRotate = kXform.m_Rotate * kXform.m_fScale;
//
//            for (j = 0; j < uiBoneVerts; j++)
//            {
//                unsigned short usVert = pkBoneVertData[j].m_usVert;
//                NIASSERT(usVert < uiVertices);
//                float fWeight = pkBoneVertData[j].m_fWeight;
//             
//                unsigned int uiOffset = usVert * uiStride;
//                pkV = (NiPoint3*) ((unsigned int) pkDstVertex + uiOffset);
//                NiPoint3 kVertex;
//                NiMatrixTimesPointPlusPoint(kVertex, kScaledRotate,
//                    (pkSrcVertex[usVert]), kXform.m_Translate);
//                pkN = (NiPoint3*) ((unsigned int) pkDstNormal + uiOffset);
//                NiPoint3 kNormal;
//                NiMatrixTimesPoint(kNormal, kXform.m_Rotate, 
//                    (pkSrcNormal[usVert]));
//                pkB = (NiPoint3*) ((unsigned int) pkDstBinormal + uiOffset);
//                NiPoint3 kBinormal;
//                NiMatrixTimesPoint(kBinormal, kXform.m_Rotate, 
//                    (pkSrcBinormal[usVert]));
//                pkT = (NiPoint3*) ((unsigned int) pkDstTangent + uiOffset);
//                NiPoint3 kTangent;
//                NiMatrixTimesPoint(kTangent, kXform.m_Rotate, 
//                    (pkSrcTangent[usVert]));
//                
//                *pkV += fWeight * kVertex;
//                *pkN += fWeight * kNormal;
//                *pkB += fWeight * kBinormal;
//                *pkT += fWeight * kTangent;
//            }
//        }
//        
//        NiPoint3::UnitizeVectors(pkDstNormal, uiVertices, uiStride);
//        NiPoint3::UnitizeVectors(pkDstBinormal, uiVertices, uiStride);
//        NiPoint3::UnitizeVectors(pkDstTangent, uiVertices, uiStride);
//    }
}

}