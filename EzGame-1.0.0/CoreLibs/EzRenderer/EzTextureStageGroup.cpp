#include "EzTextureStageGroup.h"

EzMemPool<EzTextureStageGroup>* EzTextureStageGroup::ms_pTextureStageGroupPool = NULL;

EzTextureStageGroup::EzTextureStageGroup(void)
: m_uiNumStages(0), m_uiNumSamplers(0), m_bRendererOwned(false), m_uiStageIter(0), m_uiSamplerIter(0)
{
    memset(m_uiStageArray, 0, sizeof(m_uiStageArray));
    memset(m_bStageValid, 0, sizeof(m_bStageValid));
    memset(m_uiSamplerArray, 0, sizeof(m_uiSamplerArray));
    memset(m_bSamplerValid, 0, sizeof(m_bSamplerValid));
}

EzTextureStageGroup::~EzTextureStageGroup(void)
{
}


void EzTextureStageGroup::initializePools()
{
    ms_pTextureStageGroupPool = EzNew EzMemPool<EzTextureStageGroup>;
}
void EzTextureStageGroup::shutdownPools()
{
    EzDelete ms_pTextureStageGroupPool;
}

void EzTextureStageGroup::setStageState(u32 uiState, u32 uiValue)
{
	EZASSERT(uiState < STAGEARRAYSIZE);

	if (m_bStageValid[uiState])
    {
		m_uiStageArray[uiState] = uiValue;
		return;
    }

    m_bStageValid[uiState] = true;
    m_uiStageArray[uiState] = uiValue;
    m_uiNumStages++;
}

void EzTextureStageGroup::removeStageState(u32 uiState)
{
	EZASSERT(uiState < STAGEARRAYSIZE);

	if(m_bStageValid[uiState])
	{
		m_bStageValid[uiState] = false;
		m_uiNumStages--;
	}
}


bool EzTextureStageGroup::getStageState(u32 uiState, u32& uiValue)
{
	EZASSERT(uiState < STAGEARRAYSIZE);
	
	if (m_bStageValid[uiState])
    {
		uiValue =  m_uiStageArray[uiState];
        return true;
    }

	return false;
}

void EzTextureStageGroup::setSamplerState(u32 uiState, u32 uiValue)
{
	EZASSERT(uiState < SAMPLERARRAYSIZE);

	if (m_bSamplerValid[uiState])
    {
		m_uiSamplerArray[uiState] = uiValue;
        return;
    }

    m_bSamplerValid[uiState] = true;
    m_uiSamplerArray[uiState] = uiValue;
    m_uiNumSamplers++;

}

void EzTextureStageGroup::removeSamplerState(u32 uiState)
{
	EZASSERT(uiState < SAMPLERARRAYSIZE);

	if (m_bSamplerValid[uiState])
    {
		m_bSamplerValid[uiState] = false;
		m_uiNumSamplers--;
    }
}

bool EzTextureStageGroup::getSamplerState(u32 uiState, u32& uiValue)
{
	EZASSERT(uiState < SAMPLERARRAYSIZE);

	if (m_bSamplerValid[uiState])
    {
		uiValue =  m_uiSamplerArray[uiState];
        return true;
    }

	return false;

}


void EzTextureStageGroup::removeAllStageStates()
{
    for (u32 i = 0; i < STAGEARRAYSIZE; i++)
    {
        m_bStageValid[i] = false;
    }

    m_uiNumStages = 0;
}


void EzTextureStageGroup::removeAllSamplerStates()
{
    for (u32 i = 0; i < SAMPLERARRAYSIZE; i++)
    {
        m_bSamplerValid[i] = false;
    }
    m_uiNumSamplers = 0;
}

EzTextureStageGroup* EzTextureStageGroup::alloc()
{
    return ms_pTextureStageGroupPool->alloc();
}

void EzTextureStageGroup::free(EzTextureStageGroup* pGroup)
{
	ms_pTextureStageGroupPool->free(pGroup);
}