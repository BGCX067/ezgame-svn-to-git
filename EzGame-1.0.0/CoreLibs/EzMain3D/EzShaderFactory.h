#pragma once

#include "../EzCore/EzSingleton.h"
#include "../EzCore/EzMap.h"
#include "../EzCore/EzString.h"

class EzShaderLibrary;
class EzShader;


class EzShaderFactory: public EzSingleton<EzShaderFactory>
{
public:
	EzShaderFactory(void);
	~EzShaderFactory(void);

	static void init();
	static void shutdown();

	static bool registerShaderLibrary(EzShaderLibrary* pShaderLib);
	static bool unregisterShaderLibrary(EzShaderLibrary* pShaderLib);
	static EzShaderLibrary* getShaderLib(const char* ClassName);

	static EzShaderLibrary* getFirstShaderLibrary();
	static EzShaderLibrary* getNextShaderLibrary();


	
	int registerShader(EzShader* pShader);
	void unregisterShader(const char* ClassName);

	static EzShader* getShader(const char* ShaderName);



protected:
	typedef EzMap<EzStringc, EzShaderLibrary*>				EzShaderFactoryMap;
	typedef EzMap<EzStringc, EzShader*>						EzShaderMap;

	EzShaderFactoryMap				m_MapShaderLibs;
	EzShaderMap						m_MapShaders;
	EzShaderFactoryMap::Iterator	m_itShader;
};


#define EzRegisterShader(classname) \
	EzShader* pShader =(EzShader*) EzNew classname; \
	EzShaderFactory::get().registerShader(pShader)

#define EzUnregisterShader(classname) \
	EzShaderFactory::get().unregisterShader(#classname)


