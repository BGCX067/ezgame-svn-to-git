#include "EzBSplineData.h"


EzImplementRootRTTI(EzBSplineData);


EzBSplineData::EzBSplineData(void)
{
}

EzBSplineData::~EzBSplineData(void)
{
}


const EzArray<f32>& EzBSplineData::getFloatControlPoints() const
{
	return m_FloatControlPoints;

}

EzArray<f32> EzBSplineData::getFloatControlPointRange(u32 offset, u32 count) const
{
   EzArray<f32> value;
   if (offset < 0 || count < 0 || ((offset + count) > int(m_FloatControlPoints.size())))
   {
      //throw runtime_error("Invalid offset or count.");
	   return value;

   }

   value.set_used(count);
   value.reallocate(count);

   for(u32 n = 0; n < count; ++n)
   {
	   value[n] = m_FloatControlPoints[n + offset];

   }

	return value;

}
const EzArray<s16>& EzBSplineData::getShortControlPoints() const
{
	return m_ShortControlPoints;

}
EzArray<s16> EzBSplineData::getShortControlPointRange(u32 offset, u32 count) const
{

   EzArray<s16> value;
   if (offset < 0 || count < 0 || ((offset + count) > int(m_ShortControlPoints.size())))
   {
      //throw runtime_error("Invalid offset or count.");
	   return value;

   }

   value.set_used(count);
   value.reallocate(count);

   for(u32 n = 0; n < count; ++n)
   {
	   value[n] = m_ShortControlPoints[n + offset];

   }

	return value;

}


// calculate the blending value
static f32 blend(int k, int t, int *u, f32 v)  
{
   f32 value;
   if (t==1) {			// base case for the recursion
      value = ((u[k]<=v) && (v<u[k+1])) ? 1.0f : 0.0f;
   } else {
      if ((u[k+t-1]==u[k]) && (u[k+t]==u[k+1]))  // check for divide by zero
         value = 0;
      else if (u[k+t-1]==u[k]) // if a term's denominator is zero,use just the other
         value = (u[k+t] - v) / (u[k+t] - u[k+1]) * blend(k+1, t-1, u, v);
      else if (u[k+t]==u[k+1])
         value = (v - u[k]) / (u[k+t-1] - u[k]) * blend(k, t-1, u, v);
      else
         value = (v - u[k]) / (u[k+t-1] - u[k]) * blend(k, t-1, u, v) +
            (u[k+t] - v) / (u[k+t] - u[k+1]) * blend(k+1, t-1, u, v);
   }
   return value;
}

// figure out the knots
static void compute_intervals(int *u, int n, int t)
{
   for (int j=0; j<=n+t; j++) {
      if (j<t)  u[j]=0;
      else if ((t<=j) && (j<=n))  u[j]=j-t+1;
      else if (j>n) u[j]=n-t+2;  // if n-t=-2 then we're screwed, everything goes to 0
   }
}

void EzBSplineData::getCompactedValueDegree3(f32 fTime, f32* afPos, u16 uiDimension, u32 offset, u32 NumControlPoints, f32 mult, f32 bias) 
{
	int t = 3+1;
    int n = NumControlPoints-1;

	int *u = EzAlloc(int, n+t+1);


	for (int j=0; j<uiDimension; j++)
      afPos[j] = 0;

	compute_intervals(u, n, t);

	for(s32 k = 0; k <= n; k++)
	{

		f32 temp = blend(k, t, u, fTime);  

		for (u16 i=0; i<uiDimension; ++i)
		{
			afPos[i] = afPos[i] + (f32(m_ShortControlPoints[offset + (k * uiDimension + i)]) / f32(SHRT_MAX)) * temp;
      	}
        
	}

	for (u16 i=0; i<uiDimension; ++i)
	{
		afPos[i] = afPos[i] * mult + bias;

	}

	EzFree(u);
}

void EzBSplineData::setFloatControlPoints(const EzArray<f32> fPoints)
{
	m_FloatControlPoints = fPoints;

}


void EzBSplineData::setShortControlPoints(const EzArray<s16> iPoints)
{
	m_ShortControlPoints = iPoints;

}
