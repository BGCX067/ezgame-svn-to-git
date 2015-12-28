#pragma once

#include "../core/EzCore.h"
#include "CNode.h"

namespace GameEasy
{

struct setting
{
	Isakstring  workingfolder;
	Isakstring  boneFilename;
};

struct SCostumItem
{
	u16			   id;
	Isakstring		   name;
	Isakstring		   filename;
};


struct SEnumList
{
	Isakstring			    name;
	EzArray<Isakstring>		enumdefine;
};


struct SCostumParts
{
	Isakstring					name;
	u16						currentItem;
	map<u16, SCostumItem>	itemlist;
};

}


namespace GameEasy
{

class CLoaderCostum;


class CCostum : public CObject
{
public:
	CCostum(void);
	~CCostum(void);

	void setCostumChange(u32 idx, u16 item_id);
	const u32 getCostumCount();
	const u32 getCostumPartCount(u32 idx);

	CNode*  getGeometryNode();
	CNode*  getSkeleton();

	void makeShape(bool bThreadLoad);

protected:
	bool loadSkeleton(Isakstring filename);
	bool attachloadMesh(Isakstring name, Isakstring filename);

private:
	const bool setCostumPart(u32 idx, CNode* pNode);

	EzArray<SCostumParts*>				m_aParts;
	EzArray<SEnumList>					m_aEnumdata;
	EzNodePtr							m_SkeletonPtr;
	EzNodePtr							m_GeometryDataPtr;


	

	Isakstring								m_WorkingFolder;
	Isakstring								m_BoneFileName;

	friend								CLoaderCostum;
};


}
