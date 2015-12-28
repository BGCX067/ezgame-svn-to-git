#include "EzCostum.h"
#include "../EzMain3D/EzStream.h"
#include "../EzMain3D/EzAVObject.h"
#include "../EzMain3D/EzGeometry.h"
#include "../EzCore/EzDebugConsole.h"

namespace GameEasy
{


EzCostum::EzCostum(void)
: m_SkeletonPtr(0), m_GeometryDataPtr(0)
{
	m_GeometryDataPtr = new EzNode(0);
}

EzCostum::~EzCostum(void)
{
}

bool EzCostum::loadSkeleton(EzStringc filename)
{
	EzStream stream;
	EzAVObject* pAVObject = 0;

	if(stream.load(filename.c_str()))
	{
		if(stream.getCount() == 1)
		{
			m_SkeletonPtr = EzDynamicCast(EzNode, stream.getAt(0));
			m_SkeletonPtr->update(0.0f);
			return true;
		}
	}



	return false;
}


bool EzCostum::attachloadMesh(EzStringc name, EzStringc filename)
{
	EzStream stream;
	EzNode* pRootNode = 0;
	EzArray<EzAVObject*> aBone;

	if(stream.load(filename.c_str()))
	{
		for(u32 n = 0; n < stream.getCount(); ++n)
		{
			pRootNode = EzDynamicCast(EzNode, stream.getAt(n));

			if(pRootNode)
			{
				EzList<EzAVObjectPtr>::ConstIterator it = pRootNode->getChildren().begin();
				for (; it != pRootNode->getChildren().end(); ++it)
				{
					const EzAVObject* pChild = (EzAVObject*) *it;

					if(EzIsKindOf(EzGeometry, (*it)))
					{
						EzGeometry* pGeome = EzDynamicCast(EzGeometry, (*it));

						EzSkinInstance* pSkinInstance = pGeome->getSkinInstance();

						EzAVObject* pSkeletonRoot = pSkinInstance->getSkeletonRoot();

						if(pSkeletonRoot)
						{
						
							EzAVObject* pTempRoot = m_SkeletonPtr->getObjectByName(pSkeletonRoot->getName());

							if(pTempRoot)
							{
								pSkinInstance->setSkeletonRoot(EzDynamicCast(EzNode, pTempRoot));
							}
						}

						EzAVObject*const* ppBones = pSkinInstance->getBones();

						for(u16 k = 0; k < pSkinInstance->getBoneCount(); k++)
						{	
							EzAVObject* pTemp = m_SkeletonPtr->getObjectByName(ppBones[k]->getName());

							if(pTemp)
							{
								aBone.push_back(ppBones[k]);
								pSkinInstance->setBones(k, EzDynamicCast(EzNode, pTemp));
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

		for(u32 n = 0; n < aBone.size(); ++n)
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

void EzCostum::makeShape(bool bThreadLoad)
{
	u16			uCurrentItem;
	EzStringc		fullname;

	fullname = m_WorkingFolder + "\\" + m_BoneFileName;

	if(!loadSkeleton(fullname))
	{
	//	error(0, "Skeleton data Load Fail");
		return;

	}

	for(size_t n = 0; n < m_aParts.size(); ++n)
	{
		uCurrentItem = m_aParts[n]->currentItem;

		fullname = m_WorkingFolder + "\\" + m_aParts[n]->ItemArray[uCurrentItem].filename;

		if(bThreadLoad)
		{
		}
		else
		{
			attachloadMesh(m_aParts[n]->name, fullname);
		}
	}

	m_SkeletonPtr->addChild(EzDynamicCast(EzAVObject, m_GeometryDataPtr));

}

EzNode*  EzCostum::getGeometryNode()
{
	return m_SkeletonPtr;
}

EzNode*  EzCostum::getSkeleton()
{
	return m_SkeletonPtr;
}

const bool EzCostum::setCostumPart(u32 idx, EzNode* pNode)
{
	//if(m_aParts_node[idx])
	//{
	//	m_aParts_node[idx]->release();
	//}

	//m_aParts_node[idx] = pNode;
	return true;
}

const u32 EzCostum::getCostumCount()
{
	return m_aParts.size();
}


const u32 EzCostum::getCostumPartCount(u32 idx)
{
	return m_aParts[idx]->ItemArray.size();

}

void EzCostum::setCostumChange(u32 idx, u16 item_id)
{
	//EzMap<u16, SCostumItem>::Iterator  it;

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

