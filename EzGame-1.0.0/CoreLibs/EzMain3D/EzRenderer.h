#pragma once


#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzArray.h"
#include "EzCamera.h"
#include "EzRenderPass.h"
#include "EzHardwareSkinPartition.h"


class EzRendererData;
class EzTriBasedGeomData;
class EzSkinInstance;
class EzHardwareSkinPartition;
class EzVideoDriver;
class EzRenderPass;
class EzGeometryData;
class EzGeometry;
class EzTriShape;
class EzCamera;

class EzRenderer : public EzRefObject
{
public:
	EzRenderer(EzVideoDriver* pDriver);
	~EzRenderer(void);

	virtual unsigned int setupRenderingPass(EzGeometry* pGeometry, EzRendererData* pRendererData);
    virtual EzRendererData* prepareGeometryForRendering(EzGeometry* pGemData, EzSkinInstance* pSkinInstance);

	void setCamera(EzCamera* pCamera) {	m_spActiveCamera = pCamera;}
	EzCamera* getActiveCamera() { return m_spActiveCamera; }

	void renderShape(EzTriShape* pTriShape);
	void setupRender(EzGeometry* pGeometry);
	void updateRenderState();

	void setBackgroundColor(EzColor color);

	void begin();
	void end();

	EzVideoDriver* getVideoDriver();


protected:

	void bindRenderData(EzRendererData*	pRendererData);

	EzRendererData* packageMeshForSoftwareSkinning(EzTriBasedGeomData* pGemData, EzSkinInstance* pSkinInstance);
	EzRendererData* packageMeshForHardwareSkinning(EzTriBasedGeomData* pGemData, EzSkinInstance* pSkinInstance, EzHardwareSkinPartition* pHWSkinPartition);
	EzRendererData* packageMeshFromHWPartition(EzTriBasedGeomData* pGemData, EzHardwareSkinPartition::Partition* pPartition);
	EzRendererData* packageMeshStatic(EzTriBasedGeomData* pGemData);

	EzVideoDriver*			m_pDriver;
	EzRenderPassPtr 		m_spCurrentPass;
	EzCameraPtr				m_spActiveCamera;
	EzColor					m_BackGroundColor;
};


inline EzVideoDriver* EzRenderer::getVideoDriver()
{
	return m_pDriver;
}

inline void EzRenderer::setBackgroundColor(EzColor color)
{
	m_BackGroundColor = color;
}