#include "EzBlendInterpolator.h"

EzImplementRTTI(EzBlendInterpolator, EzInterpolator);

EzBlendInterpolator::EzBlendInterpolator(void)
: m_ucSingleIdx(UCHAR_MAX), m_pSingleInterpolator(NULL),m_fSingleTime(-FLT_MAX), m_ucInterpCount(0)
{
	m_aInterp.set_used(8);
}

EzBlendInterpolator::~EzBlendInterpolator(void)
{
}

u8 EzBlendInterpolator::addInterpInfo(EzInterpolator* pInterpolator, f32 fWeight, c8 cPriority, f32 fEaseSpinner)
{
//	EZASSERT(pkInterpolator && InterpolatorIsCorrectType(pkInterpolator));

	// Get new interp array index.
	u8 ucIndex;
	for (ucIndex = 0; ucIndex < m_aInterp.size(); ucIndex++)
	{
		if (m_aInterp[ucIndex].m_spInterpolator == NULL)
		{
			break;
		}
	}

	// Set new interp array data.
	InterpArrayItem& NewItem = m_aInterp[ucIndex];
	NewItem.m_spInterpolator = pInterpolator;
	NewItem.m_fWeight = fWeight;
	NewItem.m_cPriority = cPriority;
	NewItem.m_fEaseSpinner = fEaseSpinner;

	// Update high priority.
	if (cPriority > m_cNextHighPriority)
	{
		if (cPriority > m_cHighPriority)
		{
			m_cNextHighPriority = m_cHighPriority;
			m_cHighPriority = cPriority;
		}
		else if (cPriority != m_cHighPriority)
		{
			m_cNextHighPriority = cPriority;
		}
	}

	// Set single index.
	if (++m_ucInterpCount == 1)
	{
		m_ucSingleIdx = ucIndex;
		m_pSingleInterpolator = pInterpolator;
	}
	else
	{
		m_ucSingleIdx = UCHAR_MAX;
		m_pSingleInterpolator = NULL;
		m_fSingleTime = -FLT_MAX;
	}

	clearWeightSums();    
	setComputeNormalizedWeights(true);

	return ucIndex;
}

EzInterpolatorPtr EzBlendInterpolator::removeInterpInfo(u8 ucIndex)
{
	EZASSERT(ucIndex < m_aInterp.size());
	InterpArrayItem& Item = m_aInterp[ucIndex];

	// Remove interp info.
	EzInterpolatorPtr spInterpolator = Item.m_spInterpolator;
	c8 cPriority = Item.m_cPriority;
	Item.clearValues();

	// Update high priorities and weight sums.
	if (cPriority == m_cHighPriority ||	cPriority == m_cNextHighPriority)
	{
		// Determine highest priority.
		m_cHighPriority = m_cNextHighPriority = SCHAR_MIN;
		for (unsigned char uc = 0; uc < m_aInterp.size(); uc++)
		{
			InterpArrayItem& TempItem = m_aInterp[uc];
			if (TempItem.m_spInterpolator != NULL)
			{
				if (TempItem.m_cPriority > m_cNextHighPriority)
				{
					if (TempItem.m_cPriority > m_cHighPriority)
					{
						m_cNextHighPriority = m_cHighPriority;
						m_cHighPriority = TempItem.m_cPriority;
					}
					else if (TempItem.m_cPriority < m_cHighPriority)
					{
						m_cNextHighPriority = TempItem.m_cPriority;
					}
				}
			}
		}
	}

	// Set single index.
	if (--m_ucInterpCount == 1)
	{
		if (m_aInterp.size() == 2)
		{
			if (ucIndex == 0)
			{
				m_ucSingleIdx = 1;
			}
			else
			{
				m_ucSingleIdx = 0;
			}
		}
		else
		{
			for (unsigned char uc = 0; uc < m_aInterp.size(); uc++)
			{
				if (m_aInterp[uc].m_spInterpolator != NULL)
				{
					m_ucSingleIdx = uc;
				}
			}
		}
		m_pSingleInterpolator = m_aInterp[m_ucSingleIdx]
		.m_spInterpolator;
		m_fSingleTime = m_aInterp[m_ucSingleIdx].m_fUpdateTime;
	}
	else if (m_ucInterpCount == 0)
	{
		m_ucSingleIdx = UCHAR_MAX;
		m_pSingleInterpolator = NULL;
		m_fSingleTime = -FLT_MAX;
	}

	clearWeightSums();
	setComputeNormalizedWeights(true);

	return spInterpolator;
}

void EzBlendInterpolator::computeNormalizedWeights()
{
	if (!getComputeNormalizedWeights())
	{
		return;
	}

	setComputeNormalizedWeights(false);

	if (m_ucInterpCount == 1)
	{
		m_aInterp[m_ucSingleIdx].m_fNormalizedWeight = 1.0f;
		return;
	}
	else if (m_ucInterpCount == 2)
	{
		computeNormalizedWeightsFor2();
		return;
	}

	unsigned char uc;

	if (m_fHighSumOfWeights == -FLT_MAX)
	{
		// Compute sum of weights for highest and next highest priorities,
		// along with highest ease spinner for the highest priority.
		m_fHighSumOfWeights = 0.0f;
		m_fNextHighSumOfWeights = 0.0f;
		m_fHighEaseSpinner = 0.0f;
		for (uc = 0; uc < m_aInterp.size(); uc++)
		{
			InterpArrayItem& kItem = m_aInterp[uc];
			if (kItem.m_spInterpolator != NULL)
			{
				float fRealWeight = kItem.m_fWeight * kItem.m_fEaseSpinner;
				if (kItem.m_cPriority == m_cHighPriority)
				{
					m_fHighSumOfWeights += fRealWeight;
					if (kItem.m_fEaseSpinner > m_fHighEaseSpinner)
					{
						m_fHighEaseSpinner = kItem.m_fEaseSpinner;
					}
				}
				else if (kItem.m_cPriority == m_cNextHighPriority)
				{
					m_fNextHighSumOfWeights += fRealWeight;
				}
			}
		}
	}

	EZASSERT(m_fHighEaseSpinner >= 0.0f && m_fHighEaseSpinner <= 1.0f);
	f32 fOneMinusHighEaseSpinner = 1.0f - m_fHighEaseSpinner;
	f32 fTotalSumOfWeights = m_fHighEaseSpinner * m_fHighSumOfWeights +
		fOneMinusHighEaseSpinner * m_fNextHighSumOfWeights;
	f32 fOneOverTotalSumOfWeights = 
		(fTotalSumOfWeights > 0.0f) ? (1.0f / fTotalSumOfWeights) : 0.0f;

	// Compute normalized weights.
	for (uc = 0; uc < m_aInterp.size(); uc++)
	{
		InterpArrayItem& Item = m_aInterp[uc];
		if (Item.m_spInterpolator != NULL)
		{
			if (Item.m_cPriority == m_cHighPriority)
			{
				Item.m_fNormalizedWeight = m_fHighEaseSpinner *
					Item.m_fWeight * Item.m_fEaseSpinner *
					fOneOverTotalSumOfWeights;
			}
			else if (Item.m_cPriority == m_cNextHighPriority)
			{
				Item.m_fNormalizedWeight = fOneMinusHighEaseSpinner *
					Item.m_fWeight * Item.m_fEaseSpinner *
					fOneOverTotalSumOfWeights;
			}
			else
			{
				Item.m_fNormalizedWeight = 0.0f;
			}
		}
	}

	// Exclude weights below threshold, computing new sum in the process.
	float fSumOfNormalizedWeights = 1.0f;
	if (m_fWeightThreshold > 0.0f)
	{
		fSumOfNormalizedWeights = 0.0f;
		for (uc = 0; uc < m_aInterp.size(); uc++)
		{
			InterpArrayItem& Item = m_aInterp[uc];
			if (Item.m_spInterpolator != NULL &&
				Item.m_fNormalizedWeight != 0.0f)
			{
				if (Item.m_fNormalizedWeight < m_fWeightThreshold)
				{
					Item.m_fNormalizedWeight = 0.0f;
				}
				fSumOfNormalizedWeights += Item.m_fNormalizedWeight;
			}
		}
	}

	// Renormalize weights if any were excluded earlier.
	if (fSumOfNormalizedWeights != 1.0f)
	{
		// Renormalize weights.
		float fOneOverSumOfNormalizedWeights = 
			(fSumOfNormalizedWeights > 0.0f) ?
			(1.0f / fSumOfNormalizedWeights) : 0.0f;

		for (uc = 0; uc < m_aInterp.size(); uc++)
		{
			InterpArrayItem& kItem = m_aInterp[uc];
			if (kItem.m_fNormalizedWeight != 0.0f)
			{
				kItem.m_fNormalizedWeight = kItem.m_fNormalizedWeight *	fOneOverSumOfNormalizedWeights;
			}
		}
	}

	// Only use the highest weight, if so directed.
	if (getOnlyUseHighestWeight())
	{
		float fHighest = -1.0f;
		u8 ucHighIndex = UCHAR_MAX;
		for (uc = 0; uc < m_aInterp.size(); uc++)
		{
			if (m_aInterp[uc].m_fNormalizedWeight > fHighest)
			{
				ucHighIndex = uc;
				fHighest = m_aInterp[uc].m_fNormalizedWeight;
			}
			m_aInterp[uc].m_fNormalizedWeight = 0.0f;
		}

		// Set the highest index to 1.0
		m_aInterp[ucHighIndex].m_fNormalizedWeight = 1.0f;
	}
}

void EzBlendInterpolator::computeNormalizedWeightsFor2()
{
	EZASSERT(m_ucInterpCount == 2);

	// Get pointers to the two items.
	InterpArrayItem* pItem1 = NULL;
	InterpArrayItem* pItem2 = NULL;
	for (unsigned char uc = 0; uc < m_aInterp.size(); uc++)
	{
		if (m_aInterp[uc].m_spInterpolator)
		{
			if (!pItem1)
			{
				pItem1 = &m_aInterp[uc];
				continue;
			}
			else if (!pItem2)
			{
				pItem2 = &m_aInterp[uc];
				break;
			}
		}
	}

	EZASSERT(pItem1 && pItem2);

	// Calculate the real weight of each item.
	float fRealWeight1 = pItem1->m_fWeight * pItem1->m_fEaseSpinner;
	float fRealWeight2 = pItem2->m_fWeight * pItem2->m_fEaseSpinner;
	if (fRealWeight1 == 0.0f && fRealWeight2 == 0.0f)
	{
		pItem1->m_fNormalizedWeight = 0.0f;
		pItem2->m_fNormalizedWeight = 0.0f;
		return;
	}

	// Compute normalized weights.
	if (pItem1->m_cPriority > pItem2->m_cPriority)
	{
		if (pItem1->m_fEaseSpinner == 1.0f)
		{
			pItem1->m_fNormalizedWeight = 1.0f;
			pItem2->m_fNormalizedWeight = 0.0f;
			return;
		}

		float fOneMinusEaseSpinner = 1.0f - pItem1->m_fEaseSpinner;
		float fOneOverSumOfWeights = 1.0f / (pItem1->m_fEaseSpinner *
			fRealWeight1 + fOneMinusEaseSpinner * fRealWeight2);
		pItem1->m_fNormalizedWeight = pItem1->m_fEaseSpinner * fRealWeight1
			* fOneOverSumOfWeights;
		pItem2->m_fNormalizedWeight = fOneMinusEaseSpinner * fRealWeight2 *
			fOneOverSumOfWeights;
	}
	else if (pItem1->m_cPriority < pItem2->m_cPriority)
	{
		if (pItem2->m_fEaseSpinner == 1.0f)
		{
			pItem1->m_fNormalizedWeight = 0.0f;
			pItem2->m_fNormalizedWeight = 1.0f;
			return;
		}

		float fOneMinusEaseSpinner = 1.0f - pItem2->m_fEaseSpinner;
		float fOneOverSumOfWeights = 1.0f / (pItem2->m_fEaseSpinner *
			fRealWeight2 + fOneMinusEaseSpinner * fRealWeight1);
		pItem1->m_fNormalizedWeight = fOneMinusEaseSpinner * fRealWeight1 *
			fOneOverSumOfWeights;
		pItem2->m_fNormalizedWeight = pItem2->m_fEaseSpinner * fRealWeight2
			* fOneOverSumOfWeights;
	}
	else
	{
		float fOneOverSumOfWeights = 1.0f / (fRealWeight1 + fRealWeight2);
		pItem1->m_fNormalizedWeight = fRealWeight1 * fOneOverSumOfWeights;
		pItem2->m_fNormalizedWeight = fRealWeight2 * fOneOverSumOfWeights;
	}

	// Only use the highest weight, if so desired.
	if (getOnlyUseHighestWeight())
	{
		if (pItem1->m_fNormalizedWeight >= pItem2->m_fNormalizedWeight)
		{
			pItem1->m_fNormalizedWeight = 1.0f;
			pItem2->m_fNormalizedWeight = 0.0f;
		}
		else
		{
			pItem1->m_fNormalizedWeight = 0.0f;
			pItem2->m_fNormalizedWeight = 1.0f;
		}
		return;
	}

	// Exclude weights below threshold.
	if (m_fWeightThreshold > 0.0f)
	{
		bool bReduced1 = false;
		if (pItem1->m_fNormalizedWeight < m_fWeightThreshold)
		{
			pItem1->m_fNormalizedWeight = 0.0f;
			bReduced1 = true;
		}

		bool bReduced2 = false;
		if (pItem2->m_fNormalizedWeight < m_fWeightThreshold)
		{
			pItem2->m_fNormalizedWeight = 0.0f;
			bReduced1 = true;
		}

		if (bReduced1 && bReduced2)
		{
			return;
		}
		else if (bReduced1)
		{
			pItem2->m_fNormalizedWeight = 1.0f;
			return;
		}
		else if (bReduced2)
		{
			pItem1->m_fNormalizedWeight = 1.0f;
			return;
		}
	}
}

void EzBlendInterpolator::setPriority(char cPriority, unsigned char ucIndex)
{
	EZASSERT(ucIndex < m_aInterp.size());

	// Only set priority if it differs from the current priority.
	if (m_aInterp[ucIndex].m_cPriority == cPriority)
	{
		return;
	}

	m_aInterp[ucIndex].m_cPriority = cPriority;

	if (cPriority > m_cHighPriority)
	{
		m_cNextHighPriority = m_cHighPriority;
		m_cHighPriority = cPriority;
	}
	else
	{

		m_cHighPriority = m_cNextHighPriority = SCHAR_MIN;
		for (unsigned char uc = 0; uc < m_aInterp.size(); uc++)
		{
			InterpArrayItem& TempItem = m_aInterp[uc];
			if (TempItem.m_spInterpolator != NULL)
			{
				if (TempItem.m_cPriority > m_cNextHighPriority)
				{
					if (TempItem.m_cPriority > m_cHighPriority)
					{
						m_cNextHighPriority = m_cHighPriority;
						m_cHighPriority = TempItem.m_cPriority;
					}
					else if (TempItem.m_cPriority < m_cHighPriority)
					{
						m_cNextHighPriority = TempItem.m_cPriority;
					}
				}
			}
		}
	}

	clearWeightSums();
	setComputeNormalizedWeights(true);
}

