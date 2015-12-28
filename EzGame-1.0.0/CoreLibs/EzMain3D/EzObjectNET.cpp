#include "EzAnimator.h"
#include "EzObjectNET.h"
#include "EzStream.h"


EzImplementRTTI(EzObjectNET, EzObject);


EzObjectNET::EzObjectNET(void)
: m_spAnimator(0)
{
}

EzObjectNET::~EzObjectNET(void)
{
}

void EzObjectNET::loadBinary(EzStream& Stream)
{
	EzObject::loadBinary(Stream);

	EzStringc  name;

	EzStreamStringLoad(Stream, name);

	setName(name.c_str());


}
void EzObjectNET::saveBinary(EzStream& Stream)
{
	EzObject::saveBinary(Stream);

	EzStreamStringWrite(Stream, getName());


	EzAnimator* pAnimator = m_spAnimator;
	while (pAnimator && !pAnimator->isStreamable())
	{
		pAnimator = pAnimator->getNextAnimator();
		Stream.getLinkIDFromObject(pAnimator);
	}
}

void EzObjectNET::copyMember(EzObjectNET* pDest)
{

	pDest->setName(getName());

	for(unsigned int n = 0 ; n < m_aExtraDataList.size(); ++n)
	{
		pDest->m_aExtraDataList.push_back(m_aExtraDataList[n]);
		m_aExtraDataList[n]->addRef();
	}

	pDest->m_spAnimator = m_spAnimator;

}



void EzObjectNET::addExtraData( EzExtraData * obj) 
{
	m_aExtraDataList.push_back( obj);
	obj->addRef();
}


void EzObjectNET::removeExtraData( EzExtraData * obj ) 
{

	for(unsigned int i = 0; i < m_aExtraDataList.size(); i++)
	{
		if(m_aExtraDataList[i] == obj)
		{
			m_aExtraDataList.erase(i);
			obj->release();
			break;
		}
	}
}

void EzObjectNET::clearExtraData() 
{
	for(unsigned int i = 0; i < m_aExtraDataList.size(); i++)
	{
		m_aExtraDataList[i]->release();
	}

	m_aExtraDataList.clear();
}

const EzArray<EzExtraData*>& EzObjectNET::getExtraData() const
{
	return m_aExtraDataList;

}

EzAnimator* EzObjectNET::getAnimator()
{
	return m_spAnimator;
}

bool EzObjectNET::isAnimated() 
{
	return ( m_spAnimator != NULL );
}

void EzObjectNET::addAnimator( EzAnimator* pAnimator ) 
{
	pAnimator->setTarget( this );

	if(!m_spAnimator)
	{
		m_spAnimator = pAnimator;
	}
	else
	{
		pAnimator->setNextAnimator(m_spAnimator);
		m_spAnimator = pAnimator;
	}
}

void EzObjectNET::removeAnimator( EzAnimator* pAnimator ) 
{
	EZASSERT(pAnimator);

	if(m_spAnimator && pAnimator)
	{
		if(m_spAnimator == pAnimator)
		{
			EzAnimatorPtr spSave = pAnimator;				// Prevent early deletion
			m_spAnimator = pAnimator->getNextAnimator();
			pAnimator->setNextAnimator(NULL);

		}
		else
		{

			EzAnimator* pPrev = m_spAnimator;
			EzAnimator* pCurr = pPrev->getNextAnimator();
		

			while(pCurr && pCurr !=  pAnimator) 
			{
				pPrev = pCurr;
				pCurr = pCurr->getNextAnimator();
			}

			if (pCurr)
			{
				EzAnimatorPtr spSave = pAnimator;			// Prevent early deletion
				pPrev->setNextAnimator( pAnimator->getNextAnimator() );
				pAnimator->setNextAnimator( NULL );
			} 
		}
	}
}
void EzObjectNET::clearAnimators() 
{
	EzAnimatorPtr spAnimator = m_spAnimator;
	while ( spAnimator != NULL ) 
	{
		spAnimator->setTarget(NULL);
		spAnimator = spAnimator->getNextAnimator();
	}
}


bool EzObjectNET::isEqual(EzObject* pObject)
{
	if(!EzObject::isEqual(pObject))
		return false;


	return true;
}