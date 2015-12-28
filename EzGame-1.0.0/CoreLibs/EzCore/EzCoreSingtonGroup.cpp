#include "EzCoreSingtonGroup.h"
#include "EzMemManager.h"
#include "EzOSOperator.h"
#include "EzLogManager.h"


template<> 
EzCoreSingtonGroup* EzSingleton<EzCoreSingtonGroup>::ms_pSingleton = 0;

EzCoreSingtonGroup::EzCoreSingtonGroup(void)
{

}

EzCoreSingtonGroup::~EzCoreSingtonGroup(void)
{
}

void EzCoreSingtonGroup::init()
{
	EZASSERT(!ms_pSingleton);

	ms_pSingleton = EzExternalNew EzCoreSingtonGroup();

	EzLogManager::init();

	EzMemManager::init();
	EzOSOperator::init();
}

void EzCoreSingtonGroup::shutdown()
{

	EZASSERT(ms_pSingleton);

	EzOSOperator::shutdown();
	EzMemManager::shutdown();

	EzLogManager::shutdown();

	EzExternalDelete ms_pSingleton;
}

