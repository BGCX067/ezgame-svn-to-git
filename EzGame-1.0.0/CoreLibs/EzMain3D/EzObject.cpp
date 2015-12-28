#include "EzObject.h"


EzImplementRootRTTI(EzObject);


EzObject::EzObject(void)
{
}

EzObject::~EzObject(void)
{
}

void EzObject::loadBinary(EzStream& Stream)
{

}
void EzObject::saveBinary(EzStream& Stream)
{

}

bool EzObject::isEqual(EzObject* pObject)
{
    if ( !pObject )
        return false;

    if ( strcmp(getRTTI()->getName(),pObject->getRTTI()->getName()) != 0 )
        return false;

    return true;

}
