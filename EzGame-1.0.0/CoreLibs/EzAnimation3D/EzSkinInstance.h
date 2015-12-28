#pragma once

#include "../EzMain3D/EzAVObject.h"
#include "EzSkinData.h"
#include "EzSkinPartition.h"
#include "..\core\EzCore.h"


namespace GameEasy
{

class EzNode;
class EzSkinPartition;

EzSmartPointer(EzSkinInstance);

class EzSkinInstance  : public EzObject
{
	EzDeclareRTTI;

public:
	EzSkinInstance(EzNode** ppBones, u16 usBoneCount);
	~EzSkinInstance(void);

	u16 getBoneCount() const;

	void setBones(u16 idx, EzNode* pBone);
	EzNode*const* getBones() const;
	
	EzNode* getSkeletonRoot() const;
	void setSkeletonRoot(EzNode* root);
	const EzSkinData* getSkinData() const;
	const EzSkinPartition* getSkinPartition() const;

	void setSkinData( EzSkinData* n );
	void setSkinPartition( EzSkinPartition * n );

	void deform(EzVector3df *pSrcVertices, EzVector3df *pSrcNormal, u32 vertexCount,
		EzVector3df *pDestVertices, EzVector3df *pDestNormal, u32 uiStride);


protected:
	EzSkinData*			m_pSkinData;
	EzSkinPartition*	SSkinPartition;
	
	u16				m_usBoneCount;
	EzNode*			m_pSkeletonRoot;
	EzNode**		m_ppBones;


};

}
