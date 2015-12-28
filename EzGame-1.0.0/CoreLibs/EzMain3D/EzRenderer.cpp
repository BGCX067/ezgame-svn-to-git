#include "EzRenderer.h"
#include "EzVideoDriver.h"

#include "../EzCore/EzSystem.h"
#include "EzDirectionalLight.h"
#include "EzRendererData.h"
#include "EzGeometryData.h"
#include "EzSkinInstance.h"
#include "EzTexturingProperty.h"
#include "EzMaterialProperty.h"
#include "EzTriBasedGeomData.h"
#include "EzCamera.h"
#include "EzTriShape.h"
#include "EzShader.h"


#include "Ez3DBufferManager.h"
#include "EzVertexBufferBinding.h"
#include "EzVertexData.h"
#include "EzIndexData.h"


EzRenderer::EzRenderer(EzVideoDriver* pDriver)
:m_pDriver(pDriver), m_spCurrentPass(NULL), m_spActiveCamera(NULL), m_BackGroundColor(0, 0, 0)
{
}

EzRenderer::~EzRenderer(void)
{
	m_spActiveCamera = NULL;
	m_spCurrentPass = NULL;
}

EzRendererData* EzRenderer::packageMeshFromHWPartition(EzTriBasedGeomData* pGemData, EzHardwareSkinPartition::Partition* pPartition)
{
	EzRendererData* pRendererData = EzNew EzRendererData;
	unsigned int uiPosOffset;
	unsigned int uiWeightOffset;
	unsigned int uiBoneIndexOffset;
	unsigned int uiNormOffset;
	unsigned int uiColorOffset;
	unsigned int uiTexOffset;


	EzVertexData* pVertexData = m_pDriver->createVertexData(pGemData, pPartition, uiPosOffset, 
		uiWeightOffset, uiBoneIndexOffset, uiNormOffset, uiColorOffset, uiTexOffset);


	pRendererData->setVertexData(pVertexData);

	if(pPartition->m_usStripCount > 0)
	{
		pRendererData->setPrimitiveType(EPT_TRIANGLE_STRIP);

		unsigned short usStripStart = 0;
		unsigned short* pLockIndex;
		unsigned short* pIndex;

		EzIndexBuffer* pIndexBuffer = createIndexBuffer(EzIndexBuffer::EIT_16BIT, pGemData->getNumTrianglePoints(), Ez3DBuffer::ESTATIC);
		pLockIndex = (unsigned short*)pIndexBuffer->lock(EzIndexBuffer::ENORMAL);

		for(unsigned short usStripCount = 0; usStripCount < pPartition->m_usStripCount; ++usStripCount )
		{
			pIndex = &pPartition->m_pusTriList[usStripStart];

			for(unsigned short usStripLengths = 0; usStripLengths < pPartition->m_pusStripLengths[usStripCount]; ++usStripLengths)
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


	unsigned int uiCount = 0;
	EzVertexBuffer* pVertexStream0 = pVertexData->getVertexBuffer(0);
	const EzPoint3* pVertices = pGemData->getVertices();
	const EzPoint3* pNomals = pGemData->getVertexNormals();
	unsigned short* pusVertMap = pPartition->m_pusVertexMap;
	u8* pLockVertex;

	pLockVertex = (u8*) pVertexStream0->lock(Ez3DBuffer::ENORMAL);

	unsigned int uiVertexStride = pVertexStream0->getStride();

	for (unsigned int ui = 0; ui < pPartition->m_usVertexCount; ++ui, pLockVertex += uiVertexStride)
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
			float* pweights;
			float* pBoneIndex =(float*)(u8*)(pLockVertex + uiBoneIndexOffset);
			pweights = (float*)(u8*)(pLockVertex + uiWeightOffset);

			for(unsigned short us = 0; us < pPartition->m_usWeightsPerVertexCount; ++us)
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
	
	for(unsigned short us = 0; us < pHWSkinPartition->getPartitionCount(); ++us)
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
	unsigned short* pLockIndex;
	EzIndexBuffer* pIndexBuffer = createIndexBuffer(EzIndexBuffer::EIT_16BIT, pGemData->getNumIndexSize(), Ez3DBuffer::ESTATIC);

	pLockIndex = (unsigned short*)pIndexBuffer->lock(EzIndexBuffer::ENORMAL);

	pGemData->getIndexData(pLockIndex);

	pIndexBuffer->unlock();

	EzIndexData* pIndexData = EzNew EzIndexData(pIndexBuffer);

	pRendererData->setIndexData(pIndexData);


	EzVertexBuffer* pVertexStream0;
	u8*	pLockVertex;
	unsigned int uiPosOffset;
	unsigned int uiWeightOffset;
	unsigned int uiBoneIndexOffset;
	unsigned int uiNormOffset;
	unsigned int uiColorOffset;
	unsigned int uiTexOffset;
	EzVertexData* pVertexData = m_pDriver->createVertexData(pGemData, NULL, uiPosOffset, uiWeightOffset, uiBoneIndexOffset, uiNormOffset, uiColorOffset, uiTexOffset);
	EzVertexBufferBinding* pVertexBufferBinding = pVertexData->getVertexBinding();

	pVertexStream0 = pVertexBufferBinding->getBuffer(0);
	pVertexDec = pVertexData->getVertexDeclaration();

	pLockVertex = (u8*) pVertexStream0->lock(Ez3DBuffer::ENORMAL);

	for(unsigned int ui = 0; ui < pVertexStream0->getNumVertices(); ++ui, pLockVertex += pVertexStream0->getStride())
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

	unsigned int uiPosOffset;
	unsigned int uiWeightOffset;
	unsigned int uiBoneIndexOffset;
	unsigned int uiNormOffset;
	unsigned int uiColorOffset;
	unsigned int uiTexOffset;
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

	for(unsigned int ui = 0; ui < pVertexBuffer1->getNumVertices(); ++ui, pLockVertex += pVertexBuffer1->getStride())
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


	unsigned short* pLockIndex;
	EzIndexBuffer* pIndexBuffer = createIndexBuffer(EzIndexBuffer::EIT_16BIT, pGemData->getNumTrianglePoints(), Ez3DBuffer::ESTATIC_WRITE_ONLY);

	pLockIndex = (unsigned short*)pIndexBuffer->lock(EzIndexBuffer::ENORMAL);

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

unsigned int EzRenderer::setupRenderingPass(EzGeometry* pGeometry, EzRendererData* pRendererData)
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
	m_pDriver->beginScene(true, true, m_BackGroundColor);

	m_pDriver->setTransform ( ETS_PROJECTION, IdentityMatrix );
	m_pDriver->setTransform ( ETS_VIEW, IdentityMatrix );
	m_pDriver->setTransform ( ETS_WORLD, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_0, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_1, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_2, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_3, IdentityMatrix );

	if(m_spActiveCamera)
	{
		m_spActiveCamera->update(0);

		//m_vCamWorldPos = m_spActiveCamera->getWorldPosition();
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
