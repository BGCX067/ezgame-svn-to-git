#include "EzBSplineInterpolator.h"

EzBSplineInterpolator::EzBSplineInterpolator(void)
:m_pBSpplineData(NULL), m_pBasisData(NULL)
{
}

EzBSplineInterpolator::~EzBSplineInterpolator(void)
{
}

f32 EzBSplineInterpolator::getStartTime() const
{
	return m_fStartTime;

}

void EzBSplineInterpolator::setStartTime( f32 value )
{
	m_fStartTime = value;

}
f32 EzBSplineInterpolator::getStopTime() const
{
	return m_fStopTime;

}
void EzBSplineInterpolator::setStopTime( f32 value )
{
	m_fStopTime = value;

}

EzBSplineData* EzBSplineInterpolator::getSplineData() const
{
	return m_pBSpplineData;

}
void EzBSplineInterpolator::setSplineData( EzBSplineData * value )
{
	m_pBSpplineData = value;

}
EzBSplineBasisData* EzBSplineInterpolator::getBasisData() const
{
	return m_pBasisData;

}
void EzBSplineInterpolator::setBasisData( EzBSplineBasisData * value )
{
	m_pBasisData = value;

}
/*********************************************************************
Simple b-spline curve algorithm

Copyright 1994 by Keith Vertanen (vertankd@cda.mrs.umn.edu)

Released to the public domain (your mileage may vary)

Found at: Programmers Heaven (www.programmersheaven.com/zone3/cat415/6660.htm)
Modified by: Theo 
- reformat and convert doubles to f32s
- removed point structure in favor of arbitrary sized f32 array
**********************************************************************/
static void copy_f32s(f32* dest, f32 *src, f32 l)
{
   for (s32 i=0; i<l; ++i)
      dest[i] = src[i];
}

// calculate the blending value
static f32 blend(s32 k, s32 t, s32 *u, f32 v)  
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
static void compute_intervals(s32 *u, s32 n, s32 t)
{
   for (int j=0; j<=n+t; j++) {
      if (j<t)
         u[j]=0;
      else if ((t<=j) && (j<=n))
         u[j]=j-t+1;
      else if (j>n)
         u[j]=n-t+2;  // if n-t=-2 then we're screwed, everything goes to 0
   }
}

static void compute_point(s32 *u, s32 n, s32 t, f32 v, int l, f32 *control, f32 *output)
{
   // initialize the variables that will hold our output
   for (int j=0; j<l; j++)
      output[j] = 0;
   for (int k=0; k<=n; k++) {
      f32 temp = blend(k,t,u,v);  // same blend is used for each dimension coordinate
      for (int j=0; j<l; j++)
         output[j] = output[j] + control[k*l + j] * temp;
   }
}

/*********************************************************************
bspline(int n, int t, int l, f32 *control, f32 *output, int num_output)

Parameters:
n          - the number of control points minus 1
t          - the degree of the polynomial plus 1
l          - size of control and output f32 vector block
control    - control point array made up of f32 structure
output     - array in which the calculate spline points are to be put
num_output - how many points on the spline are to be calculated

Pre-conditions:
n+2>t  (no curve results if n+2<=t)
control array contains the number of points specified by n
output array is the proper size to hold num_output point structures

control and output vectors must be contiguous f32 arrays

**********************************************************************/
void EzBSplineInterpolator::bspline(s32 n, s32 t, s32 l, f32 *control, f32 *output, s32 num_output)
{
   f32 *calc = new f32[l];
   s32 *u = new s32[n+t+1];
   compute_intervals(u, n, t);

   f32 increment=(f32)(n-t+2)/(num_output-1);  // how much parameter goes up each time
   f32 interval=0;
   for (s32 output_index=0; output_index<num_output-1; output_index++) 
   {
      compute_point(u, n, t, interval, l, control, calc);
      copy_f32s(&output[output_index*l], calc, l);
      interval=interval+increment;  // increment our parameter
   }
   copy_f32s(&output[(num_output-1)*l], &control[n*l], l); // put in the last points
   delete [] u;
   delete [] calc;
}

bool EzBSplineInterpolator::bsplineinterpolateQuat(EzQuaternion & value, s32 n, s32 t, s32 l, f32 *control, f32 mult, f32 bias )
{
	int *u = new int[n+t+1];
	compute_intervals(u, n, t);
    //  compute_point(u, n, t, interval, subArray, value, mult, bias);
   delete [] u;

   return true;
}
