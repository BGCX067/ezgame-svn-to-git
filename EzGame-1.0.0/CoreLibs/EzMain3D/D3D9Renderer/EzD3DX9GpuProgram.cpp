#include "EzD3DX9GpuProgram.h"
#include "EzD3DX9Driver.h"


EzImplementRTTI(EzD3DX9GPUProgram, EzGPUProgram);


EzD3DX9GPUProgram::EzD3DX9GPUProgram(IDirect3DDevice9* pDev)
: m_pDevice(pDev)
{
}

EzD3DX9GPUProgram::~EzD3DX9GPUProgram(void)
{
}

void EzD3DX9GPUProgram::loadImpl(void)
{
	// Normal load-from-source approach
	if (m_bLoadFromFile)
	{
		//// find & load source code
		//DataStreamPtr stream = 
		//	ResourceGroupManager::getSingleton().openResource(
		//	mFilename, mGroup, true, this);
		//mSource = stream->getAsString();
	}

	// Call polymorphic load
	loadFromSource();
}


void EzD3DX9GPUProgram::loadFromSource(void)
{
	// Create the shader
	// Assemble source into microcode
	LPD3DXBUFFER microcode;
	LPD3DXBUFFER errors;

	HRESULT hr = stubD3DXAssembleShader(
		m_Source.c_str(),
		static_cast<UINT>(m_Source.size()),
		NULL,               // no #define support
		NULL,               // no #include support
		0,                  // standard compile options
		&microcode,
		&errors);

	if (FAILED(hr))
	{
		//String message = "Cannot assemble D3D9 shader " + mName + " Errors:\n" +
		//	static_cast<const char*>(errors->GetBufferPointer());
		//errors->Release();
		//OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, message,
		//	"D3D9GpuProgram::loadFromSource");

	}

	loadFromLowLevel(microcode);

	if(microcode)
		microcode->Release();

	if(errors)
		errors->Release();
}

EzD3DX9GPUVertexProgram::EzD3DX9GPUVertexProgram(IDirect3DDevice9* pDev)
:EzD3DX9GPUProgram(pDev)
{
	setType(EGP_VERTEX_PROGRAM);

}
EzD3DX9GPUVertexProgram::~EzD3DX9GPUVertexProgram()
{

}

void EzD3DX9GPUVertexProgram::loadFromLowLevel(LPD3DXBUFFER microcode)
{
	//if (isSupported())
	//{
		// Create the shader
		HRESULT hr = m_pDevice->CreateVertexShader( 
			static_cast<DWORD*>(microcode->GetBufferPointer()), 
			&m_pVertexShader);

		if (FAILED(hr))
		{
			//OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
			//	"Cannot create D3D9 vertex shader " + mName + " from microcode",
			//	"D3D9GpuVertexProgram::loadFromMicrocode");

		}
	//}
	//else
	//{
	//	//LogManager::getSingleton().logMessage(
	//	//	"Unsupported D3D9 vertex shader '" + mName + "' was not loaded.");
	//}
}

