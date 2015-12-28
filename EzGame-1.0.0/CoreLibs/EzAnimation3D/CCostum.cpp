#include "CCostum.h"
#include "CObject.h"
#include "CStream.h"
#include "CAVObject.h"
#include "CGeometry.h"
#include "../core/CDebugConsole.h"

namespace GameEasy
{


CCostum::CCostum(void)
: m_SkeletonPtr(0), m_GeometryDataPtr(0)
{
	m_GeometryDataPtr = new CNode(0);
}

CCostum::~CCostum(void)
{
}

bool CCostum::loadSkeleton(Isakstring filename)
{
	CStream stream;
	CAVObject* pAVObject = 0;

	if(stream.load(filename.c_str()))
	{
		if(stream.getCount() == 1)
		{
			m_SkeletonPtr = IsakDynamicCast(CNode, stream.getAt(0));
			return true;
		}
	}

	return false;
}


bool CCostum::attachloadMesh(Isakstring name, Isakstring filename)
{
	CStream stream;
	CNode* pRootNode = 0;
	EzArray<CAVObject*> aBone;

	if(stream.load(filename.c_str()))
	{
		for(int n = 0; n < stream.getCount(); ++n)
		{
			pRootNode = IsakDynamicCast(CNode, stream.getAt(n));

			if(pRootNode)
			{
				list<CAVObjectPtr>::ConstIterator it = pRootNode->getChildren().begin();
				for (; it != pRootNode->getChildren().end(); ++it)
				{
					const CAVObject* pChild = (CAVObject*) *it;

					if(IsakIsKindOf(CGeometry, (*it)))
					{
						CGeometry* pGeome = IsakDynamicCast(CGeometry, (*it));

						CSkinInstance* pSkinInstance = pGeome->getSkinInstance();

						EzArray<CNode*> Bones = pSkinInstance->getBones();

						for(u16 k = 0; k < Bones.size(); k++)
						{	
							CAVObject* pTemp = m_SkeletonPtr->findByObjectName(Bones[k]->getName());

							if(pTemp)
							{
								aBone.push_back(Bones[k]);
								Bones[k] = IsakDynamicCast(CNode, pTemp);
							}
							else
							{
								return false;
							}
						}
					}
				}
			}
		}

		for(int n = 0; n < aBone.size(); ++n)
		{
			pRootNode->removeChild(aBone[n]);
		}

		pRootNode->setName(name.c_str());

		m_GeometryDataPtr->addChild(pRootNode);


		aBone.clear();

		return true;
	}

	return false;
}

void CCostum::makeShape(bool bThreadLoad)
{
	u16			uCurrentItem;
	Isakstring		fullname;

	fullname = m_WorkingFolder + "\\" + m_BoneFileName;

	if(!loadSkeleton(fullname))
	{
	//	error(0, "Skeleton data Load Fail");
		return;

	}

	map<u16, SCostumItem>::Iterator  it;

	for(size_t n = 0; n < m_aParts.size(); ++n)
	{
		uCurrentItem = m_aParts[n]->currentItem;

		it = m_aParts[n]->itemlist.find(uCurrentItem);

		if(!it.atEnd())
		{
			fullname = m_WorkingFolder + "\\" + it->getValue().filename;

			if(bThreadLoad)
			{
			}
			else
			{
				attachloadMesh(m_aParts[n]->name, fullname);
			}
		}

	}

	m_SkeletonPtr->addChild(IsakDynamicCast(CAVObject, m_GeometryDataPtr));

}

CNode*  CCostum::getGeometryNode()
{
	return m_GeometryDataPtr;
}

CNode*  CCostum::getSkeleton()
{
	return m_SkeletonPtr;
}

const bool CCostum::setCostumPart(u32 idx, CNode* pNode)
{
	//if(m_aParts_node[idx])
	//{
	//	m_aParts_node[idx]->release();
	//}

	//m_aParts_node[idx] = pNode;
	return true;
}

const u32 CCostum::getCostumCount()
{
	return m_aParts.size();
}


const u32 CCostum::getCostumPartCount(u32 idx)
{
	return m_aParts[idx]->itemlist.size();

}

void CCostum::setCostumChange(u32 idx, u16 item_id)
{
	//map<u16, SCostumItem>::Iterator  it;

	//it = m_aParts[idx]->itemlist.find(item_id);

	//if(!it.atEnd())
	//{
	//	m_aParts[idx]->currentItem = item_id;
	//	if(m_aParts_node[idx] != NULL)
	//	{
	//		m_aParts_node[idx]->release();
	//	}
	//}
}

}

