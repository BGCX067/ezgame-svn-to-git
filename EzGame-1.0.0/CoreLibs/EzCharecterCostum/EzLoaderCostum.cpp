#include "../EzCore/EzXML.h"
#include "EzLoaderCostum.h"
#include <string.h>

namespace GameEasy
{

EzLoaderCostum::EzLoaderCostum(EzFile *pFile)
{
	m_pXml = createEzXMLReader(pFile);
	m_pXml ? bOpen = true : bOpen = false;

	if(bOpen)
	{
		bOpen = Parsing();
	}

}

EzLoaderCostum::~EzLoaderCostum(void)
{
}



void EzLoaderCostum::setPartsChange(u32 idx, u16 item_id)
{
	EzMap<u16, SCostumItem>::Iterator  it;
}

const c8* EzLoaderCostum::getPartsName(u32 idx)
{
	return m_aParts[idx]->name.c_str();
}

const SCostumParts* EzLoaderCostum::getPartsData(u32 idx)
{
	return m_aParts[idx];
}



bool EzLoaderCostum::add()
{
	const char* attrName;

	if(!strcmp(m_aStack.getLast().c_str(), "setting"))
	{
		for(u32 idx = 0; idx < m_pXml->getAttributeCount(); ++idx)
		{

			attrName = m_pXml->getAttributeName(idx);

			if(!strcmp(attrName, "workingfolder"))
			{
				Settingdata.workingfolder = m_pXml->getAttributeValue(idx);
			}
			else if(!strcmp(attrName, "bone"))
			{
				Settingdata.boneFilename = m_pXml->getAttributeValue(idx);

			}
			else
			{
				return false;
			}
		}

	}
	else if(!strcmp(m_aStack.getLast().c_str(), "parts"))
	{
		for(u32 idx = 0; idx < m_pXml->getAttributeCount(); ++idx)
		{
			attrName = m_pXml->getAttributeName(idx);

			if(!strcmp(attrName, "name"))
			{
				printf("working");
			}
			else if(!strcmp(attrName, "file"))
			{
				printf("file");
			}
		}


	}

	return true;
}



bool EzLoaderCostum::partsAttribute()
{
	const char* attrName;

	SCostumParts* part = new SCostumParts;

	for(u32 idx = 0; idx < m_pXml->getAttributeCount(); ++idx)
	{

		attrName = m_pXml->getAttributeName(idx);

		if(!strcmp(attrName, "name"))
		{
			part->name = m_pXml->getAttributeValue(idx);
			
		}
		//else if(!strcmp(attrName, "enumtype"))
		//{
		//	currentenum = m_pXml->getAttributeValue(idx);

		//}
		else if(!strcmp(attrName, "current"))
		{
			part->currentItem = atoi(m_pXml->getAttributeValue(idx));

		//	if(!part->currentItem)
		//	{
		//		delete part;
		//		return false;
		//	}
		}
	}

	m_aParts.push_back(part);

	return true;

}


bool EzLoaderCostum::parts()
{
	const char* attrName;

	SCostumItem value;

	for(u32 idx = 0; idx < m_pXml->getAttributeCount(); ++idx)
	{

		attrName = m_pXml->getAttributeName(idx);

		if(!strcmp(attrName, "name"))
		{
			value.name = m_pXml->getAttributeValue(idx);
		}
		else if(!strcmp(attrName, "file"))
		{
			value.filename = m_pXml->getAttributeValue(idx);
		}

	}

	m_aParts.getLast()->ItemArray.push_back(value);
	

	return true;
}

bool EzLoaderCostum::Parsing()
{
	const char* element;
	const char* end_element;

	while(m_pXml && m_pXml->read())
	{
		switch(m_pXml->getNodeType())
		{
		case EXN_TEXT:
			//	// in this xml file, the only text which occurs is the messageText
			//messageText = xml->getNodeData();
			break;
		case EXN_ELEMENT:

			element = m_pXml->getNodeName();

			if(!strcmp("parts", element))
			{
				partsAttribute();
			}
			else if(!strcmp("add", element))
			{
				if(!strcmp(m_aStack.getLast().c_str(), "parts"))
				{
					if(!parts()) return false;
				}
				else
				{
					if(!add()) return false;
				}
			}

			m_aStack.push_back(EzStringc(element));
	
			break;

		case EXN_ELEMENT_END:
			if(m_aStack.size() == 0)
				return false;

			end_element = m_pXml->getNodeName();

			if(strcmp(m_aStack.getLast().c_str(), end_element))
			{
				return false;
			}

			m_aStack.erase(m_aStack.size() - 1);


			break;

		}
	}


	return true;
}


EzCostum* EzLoaderCostum::getCostumObject()
{
	EzCostum* pCostum = new EzCostum;


	pCostum->m_aParts = m_aParts;

	pCostum->m_WorkingFolder = Settingdata.workingfolder;
	pCostum->m_BoneFileName = Settingdata.boneFilename;

	return pCostum;
}

}
