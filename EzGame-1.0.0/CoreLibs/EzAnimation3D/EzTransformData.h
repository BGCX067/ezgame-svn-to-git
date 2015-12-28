#pragma once

#include "EzKeyframeData.h"



EzSmartPointer(EzTransformData);

class EzTransformData : public EzKeyframeData
{
	EzDeclareRTTI;

public:
	EzTransformData(void);
	~EzTransformData(void);

	EzPoint3 getKeyTranslations(f32 fTime, u32& uiLastIdx);
	EzQuaternion getKeyRotations(f32 fTime, u32& uiLastIdx);
	f32 getKeyScales(f32 fTime, u32& uiLastIdx);
};

