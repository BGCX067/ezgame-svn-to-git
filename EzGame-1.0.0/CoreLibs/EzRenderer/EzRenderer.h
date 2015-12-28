#pragma once


#include "../EzMain3D/EzHardwareSkinPartition.h"
#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzArray.h"
#include "EzRenderPass.h"


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

	static void initializePools();
	static void shutdownPools();

	virtual u32 setupRenderingPass(EzGeometry* pGeometry, EzRendererData* pRendererData);
    virtual EzRendererData* prepareGeometryForRendering(EzGeometry* pGemData, EzSkinInstance* pSkinInstance);

	void setCamera(EzCamera* pCamera) {	m_pActiveCamera = pCamera;}
	EzCamera* getActiveCamera() { return m_pActiveCamera; }

	void renderShape(EzTriShape* pTriShape);
	void setupRender(EzGeometry* pGeometry);
	void updateRenderState();

	void begin();
	void end();

	EzVideoDriver* getVideoDriver();


protected:

	void bindRenderData(EzRendererData*	pRendererData);

	EzRendererData* packageMeshForSoftwareSkinning(EzTriBasedGeomData* pGemData, EzSkinInstance* pSkinInstance);
	EzRendererData* packageMeshForHardwareSkinning(EzTriBasedGeomData* pGemData, EzSkinInstance* pSkinInstance, EzHardwareSkinPartition* pHWSkinPartition);
	EzRendererData* packageMeshFromHWPartition(EzTriBasedGeomData* pGemData, EzHardwareSkinPartition::Partition* pPartition);
	EzRendererData* packageMeshStatic(EzTriBasedGeomData* pGemData);

	EzVideoDriver*	m_pDriver;
	EzRenderPassPtr m_spCurrentPass;

	EzCamera*		m_pActiveCamera;
};


inline EzVideoDriver* EzRenderer::getVideoDriver()
{
	return m_pDriver;
}