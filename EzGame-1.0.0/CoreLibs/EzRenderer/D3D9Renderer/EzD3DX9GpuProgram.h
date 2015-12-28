#pragma once

#include "../EzGPUProgram.h"

#include <d3d9.h>
#include <d3dx9.h>


class EzD3DX9GPUProgram : public EzGPUProgram
{
public:
	EzD3DX9GPUProgram(IDirect3DDevice9* pDev);
	~EzD3DX9GPUProgram(void);

protected:
	IDirect3DDevice9* m_pDevice;

protected:
	/** @copydoc Resource::loadImpl */
	void loadImpl();
	/** Overridden from GpuProgram */
	void loadFromSource();

	virtual void loadFromLowLevel(ID3DXBuffer* pLowLevelCode)
	{

	}

};



/** Direct3D implementation of low-level vertex programs. */
class EzD3DX9GPUVertexProgram : public EzD3DX9GPUProgram
{
public:
	EzD3DX9GPUVertexProgram(IDirect3DDevice9* pDev);
	~EzD3DX9GPUVertexProgram();

	IDirect3DVertexShader9* getVertexShader(void) const;


protected:
	virtual void loadFromLowLevel(ID3DXBuffer* pLowLevelCode);

protected:
	IDirect3DVertexShader9* m_pVertexShader;
};


inline IDirect3DVertexShader9* EzD3DX9GPUVertexProgram::getVertexShader(void) const 
{ 
	return m_pVertexShader; 
}
