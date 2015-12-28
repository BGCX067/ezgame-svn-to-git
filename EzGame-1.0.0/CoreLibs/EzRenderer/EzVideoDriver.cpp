#include "EzVideoDriver.h"
#include "EzImageLoader.h"
#include "../EzCore/EzFile.h"
#include "../EzCore/EzMeasurePerformance.h"

#include "../EzMain3D/EzGeometryData.h"



EzImplementRootRTTI(EzVideoDriver);

EzVideoDriver::EzVideoDriver(void)
: m_uiTextureCreationFlags(0), m_AmbientColor(1.0f, 1.0f, 1.0f, 1.0f), m_DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
  ,m_EmissiveColor(0.0f, 0.0f, 0.0f, 0.0f), m_SpecularColor(1.0f, 1.0f, 1.0f, 1.0f), m_fSininess(0.0f), m_lastDynamicEffectState(0), m_pEntity(NULL)
  , m_uiMatLastRevisionID(UINT_MAX), m_eLastTextureFilterMode(FILTER_NEAREST), m_eLastTextureClampMode(ETC_CLAMP_U_CLAMP_V)
  , m_eLastTextureApplyMode(APPLY_MODULATE), m_uiMaxTextureBlendStages(0), m_pRenderer(NULL)
{
	setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true);

	for (u32 i=0; i<EVDF_COUNT; ++i)
		m_bFeatureEnabled[i]=true;

}

EzVideoDriver::~EzVideoDriver(void)
{
}

bool EzVideoDriver::beginScene(bool backBuffer, bool zBuffer, SColor color)
{
	m_uiPrimitiveCount = 0;


	return true;
}


bool EzVideoDriver::endScene()
{
	return true;
}



void EzVideoDriver::drawVertexPrimitive(EPRIMITIVE_TYPE eType, u32 uiVertexStart, u32 uiIndexNum, u32 uiIndexStart)
{
	u32 uiPrimitiveCount = 0;	
	switch(eType)
	{

	case EPT_POINTS:
	case EPT_LINES:
	case EPT_LINE_STRIP:
		break;

	case EPT_TRIANGLE_LIST:
		uiPrimitiveCount = (u32) uiIndexNum / 3;
		break;

	case EPT_TRIANGLE_STRIP:
	case EPT_TRIANGLE_FAN:
		uiPrimitiveCount = (u32) uiIndexNum - 2;
		break;

	}

	EZMEASURE_PERFORMANCE_AGGREGATEVALUE(PRIMITIVE_COUNT, uiPrimitiveCount);


}

//! Returns driver and operating system specific data about the IVideoDriver.
const SExposedVideoData& EzVideoDriver::getExposedVideoData()
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
		((((u32)!enabled)-1) & flag);
}



// draw
//! Draws a 3d axis aligned box.
void EzVideoDriver::draw3DBox(const EzAabbox3d<f32>& box, SColor color)
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
