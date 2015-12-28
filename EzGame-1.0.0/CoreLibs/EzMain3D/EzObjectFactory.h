#pragma once

#include "../EzCore/EzSingleton.h"
#include "../EzCore/EzMap.h"
#include "../EzCore/EzString.h"

class EzObject;

typedef EzObject* (*createObjfunc)();

class EzObjectFactory : public EzSingleton<EzObjectFactory>
{
public:
	EzObjectFactory(void);
	~EzObjectFactory(void);

	static void init();
	static void shutdown();

	int registerLoader(const char* ClassName, createObjfunc pfnFunc);
	void unregisterLoader(const char* ClassName);
	EzObject* createObject(const char* ClassName);


protected:

	typedef EzMap<EzStringc, createObjfunc>				EzObjectFactoryMap;

	EzObjectFactoryMap*  m_pLoaders;
};


inline EzObject* EzCreateObject(const char* ClassName) 
{
	return EzObjectFactory::get().createObject(ClassName);
}

#define EzRegisterObject(classname) \
	EzObjectFactory::get().registerLoader(#classname, classname::createObject)

#define EzUnregisterStream(classname) \
	EzObjectFactory::get().unregisterLoader(#classname)


