#include "EzGeometry.h"
#include "EzRenderPass.h"
#include "EzCullingProcess.h"
#include "EzStream.h"
#include "EzShaderFactory.h"
 

EzImplementRTTI(EzGeometry, EzAVObject);

EzGeometry::EzGeometry(void)
: m_pGeometryData(0), m_pSkinInstance(0),  m_ApplyMode(APPLY_MODULATE), m_pRendererData(NULL), m_spRenderPass(NULL), m_pShaderLib(NULL)
{
}

EzGeometry::~EzGeometry(void)
{
}

void EzGeometry::loadBinary(EzStream& Stream)
{
	EzStringc strShaderName;
	EzAVObject::loadBinary(Stream);

	m_pGeometryData = EzDynamicCast(EzGeometryData, EzStreamLoadObject(Stream));
	m_pSkinInstance = EzDynamicCast(EzSkinInstance, EzStreamLoadObject(Stream));

	EzStreamStringLoad(Stream, strShaderName);

	m_pShaderLib = EzShaderFactory::getShaderLib(strShaderName.c_str());
}


void EzGeometry::saveBinary(EzStream& Stream)
{
	EzAVObject::saveBinary(Stream);

	EzStreamWriteObject(Stream, m_pGeometryData);
	EzStreamWriteObject(Stream, m_pSkinInstance);

	EZASSERT(m_pShaderLib)

	EzStreamStringWrite(Stream, m_pShaderLib->getName().c_str());


}

void EzGeometry::onVisible(EzCullingProcess& Culler)
{
	Culler.append(this);

}

void EzGeometry::copyMember(EzGeometry* pDest)
{
	EzCopyMember(EzAVObject);

	pDest->m_pGeometryData = m_pGeometryData;

	if(m_pGeometryData)
		m_pGeometryData->addRef();

	pDest->m_pSkinInstance = m_pSkinInstance;

	if(m_pSkinInstance)
		m_pSkinInstance->addRef();

	pDest->m_ApplyMode = m_ApplyMode;
}

EzGeometryData* EzGeometry::getData() const
{
	return m_pGeometryData;
}

void EzGeometry::updateWorldBound()
{
	m_WorldBound.update(m_pGeometryData->getBound(), getWorldTransform().getMatrix());
}

void EzGeometry::updateEffectsDownPass(EzDynamicEffectState* pParentState)
{
	if (pParentState)
	{
		m_pEffectState = pParentState;
	}
	else
	{
		m_pEffectState = NULL;   
	}
}


EzSkinInstance*  EzGeometry::getSkinInstance() const
{
	return m_pSkinInstance;
}

void EzGeometry::setSkinInstance(EzSkinInstance* skinInstance)
{
	m_pSkinInstance = skinInstance;
}


void EzGeometry::setData( EzGeometryData * data )
{
	m_WorldBound = data->getBound();
	m_pGeometryData = data;


}
