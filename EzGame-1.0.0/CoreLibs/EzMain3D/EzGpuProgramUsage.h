#pragma once

#include "EzGPUProgram.h"
#include "EzGpuProgramParameters.h"
#include "EzObject.h"

class EzGpuProgramUsage : public EzObject
{
public:
	EzGpuProgramUsage(void);
	~EzGpuProgramUsage(void);

	void setProgram(EzGPUProgram* pProg);
	EzGPUProgram* getProgram();
	EzGpuProgramParameters* getParameters();
	void setParameters(EzGpuProgramParameters* pParmeters);

protected:
	EGPUProgramType				m_eType;	
	EzGPUProgram*				m_pProgram;
	EzGpuProgramParameters*     m_pParmeters;		
};

inline void EzGpuProgramUsage::setProgram(EzGPUProgram* pProg)
{
	m_pProgram = pProg;
}

inline EzGPUProgram* EzGpuProgramUsage::getProgram()
{
	return m_pProgram;

}

inline EzGpuProgramParameters* EzGpuProgramUsage::getParameters()
{
	return m_pParmeters;

}

inline void EzGpuProgramUsage::setParameters(EzGpuProgramParameters* pParmeters)
{
	m_pParmeters = pParmeters;
}
