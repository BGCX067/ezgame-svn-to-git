#include "EzLogReceiver.h"



template<> EzLogReceiver* EzSingleton<EzLogReceiver>::ms_pSingleton = 0;

EzLogReceiver::EzLogReceiver(void)
{
}

EzLogReceiver::~EzLogReceiver(void)
{
}

void EzLogReceiver::postEvent(const SEvent & Event)
{
	printf("111");
}

