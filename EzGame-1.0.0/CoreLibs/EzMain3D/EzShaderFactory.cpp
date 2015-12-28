#include "EzShaderFactory.h"
#include "EzShaderLibrary.h"

template<> 
EzShaderFactory* EzSingleton<EzShaderFactory>::ms_pSingleton = 0;


EzShaderFactory::EzShaderFactory(void)
{
}

EzShaderFactory::~EzShaderFactory(void)
{
}

void EzShaderFactory::init()
{
	EzExternalNew EzShaderFactory;


	//EzShaderLibrary* pShaderLib = EzNew EzShaderLibrary;

	//pShaderLib->setName("Default Shader");
	//
	//registerShaderLibrary(pShaderLib);


}

void EzShaderFactory::shutdown()
{
	EzExternalDelete ms_pSingleton;
	ms_pSingleton = NULL;

}


bool EzShaderFactory::registerShaderLibrary(EzShaderLibrary* pShaderLib)
{

	EzShaderFactoryMap::Node* pNode;

	pNode = get().m_MapShaderLibs.find(pShaderLib->getName());

	if(pNode)
	{
		return false;
	}


	get().m_MapShaderLibs.insert(pShaderLib->getName(), pShaderLib);

	return true;
}

bool EzShaderFactory::unregisterShaderLibrary(EzShaderLibrary* pShaderLib)
{
	bool bRet = get().m_MapShaderLibs.remove(pShaderLib->getName());

	if(bRet)
	{
		EzDelete pShaderLib;
	}

	return bRet;
}

EzShaderLibrary* EzShaderFactory::getShaderLib(const char* ClassName)
{
	EzShaderFactoryMap::Node* pNode;
	pNode = get().m_MapShaderLibs.find(EzStringc(ClassName));

	if(pNode)
	{
		return pNode->getValue();
	}

	return NULL;
}

EzShaderLibrary* EzShaderFactory::getFirstShaderLibrary()
{
	EzShaderFactoryMap::Iterator  it;

	get().m_itShader = get().m_MapShaderLibs.getIterator();

	if(!get().m_itShader.atEnd())
	{
		return get().m_itShader->getValue();
	}

	return NULL;
}

EzShaderLibrary* EzShaderFactory::getNextShaderLibrary()
{

	get().m_itShader++;

	if(get().m_itShader.atEnd())
	{
		return NULL;
	}
	else
	{
		return get().m_itShader->getValue();
	}
}


int EzShaderFactory::registerShader(EzShader* pShader)
{
#ifdef _DEBUG
	EzShaderMap::Node* pNode;
	pNode = m_MapShaders.find(pShader->getName());
	EZASSERT(!pNode);
#endif

	m_MapShaders.insert(pShader->getName(), pShader);
	return 0;

}

void EzShaderFactory::unregisterShader(const char* ClassName)
{
	m_MapShaders.remove(EzStringc(ClassName));
}

EzShader* EzShaderFactory::getShader(const char* ShaderName)
{
	EzShaderMap::Node* pNode;
	pNode = get().m_MapShaders.find(ShaderName);

	if(pNode)
		return pNode->getValue();

	return NULL;
}