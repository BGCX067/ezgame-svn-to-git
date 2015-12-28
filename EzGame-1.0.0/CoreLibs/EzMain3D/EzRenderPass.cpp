#include "EzGeometry.h"
#include "EzRenderPass.h"
#include "EzVideoDriver.h"


EzMemPool<EzRenderPass>* EzRenderPass::ms_pRenderPassPool = NULL;
EzCriticalSection EzRenderPass::ms_PassPoolCriticalSection;

unsigned int EzRenderPass::ms_uiMaxTextureBlendStages = 2;

EzRenderPass::EzRenderPass(void)
: m_pVertexprogramUsage(NULL), m_pPixelprogramUsage(NULL), m_pNextPass(NULL), m_pRenderData(NULL),
m_uiStageCount(0), m_uiCurrentStage(0), m_bSetupRenderPass(false)
{
	m_aStages.set_used(ms_uiMaxTextureBlendStages);
}

EzRenderPass::~EzRenderPass(void)
{
}

void EzRenderPass::initializePools()
{
    ms_PassPoolCriticalSection.lock();
    EZASSERT(ms_pRenderPassPool == NULL);
    ms_pRenderPassPool = EzNew EzMemPool<EzRenderPass>;
    ms_PassPoolCriticalSection.unlock();
}

void EzRenderPass::shutdownPools()
{
    ms_PassPoolCriticalSection.lock();
    EzDelete ms_pRenderPassPool;
    ms_pRenderPassPool = NULL;
    ms_PassPoolCriticalSection.unlock();
}

EzRenderPassPtr EzRenderPass::createNewPass()
{
    ms_PassPoolCriticalSection.lock();
    EZASSERT(ms_pRenderPassPool != NULL);
	EzRenderPassPtr spReturn = ms_pRenderPassPool->alloc();
    ms_PassPoolCriticalSection.unlock();

   // spReturn->SetRendererOwned();

    return spReturn;
}

void EzRenderPass::returnPassToPool()
{
    //if (!IsRendererOwned())
    //{
    //    EzDelete this;
    //    return;
    //}

    //m_spPixelConstantMap = NULL;
    //m_spPixelShader = NULL;
    //m_spVertexConstantMap = NULL;
    //m_spVertexShader = NULL;

    //NiD3DRenderStateGroup::ReleaseRenderStateGroup(m_pkRenderStateGroup);
    //m_pkRenderStateGroup = NULL;

    //SetPixelShaderProgramFileName(NULL);
    //SetPixelShaderProgramEntryPoint(NULL);
    //SetPixelShaderProgramShaderTarget(NULL);
    //SetVertexShaderProgramFileName(NULL);
    //SetVertexShaderProgramEntryPoint(NULL);
    //SetVertexShaderProgramShaderTarget(NULL);
    //ReleaseTextureStages();

    //m_bSoftwareVP = false;

    ms_PassPoolCriticalSection.lock();
	EZASSERT(ms_pRenderPassPool != NULL);
    ms_pRenderPassPool->free(this);
    ms_PassPoolCriticalSection.unlock();
}




void EzRenderPass::setupRenderingPass(EzGeometry* pGeometry, EzRendererData* pRendererData)
{

	//EzPropertyState* pPropState = pGeometry->getPropertyState();


	//EzTexturingProperty* pTexturingProperty = pPropState->getTexturing();
	//
	//if(m_bSetupRenderPass)
	//	return;

	//if(pTexturingProperty)
	//{
	//	if(pTexturingProperty->hasTextureMap(EzTexturingProperty::BASE_MAP))
	//	{
	//		EzTextureStage* pBaseStage = EzTextureStage::alloc();
	//		EzTextureDesc* pTexDesc = pTexturingProperty->getTextureDesc(EzTexturingProperty::BASE_MAP);

	//		EZASSERT(pBaseStage);

	//	
	//		pBaseStage->setTexture(pTexturingProperty->getTextureRanderData(EzTexturingProperty::BASE_MAP));
	//		pBaseStage->setStageState(ETS_TEXCOORDINDEX, pTexDesc->uvSet);
	//		pBaseStage->setStageState(ETS_COLOROP,ETOP_MODULATE);
	//		//pBaseStage->setStageState(ETS_COLORARG1, ETA_TEXTURE);
	//		//pBaseStage->setStageState(ETS_COLORARG2, ETA_DIFFUSE);
	//		//pBaseStage->setStageState(ETS_ALPHAOP,  ETOP_DISABLE);

	//		pBaseStage->setStageState(ETS_COLOROP, D3DTOP_SELECTARG1);
	//		pBaseStage->setStageState(ETS_COLORARG1, D3DTA_TEXTURE);
	//		pBaseStage->setStageState(ETS_COLORARG2, D3DTA_CURRENT);
	//		pBaseStage->setStageState(ETS_ALPHAOP, D3DTOP_SELECTARG2);
	//		pBaseStage->setStageState(ETS_ALPHAARG1, D3DTA_TEXTURE);

	//		pBaseStage->setTextureClampMode(pTexDesc->eClampMode);
	//		pBaseStage->setTextureFilterMode(pTexDesc->eFilterMode);

	//		if(pTexDesc->hasTextureTransform)
	//		{
	//			EzMatrix4  mat;

	//			if(pTexDesc->tiling.X == 0)
	//			{
	//				pTexDesc->tiling.X = 1.0f;
	//			}
	//
	//			if(pTexDesc->tiling.Y == 0)
	//			{
	//				pTexDesc->tiling.Y = 1.0f;
	//			}
	//	
	//			mat.setScale(EzPoint3(pTexDesc->tiling.X, pTexDesc->tiling.Y, 1.0f));

	//			pBaseStage->setStageState(ETS_TEXTURETRANSFORMFLAGS, ETTFF_COUNT2 );

	//			pBaseStage->setTextureTransformation(mat);

	//			appendStage(pBaseStage);
	//		}
	//	}
	//}

	m_pRenderData = pRendererData;

	m_bSetupRenderPass = true;

}

bool EzRenderPass::setStage(unsigned int uiStageNum, EzTextureStage* pStage)
{
    if (uiStageNum >= ms_uiMaxTextureBlendStages)
        return false;

	m_aStages[uiStageNum] = pStage;


    if (pStage)
    {
        pStage->setStage(uiStageNum);
        if (m_uiCurrentStage == uiStageNum)
            m_uiCurrentStage++;

        m_uiStageCount++;
    }
    else if (m_uiCurrentStage == uiStageNum + 1)
    {
        while (m_uiCurrentStage != 0)
        {
            if (m_aStages[m_uiCurrentStage - 1] == NULL)
            {
                m_uiCurrentStage--;
                break;
            }
        }
    }

	return true;
}


void EzRenderPass::updateStage(EzVideoDriver* pDriver)
{
	for(unsigned int uiStageNum = 0 ; uiStageNum < ms_uiMaxTextureBlendStages; uiStageNum++)
	{
		if(m_uiStageCount == uiStageNum)
		{
			break;
		}

		if(m_aStages[uiStageNum])
		{

			pDriver->texturePipeline(m_aStages[uiStageNum]);
		}
	}

	pDriver->disableStage(m_uiStageCount);
}