#include "EzTextureStage.h"

EzMemPool<EzTextureStage>* EzTextureStage::ms_pTextureStagePool = NULL;

EzTextureStage::EzTextureStage(void)
: m_pTexture(NULL), m_uiStage(0),  m_eFilterMode(FILTER_NEAREST), m_eClampMode(ETC_CLAMP_U_CLAMP_V)
{
	m_pTextureStageGroup = EzTextureStageGroup::alloc();
}

EzTextureStage::~EzTextureStage(void)
{
}


void EzTextureStage::initializePools()
{
    ms_pTextureStagePool = EzNew EzMemPool<EzTextureStage>;
}
void EzTextureStage::shutdownPools()
{
    EzDelete ms_pTextureStagePool;
}

EzTextureStage* EzTextureStage::alloc()
{
    return ms_pTextureStagePool->alloc();
}

void EzTextureStage::free(EzTextureStage* pGroup)
{
	ms_pTextureStagePool->free(pGroup);
}

void EzTextureStage::update()
{

}


//
//void EzTextureStage::setClampMode(NiTexturingProperty::ClampMode eClamp)
//{
//    SetSamplerState((unsigned int)NiD3DRenderState::NISAMP_ADDRESSU,
//        ms_pkD3DRenderer->GetD3DClampMode(eClamp).m_eU);
//    SetSamplerState((unsigned int)NiD3DRenderState::NISAMP_ADDRESSV,
//        ms_pkD3DRenderer->GetD3DClampMode(eClamp).m_eV);
//}
////---------------------------------------------------------------------------
//void NiD3DTextureStage::setFilterMode(NiTexturingProperty::FilterMode eFilter)
//{
//    SetSamplerState((unsigned int)NiD3DRenderState::NISAMP_MAGFILTER, 
//        ms_pkD3DRenderer->GetD3DFilterMode(eFilter).m_eMag);
//    SetSamplerState((unsigned int)NiD3DRenderState::NISAMP_MINFILTER, 
//        ms_pkD3DRenderer->GetD3DFilterMode(eFilter).m_eMin);
//    SetSamplerState((unsigned int)NiD3DRenderState::NISAMP_MIPFILTER, 
//        ms_pkD3DRenderer->GetD3DFilterMode(eFilter).m_eMip);
//}
