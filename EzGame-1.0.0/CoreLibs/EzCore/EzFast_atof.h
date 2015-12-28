// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Nerilicht Engine" and the "irrXML" project.
// For conditions of distribution and use, see copyright notice in irrlicht.h and irrXML.h

#ifndef __FAST_A_TO_F_H_INCLUDED__
#define __FAST_A_TO_F_H_INCLUDED__

#include <stdlib.h>
#include "Math.h"
#include <float.h> // For FLT_MAX
#include <limits.h> // For INT_MAX / UINT_MAX


// we write [16] here instead of [] to work around a swig bug
const float fast_atof_table[16] = {
	0.f,
	0.1f,
	0.01f,
	0.001f,
	0.0001f,
	0.00001f,
	0.000001f,
	0.0000001f,
	0.00000001f,
	0.000000001f,
	0.0000000001f,
	0.00000000001f,
	0.000000000001f,
	0.0000000000001f,
	0.00000000000001f,
	0.000000000000001f
};

//! Convert a simple EzStringc of base 10 digits into a signed 32 bit integer.
//! \param[in] in: The EzStringc of digits to convert. Only a leading - or + followed 
//!					by digits 0 to 9 will be considered.  Parsing stops at the
//!					first non-digit.
//! \param[out] out: (optional) If provided, it will be set to point at the first
//!					 character not used in the calculation.
//! \return The signed integer value of the digits. If the EzStringc specifies too many
//!			digits to encode in an int then +INT_MAX or -INT_MAX will be returned.
inline int EzStrtol10(const char* in, const char** out=0)
{
	if(!in)
		return 0;

	bool negative = false;
	if('-' == *in)
	{
		negative = true;
		++in;
	}
	else if('+' == *in)
		++in;

	unsigned int unsignedValue = 0;

	while ( ( *in >= '0') && ( *in <= '9' ))
	{
		unsignedValue = ( unsignedValue * 10 ) + ( *in - '0' );
		++in;

		if(unsignedValue > (unsigned int)INT_MAX)
		{
			unsignedValue = (unsigned int)INT_MAX;
			break;
		}
	}
	if (out)
		*out = in;

	if(negative)
		return -((int)unsignedValue);
	else
		return (int)unsignedValue;
}

//! Converts a sequence of digits into a whole positive floating point value.
//! Only digits 0 to 9 are parsed.  Parsing stops at any other character, 
//! including sign characters or a decimal point.
//! \param in: the sequence of digits to convert.
//! \param out: (optional) will be set to point at the first non-converted character.
//! \return The whole positive floating point representation of the digit sequence.
inline float EzStrtof10(const char* in, const char * * out = 0)
{
	if(out)
		*out = in;

	if(!in)
		return 0.f;

	static const unsigned int MAX_SAFE_INT_VALUE = UINT_MAX / 10 - 10;
	float floatValue = 0.f;
	unsigned int intValue = 0;

	// Use integer arithmetic for as long as possible, for speed
	// and precision.
	while ( ( *in >= '0') && ( *in <= '9' ) )
	{
		// If it looks like we're going to overflow, bail out
		// now and start using floating point.
		if(intValue >= MAX_SAFE_INT_VALUE)
			break;

		intValue = ( intValue * 10) + ( *in - '0' );
		++in;
	}

	floatValue = (float)intValue;

	// If there are any digits left to parse, then we need to use 
	// floating point arithmetic from here.
	while ( ( *in >= '0') && ( *in <= '9' ) )
	{
		floatValue = ( floatValue * 10.f ) + (float)( *in - '0' );
		++in;
		if(floatValue > FLT_MAX) // Just give up.
			break;
	}

	if(out)
		*out = in;

	return floatValue;
}

//! Provides a fast function for converting a EzStringc into a float.
//! This is not guaranteed to be as accurate as atof(), but is 
//! approximately 6 to 8 times as fast.
//! \param[in] in: The EzStringc to convert. 
//! \param[out] out: The resultant float will be written here.
//! \return A pointer to the first character in the EzStringc that wasn't
//!         use to create the float value.
inline const char* EzFast_atof_move( const char * in, float & out)
{
	// Please run this regression test when making any modifications to this function:
	// https://sourceforge.net/tracker/download.php?group_id=74339&atid=540676&file_id=298968&aid=1865300

	out = 0.f;
	if(!in)
		return 0;

	bool negative = false;
	if(*in == '-')
	{
		negative = true;
		++in;
	}

	float value = EzStrtof10 ( in, &in );

	if (*in == '.')
	{
		++in;

		const char * afterDecimal = in;
		float decimal = EzStrtof10 ( in, &afterDecimal );
		decimal *= fast_atof_table[afterDecimal - in];

		value += decimal;

		in = afterDecimal;
	}

	if ('e' == *in || 'E' == *in)
	{
		++in;
		// Assume that the exponent is a whole number.
		// strtol10() will deal with both + and - signs,
		// but cast to (float) to prevent overflow at FLT_MAX
		value *= (float)pow(10.0f, (float)EzStrtol10(in, &in));
	}

	if(negative)
		out = -value;
	else
		out = value;

	return in;
}

//! Convert a EzStringc to a floating point number
//! \param floatAsString: The EzStringc to convert. 
inline float EzFast_atof(const char* floatAsString)
{
	float ret;
	EzFast_atof_move(floatAsString, ret);
	return ret;
}


#endif

