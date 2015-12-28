#pragma once

#include "../core/EzRefObject.h"
#include "SSkinPartition.h"


namespace GameEasy
{


class EzSkinPartition : public EzRefObject
{
	EzDeclareRootRTTI(EzSkinPartition);
	

public:
	EzSkinPartition(void);
	~EzSkinPartition(void);

protected:
	/*! Skin partition objects. */
	EzArray<SSkinPartition > SkinPartitionBlocks;
};

}
