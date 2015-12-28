#pragma once

#include "EzSingleton.h"


class EZCORE_ENTRY EzCoreSingtonGroup : public EzSingleton<EzCoreSingtonGroup>
{
public:
	EzCoreSingtonGroup(void);
	~EzCoreSingtonGroup(void);

	static void init();
	static void shutdown();

};

#define EzCoreInit			EzCoreSingtonGroup::init
#define EzCoreShutdown		EzCoreSingtonGroup::shutdown

