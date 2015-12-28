#pragma once

#include "EzSingleton.h"

class EZCORE_ENTRY EzLogManager :  public EzSingleton<EzLogManager>
{
public:
	EzLogManager(void);
	~EzLogManager(void);

	static void init();
	static void shutdown();
};

