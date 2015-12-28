#pragma once

#include "../EzCore/EzSingleton.h"

class EZCORE_ENTRY EzMain3DSingtonGroup : public EzSingleton<EzMain3DSingtonGroup>
{
public:
	EzMain3DSingtonGroup(void);
	~EzMain3DSingtonGroup(void);

	static void init();
	static void shutdown();

};

#define EzMain3DInit			EzMain3DSingtonGroup::init
#define EzMain3DShutdown		EzMain3DSingtonGroup::shutdown