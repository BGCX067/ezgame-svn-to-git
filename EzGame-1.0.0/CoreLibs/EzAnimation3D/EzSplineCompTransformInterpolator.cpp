#include "EzBSplineCompTransformInterpolator.h"
#include "EzBSplineBasisData.h"
#include "EzBSplineData.h"


static const int SizeofQuat = 4;
static const int SizeofTrans = 3;
static const int SizeofScale = 1;
static const int Degree = 3;

EzBSplineCompTransformInterpolator::EzBSplineCompTransformInterpolator(void)
{
}

EzBSplineCompTransformInterpolator::~EzBSplineCompTransformInterpolator(void)
{
}
f32 EzBSplineCompTransformInterpolator::getTranslateBias() const
{
	return m_fTranslationBias;

}
void EzBSplineCompTransformInterpolator::setTranslateBias( f32 value )
{
	m_fTranslationBias = value;
}

f32 EzBSplineCompTransformInterpolator::getTranslateMultiplier() const
{
	return m_fTranslationMultiplier;

}

void EzBSplineCompTransformInterpolator::setTranslateMultiplier( f32 value )
{
	m_fTranslationMultiplier = value;

}
f32 EzBSplineCompTransformInterpolator::getRotationBias() const
{
	return m_fRotationBias;

}
void EzBSplineCompTransformInterpolator::setRotationBias( f32 value )
{
	m_fRotationBias = value;

}

f32 EzBSplineCompTransformInterpolator::getRotationMultiplier() const
{
	return m_fRotationMultiplier;

}

void EzBSplineCompTransformInterpolator::setRotationMultiplier( f32 value )
{
	m_fRotationMultiplier = value;

}
f32 EzBSplineCompTransformInterpolator::getScaleBias() const
{
	return m_fScaleBias;

}
void EzBSplineCompTransformInterpolator::setScaleBias( f32 value )
{
	m_fScaleBias = value;

}
f32 EzBSplineCompTransformInterpolator::getScaleMultiplier() const
{
	return m_fScaleMultiplier;

}
void EzBSplineCompTransformInterpolator::setScaleMultiplier( f32 value )
{
	m_fScaleMultiplier = value;
}
EzArray<EzQuaternion> EzBSplineCompTransformInterpolator::getQuatRotateControlData() const
{
	EzArray<EzQuaternion> value;

	return value;


}
EzArray<EzPoint3> EzBSplineCompTransformInterpolator::getTranslateControlData() const
{
	EzArray<EzPoint3> value;

	return value;

}
EzArray<f32> EzBSplineCompTransformInterpolator::getScaleControlData() const
{
	EzArray<f32> value;

	return value;


}
EzArray<SAniKeyFrame<EzQuaternion>> EzBSplineCompTransformInterpolator::sampleQuatRotateKeys(s32 npoints, s32 degree) const
{
	EzArray< SAniKeyFrame<EzQuaternion>> value;

	if ((m_uiRotationOffset != USHRT_MAX) && m_pBSpplineData && m_pBasisData) 
	{ // has rotation data
      s32 nctrl = m_pBasisData->getNumControlPoints();
      s32 npts = nctrl * SizeofQuat;
      EzArray<s16> points = m_pBSpplineData->getShortControlPointRange(m_uiRotationOffset, npts);
      EzArray<f32> control;
      EzArray<f32> output;


	  control.set_used(npts);
	  control.reallocate(npts);
	  output.set_used(npoints*SizeofQuat);
	  output.reallocate(npoints*SizeofQuat);

	  if(points.size() == 0)
		  return value;


      for (s32 i=0, j=0; i<nctrl; ++i) 
	  {
         for (s32 k=0; k<SizeofQuat; ++k)
		 {
            control[i*SizeofQuat + k] = f32(points[j++]) / f32 (SHRT_MAX);
		 }
      }

      if (degree>=nctrl)
         degree = nctrl - 1;
      // fit data
      bspline(nctrl-1, degree+1, SizeofQuat, &control[0], &output[0], npoints);

      //// copy to key
      f32 fTime = getStartTime();
      f32 fIncr = (getStopTime() - getStartTime()) / f32(npoints) ;

	  value.set_used(npoints);
	  value.reallocate(npoints);

      for (s32 i=0, j=0; i<npoints; i++) 
	  {
		  SAniKeyFrame<EzQuaternion> key;
		  
		  key.fTime = fTime;
		  key.backward_tangent.set(1.0f,0.0f,0.0f,0.0f);
		  key.forward_tangent.set(1.0f,0.0f,0.0f,0.0f); 

		  key.data.W = output[j++] * m_fRotationMultiplier + m_fRotationBias;
		  key.data.X = output[j++] * m_fRotationMultiplier + m_fRotationBias;
          key.data.Y = output[j++] * m_fRotationMultiplier + m_fRotationBias;
          key.data.Z = output[j++] * m_fRotationMultiplier + m_fRotationBias;
          value[i] = key;
          fTime += fIncr;
      }
   }


	return value;

}
EzArray< SAniKeyFrame<EzPoint3>> EzBSplineCompTransformInterpolator::sampleTranslateKeys(s32 npoints, s32 degree) const
{
	EzArray< SAniKeyFrame<EzPoint3>> value;

	return value;

}

s32 EzBSplineCompTransformInterpolator::getNumControlPoints() const
{
   if (m_pBasisData)
   {
      return m_pBasisData->getNumControlPoints();
   }

   return 0;

}

bool EzBSplineCompTransformInterpolator::update(f32 fTime, EzTransform &Value)
{

	if (!timeHasChanged(fTime))
	{
		Value = m_TransformValue;

		if (m_TransformValue.isTransformInvalid())
			return false;
		return true;
	}


   f32 fInterval = ((fTime-m_fStartTime)/(m_fStopTime-m_fStartTime)) * f32(getNumControlPoints() - Degree);

   if((m_uiTranslationOffset != USHRT_MAX) && m_pBSpplineData && m_pBasisData) 
   {
	   s32 nctrl = m_pBasisData->getNumControlPoints();

	   f32 afValues[3];

	   m_pBSpplineData->getCompactedValueDegree3(fInterval, afValues, SizeofTrans, m_uiTranslationOffset, nctrl, m_fTranslationMultiplier, m_fTranslationBias);

	   m_TransformValue.setTranslation(EzPoint3(afValues[0], afValues[2], afValues[1]));
   }

	if ((m_uiRotationOffset != USHRT_MAX) && m_pBSpplineData && m_pBasisData) 
	{ // has rotation data
      s32 nctrl = m_pBasisData->getNumControlPoints();

	  f32 afValues[4];

	  m_pBSpplineData->getCompactedValueDegree3(fInterval, afValues, SizeofQuat, m_uiRotationOffset, nctrl, m_fRotationMultiplier, m_fRotationBias);

	  EzQuaternion quat;

	  quat.set(afValues[1], afValues[3], afValues[2], afValues[0]);

	  m_TransformValue.setRotation(quat);
	}

	if((m_uiScaleOffset != USHRT_MAX) && m_pBSpplineData && m_pBasisData) 
	{
		printf("111");

	}


	Value = m_TransformValue;

	m_fLastTime = fTime;
	


	//Value.setTranslation(m_Translation);


	/*sampleQuatRotateKeys(50, 3);*/

    //if (!TimeHasChanged(fTime))
    //{
    //    kValue = m_kTransformValue;
    //        
    //    if (m_kTransformValue.IsTransformInvalid())
    //        return false;
    //    return true;
    //}

	//f32 interval = ((time-start)/(stop-start)) * f32(nCtrl-degree);

 //   //// Normalize the time to the range of the keys
 //   f32 fNormTime = (fTime - m_fStartTime)/(m_fStopTime - m_fStartTime);
    //f32 afValues[4];

    //if (NiBSplineData::INVALID_HANDLE != m_kTransCPHandle)
    //{
    //    NIASSERT(m_spData);
    //    NIASSERT(m_spBasisData);
    //    m_spData->GetCompactedValueDegree3(fNormTime, (f32*)afValues, 3, 
    //        m_spBasisData, m_kTransCPHandle, 
    //        m_afCompScalars[POSITION_OFFSET], 
    //        m_afCompScalars[POSITION_RANGE]);
    //    m_kTransformValue.SetTranslate(NiPoint3(afValues[0],
    //        afValues[1], afValues[2]));
    //}
    //if (NiBSplineData::INVALID_HANDLE != m_kRotCPHandle)
    //{
    //    NIASSERT(m_spData);
    //    NIASSERT(m_spBasisData);
    //    m_spData->GetCompactedValueDegree3(fNormTime, (f32*)afValues, 4, 
    //        m_spBasisData, m_kRotCPHandle, 
    //        m_afCompScalars[ROTATION_OFFSET], 
    //        m_afCompScalars[ROTATION_RANGE]);
    //    NiQuaternion kQuat(afValues[0], afValues[1], afValues[2], 
    //        afValues[3]);
    //    // We're using a fast normalize here because the BSpline 
    //    // curve interpolation should be pretty close to being
    //    // unitized already. This just snaps it back to the 
    //    // surface of the hypersphere.
    //    kQuat.FastNormalize();
    //    m_kTransformValue.SetRotate(kQuat);
    //}
    //if (NiBSplineData::INVALID_HANDLE != m_kScaleCPHandle)
    //{
    //    NIASSERT(m_spData);
    //    NIASSERT(m_spBasisData);
    //    m_spData->GetCompactedValueDegree3(fNormTime, (f32*)afValues, 1, 
    //        m_spBasisData, m_kScaleCPHandle, 
    //        m_afCompScalars[SCALE_OFFSET], 
    //        m_afCompScalars[SCALE_RANGE]); 
    //    m_kTransformValue.SetScale(afValues[0]);
    //}
    //
    //kValue = m_kTransformValue;
    //if (m_kTransformValue.IsTransformInvalid())
    //{
    //    return false;
    //}

    //m_fLastTime = fTime;
    return true;
}
