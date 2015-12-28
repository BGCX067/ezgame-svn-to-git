#pragma once

#include "Context.h"

class CPacketProcess
{
public:
	virtual ~CPacketProcess(void);

	virtual void Processing(LPSOCKETCONTEXT lpSocketContext, char *pData) = 0;
	virtual void Release() = 0;

};
