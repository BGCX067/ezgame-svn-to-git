#include "EzObjectFactory.h"
#include "EzNode.h"
#include "EzTriShape.h"
#include "EzTriShapeData.h"
#include "EzMaterialProperty.h"
#include "EzTexturingProperty.h"



template<> 
EzObjectFactory* EzSingleton<EzObjectFactory>::ms_pSingleton = 0;

EzObjectFactory::EzObjectFactory(void)
:m_pLoaders(NULL)
{
}

EzObjectFactory::~EzObjectFactory(void)
{
}

void EzObjectFactory::init()
{

	EzExternalNew EzObjectFactory;

	get().m_pLoaders = EzNew EzObjectFactoryMap; 

	EzRegisterObject(EzNode);
	EzRegisterObject(EzTriShape);
	EzRegisterObject(EzTriShapeData);
	EzRegisterObject(EzMaterialProperty);
	EzRegisterObject(EzTexturingProperty);


}

void EzObjectFactory::shutdown()
{

	EzDelete get().m_pLoaders;
	EzExternalDelete ms_pSingleton;
	ms_pSingleton = NULL;
	
}

EzObject * EzObjectFactory::createObject(const char* ClassName)
{
	EzObjectFactoryMap::Node* pNode;
	pNode = m_pLoaders->find(EzStringc(ClassName));
	
	if(pNode)
	{
		createObjfunc pfnFunc = pNode->getValue();

		return pfnFunc();
	}

	return NULL;

}

int EzObjectFactory::registerLoader(const char* ClassName, createObjfunc pfnFunc)
{
#ifdef _DEBUG
	EzObjectFactoryMap::Node* pNode;
	pNode = m_pLoaders->find(EzStringc(ClassName));
	EZASSERT(!pNode);
#endif

	m_pLoaders->insert(EzStringc(ClassName), pfnFunc);
	return 0;

}

void EzObjectFactory::unregisterLoader(const char* ClassName)
{
	m_pLoaders->remove(EzStringc(ClassName));
}
