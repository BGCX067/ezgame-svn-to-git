#include "stdafx.h"
#include "MathUtil.h"

int GetRandomNumber(int iStart, int iEnd)
{
    int iRange, iResult;

	if (iStart >= iEnd) return iStart;
	
	iRange = (iEnd - iStart)+1;
	
	if (iRange >= 32767) return iStart; // ������ ���.

	iResult = iStart + (rand() / (RAND_MAX / (double)iRange));
	
	return iResult;
}


