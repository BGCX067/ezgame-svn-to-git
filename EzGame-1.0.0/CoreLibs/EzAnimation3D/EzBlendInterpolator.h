#ifndef _EZBLENDINTERPOLATOR_H_INCLUDE_
#define _EZBLENDINTERPOLATOR_H_INCLUDE_

#include "EzInterpolator.h"


EzSmartPointer(EzBlendInterpolator);

class EzBlendInterpolator : public EzInterpolator
{
	EzDeclareRTTI;
	EzDeclareFlags(u8);

public:
	EzBlendInterpolator(void);
	~EzBlendInterpolator(void);


	f32 getWeight(u8 ucIndex = 0) const;
	void setWeight(f32 fWeight, u8 ucIndex = 0);

	char getPriority(c8 ucIndex = 0) const;
	void setPriority(c8 cPriority, u8 ucIndex = 0);

	f32 getEaseSpinner(u8 ucIndex = 0) const;
	void setEaseSpinner(f32 fEaseSpinner, u8 ucIndex = 0);

	float getTime(unsigned char ucIndex = 0) const;
	void setTime(float fTime, unsigned char ucIndex = 0);

	inline bool getManagerControlled() const;
	inline void setManagerControlled(bool bManagerControlled);
	inline bool getComputeNormalizedWeights() const;
	inline void setComputeNormalizedWeights(bool bComputeNormalizedWeights);
	inline bool getOnlyUseHighestWeight() const;

	inline void setOnlyUseHighestWeight(bool bOnlyUseHighestWeight);

	u8 addInterpInfo(EzInterpolator* pInterpolator, f32 fWeight, c8 cPriority = 0, f32 fEaseSpinner = 1.0f);
	EzInterpolatorPtr removeInterpInfo(u8 ucIndex);


protected:
	struct InterpArrayItem 
	{
		InterpArrayItem()
		{
			clearValues();
		}

		inline void clearValues()
		{
			m_spInterpolator = NULL;
			m_fWeight = 0.0f;
			m_fNormalizedWeight = 0.0f;
			m_cPriority = 0;
			m_fEaseSpinner = 0.0f;
			m_fUpdateTime = -FLT_MAX;
		}

		EzInterpolatorPtr	m_spInterpolator;
		f32					m_fWeight;
		f32					m_fNormalizedWeight;
		c8					m_cPriority;
		u8					m_ucBlendIdx;
		f32					m_fEaseSpinner;
		f32					m_fUpdateTime;
	};

	// flags
	enum
	{
		MANAGER_CONTROLLED_MASK         = 0X0001,
		ONLY_USE_HIGHEST_WEIGHT_MASK    = 0X0002,
		COMPUTE_NORMALIZED_WEIGHTS_MASK = 0x0004
	};

	bool getUpdateTimeForItem(f32& fTime, InterpArrayItem& Item);
	bool getSingleUpdateTime(f32& fTime);
	void clearWeightSums();
	void computeNormalizedWeightsFor2();
	void computeNormalizedWeights();


	EzArray<InterpArrayItem>	m_aInterp;
	EzInterpolator*				m_pSingleInterpolator;

	u8		m_ucSingleIdx;
	c8		m_cHighPriority;
	c8		m_cNextHighPriority;
	u8		m_ucInterpCount;

	f32		m_fWeightThreshold;
	f32		m_fSingleTime;
	f32		m_fHighSumOfWeights;
	f32		m_fNextHighSumOfWeights;
	f32		m_fHighEaseSpinner;
};



inline bool EzBlendInterpolator::getOnlyUseHighestWeight() const
{
	return getBit(ONLY_USE_HIGHEST_WEIGHT_MASK);
}

inline void EzBlendInterpolator::setOnlyUseHighestWeight(bool bOnlyUseHighestWeight)
{
	setBit(bOnlyUseHighestWeight, ONLY_USE_HIGHEST_WEIGHT_MASK);
}

inline bool EzBlendInterpolator::getSingleUpdateTime(f32& fTime)
{
	//EZASSERT(m_ucSingleIdx != FLT_MAX && 
	//	m_pkSingleInterpolator != NULL);

	if (getManagerControlled())
	{
		fTime = m_fSingleTime;
	}

	if (fTime == -FLT_MAX)
	{
		return false;
	}

	return true;
}


inline bool EzBlendInterpolator::getUpdateTimeForItem(f32& fTime, InterpArrayItem& Item)
{
	EzInterpolator* pInterpolator = Item.m_spInterpolator;
	if (pInterpolator && Item.m_fNormalizedWeight != 0.0f)
	{
		if (getManagerControlled())
		{
			fTime = Item.m_fUpdateTime;
		}

		if (fTime == -FLT_MAX)
		{
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

inline bool EzBlendInterpolator::getComputeNormalizedWeights() const
{
	return getBit(COMPUTE_NORMALIZED_WEIGHTS_MASK);
}

inline void EzBlendInterpolator::setComputeNormalizedWeights(
	bool bComputeNormalizedWeights)
{
	setBit(bComputeNormalizedWeights, COMPUTE_NORMALIZED_WEIGHTS_MASK);
}


inline bool EzBlendInterpolator::getManagerControlled() const
{
	return getBit(MANAGER_CONTROLLED_MASK);
}

inline void EzBlendInterpolator::setManagerControlled(bool bManagerControlled)
{
	setBit(bManagerControlled, MANAGER_CONTROLLED_MASK);
}

inline void EzBlendInterpolator::clearWeightSums()
{
	m_fHighSumOfWeights = -FLT_MAX;
	m_fNextHighSumOfWeights = -FLT_MAX;
	m_fHighEaseSpinner = -FLT_MAX;
}

inline f32 EzBlendInterpolator::getTime(u8 ucIndex) const
{
	EZASSERT(ucIndex < m_aInterp.size());

	if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
	{
		// Return the cached time for a single interpolator.
		return m_fSingleTime;
	}

	return m_aInterp[ucIndex].m_fUpdateTime;
}

inline void EzBlendInterpolator::setTime(f32 fTime, u8 ucIndex)
{
	EZASSERT(ucIndex < m_aInterp.size());

	if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
	{
		// Set the cached time for a single interpolator.
		m_fSingleTime = fTime;
		return;
	}

	m_aInterp[ucIndex].m_fUpdateTime = fTime;
}

inline f32 EzBlendInterpolator::getWeight(u8 ucIndex) const
{
	EZASSERT(ucIndex < m_aInterp.size());

	if ( m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
	{
		return 1.0f;
	}

	return m_aInterp[ucIndex].m_fWeight;

}
inline void EzBlendInterpolator::setWeight(f32 fWeight, u8 ucIndex)
{
	EZASSERT(ucIndex < m_aInterp.size());
	EZASSERT(fWeight >= 0.0f);

	if ( m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
	{
		return;
	}

	if (m_aInterp[ucIndex].m_fWeight == fWeight)
	{
		return;
	}

	m_aInterp[ucIndex].m_fWeight = fWeight;
	clearWeightSums();
	setComputeNormalizedWeights(true);

}

inline char EzBlendInterpolator::getPriority(c8 ucIndex) const
{
	EZASSERT(ucIndex < m_ucInterpCount);
	return m_aInterp[ucIndex].m_cPriority;

}
inline f32 EzBlendInterpolator::getEaseSpinner(u8 ucIndex) const
{
	EZASSERT(ucIndex < m_ucInterpCount);

	if ( m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
	{
		// Do not get the ease spinner for a single interpolator.
		return 1.0f;
	}

	return m_aInterp[ucIndex].m_fEaseSpinner;
}

inline void EzBlendInterpolator::setEaseSpinner(f32 fEaseSpinner, u8 ucIndex)
{
	EZASSERT(ucIndex < m_aInterp.size());
	EZASSERT(fEaseSpinner >= 0.0f && fEaseSpinner <= 1.0f);

	if ( m_aInterp.size() == 1 && ucIndex == m_ucSingleIdx)
	{
		return;
	}

	if (m_aInterp[ucIndex].m_fEaseSpinner == fEaseSpinner)
	{
		return;
	}

	m_aInterp[ucIndex].m_fEaseSpinner = fEaseSpinner;
	clearWeightSums();
	setComputeNormalizedWeights(true);
}


#endif