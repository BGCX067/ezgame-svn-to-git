#pragma once

#include "../EzCore/EzCore.h"
#include "../EzMain3D/EzNode.h"

namespace GameEasy
{

struct setting
{
	EzStringc  workingfolder;
	EzStringc  boneFilename;
};

struct SCostumItem
{
	EzStringc			 name;
	EzStringc		     filename;
};


struct SCostumParts
{
	EzStringc				name;
	u16						currentItem;
	EzArray<SCostumItem>	ItemArray;
};

}


namespace GameEasy
{

class EzLoaderCostum;


class EzCostum : public EzRefObject
{
public:
	EzCostum(void);
	~EzCostum(void);

	void setCostumChange(u32 idx, u16 item_id);
	const u32 getCostumCount();
	const u32 getCostumPartCount(u32 idx);

	EzNode*  getGeometryNode();
	EzNode*  getSkeleton();

	void makeShape(bool bThreadLoad);

protected:
	bool loadSkeleton(EzStringc filename);
	bool attachloadMesh(EzStringc name, EzStringc filename);

private:
	const bool setCostumPart(u32 idx, EzNode* pNode);

	EzArray<SCostumParts*>				m_aParts;
	EzNodePtr							m_SkeletonPtr;
	EzNodePtr							m_GeometryDataPtr;


	

	EzStringc							m_WorkingFolder;
	EzStringc							m_BoneFileName;

	friend								EzLoaderCostum;
};


}
