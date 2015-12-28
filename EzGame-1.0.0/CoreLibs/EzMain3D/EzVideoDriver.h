#pragma once


#include "..\EzCore\EzCore.h"
#include "EzTexture.h"
#include "EzImage.h"
#include "EzExposedVideoData.h"
#include "EzTexturingProperty.h"
#include "EzHardwareSkinPartition.h"
#include "EzRenderPass.h"

//#include "./D3D9Renderer/EzD3DX9TextureStage.h"




//! EzArray holding the built in material type names
const char* const sBuiltInMaterialTypeNames[] =
{
	"solid",
	"solid_2layer",
	"lightmap",
	"lightmap_add",
	"lightmap_m2",
	"lightmap_m4",
	"lightmap_light",
	"lightmap_light_m2",
	"lightmap_light_m4",
	"detail_map",
	"sphere_map",
	"reflection_2layer",
	"trans_add",
	"trans_alphach",
	"trans_alphach_ref",
	"trans_vertex_alpha",
	"trans_reflection_2layer",
	"normalmap_solid",
	"normalmap_trans_add",
	"normalmap_trans_vertexalpha",
	"parallaxmap_solid",
	"parallaxmap_trans_add",
	"parallaxmap_trans_vertexalpha",
	"onetexture_blend",
	0
};



class EzImageLoader;
class EzGeometry;
class EzGeometryData;
class EzDynamicEffectState;
class EzSkinInstance;
class EzLight;
class EzRendererData;
class EzSkinInstance;
class EzRenderEntities;
class EzVertexData;
class EzTriBasedGeomData;
class EzRenderer;
class EzVertexDeclaration;
class EzMaterialProperty;


class EzVideoDriver : public EzRefObject
{
	EzDeclareRootRTTI(EzVideoDriver);

public:

	//! An enum for all types of drivers the EzMain3D Engine supports.
	enum EDRIVER_TYPE
	{
		EDT_NULL,
		EDT_DIRECTX9
	};


	static EzVideoDriver* createDriver(EDRIVER_TYPE eType, const EzDimension2d& screenSize,	void* WindowID, unsigned int uiBits, bool bFullscreen, bool bStencilbuffer,
		bool bPureSoftware, bool bHighPrecisionFPU,	bool bVsync, EMULTISAMPLE_TYPE eMultiSampleType, bool bMuiltView);
	virtual void release() = 0;

	virtual bool isHardwareSkinning() { return false; }


	virtual	EzVertexData* createVertexData(EzTriBasedGeomData* pGemData, EzHardwareSkinPartition::Partition* pPartition, 
		unsigned int& uiPosOffset, unsigned int& uiWeightOffset, unsigned int& uiBoneIndexOffset, unsigned int& uiNormOffset, unsigned int& uiColorOffset, unsigned int& uiTexOffset) = 0;
	virtual bool setMultiSample(EMULTISAMPLE_TYPE eMultiSampleType) = 0;
	virtual void setTargetWindow(void* WindowID) = 0;
	virtual bool setResize(void* WindowId, int iWidth, int iHigth) = 0;
	virtual bool createMultiRenderer(void* WindowID, int iWidth, int iHigth) = 0;

	virtual bool setResize(const EzDimension2d& size, void* WindowId) = 0;

	//draw funtion start
	/////////////////
	virtual void draw3DLine(const EzPoint3& start, const EzPoint3& end, EzColor color) = 0;
	void draw3DBox(const EzAabbox3d& box, EzColor color);
	//draw funtion end
	//////////////////



	virtual void drawBuffer(EzRendererData* pRendererData) = 0;
	virtual void render(EzGeometry* pGeometry) = 0;

	virtual void setTransform(ETRANSFORMATION_STATE state,const EzMatrix4& mat) = 0;
	
	virtual bool queryFeature(EVIDEO_DRIVER_FEATURE feature) const = 0;

	virtual unsigned int getMaximalDynamicLightAmount() const = 0;
	virtual void setAmbientLight(const EzColor& color) = 0;

	// add state
	virtual void setEffectState(EzDynamicEffectState* pEffectState) = 0;
//	virtual bool setTexture(unsigned int eMap, EzTextureDesc* pTexDesc) = 0;
	virtual void setLighting(bool bLight) = 0;
	virtual	bool setVertexDeclaration(EzVertexDeclaration* decl) = 0;
	virtual bool setMeshBufferDataBinding(EzVertexData* pVertexData) = 0;


	virtual void setMaterial(EzMaterialProperty* pMat) = 0;

	
	unsigned int getTextureBlendStagesCount(); 


	bool getTextureCreationFlag(eTEXTURE_CREATION_FLAG flag) const;

	const EzExposedVideoData& getExposedVideoData();

	//! returns a device dependent texture from a software surface (EzImage)
	virtual EzTexture* createDeviceDependentTexture(EzImage* surface, const char* name) = 0;

	virtual void setTextureCreationFlag(eTEXTURE_CREATION_FLAG flag, bool enabled);


	inline EzRenderer* getRenderer() { return m_pRenderer;}

// debug data
	inline unsigned int getPrimitiveCount() { return m_uiPrimitiveCount; }

	inline void texturePipeline(EzTextureStage* pTexStage);
	inline void disableStage(unsigned int uiStage);




protected:
	EzVideoDriver(void);
	virtual ~EzVideoDriver(void);

	virtual bool beginScene(bool backBuffer, bool zBuffer, const EzColor& color);
	virtual bool endScene();


	EzRenderer*					m_pRenderer;
	static EzVideoDriver* 		m_spVideoDriver;

	virtual void setDynamicLight(const unsigned int uindex, EzLight* pLight) = 0;
	virtual void clearDynamicLights() = 0;
	virtual void drawVertexPrimitive(EPRIMITIVE_TYPE eType, unsigned int uiVertexStart, unsigned int uiIndexNum, unsigned int uiIndexStart);

	ETEXTURE_FILTERMODE				m_eLastTextureFilterMode;
	ETEXTURE_CLAMP					m_eLastTextureClampMode;
	ETEXTURE_APPLY_MODE				m_eLastTextureApplyMode;


	unsigned int								m_uiMaxTextureBlendStages;

	bool							m_bFeatureEnabled[EVDF_COUNT];   // queries the features of the driver

	EzColor							m_lastAmbientColor; 
	EzColor							m_lastDiffuseColor;
	EzColor							m_lastSpecularColor;
	EzColor							m_lastEmissiveColor;
	float								m_flastShininess;

	EzColor							m_AmbientColor; 
	EzColor							m_DiffuseColor;
	EzColor							m_SpecularColor;
	EzColor							m_EmissiveColor;
	float							m_fSininess;


	unsigned int								m_uiTextureCreationFlags;


	bool							m_bFullscreen;
	bool							m_bStencilBuffer;

	EzExposedVideoData				m_ExposedData;

	EzDynamicEffectState*			m_lastDynamicEffectState;

	EzRenderEntities*				m_pEntity;

	unsigned int								m_uiMatLastRevisionID;

	// debug data
	unsigned int		m_uiPrimitiveCount;


	friend EzRenderer;
	
};

inline void EzVideoDriver::texturePipeline(EzTextureStage* pTexStage)
{
//	m_pTextureStage->texturePipeline(pTexStage);

}

inline void EzVideoDriver::disableStage(unsigned int uiStage)
{
//	m_pTextureStage->disableStage(uiStage);
//	m_pTextureStage->setTextureBlendStageCount(uiStage);

}


inline unsigned int EzVideoDriver::getTextureBlendStagesCount()
{
	return 1;
	//return m_uiMaxTextureBlendStages;

}