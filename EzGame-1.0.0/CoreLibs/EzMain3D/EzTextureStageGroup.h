#pragma once

#include "../EzCore/EzCore.h"
#include "../EzCore/EzRefObject.h"

class EzTextureStageGroup :  public EzRefObject
{
public:
	EzTextureStageGroup(void);
	~EzTextureStageGroup(void);


	static void initializePools();
	static void shutdownPools();
	static void free(EzTextureStageGroup* pGroup);
	static EzTextureStageGroup* alloc();


    // Add and remove states
    void setStageState(unsigned int uiState, unsigned int uiValue);
    void removeStageState(unsigned int uiState);
    bool getStageState(unsigned int uiState, unsigned int& uiValue);

    void setSamplerState(unsigned int uiState, unsigned int uiValue);
    void removeSamplerState(unsigned int uiState);
    bool getSamplerState(unsigned int uiState, unsigned int& uiValue);

	void removeAllStageStates();
	void removeAllSamplerStates();


	inline bool getFirstStageState(unsigned int& uiState, unsigned int& uiValue);
	inline bool getNextStageState(unsigned int& uiState, unsigned int& uiValue);
	inline bool getFirstSamplerState(unsigned int& uiState, unsigned int& uiValue);
	inline bool getNextSamplerState(unsigned int& uiState, unsigned int& uiValue);


protected:
	enum 
	{
        STAGEARRAYSIZE = 33,
        SAMPLERARRAYSIZE = 14
	};

	bool m_bRendererOwned;

	unsigned int		m_uiStageIter;
	unsigned int		m_uiSamplerIter;



    unsigned int  m_uiNumStages;
    unsigned int  m_uiStageArray[STAGEARRAYSIZE];
    bool m_bStageValid[STAGEARRAYSIZE];

    unsigned int m_uiNumSamplers;
    unsigned int m_uiSamplerArray[SAMPLERARRAYSIZE];
    bool m_bSamplerValid[SAMPLERARRAYSIZE];


	static EzMemPool<EzTextureStageGroup>* ms_pTextureStageGroupPool;

};


inline bool EzTextureStageGroup::getFirstStageState(unsigned int& uiState, unsigned int& uiValue)
{
    m_uiStageIter = 0;
    return getNextStageState(uiState, uiValue);
}

inline bool EzTextureStageGroup::getNextStageState(unsigned int& uiState, unsigned int& uiValue)
{
    while (m_uiStageIter < STAGEARRAYSIZE)
    {
        uiState = m_uiStageIter++;
        if (m_bStageValid[uiState])
        {
            uiValue = m_uiStageArray[uiState];
            return true;
        }
    }

    return false;
}


inline bool EzTextureStageGroup::getFirstSamplerState(unsigned int& uiState, unsigned int& uiValue)
{
    m_uiSamplerIter = 0;
    return getNextSamplerState(uiState, uiValue);
}

inline bool EzTextureStageGroup::getNextSamplerState(unsigned int& uiState, unsigned int& uiValue)
{
    while (m_uiSamplerIter < SAMPLERARRAYSIZE)
    {
        uiState = m_uiSamplerIter++;
        if (m_bSamplerValid[uiState])
        {
            uiValue = m_uiSamplerArray[uiState];
            return true;
        }
    }

    return false;
}