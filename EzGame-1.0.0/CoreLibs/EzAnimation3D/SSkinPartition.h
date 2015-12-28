#ifndef _SKINPARTITION_H_
#define _SKINPARTITION_H_

#include "../core/EzCore.h"

namespace GameEasy
{

struct SSkinPartition {
	/*! Default Constructor */
	SSkinPartition() {}; 
	/*! Default Destructor */
	~SSkinPartition() {};
	/*! Copy Constructor */
	SSkinPartition( const SSkinPartition & src )
	{
		*this = src;
	}
	/*! Copy Operator */
	SSkinPartition & operator=( const SSkinPartition & src )
	{
		//this->NumVertices = src.NumVertices;
		//this->NumTriangles = src.NumTriangles;
		//this->NumBones = src.NumBones;
		//this->NumStrips = src.NumStrips;
		this->NumWeightsPerVertex = src.NumWeightsPerVertex;
		this->Bones = src.Bones;
		this->HasVertexMap = src.HasVertexMap;
		this->VertexMap = src.VertexMap;
		this->HasVertexWeights = src.HasVertexWeights;
		this->VertexWeights = src.VertexWeights;
		//this->StripLengths = src.StripLengths;
		this->HasFaces = src.HasFaces;
		this->Strips = src.Strips;
		this->Triangles = src.Triangles;
		this->HasBoneIndices = src.HasBoneIndices;
		this->BoneIndices = src.BoneIndices;
		return *this;
	};

	///*! Number of vertices in this submesh. */
	//mutable unsigned short numVertices;
	///*! Number of triangles in this submesh. */
	//mutable unsigned short numTriangles;
	///*! Number of bones influencing this submesh. */
	//mutable unsigned short numBones;
	///*! Number of strips in this submesh (zero if not stripped). */
	//mutable unsigned short numStrips;
	/*!
	 * Number of weight coefficients per vertex. The Gamebryo engine seems to work well
	 * only if this number is equal to 4, even if there are less than 4 influences per
	 * vertex.
	 */
	mutable unsigned short NumWeightsPerVertex;
	/*! List of bones. */
	EzArray<u16> Bones;
	/*! Do we have a vertex EzMap? */
	bool HasVertexMap;
	/*!
	 * Maps the weight/influence lists in this submesh to the vertices in the shape
	 * being skinned.
	 */
	EzArray<u16> VertexMap;
	/*! Do we have vertex weights? */
	bool HasVertexWeights;
	/*! The vertex weights. */
	EzArray<EzArray<f32>> VertexWeights;


	/*! Do we have triangle or strip data? */
	bool HasFaces;
	/*! The strips. */
	EzArray<EzArray<u16>> Strips;
	/*! The triangles. */
	EzArray<triangle3di > Triangles;
	/*! Do we have bone indices? */
	bool HasBoneIndices;
	/*! Bone indices, they index into 'Bones'. */
	EzArray<EzArray<u8> > BoneIndices;
};

}
#endif