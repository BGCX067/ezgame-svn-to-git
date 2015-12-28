#include "../EzCore/EzFile.h"
#include "EzStream.h"
#include "EzTextureSource.h"
#include "EzVersion.h"
#include "EzNode.h"
#include "EzObjectFactory.h"



EzObject* EzStreamLoadObject(EzStream& Stream)
{
	unsigned int uiLinkID;
	EzObject* pObject = NULL;
	EzStringc ClassName;

	EzStreamLoad<unsigned int>(Stream, uiLinkID);

	if(uiLinkID != 0)
	{
		EzStreamStringLoad(Stream, ClassName);

		return Stream.getObjectFromLinkID(ClassName.c_str(), uiLinkID);
	}

	return NULL;
}



EzStream::EzStream(void)
:m_file(NULL), m_bLittleEndian(true), m_lastlinkID(0)
{
}

EzStream::~EzStream(void)
{
}


void EzStream::writeTreeNode(EzAVObject* pParent)
{
	EzNode* pNode = EzDynamicCast(EzNode, pParent);
	EZASSERT(pNode);

	EzNode::EzNodeChildList  children = pNode->getChildren();
	EzNode::EzNodeChildList::Iterator  it = children.begin();


	EzStreamWrite<int>(*this, children.getSize());
	EzStreamWriteObject(*this, EzDynamicCast(EzObject, pNode));

	//save Children
	for (; it != children.end(); ++it)
	{
		if(EzIsKindOf(EzNode, *it))
		{
			EzStreamWrite<unsigned char>(*this, 1);
			writeTreeNode(*it);
		}
		else
		{
			EzStreamWrite<unsigned char>(*this, 0);
			EzStreamWriteObject(*this, EzDynamicCast(EzObject, *it));
		}

	}

}

void EzStream::loadBinary(EzAVObject* pParent)
{
	EzNode* pNode = EzDynamicCast(EzNode, pParent);
	EZASSERT(pNode);

	EzNode::EzNodeChildList  children = pNode->getChildren();
	EzNode::EzNodeChildList::Iterator  it = children.begin();

	pNode->loadBinary(*this);

	for (; it != children.end(); ++it)
	{

		if(EzIsKindOf(EzNode, *it))
		{
			loadBinary(*it);
		}
		else
		{
			(*it)->loadBinary(*this);
		}
	}
}

void EzStream::saveBinary(EzAVObject* pParent)
{
	EzNode* pNode = EzDynamicCast(EzNode, pParent);
	EZASSERT(pNode);

	EzNode::EzNodeChildList  children = pNode->getChildren();
	EzNode::EzNodeChildList::Iterator  it = children.begin();



	pNode->saveBinary(*this);

	for (; it != children.end(); ++it)
	{

		if(EzIsKindOf(EzNode, *it))
		{
			saveBinary(*it);
		}
		else
		{
			(*it)->saveBinary(*this);
		}
	}
}

bool EzStream::write(const char* FileName, EzAVObject* pRoot, const EzfInfo &info)
{
	EzFile* file = EzFile::open(FileName, EzFile::WRITE_ONLY);

	if(!file)
		return false;

	m_file = file;

	saveHeader(info);

	writeTreeNode(pRoot);

//	saveBinary(pRoot);


	file->close();



	return true;



}

void EzStream::saveHeader(const EzfInfo &info)
{
	m_file->puts("GameEasy File Format, Version "GAMEEASY_EZF_VERSION_STRING"\n");
	
	EzStreamWrite<bool>(*this, m_bLittleEndian);
	EzStreamWrite<int>(*this, m_fileInfo.uiVersion);
	EzStreamWrite<int>(*this, info.uiUserVersion);
	EzStreamStringWrite(*this, info.exportInfo.c_str());
	EzStreamStringWrite(*this, info.createName.c_str());
}


EzAVObject* EzStream::loadTreeNode()
{
	EzObject* pObject;
	EzNode* pParent;
	int iChildNum;
	unsigned char ucIsNode;

	EzStreamLoad<int>(*this, iChildNum);
	pParent = EzStaticCast(EzNode, EzStreamLoadObject(*this));

	//save Children
	for (int i = 0; i < iChildNum; ++i)
	{

		EzStreamLoad<unsigned char>(*this, ucIsNode);

		if(ucIsNode)
		{
			pParent->addChild(loadTreeNode());
		}
		else
		{
			pObject = EzStreamLoadObject(*this);
			pParent->addChild(EzStaticCast(EzAVObject, pObject));
		}
	}

	return EzStaticCast(EzAVObject, pParent);


}

bool EzStream::loadHeader()
{
	EzStringc HeaderText;
	m_file->gets(HeaderText);

	if(strncmp(HeaderText.c_str(), "GameEasy File Format", 18) == 0)
	{
		EzStreamLoad<bool>(*this, m_bLittleEndian);
		EzStreamLoad<unsigned int>(*this, m_fileInfo.uiVersion);
		EzStreamLoad<unsigned int>(*this, m_fileInfo.uiUserVersion);
		EzStreamStringLoad(*this, m_fileInfo.exportInfo);
		EzStreamStringLoad(*this, m_fileInfo.createName);

		return true;

	}

	return false;
	
}



bool EzStream::load(const char* FileName)
{

	EzFile* file = EzFile::open(FileName, EzFile::READ_ONLY);

	EzArray<EzAVObject*> aLoadRoot;

	if(!file)
	{
		return false;
	}

	m_file = file;


	if(!loadHeader())
		return false;

	EzAVObject *pRoot;

	pRoot = loadTreeNode();

	m_aRoots.push_back(pRoot);

	file->close();

	return true;
}

EzObject* EzStream::getObjectFromLinkID(const char* ClassName , unsigned int uiLinkID)
{
	EzLinkIDMap::Node *pFindNode = m_LinkIDMap.find(uiLinkID);

	if(pFindNode)
	{
		return pFindNode->getValue();
	}


	EzObject* pObject;

	pObject = EzCreateObject(ClassName);
	pObject->loadBinary(*this);

	m_LinkIDMap.insert(uiLinkID, pObject);

	return pObject;
}

unsigned int EzStream::getLinkIDFromObject(EzObject* pObject)
{
	EZASSERT(pObject);
	EzLinkObjectMap::Node *pFindNode = m_Link_map.find(pObject);

	if(pFindNode)
	{
		unsigned int uiLinkID;
		uiLinkID = pFindNode->getValue();
		EzStreamWrite<unsigned int>(*this, uiLinkID);
		return uiLinkID;

	}

	m_lastlinkID++;

	EzStreamWrite<unsigned int>(*this, m_lastlinkID);
	EzStreamStringWrite(*this, pObject->getRTTI()->getName());
	pObject->saveBinary(*this);

	m_Link_map.insert(pObject, m_lastlinkID);

	return m_lastlinkID;
}

