#include "EzExtraData.h"

EzImplementRTTI(EzExtraData, EzObject);

EzExtraData::EzExtraData(void)
: NextExtraData(0)
{
}

EzExtraData::~EzExtraData(void)
{
}

EzExtraData* EzExtraData::GetNextExtraData() const
{
	return NextExtraData;

}
void EzExtraData::SetNextExtraData( EzExtraData * obj )
{
	NextExtraData = obj; 
}
