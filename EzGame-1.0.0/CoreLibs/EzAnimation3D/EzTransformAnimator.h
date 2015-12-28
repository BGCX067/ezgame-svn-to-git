#pragma once

#include "EzKeyframeAnimator.h"


class EzTransformAnimator : public EzKeyframeAnimator
{
	EzDeclareRTTI;
public:
	EzTransformAnimator(void);
	~EzTransformAnimator(void);


	virtual void update(f32 fTime);
};

