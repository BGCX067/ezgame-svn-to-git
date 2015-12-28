#include "EzInterpolator.h"


EzImplementRTTI(EzInterpolator, EzObject);

InterpFunction EzInterpolator::InterpFunctions[NUMKEYCONTENTS*MAX_ANI_KEYFREAMTYPE];

void EzInterpolator::RegisterLoader() 
{ 
	static bool bRegistered = false; 
      
	if(bRegistered)
		return;
	
	bRegistered = true; 

	setInterpFunctions(POSKEY, LINEAR_KEY, Interpolate_pos1);
	setInterpFunctions(ROTKEY, LINEAR_KEY, Interpolate_rot1);
	setInterpFunctions(FLOATKEY, LINEAR_KEY, interpolate_Scale_Linear);

}


EzInterpolator::EzInterpolator(void)
{
	RegisterLoader();
}

EzInterpolator::~EzInterpolator(void)
{
}

bool EzInterpolator::update(f32 fTime, EzTransform &Value)
{
	return false;

}


void EzInterpolator::setInterpFunctions(EANI_KEYCONTENT content, EANI_KEYFREAMTYPE keyFreamType, InterpFunction fInterp)
{
	InterpFunctions[content * NUMKEYCONTENTS + keyFreamType] = fInterp;

}

InterpFunction EzInterpolator::getInterpFunctions(EANI_KEYCONTENT content, EANI_KEYFREAMTYPE keyFreamType)
{
	return InterpFunctions[content * NUMKEYCONTENTS + keyFreamType];

}




void Interpolate_pos1(f32 fTime, const void* pKey0, const void* pKey1, void* pResult)
{
	EzPoint3* pPosKey0 = (EzPoint3*)pKey0;
    EzPoint3* pPosKey1 = (EzPoint3*)pKey1;

    // interpolate positions
    *(EzPoint3*)pResult = fTime*(*pPosKey1) + 
        (1.0f-fTime)*(*pPosKey0);
}

void Interpolate_rot1(f32 fTime, const void* pKey0, const void* pKey1, void* pResult)
{
	EzQuaternion* pQutKey0 = (EzQuaternion*)pKey0;
    EzQuaternion* pQutKey1 = (EzQuaternion*)pKey1;
	EzQuaternion Quttemp;


	*(EzQuaternion*)pResult = Quttemp.slerp( *pQutKey0, *pQutKey1, fTime);
}

void interpolate_Scale_Linear(f32 fTime, const void* pKey0, const void* pKey1, void* pResult)
{
	f32* pOutKey0 = (f32*)pKey0;
	f32* pOutKey1 = (f32*)pKey1;

	*(f32*)pResult = (*pOutKey0) * (1-fTime) + (*pOutKey1) *fTime;
}

void interpolate_floatKey_Bezier(f32 fTime, const void* pKey0, const void* pKey1, void* pResult)
{
	f32* pOutKey0 = (f32*)pKey0;
	f32* pOutKey1 = (f32*)pKey1;

	*(f32*)pResult = (*pOutKey0) * (1-fTime) + (*pOutKey1) *fTime;
}


//const NiBezFloatKey* pBez0 = (const NiBezFloatKey*) pKey0;
//const NiBezFloatKey* pBez1 = (const NiBezFloatKey*) pKey1;
//
//// interpolate between this key and pQ
//*(float*)pResult = NiInterpScalar::Bezier(fTime,pBez0->GetValue(),
//										  pBez0->GetOutTan(),pBez1->GetValue(),pBez1->GetInTan());

