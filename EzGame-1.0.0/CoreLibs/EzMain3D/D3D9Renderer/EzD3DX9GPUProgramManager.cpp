#include "EzD3DX9GPUProgramManager.h"
#include "EzD3DX9HardwareSkinning.h"


EzD3DX9GPUProgramManager::EzD3DX9GPUProgramManager(IDirect3DDevice9* pDev)
:m_pID3DDevice(pDev)
{
	addGPUProgram("HardwareSkinning", EzNew EzD3DX9HardwareSkinning(m_pID3DDevice));
}

EzD3DX9GPUProgramManager::~EzD3DX9GPUProgramManager(void)
{
	clear();
}

EzGPUProgram* EzD3DX9GPUProgramManager::create(const char* Name, const char* Filename)
{
	return NULL;
}

void EzD3DX9GPUProgramManager::clear()
{
	EzGpuProgramMap::Iterator  it;

	for(it = m_mapGupProgram.getIterator(); it.atEnd() != true; it++)
	{
		it->getValue()->release(); 
	}

}



