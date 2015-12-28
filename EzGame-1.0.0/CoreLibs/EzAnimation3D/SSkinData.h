#ifndef _EZSKINDATA_H_
#define _EZSKINDATA_H_

#include "../core/EzCore.h"

namespace GameEasy
{
	struct SSkinWeight
	{
		/*! Default Constructor */
		SSkinWeight() : usIdx(0), fWeight(0.0f) {}
		/*! Default Destructor */
		~SSkinWeight() {}
		/*! Copy Constructor */
		SSkinWeight( const SSkinWeight & src )
		{
			*this = src;
		}

		/*! Copy Operator */
		inline SSkinWeight & operator=( const SSkinWeight & src )
		{
			this->usIdx = src.usIdx;
			this->fWeight = src.fWeight;
			return *this;
		}
		
		/*! The vertex index, in the mesh. */
		u16 usIdx;
		/*! The vertex weight - between 0.0 and 1.0 */
		f32 fWeight;
	};



	/*! Skinning data component. */
	struct SSkinData 
	{
		/*! Default Constructor */
		SSkinData() : scale(0.0f), boundingSphereRadius(0.0f) {}
		
		/*! Default Destructor */
		~SSkinData() {}
	
		/*! Copy Constructor */
		SSkinData( const SSkinData & src )
		{
			*this = src;
		}
	
		/*! Copy Operator */
		SSkinData & operator=( const SSkinData & src )
		{
			this->rotation = src.rotation;
			this->translation = src.translation;
			this->scale = src.scale;
			this->boundingSphereOffset = src.boundingSphereOffset;
			this->boundingSphereRadius = src.boundingSphereRadius;
			this->vertexWeights = src.vertexWeights;
			return *this;
		}
		/*! Rotation offset of the skin from this bone in bind position. */
		matrix3	  rotation;
		/*! Translation offset of the skin from this bone in bind position. */
		EzVector3df translation;
		
		/*!
		* Scale offset of the skin from this bone in bind position. (Assumption - this is
		* always 1.0 so far)
		*/
		f32 scale;
		
		/*! The vertex weights. */
		EzArray<SSkinWeight> vertexWeights;


		/*!
		* Translation offset of a bounding sphere holding all vertices. (Note that its a
		* Sphere Containing Axis Aligned Box not a minimum volume Sphere)
		*/
		EzVector3df boundingSphereOffset;
		/*! Radius for bounding sphere holding all vertices. */
		f32 boundingSphereRadius;

	};

}


#endif