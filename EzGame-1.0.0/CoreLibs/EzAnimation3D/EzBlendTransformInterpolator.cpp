#include "EzBlendTransformInterpolator.h"


EzBlendTransformInterpolator::EzBlendTransformInterpolator(void)
{
}

EzBlendTransformInterpolator::~EzBlendTransformInterpolator(void)
{
}

bool EzBlendTransformInterpolator::update(f32 fTime, EzTransform &Value)
{
	// Do not use the TimeHasChanged check here, because blend interpolators
	// should always update their interpolators.

	bool bReturnValue = false;
	if (m_ucInterpCount == 1)
	{
		bReturnValue = storeSingleValue(fTime, Value);
	}
	else if (m_ucInterpCount > 1)
	{
		computeNormalizedWeights();
		bReturnValue = blendValues(fTime, Value);
	}

	m_fLastTime = fTime;
	return bReturnValue;
}

bool EzBlendTransformInterpolator::isTransformValueSupported() const
{
	return true;

}

bool EzBlendTransformInterpolator::interpolatorIsCorrectType(EzInterpolator* pInterpolator) const
{
	return true;

}
bool EzBlendTransformInterpolator::storeSingleValue(f32 fTime, EzTransform& Value)
{
	if (!getSingleUpdateTime(fTime))
	{
		Value.makeInvalid();
		return false;
	}

	EZASSERT(m_pSingleInterpolator != NULL);
	if (m_pSingleInterpolator->update(fTime, Value))
	{
		return true;
	}
	else
	{
		Value.makeInvalid();
		return false;
	}

}
bool EzBlendTransformInterpolator::blendValues(f32 fTime, EzTransform& Value)
{
	f32 fTotalTransWeight = 1.0f;
	f32 fTotalScaleWeight = 1.0f;

	EzPoint3 vFinalTranslate;
	EzQuaternion FinalRotate;
	f32 fFinalScale = 0.0f;

	bool bTransChanged = false;
	bool bRotChanged = false;
	bool bScaleChanged = false;

	bool bFirstRotation = true;
	for (u8 uc = 0; uc < m_aInterp.size(); uc++)
	{	
		InterpArrayItem& Item = m_aInterp[uc];

		if (Item.m_spInterpolator && Item.m_fNormalizedWeight > 0.0f)
		{
			f32 fUpdateTime = fTime;
			if (!getUpdateTimeForItem(fUpdateTime, Item))
			{
				fTotalTransWeight -= Item.m_fNormalizedWeight;
				fTotalScaleWeight -= Item.m_fNormalizedWeight;
				continue;
			}

			EzTransform Transform;
			bool bSuccess = Item.m_spInterpolator->update(fUpdateTime, Transform);

			if (bSuccess)
			{
				// Add in the current interpolator's weighted
				// translation to the accumulated translation thus far.
				if (Transform.isTranslateValid())
				{
					vFinalTranslate += Transform.getTranslation() * Item.m_fNormalizedWeight;
					bTransChanged = true;
				}
				else
				{
					// This translate is invalid, so we need to
					// remove it's overall weight from the result
					// at the end
					fTotalTransWeight -= Item.m_fNormalizedWeight;
				}

				// Add in the current interpolator's weighted
				// rotation to the accumulated rotation thus far.
				// Since quaternion SLERP is not commutative, we can
				// get away with accumulating weighted sums of the quaternions
				// as long as we re-normalize at the end.
				if (Transform.isRotateValid())
				{
					EzQuaternion RotValue = Transform.getToMatrixRotation().toQuat();

					// Dot only represents the angle between quats when they
					// are unitized. However, we don't care about the 
					// specific angle. We only care about the sign of the
					// angle between the two quats. This is preserved when
					// quaternions are non-unit.
					if (!bFirstRotation)
					{
						f32 fCos = FinalRotate.dotProduct(FinalRotate);

						// If the angle is negative, we need to invert the
						// quat to get the best path.
						if (fCos < 0.0f)
						{
							RotValue = -RotValue;
						}
					}
					else
					{
						bFirstRotation = false;
					}

					// Multiply in the weights to the quaternions.
					// Note that this makes them non-rotations.
					RotValue = RotValue * Item.m_fNormalizedWeight;

					// Accumulate the total weighted values into the 
					// rotation
					FinalRotate.set(RotValue.X + FinalRotate.X, 
						RotValue.Y + FinalRotate.Y, 
						RotValue.Z + FinalRotate.Z, 
						RotValue.W + FinalRotate.W);

					// Need to re-normalize quaternion.
					bRotChanged = true;
				}
				// we don't need to remove the weight of invalid rotations
				// since we are re-normalizing at the end. It's just extra
				// work.

				// Add in the current interpolator's weighted
				// scale to the accumulated scale thus far.
				if (Transform.isScaleValid())
				{
					fFinalScale += Transform.getScale() * Item.m_fNormalizedWeight;
					bScaleChanged = true;
				}
				else 
				{
					// This scale is invalid, so we need to
					// remove it's overall weight from the result
					// at the end
					fTotalScaleWeight -= Item.m_fNormalizedWeight;
				}
			}
			else
			{
				// If the update failed, we should 
				// remove the weights of the interpolator
				fTotalTransWeight -= Item.m_fNormalizedWeight;
				fTotalScaleWeight -= Item.m_fNormalizedWeight;
			}
		}
	}

	// If any of the channels were animated, the final
	// transform needs to be updated
	Value.makeInvalid();
	if (bTransChanged || bRotChanged || bScaleChanged)
	{
		// Since channels may or may not actually have been
		// active during the blend, we can remove the weights for
		// channels that weren't active.

		if (bTransChanged)
		{
			// Remove the effect of invalid translations from the
			// weighted sum
			EZASSERT(fTotalTransWeight != 0.0f);
			vFinalTranslate /= fTotalTransWeight;
			Value.setTranslation(vFinalTranslate);
		}
		if (bRotChanged)
		{
			// Since we summed quaternions earlier, we have
			// non-unit quaternions, which are not rotations.
			// To make the accumulated quaternion a rotation, we 
			// need to normalize.
			FinalRotate.normalize();
			Value.setRotation(FinalRotate);
		}
		if (bScaleChanged)
		{
			// Remove the effect of invalid scales from the
			// weighted sum
			EZASSERT(fTotalScaleWeight != 0.0f);
			fFinalScale /= fTotalScaleWeight;
			Value.setScale(fFinalScale);
		}
	}
	
	if (Value.isTransformInvalid())
	{
		return false;
	}

	return true;

}

