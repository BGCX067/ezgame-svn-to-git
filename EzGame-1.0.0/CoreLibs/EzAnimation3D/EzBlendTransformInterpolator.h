#ifndef _EZBLENDTRANSFORMINTERPOLATOR_H_INCLUDE_
#define _EZBLENDTRANSFORMINTERPOLATOR_H_INCLUDE_

#include "EzBlendInterpolator.h"


class EzBlendTransformInterpolator : public EzBlendInterpolator
{
public:
	EzBlendTransformInterpolator(void);
	~EzBlendTransformInterpolator(void);

	// Virtual function overrides from base class.
	virtual bool update(f32 fTime, EzTransform &Value);
	virtual bool isTransformValueSupported() const;


protected:
	// Virtual function overrides from base class.
	virtual bool interpolatorIsCorrectType(EzInterpolator* pInterpolator) 	const;
	bool storeSingleValue(f32 fTime, EzTransform& Value);
	bool blendValues(f32 fTime, EzTransform& kValue);
};


#endif 
