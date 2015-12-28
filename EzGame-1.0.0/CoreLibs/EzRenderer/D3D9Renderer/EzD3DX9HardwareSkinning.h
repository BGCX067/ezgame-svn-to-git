#pragma once

#include "EzD3DX9GPUProgram.h"


class EzD3DX9HardwareSkinning : public EzD3DX9GPUVertexProgram
{
public:
	EzD3DX9HardwareSkinning(IDirect3DDevice9* pDev);
	~EzD3DX9HardwareSkinning(void);

	EzGpuProgramParameters* createParameters(void);
};

