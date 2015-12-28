#include "EzGPUProgramManager.h"


template<> 
EzGPUProgramManager* EzSingleton<EzGPUProgramManager>::ms_pSingleton = 0;


EzGPUProgramManager::EzGPUProgramManager(void)
{
}

EzGPUProgramManager::~EzGPUProgramManager(void)
{
}

bool EzGPUProgramManager::addGPUProgram(const char* Name, EzGPUProgram* pProgram)
{
	return m_mapGupProgram.insert(EzStringc(Name), pProgram);
}

EzGPUProgram* EzGPUProgramManager::findGPUProgram(const char* Name)
{
	EzGpuProgramMap::Node* pFindNode = m_mapGupProgram.find(EzStringc(Name));

	if(pFindNode)
	{
		return pFindNode->getValue();
	}

	return NULL;
}

