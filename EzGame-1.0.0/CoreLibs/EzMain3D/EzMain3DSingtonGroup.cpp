#include "../EzCore/EzMemoryDefines.h"
#include "../EzCore/EzCoreSingtonGroup.h"
#include "EzShaderFactory.h"
#include "EzMain3DSingtonGroup.h"
#include "EzRenderPass.h"
#include "EzTextureStageGroup.h"
#include "EzObjectFactory.h"

template<> 
EzMain3DSingtonGroup* EzSingleton<EzMain3DSingtonGroup>::ms_pSingleton = 0;

EzMain3DSingtonGroup::EzMain3DSingtonGroup(void)
{

}
EzMain3DSingtonGroup::~EzMain3DSingtonGroup(void)
{

}

void EzMain3DSingtonGroup::init()
{
	EZASSERT(!ms_pSingleton);

	ms_pSingleton = EzExternalNew EzMain3DSingtonGroup();
	
	EzCoreSingtonGroup::init();
	EzObjectFactory::init();
	EzShaderFactory::init();

    EzRenderPass::initializePools();
	EzTextureStageGroup::initializePools();
	EzTextureStage::initializePools();

}

void EzMain3DSingtonGroup::shutdown()
{

	EZASSERT(ms_pSingleton);

    EzRenderPass::shutdownPools();
	EzTextureStageGroup::shutdownPools();
	EzTextureStage::shutdownPools();
	EzObjectFactory::shutdown();

	
	EzExternalDelete ms_pSingleton;
	EzShaderFactory::shutdown();
	EzCoreSingtonGroup::shutdown();
}