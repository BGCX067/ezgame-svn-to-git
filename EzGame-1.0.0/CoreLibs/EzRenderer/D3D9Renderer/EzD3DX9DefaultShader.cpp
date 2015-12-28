#include "EzD3DX9DefaultShader.h"
#include "../../EzMain3D/EzShaderFactory.h"
#include "../../EzMain3D/EzDynamicEffectState.h"
#include "../../EzMain3D/EzGeometry.h"

EzD3DX9DefaultShader::EzD3DX9DefaultShader(void)
{
}

EzD3DX9DefaultShader::~EzD3DX9DefaultShader(void)
{
}

EzShader* EzD3DX9DefaultShader::createShader()
{
	return EzNew EzD3DX9DefaultShader;
}


u32 EzD3DX9DefaultShader::beginPipeline(EzGeometry* pGeometry)
{

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

	return 0;
}

u32 EzD3DX9DefaultShader::updatePipeline(EzGeometry* pGeometry, EzPropertyState* pPropState)
{
	return constructPipeline(pGeometry, pPropState);
}

u32 EzD3DX9DefaultShader::endPipeline(EzGeometry* pGeometry)
{
	return 0;
}


u32 EzD3DX9DefaultShader::constructPipeline(EzGeometry* pGeometry, EzPropertyState* pPropState)
{
//	EzPropertyState* pPropState = pGeometry->getPropertyState();


	EzTexturingProperty* pTexturingProperty = pPropState->getTexturing();
	
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

	//m_pRenderData = pRendererData;


	return 0;

}