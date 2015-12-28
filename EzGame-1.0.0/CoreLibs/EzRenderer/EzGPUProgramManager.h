#pragma once

#include "../EzCore/EzString.h"
#include "../EzCore/EzMap.h"
#include "../EzCore/EzSingleton.h"

class EzGPUProgram;

class EzGPUProgramManager : public EzSingleton<EzGPUProgramManager>
{
public:
	typedef EzMap<EzStringc, EzGPUProgram*>			EzGpuProgramMap;

	virtual EzGPUProgram* create(const char* Name, const char* Filename) = 0;

	bool addGPUProgram(const char* Name, EzGPUProgram* pProgram);

	EzGPUProgram* findGPUProgram(const char* Name);



	EzGPUProgramManager(void);
	~EzGPUProgramManager(void);


protected:
	EzGpuProgramMap			m_mapGupProgram;


};

inline bool addGPUProgram(const char* Name, EzGPUProgram* pProgram)
{
	return EzGPUProgramManager::get().addGPUProgram(Name, pProgram);
}

inline 	EzGPUProgram* findGPUProgram(const char* Name)
{
	return EzGPUProgramManager::get().findGPUProgram(Name);
}

