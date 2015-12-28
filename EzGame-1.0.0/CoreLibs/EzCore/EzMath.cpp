#include "EzMath.h"
#include "EzPoint3.h"
#include "EzMemoryDefines.h"

unsigned int* initSqrtTable()
{
	float f;
	unsigned int* pUIRep = (unsigned int*)&f;

	// A table of square roots with 7-bit precision requires 256 entries.
	unsigned int* pSqrtTable = EzAlloc(unsigned int, 256);

	for(unsigned int i=0; i < 128; i++)
	{
		// Build a float with the bit pattern i as mantissa and 0 as exponent.
		*pUIRep = (i<<16) | (127<<23);
		f = reciprocal_squareroot(f);

		// Store the first 7 bits of the mantissa in the table.
		pSqrtTable[i] = ((*pUIRep) & 0x7fffff);

		// Build a float with the bit pattern i as mantissa and 1 as exponent.
		*pUIRep = (i << 16) | (128 << 23);
		f = reciprocal_squareroot(f);

		// Store the first 7 bits of the mantissa in the table.
		pSqrtTable[i+128] = ((*pUIRep) & 0x7fffff);
	}

	return pSqrtTable;
}

//---------------------------------------------------------------------------
// This algorithm was published as "A High Speed, Low Precision Square Root",
// by Paul Lalonde and Robert Dawon, Dalhousie University, Halifax, Nova
// Scotia, Canada, on pp. 424-6 of "Graphics Gems", edited by Andrew Glassner,
// Academic Press, 1990.

// These results are generally faster than their full-precision counterparts
// (except on modern PC hardware), but are only worth 7 bits of binary
// precision (1 in 128).
// [A table for 7-bit precision requires 256 entries.]
void unitizeVectors(EzPoint3* pkV, unsigned int uiNum, unsigned int uiStride)
{
	float fLeng;
	unsigned short sExp;
	static unsigned int* ms_pSqrtTable = initSqrtTable();

	// This pointer allows us to treat the float as its integer bit 
	// representation.
	unsigned int *puiRep = (unsigned int*) &fLeng;

	//EZASSERT(uiStride % sizeof(float) == 0);

	// WARNING:  SERIOUS ALIASING going on here.  Be very careful with
	// optimization flags.
	for (unsigned int i = 0; i < uiNum; i++)
	{
		// Compute the squared length normally.
		fLeng = pkV->X * pkV->X + pkV->Y * pkV->Y + pkV->Z * pkV->Z;

		if (!(*puiRep)) // If the squared length is zero, exit.
		{
			fLeng = 0.0f;
		}
		else
		{
			// Shift and mask the exponent from the float.
			sExp = ((*puiRep) >> 23) - 127;

			// Mask the exponent away.
			*puiRep &= 0x7fffff;

			// If the exponent is odd, use the upper half of the square root
			// table.
			if (sExp & 0x1)
				*puiRep |= 0x800000;

			// Compute the sqrt'ed exponent (divide by 2).
			sExp >>= 1;

			// Build the new floating point representation by ORing the
			// looked-up mantissa with the computed exponent.
			*puiRep = ms_pSqrtTable[(*puiRep) >> 16] | ((sExp + 127) << 23);

			fLeng = 1.0f / fLeng;   // Invert the length.
		}

		pkV->X *= fLeng;
		pkV->Y *= fLeng; 
		pkV->Z *= fLeng;

		pkV = (EzPoint3*) ((unsigned int) pkV + uiStride);
	//	pkV->normalize();
	}
}
