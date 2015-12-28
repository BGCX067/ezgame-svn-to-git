#include "EzGPUProgram.h"


EzImplementRTTI(EzGPUProgram, EzObject);

EzGPUProgram::EzGPUProgram(void)
: m_bLoadFromFile(false)
{
}

EzGPUProgram::~EzGPUProgram(void)
{
}

EzGpuProgramParameters* EzGPUProgram::createParameters(void)
{
	return NULL;

}
