#include "../EzCore/EzRefObject.h"


unsigned int EzRefObject::ms_uiObjects = 0;

void EzRefObject::deleteThis()
{
	EzDelete this;

}


