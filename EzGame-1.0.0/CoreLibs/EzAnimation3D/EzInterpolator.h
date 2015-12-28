#pragma once

#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzCore.h"
#include "../EzMain3D/EzObject.h"
#include "SAniKeyFrame.h"


class EzObjectNET;


typedef void (*InterpFunction)(f32 fTime, const void* pKey0, const void* pKey1, void* pResult);

EzSmartPointer(EzInterpolator);

class EzInterpolator : public EzObject
{
	EzDeclareRTTI;

public:
	EzInterpolator(void);
	~EzInterpolator(void);

	virtual bool update(f32 fTime, EzTransform &Value);


	static void RegisterLoader();


	static InterpFunction getInterpFunctions(EANI_KEYCONTENT content, EANI_KEYFREAMTYPE keyFreamType);
	static void setInterpFunctions(EANI_KEYCONTENT content, EANI_KEYFREAMTYPE keyFreamType, InterpFunction fInterp);

protected:

	 bool timeHasChanged(float fTime) 
	 {
		 return (m_fLastTime != fTime);
	 }

	 f32 m_fLastTime;

	
	//static void* Interp(KeyContent content, KeyType type);


	//static void Interpolate(f32 fTime, const void* pKey0, const void* pKey1, void* pResult);




	static InterpFunction InterpFunctions[NUMKEYCONTENTS*MAX_ANI_KEYFREAMTYPE];
	
};

static void Interpolate_pos1(f32 fTime, const void* pKey0, const void* pKey1, void* pResult);
static void Interpolate_rot1(f32 fTime, const void* pKey0, const void* pKey1, void* pResult);
static void interpolate_Scale_Linear(f32 fTime, const void* pKey0, const void* pKey1, void* pResult);


    //    SetCreateFunction(content, type, classname::CreateFromStream); \
    //    SetSaveFunction(content, type, classname::SaveToStream); \
    //    SetCopyFunction(content, type, classname::Copy); \
    //    SetArrayFunction(content, type, classname::CreateArray); \
    //    SetDeleteFunction(content, type, classname::DeleteArray); \
    //    SetKeySize(content, type, sizeof(classname)); \
    //    RegisterSupportedFunctions(content, type); \
    //    return 1; \
    //}




