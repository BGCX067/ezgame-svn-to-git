#pragma once

#include "../EzCore/EzEnumTypes.h"
#include "../EzCore/EzCore.h"
#include "../EzCore/EzMemObject.h"
#include "EzHardwareSkinPartition.h"


class EzIndexData;
class EzVertexData;




class EzRendererData : public EzMemObject
{
public:
	enum  EANIMATION_TYPE 
	{
		E_NODE,
		E_HW_SKINNING,
		E_SW_SKINNING
	};

	EzRendererData(void);
	~EzRendererData(void);

	void setVertexData(EzVertexData* pData);
	EzVertexData* getVertexData();
	void setIndexData(EzIndexData* pData);
	EzIndexData* getIndexData();
	void setPrimitiveType(EPRIMITIVE_TYPE eType);
	const EPRIMITIVE_TYPE& getPrimitiveType() const;
	void setNextRendererData(EzRendererData* pRendererData);
	EzRendererData* getNextRendererData();
	void setHardwareSkinPartition(EzHardwareSkinPartition::Partition* pPartion);
	const EzHardwareSkinPartition::Partition*  getHardwareSkinPartition() const;
	const EANIMATION_TYPE& getAnimationType() const;
	void setAnimationType(const EANIMATION_TYPE& eType);



protected:
	EPRIMITIVE_TYPE			m_ePrimitiveType;
	EzVertexData*			m_pVertexData;
	EzIndexData*			m_pIndexData;
	EANIMATION_TYPE			m_eAnimationType;

	EzRendererData*			m_pNextRendererData;
	EzHardwareSkinPartition::Partition* m_pPartion;
};

inline const EzRendererData::EANIMATION_TYPE& EzRendererData::getAnimationType() const
{
	return m_eAnimationType;

}

inline void EzRendererData::setAnimationType(const EANIMATION_TYPE& eType)
{
	m_eAnimationType = eType;

}

inline void EzRendererData::setHardwareSkinPartition(EzHardwareSkinPartition::Partition* pPartion)
{
	m_pPartion = pPartion;

}

inline const EzHardwareSkinPartition::Partition*  EzRendererData::getHardwareSkinPartition() const
{
	return m_pPartion;
}



inline EzRendererData* EzRendererData::getNextRendererData()
{
	return m_pNextRendererData;
}

inline void EzRendererData::setNextRendererData(EzRendererData* pRendererData)
{
	m_pNextRendererData = pRendererData;

}


inline void EzRendererData::setVertexData(EzVertexData* pData)
{
	m_pVertexData = pData;
}

inline const EPRIMITIVE_TYPE& EzRendererData::getPrimitiveType() const
{
	return  m_ePrimitiveType;
}

inline void EzRendererData::setPrimitiveType(EPRIMITIVE_TYPE eType)
{
	m_ePrimitiveType = eType;
}

inline EzVertexData* EzRendererData::getVertexData()
{
	return m_pVertexData;
}

inline void EzRendererData::setIndexData(EzIndexData* pData)
{
	m_pIndexData = pData;
}

inline EzIndexData* EzRendererData::getIndexData()
{
	return m_pIndexData;

}

