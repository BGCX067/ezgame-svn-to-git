#include "EzApplication.h"
#include "EzDevice.h"
#include "../EzCore/EzMemManager.h"


EzImplementRootRTTI(EzApplication);

EzApplication* EzApplication::ms_pApplication = NULL;

EzApplication::EzApplication()
:m_pDevice(NULL)
{
	ms_pApplication = this;

}

EzApplication::~EzApplication(void)
{

}

bool EzApplication::postEvent(SEvent event)
{
	EZASSERT(ms_pApplication);

	switch(event.eEventType)
	{
	case EET_MOUSE_INPUT_EVENT:
		ms_pApplication->OnMouseEvent(event.MouseInput);
		return true;

	case EET_KEY_INPUT_EVENT:
		ms_pApplication->OnKeyEvent(event.KeyInput);
		return true;

	case EET_JOYSTICK_INPUT_EVENT:
		return 0;

	case EET_LOG_TEXT_EVENT:
		ms_pApplication->OnLogEvent(event.LogEvent);
		return true;

	case EET_USER_EVENT:
		ms_pApplication->OnUserEvent(event.UserEvent);
		return true;
	}

	return false;

}


