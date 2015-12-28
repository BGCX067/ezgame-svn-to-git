// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __NERI_MATH_H_INCLUDED__
#define __NERI_MATH_H_INCLUDED__

#include <math.h>
#include <stdlib.h> // for abs() etc.
#include "EzCoreLibType.h"

class EzPoint3;

EZCORE_ENTRY unsigned int* initSqrtTable();
EZCORE_ENTRY void unitizeVectors(EzPoint3* pkV, unsigned int uiNum, unsigned int uiStride);


#if defined(__BORLANDC__) || defined (__BCPLUSPLUS__) || defined (_WIN32_WCE)
	#define sqrtf(X) (float)sqrt((double)(X))
	#define sinf(X) (float)sin((double)(X))
	#define cosf(X) (float)cos((double)(X))
	#define asinf(X) (float)asin((double)(X))
	#define acosf(X) (float)acos((double)(X))
	#define atan2f(X,Y) (float)atan2((double)(X),(double)(Y))
	#define ceilf(X) (float)ceil((double)(X))
	#define floorf(X) (float)floor((double)(X))
	#define powf(X,Y) (float)pow((double)(X),(double)(Y))
	#define fmodf(X,Y) (float)fmod((double)(X),(double)(Y))
	#define fabsf(X) (float)fabs((double)(X))
#endif


	//! Rounding error constant often used when comparing float values.

#ifdef _FAST_MATH
	const float ROUNDING_ERROR_32 = 0.00005f;
	const double ROUNDING_ERROR_64 = 0.000005;
#else
	const float ROUNDING_ERROR_32 = 0.000001f;
	const double ROUNDING_ERROR_64 = 0.00000001;
#endif

#ifdef PI // make sure we don't collide with a define
#undef PI
#endif
	//! Constant for PI.
	const float PI		= 3.14159265359f;

	//! Constant for reciprocal of PI.
	const float RECIPROCAL_PI	= 1.0f/PI;

	//! Constant for half of PI.
	const float HALF_PI	= PI/2.0f;

#ifdef PI64 // make sure we don't collide with a define
#undef PI64
#endif
	//! Constant for 64bit PI.
	const double PI64		= 3.1415926535897932384626433832795028841971693993751;

	//! Constant for 64bit reciprocal of PI.
	const double RECIPROCAL_PI64 = 1.0/PI64;

	//! 32bit Constant for converting from degrees to radians
	const float DEGTORAD = PI / 180.0f;

	//! 32bit constant for converting from radians to degrees (formally known as GRAD_PI)
	const float RADTODEG   = 180.0f / PI;

	//! 64bit constant for converting from degrees to radians (formally known as GRAD_PI2)
	const double DEGTORAD64 = PI64 / 180.0;

	//! 64bit constant for converting from radians to degrees
	const double RADTODEG64 = 180.0 / PI64;

	//! Utility function to convert a radian value to degrees
	/** Provided as it can be clearer to write radToDeg(X) than RADTODEG * X
	\param radians	The radians value to convert to degrees.
	*/
	inline float radToDeg(float radians)
	{
		return RADTODEG * radians;
	}

	//! Utility function to convert a radian value to degrees
	/** Provided as it can be clearer to write radToDeg(X) than RADTODEG * X
	\param radians	The radians value to convert to degrees.
	*/
	inline double radToDeg(double radians)
	{
		return RADTODEG64 * radians;
	}

	//! Utility function to convert a degrees value to radians
	/** Provided as it can be clearer to write degToRad(X) than DEGTORAD * X
	\param degrees	The degrees value to convert to radians.
	*/
	inline float degToRad(float degrees)
	{
		return DEGTORAD * degrees;
	}

	//! Utility function to convert a degrees value to radians
	/** Provided as it can be clearer to write degToRad(X) than DEGTORAD * X
	\param degrees	The degrees value to convert to radians.
	*/
	inline double degToRad(double degrees)
	{
		return DEGTORAD64 * degrees;
	}

	//! returns minimum of two values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	inline const T& min_(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	//! returns minimum of three values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	inline const T& min_(const T& a, const T& b, const T& c)
	{
		return a < b ? min_(a, c) : min_(b, c);
	}

	//! returns maximum of two values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	inline const T& max_(const T& a, const T& b)
	{
		return a < b ? b : a;
	}

	//! returns maximum of three values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	inline const T& max_(const T& a, const T& b, const T& c)
	{
		return a < b ? max_(b, c) : max_(a, c);
	}

	//! returns abs of two values. Own implementation to get rid of STL (VS6 problems)
	template<class T>
	inline T abs_(const T& a)
	{
		return a < (T)0 ? -a : a;
	}

	//! returns linear interpolation of a and b with ratio t
	//! \return: a if t==0, b if t==1, and the linear interpolation else
	template<class T>
	inline T lerp(const T& a, const T& b, const float t)
	{
		return (T)(a*(1.f-t)) + (b*t);
	}

	//! clamps a value between low and high
	template <class T>
	inline const T clamp (const T& value, const T& low, const T& high)
	{
		return min_ (max_(value,low), high);
	}

	//! returns if a equals b, taking possible rounding errors into account
	inline bool equals(const double a, const double b, const double tolerance = ROUNDING_ERROR_64)
	{
		return (a + tolerance >= b) && (a - tolerance <= b);
	}

	//! returns if a equals b, taking possible rounding errors into account
	inline bool equals(const float a, const float b, const float tolerance = ROUNDING_ERROR_32)
	{
		return (a + tolerance >= b) && (a - tolerance <= b);
	}

	//! returns if a equals b, taking possible rounding errors into account
	inline bool equals(const int a, const int b, const int tolerance = 0)
	{
		return (a + tolerance >= b) && (a - tolerance <= b);
	}

	//! returns if a equals b, taking possible rounding errors into account
	inline bool equals(const unsigned int a, const unsigned int b, const unsigned int tolerance = 0)
	{
		return (a + tolerance >= b) && (a - tolerance <= b);
	}

	//! returns if a equals zero, taking rounding errors into account
	inline bool iszero(const float a, const float tolerance = ROUNDING_ERROR_32)
	{
		return fabsf ( a ) <= tolerance;
	}

	//! returns if a equals zero, taking rounding errors into account
	inline bool iszero(const int a, const int tolerance = 0)
	{
		return ( a & 0x7ffffff ) <= tolerance;
	}

	//! returns if a equals zero, taking rounding errors into account
	inline bool iszero(const unsigned int a, const unsigned int tolerance = 0)
	{
		return a <= tolerance;
	}

	inline int int_min ( int a, int b)
	{
		int mask = (a - b) >> 31;
		return (a & mask) | (b & ~mask);
	}

	inline int int_max ( int a, int b)
	{
		int mask = (a - b) >> 31;
		return (b & mask) | (a & ~mask);
	}

	inline int int_clamp (int value, int low, int high)
	{
		return int_min (int_max(value,low), high);
	}

	/*
		float IEEE-754 bit represenation

		0      0x00000000
		1.0    0x3f800000
		0.5    0x3f000000
		3      0x40400000
		+inf   0x7f800000
		-inf   0xff800000
		+NaN   0x7fc00000 or 0x7ff00000
		in general: number = (sign ? -1:1) * 2^(exponent) * 1.(mantissa bits)
	*/

	typedef union { unsigned int u; int s; float f; } inttofloat;

	#define FLOAT_AS_INT(f)		(*((int *) &(f)))
	#define FLOAT_AS_UINT(f)		(*((unsigned int *) &(f)))
	#define FLOAT_AS_UINTPOINTER(f)	( ((unsigned int *) &(f)))

	#define FLOAT_VALUE_0		0x00000000
	#define FLOAT_VALUE_1		0x3f800000
	#define FLOAT_SIGN_BIT		0x80000000U
	#define FLOAT_EXPON_MANTISSA	0x7FFFFFFFU

	//! code is taken from IceFPU
	//! Integer representation of a floating-point value.
#ifdef _FAST_MATH
	#define IR(x)                           ((unsigned int&)(x))
#else
	inline unsigned int IR(float x) {inttofloat tmp; tmp.f=x; return tmp.u;}
#endif

	//! Absolute integer representation of a floating-point value
	#define AIR(x)				(IR(x)&0x7fffffff)

	//! Floating-point representation of an integer value.
#ifdef _FAST_MATH
	#define FR(x)                           ((float&)(x))
#else
	inline float FR(unsigned int x) {inttofloat tmp; tmp.u=x; return tmp.f;}
	inline float FR(int x) {inttofloat tmp; tmp.s=x; return tmp.f;}
#endif

	//! integer representation of 1.0
	#define IEEE_1_0			0x3f800000
	//! integer representation of 255.0
	#define IEEE_255_0			0x437f0000

#ifdef _FAST_MATH
	#define	float_LOWER_0(f)		(float_AS_unsigned int(f) >  float_SIGN_BIT)
	#define	float_LOWER_EQUAL_0(f)	(float_AS_int(f) <= float_VALUE_0)
	#define	float_GREATER_0(f)	(float_AS_int(f) >  float_VALUE_0)
	#define	float_GREATER_EQUAL_0(f)	(float_AS_unsigned int(f) <= float_SIGN_BIT)
	#define	float_EQUAL_1(f)		(float_AS_unsigned int(f) == float_VALUE_1)
	#define	float_EQUAL_0(f)		( (float_AS_unsigned int(f) & float_EXPON_MANTISSA ) == float_VALUE_0)

	// only same sign
	#define	float_A_GREATER_B(a,b)	(float_AS_int((a)) > float_AS_int((b)))

#else

	#define	float_LOWER_0(n)		((n) <  0.0f)
	#define	float_LOWER_EQUAL_0(n)	((n) <= 0.0f)
	#define	float_GREATER_0(n)	((n) >  0.0f)
	#define	float_GREATER_EQUAL_0(n)	((n) >= 0.0f)
	#define	float_EQUAL_1(n)		((n) == 1.0f)
	#define	float_EQUAL_0(n)		((n) == 0.0f)
	#define	float_A_GREATER_B(a,b)	((a) > (b))
#endif


#ifndef REALINLINE
	#ifdef _MSC_VER
		#define REALINLINE __forceinline
	#else
		#define REALINLINE inline
	#endif
#endif


	//! conditional set based on mask and arithmetic shift
	REALINLINE unsigned int if_c_a_else_b ( const int condition, const unsigned int a, const unsigned int b )
	{
		return ( ( -condition >> 31 ) & ( a ^ b ) ) ^ b;
	}

	//! conditional set based on mask and arithmetic shift
	REALINLINE unsigned int if_c_a_else_0 ( const int condition, const unsigned int a )
	{
		return ( -condition >> 31 ) & a;
	}

	/*
		if (condition) state |= m; else state &= ~m;
	*/
	REALINLINE void setbit_cond ( unsigned int &state, int condition, unsigned int mask )
	{
		// 0, or any postive to mask
		//int conmask = -condition >> 31;
		state ^= ( ( -condition >> 31 ) ^ state ) & mask;
	}

	inline float round_( float x )
	{
		return floorf( x + 0.5f );
	}

	REALINLINE void clearFPUException ()
	{
#ifdef _FAST_MATH
#ifdef feclearexcept
		feclearexcept(FE_ALL_EXCEPT);
#elif defined(_MSC_VER)
		__asm fnclex;
#elif defined(__GNUC__) && defined(__x86__)
		__asm__ __volatile__ ("fclex \n\t");
#else
#  warn clearFPUException not supported.
#endif
#endif
	}

	REALINLINE float reciprocal_squareroot(const float x)
	{
#ifdef _FAST_MATH
		// comes from Nvidia
#if 1
		unsigned int tmp = (unsigned int(IEEE_1_0 << 1) + IEEE_1_0 - *(unsigned int*)&x) >> 1;
		float y = *(float*)&tmp;
		return y * (1.47f - 0.47f * x * y * y);
#elif defined(_MSC_VER)
		// an sse2 version
		__asm
		{
			movss	xmm0, x
			rsqrtss	xmm0, xmm0
			movss	x, xmm0
		}
		return x;
#endif
#else // no fast math
		return 1.f / sqrtf ( x );
#endif
	}



	REALINLINE float reciprocal ( const float f )
	{
#ifdef _FAST_MATH
		//! i do not divide through 0.. (fpu expection)
		// instead set f to a high value to get a return value near zero..
		// -1000000000000.f.. is use minus to stay negative..
		// must test's here (plane.normal dot anything ) checks on <= 0.f
		return 1.f / f;
		//unsigned int x = (-(AIR(f) != 0 ) >> 31 ) & ( IR(f) ^ 0xd368d4a5 ) ^ 0xd368d4a5;
		//return 1.f / FR ( x );
#else // no fast math
		return 1.f / f;
#endif
	}


	REALINLINE float reciprocal_approxim ( const float p )
	{
#ifdef _FAST_MATH
		register unsigned int x = 0x7F000000 - IR ( p );
		const float r = FR ( x );
		return r * (2.0f - p * r);
#else // no fast math
		return 1.f / p;
#endif
	}


	REALINLINE int floor32(float x)
	{
#ifdef _FAST_MATH
		const float h = 0.5f;

		int t;

#if defined(_MSC_VER)
		__asm
		{
			fld	x
			fsub	h
			fistp	t
		}
#elif defined(__GNUC__)
		__asm__ __volatile__ (
			"fsub %2 \n\t"
			"fistpl %0"
			: "=m" (t)
			: "t" (x), "f" (h)
			: "st"
			);
#else
#  warn _FAST_MATH not supported.
		return (int) floorf ( x );
#endif
		return t;
#else // no fast math
		return (int) floorf ( x );
#endif
	}


	REALINLINE int ceil32 ( float x )
	{
#ifdef _FAST_MATH
		const float h = 0.5f;

		int t;

#if defined(_MSC_VER)
		__asm
		{
			fld	x
			fadd	h
			fistp	t
		}
#elif defined(__GNUC__)
		__asm__ __volatile__ (
			"fadd %2 \n\t"
			"fistpl %0 \n\t"
			: "=m"(t)
			: "t"(x), "f"(h)
			: "st"
			);
#else
#  warn _FAST_MATH not supported.
		return (int) ceilf ( x );
#endif
		return t;
#else // not fast math
		return (int) ceilf ( x );
#endif
	}



	REALINLINE int round32(float x)
	{
#if defined(_FAST_MATH)
		int t;

#if defined(_MSC_VER)
		__asm
		{
			fld   x
			fistp t
		}
#elif defined(__GNUC__)
		__asm__ __volatile__ (
			"fistpl %0 \n\t"
			: "=m"(t)
			: "t"(x)
			: "st"
			);
#else
#  warn _FAST_MATH not supported.
		return (int) round_(x);
#endif
		return t;
#else // no fast math
		return (int) round_(x);
#endif
	}

	inline float float_max3(const float a, const float b, const float c)
	{
		return a > b ? (a > c ? a : c) : (b > c ? b : c);
	}

	inline float float_min3(const float a, const float b, const float c)
	{
		return a < b ? (a < c ? a : c) : (b < c ? b : c);
	}

	inline float fract ( float x )
	{
		return x - floorf ( x );
	}


#endif

