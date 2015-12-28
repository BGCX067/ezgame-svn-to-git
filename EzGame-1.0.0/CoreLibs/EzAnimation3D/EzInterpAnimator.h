#pragma once


#include "../EzMain3D/EzAnimator.h"


class EzInterpolator;
class EzAVObject;

EzSmartPointer(EzInterpAnimator);

class EzInterpAnimator : public EzAnimator
{
	EzDeclareRTTI;

public:
	EzInterpAnimator(void);
	~EzInterpAnimator(void);


	virtual EzInterpolator* getInterpolator(u16 usIndex = 0) const = 0;
    virtual void setInterpolator(EzInterpolator* interpolator, u16 usIndex = 0) = 0;

	virtual void update(f32 fTime) = 0;



};
