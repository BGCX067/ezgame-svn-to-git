#include "EzVideoDriver.h"
#include "EzImageLoader.h"
#include "../EzCore/EzFile.h"
#include "../EzCore/EzMeasurePerformance.h"
#include "EzGeometryData.h"
#include "D3D9Renderer/EzD3DX9Driver.h"
#include "EzRenderer.h"
#include "EzShaderFactory.h"
#include "EzShaderLibrary.h"


EzImplementRootRTTI(EzVideoDriver);

EzVideoDriver* EzVideoDriver::m_spVideoDriver = NULL;

EzVideoDriver::EzVideoDriver(void)
: m_uiTextureCreationFlags(0), m_AmbientColor(255, 255, 255, 255), m_DiffuseColor(255, 255, 255, 255)
  ,m_EmissiveColor(255, 255, 255, 255), m_SpecularColor(255, 255, 255, 255), m_fSininess(0.0f), m_lastDynamicEffectState(0), m_pEntity(NULL)
  , m_uiMatLastRevisionID(UINT_MAX), m_eLastTextureFilterMode(FILTER_NEAREST), m_eLastTextureClampMode(ETC_CLAMP_U_CLAMP_V)
  , m_eLastTextureApplyMode(APPLY_MODULATE), m_uiMaxTextureBlendStages(0), m_pRenderer(NULL)
{
	setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true);

	for (unsigned int i=0; i<EVDF_COUNT; ++i)
		m_bFeatureEnabled[i]=true;

}

EzVideoDriver::~EzVideoDriver(void)
{
	EzDelete m_pRenderer;
}

bool EzVideoDriver::beginScene(bool backBuffer, bool zBuffer, const EzColor& color)
{
	m_uiPrimitiveCount = 0;


	return true;
}


bool EzVideoDriver::endScene()
{
	return true;
}



void EzVideoDriver::drawVertexPrimitive(EPRIMITIVE_TYPE eType, unsigned int uiVertexStart, unsigned int uiIndexNum, unsigned int uiIndexStart)
{
	unsigned int uiPrimitiveCount = 0;	
	switch(eType)
	{

	case EPT_POINTS:
	case EPT_LINES:
	case EPT_LINE_STRIP:
		break;

	case EPT_TRIANGLE_LIST:
		uiPrimitiveCount = (unsigned int) uiIndexNum / 3;
		break;

	case EPT_TRIANGLE_STRIP:
	case EPT_TRIANGLE_FAN:
		uiPrimitiveCount = (unsigned int) uiIndexNum - 2;
		break;

	}

	EZMEASURE_PERFORMANCE_AGGREGATEVALUE(PRIMITIVE_COUNT, uiPrimitiveCount);


}

//! Returns driver and operating system specific data about the IVideoDriver.
const EzExposedVideoData& EzVideoDriver::getExposedVideoData()
{
	return m_ExposedData;
}



//! Returns if a texture creation flag is enabled or disabled.
bool EzVideoDriver::getTextureCreationFlag(eTEXTURE_CREATION_FLAG flag) const
{
	return (m_uiTextureCreationFlags & flag)!=0;
}

//! Enables or disables a texture creation flag.
void EzVideoDriver::setTextureCreationFlag(eTEXTURE_CREATION_FLAG flag, bool enabled)
{
	//if (enabled && ((flag == ETCF_ALWAYS_16_BIT) || (flag == ETCF_ALWAYS_32_BIT)
	//	|| (flag == ETCF_OPTIMIZED_FOR_QUALITY) || (flag == ETCF_OPTIMIZED_FOR_SPEED)))
	//{
	//	// disable other formats
	//	setTextureCreationFlag(ETCF_ALWAYS_16_BIT, false);
	//	setTextureCreationFlag(ETCF_ALWAYS_32_BIT, false);
	//	setTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY, false);
	//	setTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED, false);
	//}

	// set flag
	m_uiTextureCreationFlags = (m_uiTextureCreationFlags & (~flag)) |
		((((unsigned int)!enabled)-1) & flag);
}



// draw
//! Draws a 3d axis aligned box.
void EzVideoDriver::draw3DBox(const EzAabbox3d& box, EzColor color)
{
	EzPoint3 edges[8];
	box.getEdges(edges);

	// TODO: optimize into one big drawIndexPrimitive call.
	draw3DLine(edges[5], edges[1], color);
	draw3DLine(edges[1], edges[3], color);
	draw3DLine(edges[3], edges[7], color);
	draw3DLine(edges[7], edges[5], color);
	draw3DLine(edges[0], edges[2], color);
	draw3DLine(edges[2], edges[6], color);
	draw3DLine(edges[6], edges[4], color);
	draw3DLine(edges[4], edges[0], color);
	draw3DLine(edges[1], edges[0], color);
	draw3DLine(edges[3], edges[2], color);
	draw3DLine(edges[7], edges[6], color);
	draw3DLine(edges[5], edges[4], color);
}

EzVideoDriver* EzVideoDriver::createDriver(EDRIVER_TYPE eType, const EzDimension2d& screenSize,	void* WindowID, unsigned int uiBits, bool bFullscreen, bool bStencilbuffer,
	bool bPureSoftware, bool bHighPrecisionFPU,	bool bVsync, EMULTISAMPLE_TYPE eMultiSampleType, bool bMuiltView)
{

	EZASSERT(!m_spVideoDriver);

	switch(eType)
	{
	case EDT_DIRECTX9:
		m_spVideoDriver = createDirectX9Driver(screenSize, (HWND) WindowID, uiBits, bFullscreen, bStencilbuffer, bPureSoftware, bHighPrecisionFPU,	bVsync, eMultiSampleType, bMuiltView);
		break;
	}

	m_spVideoDriver->m_pRenderer = EzNew EzRenderer(m_spVideoDriver);

	//init Shader

	EzShaderLibrary* pShderLib = EzShaderFactory::getFirstShaderLibrary();

	while(pShderLib)
	{
		EzShader* pShader;
		pShader = EzShaderFactory::getShader(pShderLib->getName().c_str());

		pShader->setDriver(m_spVideoDriver);

		pShderLib->setShader(pShader);

		pShderLib = EzShaderFactory::getNextShaderLibrary();
	}

	return m_spVideoDriver;

}
