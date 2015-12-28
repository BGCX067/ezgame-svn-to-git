#include "stdafx.h"
#include "MathUtil.h"

int GetRandomNumber(int iStart, int iEnd)
{
    int iRange, iResult;

	if (iStart >= iEnd) return iStart;
	
	iRange = (iEnd - iStart)+1;
	
	if (iRange >= 32767) return iStart; // 범위를 벗어남.

	iResult = iStart + (rand() / (RAND_MAX / (double)iRange));
	
	return iResult;
}


