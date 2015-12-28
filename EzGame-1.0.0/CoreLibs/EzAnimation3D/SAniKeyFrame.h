#pragma once

#include "../EzCore/EzCore.h"

template <class T> 
struct SAniKeyFrame
{
	f32		fTime; /*!< The time on the animation timeline that this keyframe takes affect. */ 
	T		data; /*!< The data being keyed to the timeline. */ 
	T		forward_tangent; /*!< A piece of data of the same type as is being keyed to the time line used as the forward tangent in quadratic interpolation.  Ignored if key type is set as something else. */ 
	T		backward_tangent; /*!< A piece of data of the same type as is being keyed to the time line used as the backward tangent in quadratic interpolation.  Ignored if key type is set as something else. */ 
	f32		fTension; /*!< The amount of tension to use in tension, bias, continuity interpolation.  Ignored if key type is something else.*/
	f32		fBias; /*!< The amount of bias to use in tension, bias, continuity interpolation.  Ignored if key type is something else.*/
	f32		fContinuity; /*!< The amount of continuity to use in tension, bias, continuity interpolation.  Ignored if key type is something else.*/
};





/*! EzArray of vector keys (anything that can be interpolated, except rotations). */
template <class T >
struct SAniKeyFrameGroup 
{
	/*! The key type. */
	EANI_KEYFREAMTYPE interpolation;
	/*! The keys. */
	EzArray<SAniKeyFrame<T > > keys;
};
