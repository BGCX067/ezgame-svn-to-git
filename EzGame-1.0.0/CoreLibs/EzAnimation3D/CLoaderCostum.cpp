#include "../core/XML.h"
#include "CLoaderCostum.h"
#include <string.h>

namespace GameEasy
{

CLoaderCostum::CLoaderCostum(CFile *pFile)
{
	m_pXml = createNeriXMLReader(pFile);
	m_pXml ? bOpen = true : bOpen = false;

	if(bOpen)
	{
		bOpen = Parsing();
	}

}

CLoaderCostum::~CLoaderCostum(void)
{
}



void CLoaderCostum::setPartsChange(u32 idx, u16 item_id)
{
	map<u16, SCostumItem>::Iterator  it;


	it = m_aParts[idx]->itemlist.find(item_id);

	if(!it.atEnd())
	{
		m_aParts[idx]->currentItem = item_id;
	}
}

const c8* CLoaderCostum::getPartsName(u32 idx)
{
	return m_aParts[idx]->name.c_str();
}

const SCostumParts* CLoaderCostum::getPartsData(u32 idx)
{
	return m_aParts[idx];
}



bool CLoaderCostum::add()
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


bool CLoaderCostum::makeenum()
{
	const char* attrName;

	SEnumList data;

	for(u32 idx = 0; idx < m_pXml->getAttributeCount(); ++idx)
	{

		attrName = m_pXml->getAttributeName(idx);

		if(!strcmp(attrName, "name"))
		{
			data.name = m_pXml->getAttributeValue(idx);
		}
	}

	m_aEnumdata.push_back(data);
	return true;

}


u16 CLoaderCostum::getIDFromEnum(const c8* name, const c8* value)
{
	for(u32 n = 0 ; n < m_aEnumdata.size(); n++)
	{
		if(!strcmp(m_aEnumdata[n].name.c_str(), name))
		{
			for(u32 k = 0 ; k < m_aEnumdata[n].enumdefine.size(); k++)
			{
				if(!strcmp(m_aEnumdata[n].enumdefine[k].c_str(), value))
				{
					return k + 1;
				}
			}
		}
	}

	return 0;

}

bool CLoaderCostum::setenum()
{
	const char* attrName;

	for(u32 idx = 0; idx < m_pXml->getAttributeCount(); ++idx)
	{

		attrName = m_pXml->getAttributeName(idx);

		if(!strcmp(attrName, "name"))
		{
			m_aEnumdata.getLast().enumdefine.push_back(Isakstring(m_pXml->getAttributeValue(idx)));
		}
	}

	return true;
}


bool CLoaderCostum::partsAttribute()
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
		else if(!strcmp(attrName, "enumtype"))
		{
			currentenum = m_pXml->getAttributeValue(idx);

		}
		else if(!strcmp(attrName, "current"))
		{
			part->currentItem = getIDFromEnum(currentenum.c_str(), m_pXml->getAttributeValue(idx));

			if(!part->currentItem)
			{
				delete part;
				return false;
			}
		}
	}

	m_aParts.push_back(part);

	return true;

}


bool CLoaderCostum::parts()
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
		else if(!strcmp(attrName, "id"))
		{
			value.id = getIDFromEnum(currentenum.c_str(), m_pXml->getAttributeValue(idx));
		}

	}

	m_aParts.getLast()->itemlist.insert(value.id, value);
	

	return true;
}

bool CLoaderCostum::Parsing()
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
			else if(!strcmp("enum", element))
			{
				if(!makeenum()) return false;
			}
			else if(!strcmp("option", element))
			{
				if(!setenum()) return false;
			}


			m_aStack.push_back(Isakstring(element));
	
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


CCostum* CLoaderCostum::getCostumObject()
{
	CCostum* pCostum = new CCostum;


	pCostum->m_aParts = m_aParts;
	pCostum->m_aEnumdata = m_aEnumdata;

	pCostum->m_WorkingFolder = Settingdata.workingfolder;
	pCostum->m_BoneFileName = Settingdata.boneFilename;

	return pCostum;
}

}
