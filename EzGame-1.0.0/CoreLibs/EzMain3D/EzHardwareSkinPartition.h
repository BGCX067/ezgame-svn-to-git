#ifndef __EZHARDWARDSKINPARTITION_H_INCLUDE__
#define __EZHARDWARDSKINPARTITION_H_INCLUDE__

#include "EzObject.h"
#include "../EzCore/EzDebugHelper.h"

EzSmartPointer(EzHardwareSkinPartition);

class EzRendererData;
class EzSkinInstance;

class EzHardwareSkinPartition : public EzObject
{
	EzDeclareRTTI;

public:
	class Partition
	{
	public:
		Partition();
		virtual ~Partition();

		unsigned short*		m_pusBones;
		float*				m_pfWeights;
		unsigned short*		m_pusVertexMap;
		unsigned char*		m_pucBonePalette;
		unsigned short*		m_pusTriList;
		unsigned short*	 	m_pusStripLengths;

		unsigned short		 m_usVertexCount;
		unsigned short		 m_usTriangleCount;
		unsigned short		 m_usBoneCount;	
		unsigned short		 m_usStripCount;
		unsigned short		 m_usWeightsPerVertexCount;

		
		float*				m_pfBoneMatrices;
		unsigned int				m_uiNumMatrices;
		unsigned int				m_uiBoneMatricesAllocatedSize;
		EzRendererData* m_pRendererData;

	};


	EzHardwareSkinPartition(Partition* pPartitions, unsigned int	uiPartitionCount);
	~EzHardwareSkinPartition(void);

	unsigned int	getPartitionCount() const;
	Partition* getPartitions() const;
	
	void calculateBoneMatrices(const unsigned int& uiIndex, EzSkinInstance* pSkinInstance);

	const float* getBoneMatrices(const unsigned int& uiIndex) const;
	const unsigned int& getNumBoneMatrices(const unsigned int& uiIndex) const;
	const unsigned int& getBoneMatrixAllocatedSize(const unsigned int& uiIndex) const;


protected:
	unsigned int					m_uiPartitionCount;
	Partition*			m_pPartitions;

};


inline EzHardwareSkinPartition::EzHardwareSkinPartition(Partition* pPartitions, unsigned int	uiPartitionCount)
: m_uiPartitionCount(uiPartitionCount), m_pPartitions(pPartitions)

{
}


inline EzHardwareSkinPartition::~EzHardwareSkinPartition(void)
{
}


inline unsigned int EzHardwareSkinPartition::getPartitionCount() const
{
	return m_uiPartitionCount;

}
inline EzHardwareSkinPartition::Partition* EzHardwareSkinPartition::getPartitions() const
{
	return m_pPartitions;

}

inline const float* EzHardwareSkinPartition::getBoneMatrices(const unsigned int& uiIndex) const
{
	EZASSERT(uiIndex < m_uiPartitionCount);
	return m_pPartitions[uiIndex].m_pfBoneMatrices;
}

inline const unsigned int& EzHardwareSkinPartition::getNumBoneMatrices(const unsigned int& uiIndex) const
{
	EZASSERT(uiIndex < m_uiPartitionCount);
	return m_pPartitions[uiIndex].m_uiNumMatrices;
}

inline const unsigned int& EzHardwareSkinPartition::getBoneMatrixAllocatedSize(const unsigned int& uiIndex) const
{
	EZASSERT(uiIndex < m_uiPartitionCount);
	return m_pPartitions[uiIndex].m_uiBoneMatricesAllocatedSize;
}

#endif
