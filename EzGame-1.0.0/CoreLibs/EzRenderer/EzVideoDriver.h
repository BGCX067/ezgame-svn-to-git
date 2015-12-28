#pragma once


#include "..\EzCore\EzCore.h"
#include "EzTexture.h"
#include "EzImage.h"
#include "SExposedVideoData.h"
#include "../EzMain3D/EzTexturingProperty.h"
#include "../EzMain3D/EzHardwareSkinPartition.h"
#include "../EzCore/EzRefObject.h"
#include "EzRenderPass.h"

#include "./D3D9Renderer/EzD3DX9TextureStage.h"




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

	EzVideoDriver(void);
	~EzVideoDriver(void);


	virtual bool isHardwareSkinning() { return false; }


	virtual	EzVertexData* createVertexData(EzTriBasedGeomData* pGemData, EzHardwareSkinPartition::Partition* pPartition, 
		u32& uiPosOffset, u32& uiWeightOffset, u32& uiBoneIndexOffset, u32& uiNormOffset, u32& uiColorOffset, u32& uiTexOffset) = 0;
	virtual bool setMultiSample(EMULTISAMPLE_TYPE eMultiSampleType) = 0;
	virtual void setTargetWindow(void* WindowID) = 0;
	virtual bool setResize(void* WindowId, s32 iWidth, s32 iHigth) = 0;
	virtual bool createMultiRenderer(void* WindowID, s32 iWidth, s32 iHigth) = 0;

	virtual bool beginScene(bool backBuffer = true, bool zBuffer = true, SColor color = SColor(255, 255, 255, 255));
	virtual bool endScene();
	virtual bool setResize(const EzDimension2d<s32>& size, void* WindowId) = 0;

	//draw funtion start
	/////////////////
	virtual void draw3DLine(const EzPoint3& start, const EzPoint3& end, SColor color) = 0;
	void draw3DBox(const EzAabbox3d<f32>& box, SColor color);
	//draw funtion end
	//////////////////



	virtual void drawBuffer(EzRendererData* pRendererData) = 0;
	virtual void render(EzGeometry* pGeometry) = 0;

	virtual void setTransform(ETRANSFORMATION_STATE state,const EzMatrix4& mat) = 0;
	
	virtual bool queryFeature(EVIDEO_DRIVER_FEATURE feature) const = 0;

	virtual u32 getMaximalDynamicLightAmount() const = 0;
	virtual void setAmbientLight(const EzColor& color) = 0;

	// add state
	virtual void setEffectState(EzDynamicEffectState* pEffectState) = 0;
	virtual bool setTexture(u32 eMap, EzTextureDesc* pTexDesc) = 0;
	virtual void setLighting(bool bLight) = 0;
	virtual	bool setVertexDeclaration(EzVertexDeclaration* decl) = 0;
	virtual bool setMeshBufferDataBinding(EzVertexData* pVertexData) = 0;


	virtual void setMaterial(EzMaterialProperty* pMat) = 0;

	
	u32 getTextureBlendStagesCount(); 


	bool getTextureCreationFlag(eTEXTURE_CREATION_FLAG flag) const;

	const SExposedVideoData& getExposedVideoData();

	//! returns a device dependent texture from a software surface (EzImage)
	virtual EzTexture* createDeviceDependentTexture(EzImage* surface, const c8* name) = 0;

	virtual void setTextureCreationFlag(eTEXTURE_CREATION_FLAG flag, bool enabled);


	inline EzRenderer* getRenderer() { return m_pRenderer;}

// debug data
	inline u32 getPrimitiveCount() { return m_uiPrimitiveCount; }

	inline void texturePipeline(EzTextureStage* pTexStage);
	inline void disableStage(u32 uiStage);




protected:

	EzRenderer*					m_pRenderer;

	EzD3DX9TextureStage*		m_pTextureStage;

	virtual void setDynamicLight(const u32 uindex, EzLight* pLight) = 0;
	virtual void clearDynamicLights() = 0;
	virtual void drawVertexPrimitive(EPRIMITIVE_TYPE eType, u32 uiVertexStart, u32 uiIndexNum, u32 uiIndexStart);

	ETEXTURE_FILTERMODE				m_eLastTextureFilterMode;
	ETEXTURE_CLAMP					m_eLastTextureClampMode;
	ETEXTURE_APPLY_MODE				m_eLastTextureApplyMode;


	u32								m_uiMaxTextureBlendStages;

	bool							m_bFeatureEnabled[EVDF_COUNT];   // queries the features of the driver

	EzColor							m_lastAmbientColor; 
	EzColor							m_lastDiffuseColor;
	EzColor							m_lastSpecularColor;
	EzColor							m_lastEmissiveColor;
	f32								m_flastShininess;

	EzColor							m_AmbientColor; 
	EzColor							m_DiffuseColor;
	EzColor							m_SpecularColor;
	EzColor							m_EmissiveColor;
	f32								m_fSininess;


	u32								m_uiTextureCreationFlags;


	bool							m_bFullscreen;
	bool							m_bStencilBuffer;

	SExposedVideoData				m_ExposedData;

	EzDynamicEffectState*			m_lastDynamicEffectState;

	EzRenderEntities*				m_pEntity;

	u32								m_uiMatLastRevisionID;

	// debug data
	u32		m_uiPrimitiveCount;
	
};

inline void EzVideoDriver::texturePipeline(EzTextureStage* pTexStage)
{
	m_pTextureStage->texturePipeline(pTexStage);

}

inline void EzVideoDriver::disableStage(u32 uiStage)
{
	m_pTextureStage->disableStage(uiStage);
	m_pTextureStage->setTextureBlendStageCount(uiStage);

}


inline u32 EzVideoDriver::getTextureBlendStagesCount()
{
	return 1;
	//return m_uiMaxTextureBlendStages;

}