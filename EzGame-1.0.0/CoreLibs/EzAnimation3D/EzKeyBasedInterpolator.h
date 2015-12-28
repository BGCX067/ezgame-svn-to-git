#pragma once

#include "EzInterpolator.h"


class EzKeyBasedInterpolator : public EzInterpolator 
{
public:
	EzKeyBasedInterpolator(void);
	~EzKeyBasedInterpolator(void);


	/*!
	 * This function will adjust the times in all the keys stored in the data
	 * objects referenced by this interpolator such that phase will equal 0 and
	 * frequency will equal one.  In other words, it will cause the key times
	 * to be in seconds starting from zero.
	 * \param[in] phase The phase shift to remove from any keys stored in this
	 * object.
	 * \param[in] frequency The frequency to normalize to 1.0 for any keys
	 * stored in this object
	 */
	virtual void normalizeKeys( f32 phase, f32 frequency );
};


