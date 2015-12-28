#pragma once

#include "EzAVObject.h"
#include "EzSkinData.h"
#include "EzHardwareSkinPartition.h"
#include "..\EzCore\EzCore.h"
//#include "EzGpuProgramParameters.h"


class EzSkinPartition;

EzSmartPointer(EzSkinInstance);

class EzSkinInstance  : public EzObject
{
	EzDeclareRTTI;

public:
	EzSkinInstance(EzAVObject** ppBones, unsigned short usBoneCount);
	~EzSkinInstance(void);

	unsigned short getBoneCount() const;

	void setBones(unsigned short idx, EzAVObject* pBone);
	EzAVObject*const* getBones() const;
	
	EzAVObject* getSkeletonRoot() const;
	void setSkeletonRoot(EzAVObject* root);
	EzSkinData* getSkinData() const;
	EzHardwareSkinPartition* getHWSkinPartition() const;

	void setSkinData( EzSkinData* pData );
	void setHWSkinPartition( EzHardwareSkinPartition * pData );


	void calculateBoneMatrices(const unsigned int& uiIndex);

	void deform(const EzPoint3 *pSrcVertices, const EzPoint3 *pSrcNormal, const unsigned int& vertexCount,
		EzPoint3 *pDestVertices, EzPoint3 *pDestNormal, unsigned int uiStride);


	void getWorldToSkinTransform(EzTransform& WorldToSkin) const;


protected:
	EzSkinDataPtr				m_spSkinData;
	EzHardwareSkinPartitionPtr	m_spHWSkinPartition;
	
	unsigned short							m_usBoneCount;
	EzAVObject*					m_pSkeletonRoot;
	EzAVObject**				m_ppBones;
};

inline void EzSkinInstance::getWorldToSkinTransform(EzTransform& WorldToSkin) const
{
	EzTransform WorldToRootParent;

	m_pSkeletonRoot->getWorldTransform().getInvert(WorldToRootParent);
	WorldToSkin = m_spSkinData->getOverallTransform() * WorldToRootParent;
}


inline unsigned short EzSkinInstance::getBoneCount() const
{
	return m_usBoneCount;

}

inline void EzSkinInstance::setBones(unsigned short idx, EzAVObject* pBone)
{
	m_ppBones[idx] = pBone;
}

inline EzAVObject*const* EzSkinInstance::getBones() const
{
	return m_ppBones;
}

inline EzAVObject* EzSkinInstance::getSkeletonRoot() const
{
	return m_pSkeletonRoot;

}
inline EzSkinData* EzSkinInstance::getSkinData() const
{
	return m_spSkinData;

}
inline EzHardwareSkinPartition* EzSkinInstance::getHWSkinPartition() const
{
	return m_spHWSkinPartition;

}
inline void EzSkinInstance::setSkinData( EzSkinData* pData )
{
	m_spSkinData = pData;
}

inline void EzSkinInstance::setHWSkinPartition( EzHardwareSkinPartition* pData )
{
	m_spHWSkinPartition = pData;

}

inline void EzSkinInstance::setSkeletonRoot(EzAVObject* root)
{
	m_pSkeletonRoot = root;
}
