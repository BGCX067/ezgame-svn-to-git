#include "EzRenderer.h"
#include "EzVideoDriver.h"

#include "../EzCore/EzSystem.h"
#include "../EzMain3D/EzDirectionalLight.h"
#include "../EzMain3D/EzRendererData.h"
#include "../EzMain3D/EzGeometryData.h"
#include "../EzMain3D/EzSkinInstance.h"
#include "../EzMain3D/EzTexturingProperty.h"
#include "../EzMain3D/EzMaterialProperty.h"
#include "../EzMain3D/EzTriBasedGeomData.h"
#include "../EzMain3D/EzCamera.h"
#include "../EzMain3D/EzTriShape.h"
#include "../EzMain3D/EzShader.h"
#include "../EzApplication/EzLogReceiver.h"

#include "Ez3DBufferManager.h"
#include "EzVertexBufferBinding.h"
#include "EzVertexData.h"
#include "EzIndexData.h"
#include "EzRenderPass.h"
#include "EzTextureStageGroup.h"

EzRenderer::EzRenderer(EzVideoDriver* pDriver)
:m_pDriver(pDriver), m_spCurrentPass(NULL)
{
}

EzRenderer::~EzRenderer(void)
{
}

void EzRenderer::initializePools()
{
    EzRenderPass::initializePools();
	EzTextureStageGroup::initializePools();
	EzTextureStage::initializePools();
}

void EzRenderer::shutdownPools()
{
    EzRenderPass::shutdownPools();
	EzTextureStageGroup::shutdownPools();
	EzTextureStage::shutdownPools();
   
}

EzRendererData* EzRenderer::packageMeshFromHWPartition(EzTriBasedGeomData* pGemData, EzHardwareSkinPartition::Partition* pPartition)
{
	EzRendererData* pRendererData = EzNew EzRendererData;
	u32 uiPosOffset;
	u32 uiWeightOffset;
	u32 uiBoneIndexOffset;
	u32 uiNormOffset;
	u32 uiColorOffset;
	u32 uiTexOffset;


	EzVertexData* pVertexData = m_pDriver->createVertexData(pGemData, pPartition, uiPosOffset, 
		uiWeightOffset, uiBoneIndexOffset, uiNormOffset, uiColorOffset, uiTexOffset);


	pRendererData->setVertexData(pVertexData);

	if(pPartition->m_usStripCount > 0)
	{
		pRendererData->setPrimitiveType(EPT_TRIANGLE_STRIP);

		u16 usStripStart = 0;
		u16* pLockIndex;
		u16* pIndex;

		EzIndexBuffer* pIndexBuffer = createIndexBuffer(EzIndexBuffer::EIT_16BIT, pGemData->getNumTrianglePoints(), Ez3DBuffer::ESTATIC);
		pLockIndex = (u16*)pIndexBuffer->lock(EzIndexBuffer::ENORMAL);

		for(u16 usStripCount = 0; usStripCount < pPartition->m_usStripCount; ++usStripCount )
		{
			pIndex = &pPartition->m_pusTriList[usStripStart];

			for(u16 usStripLengths = 0; usStripLengths < pPartition->m_pusStripLengths[usStripCount]; ++usStripLengths)
			{
				pLockIndex[usStripLengths] = pIndex[usStripLengths];

			}

			usStripStart += pPartition->m_pusStripLengths[usStripCount];

		}

		pIndexBuffer->unlock();

		EzIndexData* pIndexData = EzNew EzIndexData(pIndexBuffer);

		pRendererData->setIndexData(pIndexData);

	}
	else
	{
		pRendererData->setPrimitiveType(EPT_TRIANGLE_LIST);
	}


	u32 uiCount = 0;
	EzVertexBuffer* pVertexStream0 = pVertexData->getVertexBuffer(0);
	const EzPoint3* pVertices = pGemData->getVertices();
	const EzPoint3* pNomals = pGemData->getVertexNormals();
	u16* pusVertMap = pPartition->m_pusVertexMap;
	u8* pLockVertex;

	pLockVertex = (u8*) pVertexStream0->lock(Ez3DBuffer::ENORMAL);

	u32 uiVertexStride = pVertexStream0->getStride();

	for (u32 ui = 0; ui < pPartition->m_usVertexCount; ++ui, pLockVertex += uiVertexStride)
	{
		if(uiPosOffset != UINT_MAX)
		{
			EzMemoryCopy(&pLockVertex[uiPosOffset], &pVertices[pusVertMap[ui]], sizeof(EzPoint3));
		}

		if(uiNormOffset != UINT_MAX)
		{
			EzMemoryCopy(&pLockVertex[uiNormOffset], &pNomals[pusVertMap[ui]], sizeof(EzPoint3));
		}

		if(uiWeightOffset != UINT_MAX)
		{
			f32* pweights;
			f32* pBoneIndex =(f32*)(u8*)(pLockVertex + uiBoneIndexOffset);
			pweights = (f32*)(u8*)(pLockVertex + uiWeightOffset);

			for(u16 us = 0; us < pPartition->m_usWeightsPerVertexCount; ++us)
			{
				pweights[us] = pPartition->m_pfWeights[uiCount];
				pBoneIndex[us] = pPartition->m_pusBones[uiCount];
				uiCount++;
			}
		}
	}

	pVertexStream0->unlock();

	pRendererData->setHardwareSkinPartition(pPartition);
	pRendererData->setAnimationType(EzRendererData::E_HW_SKINNING);	

	return pRendererData;

}


EzRendererData* EzRenderer::packageMeshForHardwareSkinning(EzTriBasedGeomData* pGemData, EzSkinInstance* pSkinInstance,  EzHardwareSkinPartition* pHWSkinPartition)
{

	EzHardwareSkinPartition::Partition* pPartition;
	EzRendererData* pHeadRendererData = NULL;
	EzRendererData* pNextRendererData = NULL;
	EzRendererData* pRendererData = NULL;
	
	for(u16 us = 0; us < pHWSkinPartition->getPartitionCount(); ++us)
	{
		pPartition =  &pHWSkinPartition->getPartitions()[us];
				
		pRendererData = packageMeshFromHWPartition(pGemData, pPartition);
			
		if(!pHeadRendererData)
		{
			pHeadRendererData = pRendererData;
			pNextRendererData = pRendererData;
		}
		else
		{
			pNextRendererData->setNextRendererData(pRendererData);
			pNextRendererData = pRendererData;
		}
	}

	return pHeadRendererData;

}



EzRendererData* EzRenderer::packageMeshStatic(EzTriBasedGeomData* pGemData)
{
	EzVertexDeclaration* pVertexDec;
	EzRendererData* pRendererData;


	pRendererData = EzNew EzRendererData;

	// create index data
	u16* pLockIndex;
	EzIndexBuffer* pIndexBuffer = createIndexBuffer(EzIndexBuffer::EIT_16BIT, pGemData->getNumIndexSize(), Ez3DBuffer::ESTATIC);

	pLockIndex = (u16*)pIndexBuffer->lock(EzIndexBuffer::ENORMAL);

	pGemData->getIndexData(pLockIndex);

	pIndexBuffer->unlock();

	EzIndexData* pIndexData = EzNew EzIndexData(pIndexBuffer);

	pRendererData->setIndexData(pIndexData);


	EzVertexBuffer* pVertexStream0;
	u8*	pLockVertex;
	u32 uiPosOffset;
	u32 uiWeightOffset;
	u32 uiBoneIndexOffset;
	u32 uiNormOffset;
	u32 uiColorOffset;
	u32 uiTexOffset;
	EzVertexData* pVertexData = m_pDriver->createVertexData(pGemData, NULL, uiPosOffset, uiWeightOffset, uiBoneIndexOffset, uiNormOffset, uiColorOffset, uiTexOffset);
	EzVertexBufferBinding* pVertexBufferBinding = pVertexData->getVertexBinding();

	pVertexStream0 = pVertexBufferBinding->getBuffer(0);
	pVertexDec = pVertexData->getVertexDeclaration();

	pLockVertex = (u8*) pVertexStream0->lock(Ez3DBuffer::ENORMAL);

	for(u32 ui = 0; ui < pVertexStream0->getNumVertices(); ++ui, pLockVertex += pVertexStream0->getStride())
	{
		if(uiPosOffset != UINT_MAX)
		{
			EzMemoryCopy(&pLockVertex[uiPosOffset], &pGemData->getVertices()[ui], sizeof(EzPoint3));
		}
	
		if(uiNormOffset != UINT_MAX)
		{
			EzMemoryCopy(&pLockVertex[uiNormOffset], &pGemData->getVertexNormals()[ui], sizeof(EzPoint3));
		}
	}

	pVertexStream0->unlock();

	pRendererData->setVertexData(pVertexData);
	pRendererData->setPrimitiveType(pGemData->getPrimitiveType());


	return pRendererData;
}



EzRendererData* EzRenderer::packageMeshForSoftwareSkinning(EzTriBasedGeomData* pGemData, EzSkinInstance* pSkinInstance)
{
	const EzVertexElement* pElementTemp;
	EzVertexDeclaration* pVertexDec;
	EzRendererData* pRendererData;

	u32 uiPosOffset;
	u32 uiWeightOffset;
	u32 uiBoneIndexOffset;
	u32 uiNormOffset;
	u32 uiColorOffset;
	u32 uiTexOffset;
	u8* pLockVertex;

	pRendererData = EzNew EzRendererData;

	pVertexDec = generateVertexStream(pGemData, NULL, uiPosOffset, uiWeightOffset,
			uiBoneIndexOffset, uiNormOffset, uiColorOffset,	uiTexOffset);

	EzVertexBufferBinding* pVertexBinding = createVertexBufferBinding();

	EzVertexBuffer* pVertexBuffer0 = createVertexBuffer(pVertexDec->getStride(0), pGemData->getNumVertex(), Ez3DBuffer::EDYNAMIC_WRITE_ONLY);

	pVertexBinding->setBinding(0, pVertexBuffer0);

	EzVertexBuffer* pVertexColorBuffer = createVertexBuffer(pVertexDec->getStride(0), pGemData->getNumVertex(), Ez3DBuffer::EDYNAMIC_WRITE_ONLY);

	pVertexBinding->setBinding(1, pVertexColorBuffer);


	EzVertexBuffer* pVertexBuffer1 = createVertexBuffer(pVertexDec->getStride(1), pGemData->getNumVertex(), Ez3DBuffer::ESTATIC_WRITE_ONLY);

	pVertexBinding->setBinding(2, pVertexBuffer1);


	pRendererData->setVertexData(EzNew EzVertexData(pVertexDec, pVertexBinding));
	pRendererData->setPrimitiveType(pGemData->getPrimitiveType());


	pLockVertex = (u8*) pVertexBuffer1->lock(Ez3DBuffer::ENORMAL);

	for(u32 ui = 0; ui < pVertexBuffer1->getNumVertices(); ++ui, pLockVertex += pVertexBuffer1->getStride())
	{
		if(pGemData->getVertexColors())
		{
			pElementTemp = pVertexDec->findElementBySemantic(EzVertexElement::EVES_DIFFUSE);
			EZASSERT(pElementTemp);
			EzMemoryCopy(&pLockVertex[pElementTemp->getOffset()], &pGemData->getVertexColors()[ui], pElementTemp->getSize());
		}

		if(pGemData->getTexCoords())
		{
			pElementTemp = pVertexDec->findElementBySemantic(EzVertexElement::EVES_TEXTURE_COORDINATES);
			EZASSERT(pElementTemp);
			EzMemoryCopy(&pLockVertex[pElementTemp->getOffset()], &pGemData->getTexCoords()[ui], pElementTemp->getSize());
		}

	}


	pVertexBuffer1->unlock();


	u16* pLockIndex;
	EzIndexBuffer* pIndexBuffer = createIndexBuffer(EzIndexBuffer::EIT_16BIT, pGemData->getNumTrianglePoints(), Ez3DBuffer::ESTATIC_WRITE_ONLY);

	pLockIndex = (u16*)pIndexBuffer->lock(EzIndexBuffer::ENORMAL);

	pGemData->getIndexData(pLockIndex);

	pIndexBuffer->unlock();

	EzIndexData* pIndexData = EzNew EzIndexData(pIndexBuffer);

	pRendererData->setIndexData(pIndexData);

	pRendererData->setAnimationType(EzRendererData::E_SW_SKINNING);	


	return pRendererData;
}


EzRendererData* EzRenderer::prepareGeometryForRendering(EzGeometry* pGeometry, EzSkinInstance* pSkinInstance)
{

	EzTriBasedGeomData* pBaseGemData = EzDynamicCast(EzTriBasedGeomData, pGeometry->getData());
	EzRendererData* pRendererData = pGeometry->getRendererData();
	EZASSERT(pBaseGemData);

	if(!pRendererData)
	{
		if(pSkinInstance)
		{
			 EzHardwareSkinPartition* pHWSkinPartition = pSkinInstance->getHWSkinPartition();

			if(pHWSkinPartition && m_pDriver->isHardwareSkinning())
			{
				pRendererData = packageMeshForHardwareSkinning(pBaseGemData, pSkinInstance, pHWSkinPartition);
			}
			else
			{
				pRendererData = packageMeshForSoftwareSkinning(pBaseGemData, pSkinInstance);
			}
		}
		else
		{
			pRendererData = packageMeshStatic(pBaseGemData);
		}

		pGeometry->setRendererData(pRendererData);
	}

	return pRendererData;
}

u32 EzRenderer::setupRenderingPass(EzGeometry* pGeometry, EzRendererData* pRendererData)
{

	m_spCurrentPass = pGeometry->getRenderPass();

	if(m_spCurrentPass == NULL)
	{
		m_spCurrentPass = EzRenderPass::createNewPass();

		pGeometry->setRenderPass(m_spCurrentPass);
	}

	m_spCurrentPass->setupRenderingPass(pGeometry, pRendererData);

	return 0;

}

void EzRenderer::bindRenderData(EzRendererData*	pRendererData)
{
	EzVertexData* pVertexData;
	EzVertexDeclaration* pVertexDeclaration;

	pVertexData = pRendererData->getVertexData();
	pVertexDeclaration = pVertexData->getVertexDeclaration();

	m_pDriver->setVertexDeclaration(pVertexDeclaration);
	m_pDriver->setMeshBufferDataBinding(pVertexData);
}

void EzRenderer::renderShape(EzTriShape* pTriShape)
{
	EzGPUProgram* pVertexProgram = NULL;
	EzRendererData*	pRendererData;

	EzShader* pShader = pTriShape->getShader();


	pShader->beginPipeline(pTriShape);


	m_pDriver->setTransform( ETS_WORLD, pTriShape->getWorldTransform().getMatrix());


	EzGeometry* pGeometry = EzDynamicCast(EzGeometry, pTriShape);
	EzPropertyState* pPropState = pGeometry->getPropertyState();

	pShader->updatePipeline(pGeometry, pPropState);

	do 
	{
		pRendererData = m_spCurrentPass->getRenderData();

		bindRenderData(pRendererData);

		m_spCurrentPass->updateStage(m_pDriver);

		m_pDriver->drawBuffer(pRendererData);

	} while (m_spCurrentPass && m_spCurrentPass->getNext());

	pShader->endPipeline(pTriShape);
}

void EzRenderer::begin()
{
	m_pDriver->beginScene(true, true, SColor(255,100,101,140));

	m_pDriver->setTransform ( ETS_PROJECTION, IdentityMatrix );
	m_pDriver->setTransform ( ETS_VIEW, IdentityMatrix );
	m_pDriver->setTransform ( ETS_WORLD, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_0, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_1, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_2, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_3, IdentityMatrix );

	if(m_pActiveCamera)
	{
		m_pActiveCamera->update(0);

		m_pActiveCamera->render(m_pDriver);

		//m_vCamWorldPos = m_pActiveCamera->getWorldPosition();
	}

}


void EzRenderer::end()
{
	m_pDriver->endScene();
}

void EzRenderer::setupRender(EzGeometry* pGeometry)
{
	EzRendererData* pRendererData = prepareGeometryForRendering(pGeometry, pGeometry->getSkinInstance());


	EzDynamicEffectState* pEffectState = pGeometry->getEffectState();


	EzPropertyState* pPropState = pGeometry->getPropertyState();

	//m_pDriver->setShadingMode(ESHADE_GOURAUD);

	if(pEffectState)
	{
		m_pDriver->setLighting(true);
		m_pDriver->setEffectState(pEffectState);
		m_pDriver->setMaterial(pPropState->getMaterial());
	}
	else
	{
		m_pDriver->setLighting(false);
		m_pDriver->setMaterial(NULL);
	}





	setupRenderingPass(pGeometry, pRendererData);


//	prePareRanderTexture(pGeometry);
}
