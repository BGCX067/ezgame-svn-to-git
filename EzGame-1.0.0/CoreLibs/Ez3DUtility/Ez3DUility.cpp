#include "Ez3DUility.h"
#include "../EzMain3D/EzTriBasedGeomData.h"
#include "../EzMain3D/EzSkinData.h"


class BoneSet : public EzArray<u16>
{
public:
	bool contains(const BoneSet& That) const;
	u32 mergedSize(const BoneSet& That) const;
	void merge(const BoneSet& That);
	bool containsTriangle(const u16* pusTriangle,
		const VertexInteractions* pVertex) const;
	bool containsVertex(const VertexInteractions& Vertex) const;

};

class BoneSetMerger
{
public:
	BoneSetMerger(const BoneSet& Set0, const BoneSet& Set1);
	bool done() const;
	u16 getNext();
protected:
	u32 m_uiIndex[2];
	u16 m_usEl[2];
	const BoneSet& m_Set0;
	const BoneSet& m_Set1;
};

class TriangleSet : public EzArray<u16>
{
public:
	void addTriangles(u32 uiPartition, const u32* puiTriangleToPartition, u32 uiTriangles);
};

void TriangleSet::addTriangles(u32 uiPartition, const u32* puiTriangleToPartition, u32 uiTriangles)
{
	for (u32 t = 0; t < uiTriangles; t++)
	{
		if (puiTriangleToPartition[t] == uiPartition)
		{
			push_back(t);
		}
	}
}

bool BoneSet::contains(const BoneSet& That) const
{
	u32 uiThis = 0, uiThat = 0;

	while (uiThat < That.size())
	{
		while (uiThis < size() && data[uiThis] < That[uiThat])
		{
			uiThis++;
		}

		if (uiThis == size() || data[uiThis] != That[uiThat])
		{
			return false;
		}

		uiThis++;
		uiThat++;
	}

	return true;
}

u32 BoneSet::mergedSize(const BoneSet& That) const
{
	BoneSetMerger Merger(*this, That);
	u32 uiMergeSize = 0;

	while (!Merger.done())
	{
		Merger.getNext();
		uiMergeSize++;
	}

	return uiMergeSize;
}

//---------------------------------------------------------------------------
void BoneSet::merge(const BoneSet& That)
{
	// Add all elements of kThat to "this".

	BoneSetMerger Merger(*this, That);
	u32 uiMergeSize = 0;
	u32 uiNewAlloced = size() + That.size();
	u16* pusNewBase = EzAlloc(u16, uiNewAlloced);
	EZASSERT(pusNewBase != NULL);

	while (!Merger.done())
	{
		pusNewBase[uiMergeSize++] = Merger.getNext();
	}

	//EzFree(m_pBase);
}

//---------------------------------------------------------------------------
bool BoneSet::containsTriangle(const u16* pusTriangle, const VertexInteractions* pkVertex)
	const
{
	unsigned int v;

	for (v = 0; v < 3; v++)
	{
		if (!containsVertex(pkVertex[pusTriangle[v]]))
			return false;
	}

	return true;
}

//---------------------------------------------------------------------------
bool BoneSet::containsVertex(const VertexInteractions& Vertex) const
{
	// "this" is assumed to be sorted.

	// In order to "contain" a vertex, the boneset must include every bone
	// that influences the vertex.

	u32 b, uiVertexBones = Vertex.size();

	for (b = 0; b < uiVertexBones; b++)
	{
		unsigned short usBone = Vertex.getBone(b);

		//if (bsearch(&usBone, m_pBase, getSize(), sizeof(m_pBase[0]),
		//	CompareBones) == NULL)
		//{
		//	return false;
		//}
	}

	return true;
}

typedef EzArray<BoneSet*> EzBoneSetCollection;


BoneSetMerger::BoneSetMerger(const BoneSet& Set0, const BoneSet& Set1)
 : m_Set0(Set0), m_Set1(Set1)
{
	EZASSERT(Set0.size() > 0);
	EZASSERT(Set1.size() > 0);

	m_uiIndex[0] = m_uiIndex[1] = 0;
	m_usEl[0] = Set0[0];
	m_usEl[1] = Set1[0];

	// Both sets are assumed to have been sorted.
}

bool BoneSetMerger::done() const
{
	return m_usEl[0] == USHRT_MAX && m_usEl[1] == USHRT_MAX;
}

u16 BoneSetMerger::getNext()
{
	u16 usNext;

	if (m_usEl[1] < m_usEl[0])
	{
		usNext = m_usEl[1];

		m_usEl[1] = ++m_uiIndex[1] < m_Set1.size() ? m_Set1[m_uiIndex[1]] : USHRT_MAX;

		EZASSERT(usNext < m_usEl[1]);
	}
	else 
	{
		usNext = m_usEl[0];

		if (m_usEl[1] == m_usEl[0])
		{
			m_usEl[1] = ++m_uiIndex[1] < m_Set1.size() ? m_Set1[m_uiIndex[1]] : USHRT_MAX;

			EZASSERT(usNext < m_usEl[1]);
		}

		m_usEl[0] = (++m_uiIndex[0] < m_Set0.size()) ? m_Set0[m_uiIndex[0]] : USHRT_MAX;

		EZASSERT(usNext < m_usEl[0]);
	}

	return usNext;
}


VertexInteractions* createVertexInteractions(u32 uiVertices, const EzSkinData* pSkinData)
{
	// NiSkinData stores a list of vertices influenced by each bone. Shuffle 
	// the data so that we have a list of bones that influences each vertex.

	VertexInteractions* pVertexInteraction =  EzNew VertexInteractions[uiVertices];

	u32 i, uiBones = pSkinData->getBoneCount();

	Interaction Interaction;

	for (i = 0; i < uiBones; i++)
	{
		EzSkinData::BoneData* pBoneData = pSkinData->getBoneData();

		Interaction.m_uiBone = i;

		for (u32 j = 0; j < pBoneData[i].m_usVertexWeightCount; j++)
		{
			u32 uiVert = pBoneData[i].m_pVertexWeights[j].m_usIdx;
			Interaction.m_fWeight =	pBoneData[i].m_pVertexWeights[j].m_fWeight;

			pVertexInteraction[uiVert].push_back(Interaction);
		}
	}

	for (i = 0; i < uiVertices; i++)
	{
		pVertexInteraction[i].normalize();
	}

	return pVertexInteraction;
}


bool degenerate(const EzTriangle16& Tri)
{
	return Tri[0] == Tri[1] || Tri[1] == Tri[2] || Tri[2] == Tri[0];
}

void makeBoneMap(EzMap<u32, f32>& Map, EzTriangle16 Tri, const VertexInteractions* pVertexInteraction)
{
	// Creates a hash table that maps bone indices to the maximum weight that
	// bone exerts on any vertex in the triangle.

	EzMap<u32, f32>::Node* pNode;

	for (u32 v = 0; v < 3; v++)
	{
		u32 uiVert = Tri[v];
		const VertexInteractions* pI = &pVertexInteraction[uiVert];
		u32 uiVertexBones = pI->size();

		for (u32 b = 0; b < uiVertexBones; b++)
		{
			u32 uiBone = pI->getBone(b);

			pNode = Map.find(uiBone);
			if (!pNode || pNode->getValue() < pI->getWeight(b))
			{
				Map.insert(uiBone, pI->getWeight(b));
			}
		}
	}
}


bool removeExcessInteractions(EzTriBasedGeomData* pTriData, u8 ucBonesPerPartition, u8 ucBonesPerVertex, VertexInteractions* pVertexInteraction)
{
	EzMap<u32, f32> Map;
    u32 t, v;

	u32 uiNumVertices = pTriData->getNumVertex();
	u32 uiTriangles = pTriData->getNumTrianglePoints();


    EzArray<u32> CoincidentVerts;

    u32* puiIndex0;
    u32* puiIndex1;

    pTriData->indexSort(puiIndex0, puiIndex1, uiNumVertices);


    for (t = 0; t < uiTriangles; t++)
    {
        EzTriangle16 Tri;

        Tri = pTriData->getTriangle(t);

        if (degenerate(Tri))
            continue;

        makeBoneMap(Map, Tri, pVertexInteraction);

        u32 uiBones = Map.size();
        if (uiBones > ucBonesPerPartition)
        {
            for (;;)
            {
				EzMap<u32, f32>::Iterator  it = Map.getIterator();
                f32 fMinWeight = FLT_MAX;
                u32 uiMinBone = 0;

				for(; !it.atEnd(); it++)
                {
                    u32 uiBone = it->getKey();
                    f32 fWeight = it->getValue();
        
                    if (fWeight < fMinWeight)
                    {
                        fMinWeight = fWeight;
                        uiMinBone = uiBone;
                    }
                } 
                
                for (v = 0; v < 3; ++v)
                {
                    u32 uiVert = Tri[v];

                    u32 uiStart, uiEnd, uiCoincidentVert;

                    CoincidentVerts.push_back(uiVert);
                    VertexInteractions* pI = &pVertexInteraction[uiVert];

                    uiStart = uiEnd = puiIndex1[uiVert];
                    while (uiStart > 0 && pTriData->vertexCompare(uiCoincidentVert = puiIndex0[uiStart - 1], uiVert,
		                    EzGeometryData::VERTEX_MASK) == 0)
                    {
                        if (*pI == pVertexInteraction[uiCoincidentVert])
                        {
                            CoincidentVerts.push_back(uiCoincidentVert);
                        }
    
						uiStart--;
                    }

                    while (uiEnd < uiNumVertices - 1 && pTriData->vertexCompare(uiCoincidentVert = puiIndex0[uiEnd + 1], uiVert,
						     EzGeometryData::VERTEX_MASK) == 0)
                    {
                        if ( *pI == pVertexInteraction[uiCoincidentVert])
                        {
                            CoincidentVerts.push_back(uiCoincidentVert);
                        }
                        uiEnd++;
                    }

                    for (u32 j = 0; j < CoincidentVerts.size(); ++j)
                    {
                        uiCoincidentVert = CoincidentVerts[j];
                        pI = &pVertexInteraction[uiCoincidentVert];

                        if (pI->removeBone(uiMinBone))
                        {
                            if (pI->size() == 0)
                            {
                                EZASSERT(ucBonesPerPartition < 3);
                                EzFree(puiIndex0);
                                EzFree(puiIndex1);
                                return false;
                            }
                            
                            pI->normalize();
                        }
                    }

                    CoincidentVerts.clear();
				 }

                if (--uiBones == ucBonesPerPartition)
                    break;

                Map.clear();
	            makeBoneMap(Map, Tri, pVertexInteraction);
            }
        }
		
		Map.clear();
    }

    for (v = 0; v < uiNumVertices; v++)
    {
        VertexInteractions* pI = &pVertexInteraction[v];

        if (pI->size() > ucBonesPerVertex)
        {
            //pI->SortByWeight();
           // pI->Truncate(ucBonesPerVertex);
           // pI->Unitize();
        }
    }

    EzFree(puiIndex0);
    EzFree(puiIndex1);

    return true;
}


bool makePartitions(EzTriBasedGeomData* pTriData,	const EzSkinData* pSkinData, u8 ucBonesPerPartition,
		u8 ucBonesPerVertex, bool bForcePalette)
	{
		EZASSERT(ucBonesPerPartition >= ucBonesPerVertex);

		VertexInteractions* pVertexInteraction;
		EzBoneSetCollection BoneSets;
		unsigned int* puiTriangleToPartition;
		u32 uiVertices = pTriData->getNumVertex();
	

		pVertexInteraction = createVertexInteractions(uiVertices, pSkinData);


		bool bPartition = removeExcessInteractions(pTriData, 
			ucBonesPerPartition, ucBonesPerVertex, pVertexInteraction);

		//if (bPartition)
		//{
		//	MakeBoneSets(pkTriData, pkSkinData, ucBonesPerPartition,
		//		pkVertexInteraction, kBoneSets, puiTriangleToPartition);

		//	Partition* pkPartitions = NiNew Partition[m_uiPartitions];
		//	NIASSERT(pkPartitions != NULL);

		//	bool bNeedPalette = (ucBonesPerPartition != ucBonesPerVertex);

		//	TriangleSet kTriangles;
		//	for (unsigned int p = 0; p < m_uiPartitions; p++)
		//	{
		//		kTriangles.AddTriangles(p, puiTriangleToPartition,
		//			pkTriData->GetTriangleCount());

		//		BoneSet* pkBoneSet = kBoneSets.GetAt(p);

		//		pkPartitions[p].Create(kTriangles, *pkBoneSet, pkTriData,
		//			pkVertexInteraction, bNeedPalette, bForcePalette,
		//			ucBonesPerVertex, pkSkinData->GetBoneCount());

		//		kTriangles.RemoveAll();
		//		NiDelete pkBoneSet;
		//	}

		//	NiFree(puiTriangleToPartition);
		//	m_pkPartitions = pkPartitions;
		//}

		EzDelete[] pVertexInteraction;

		return bPartition;
	}


