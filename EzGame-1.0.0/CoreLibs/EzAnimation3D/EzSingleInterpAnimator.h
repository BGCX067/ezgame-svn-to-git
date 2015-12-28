#pragma once

#include "EzInterpAnimator.h"
#include "EzInterpolator.h"


class EzSingleInterpAnimator : public EzInterpAnimator
{
	EzDeclareRTTI;
public:
	EzSingleInterpAnimator(void);
	~EzSingleInterpAnimator(void);


	void update(f32 fTime);

    virtual EzInterpolator* getInterpolator(u16 usIndex = 0) const;
    virtual void setInterpolator(EzInterpolator* pInterpolator, u16 usIndex = 0);


protected:
	EzInterpolatorPtr		m_spInterpolator;
};

