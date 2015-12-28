#pragma once

#include "EzAVObject.h"
#include "EzGeometryData.h"
#include "EzSkinInstance.h"
#include "EzDynamicEffectState.h"
#include "EzRenderer.h"
#include "EzShaderLibrary.h"



EzSmartPointer(EzGeometry);

class EzRenderPass;
class EzShader;

class EzGeometry : public EzAVObject
{
	EzDeclareRTTI;
	EzDeclareStream;
	EzDeclareCopyMember(EzGeometry);

public:
	EzGeometry(void);
	~EzGeometry(void);

	EzGeometryData* getData() const;
	//
	//! Sets a Geomertry data
	void setData( EzGeometryData * data );
	void setApplyMode( ETEXTURE_APPLY_MODE eApplyMode ) { m_ApplyMode = eApplyMode; }

	virtual void onVisible(EzCullingProcess& Culler);
	virtual void updateWorldBound();
	virtual void render(EzRenderer* pRenderer);

	virtual void updateEffectsDownPass(EzDynamicEffectState* pParentState);

	// set state
	EzDynamicEffectStatePtr getEffectState() const { return m_pEffectState; }
	void setEffectState(EzDynamicEffectState* pkState) { m_pEffectState = pkState; }

	//!
	// Gets the EzSkinInstance object used by this geometry node, if any.
	// \return The EzSkinInstance object used by this geometry node, or NULL if none is used.
	EzSkinInstance* getSkinInstance() const;
	
	//! Sets a SkinInstance
	void setSkinInstance(EzSkinInstance* skinInstance);

	void setRendererData(EzRendererData* pRendererData);
	EzRendererData* getRendererData();

	void setRenderPass(EzRenderPass* pRenderPass);
	EzRenderPass* getRenderPass();


	

	void setShaderLibrary(EzShaderLibrary* pShaderLib);

	EzShader* getShader();

protected:

	EzDynamicEffectStatePtr		m_pEffectState;


	EzGeometryDataPtr			m_pGeometryData;
	EzSkinInstancePtr			m_pSkinInstance;

	ETEXTURE_APPLY_MODE			m_ApplyMode;

	EzRenderPassPtr				m_spRenderPass;

	EzRendererData*				m_pRendererData;
	EzShaderLibrary*			m_pShaderLib;
};


inline EzShader* EzGeometry::getShader()
{
	return m_pShaderLib ? m_pShaderLib->getShader() : NULL;
}

inline void EzGeometry::setShaderLibrary(EzShaderLibrary* pShaderLib)
{
	m_pShaderLib = pShaderLib;
}

inline EzRenderPass* EzGeometry::getRenderPass()
{
	return m_spRenderPass;
}

inline void EzGeometry::setRenderPass(EzRenderPass* pRenderPass)
{
	m_spRenderPass = pRenderPass;
}


inline void EzGeometry::setRendererData(EzRendererData* pRendererData)
{
	m_pRendererData = pRendererData;
}

inline EzRendererData* EzGeometry::getRendererData()
{
	return m_pRendererData;

}

inline void EzGeometry::render(EzRenderer* pRenderer)
{
	pRenderer->setupRender(this);
}

