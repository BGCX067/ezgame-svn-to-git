#pragma once

#include "../EzGPUProgramManager.h"
#include "EzD3DX9GPUProgram.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9shader.h>


class EzD3DX9GPUProgramManager : public EzGPUProgramManager
{
public:
	
	EzD3DX9GPUProgramManager(IDirect3DDevice9* pDev);
	~EzD3DX9GPUProgramManager(void);
	
	virtual EzGPUProgram* create(const char* Name, const char* Filename);
	virtual void clear();



protected:
	IDirect3DDevice9*		m_pID3DDevice;

	

};

