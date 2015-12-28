#include "EzProperty.h"

EzImplementRTTI(EzProperty, EzObjectNET);

EzProperty::EzProperty(void)
: EzObjectNET()
{
}

EzProperty::~EzProperty(void)
{
}

void EzProperty::loadBinary(EzStream& Stream)
{
	EzObjectNET::loadBinary(Stream);
}


void EzProperty::saveBinary(EzStream& Stream)
{
	EzObjectNET::saveBinary(Stream);


}

bool EzProperty::isEqual(EzObject* obj)
{
	return false;

}
