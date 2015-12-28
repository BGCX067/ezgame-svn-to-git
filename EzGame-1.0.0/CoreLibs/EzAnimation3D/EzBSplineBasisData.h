#pragma once


#include "../EzMain3D/EzObject.h"

class EzBSplineBasisData : public EzObject
{
	EzDeclareRTTI;

public:
	EzBSplineBasisData(void);
	~EzBSplineBasisData(void);

	u32 getNumControlPoints() const;
	void setNumControlPoints( u32 value );

protected:
	/*!
	 * The number of control points of the B-spline (number of frames of animation plus
	 * degree of B-spline minus one).
	 */
	u32 numControlPoints;
};
