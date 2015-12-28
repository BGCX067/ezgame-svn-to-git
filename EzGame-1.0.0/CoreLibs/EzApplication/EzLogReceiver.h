#pragma once

#include "../EzCore/EzSingleton.h"
#include "SEvent.h"
#include "EzErrorDefines.h"

class EzLogReceiver : public EzSingleton<EzLogReceiver>
{
public:
	EzLogReceiver(void);
	~EzLogReceiver(void);

	virtual void postEvent(const SEvent & Event);
};

inline void log(const u32 uErrcode, const c8* format ...)
{
	c8* tmp;
	va_list args; 

	va_start( args, format );
	int len = 4096;
	tmp = (char*)malloc( len );

#if( _MSC_VER > 1399 )
	vsprintf_s( tmp, len, format, args );
#else // if( _MSC_VER > 1399 )
	vsprintf( tmp, format, args );
#endif 

	SEvent event;

	event.eEventType = EET_LOG_TEXT_EVENT;
	event.LogEvent.Text = tmp;
	event.LogEvent.uiErrCode = uErrcode;

	if(EzLogReceiver::isInitialized())
	{
		EzLogReceiver::get().postEvent(event);
	}



	free(tmp);
}
