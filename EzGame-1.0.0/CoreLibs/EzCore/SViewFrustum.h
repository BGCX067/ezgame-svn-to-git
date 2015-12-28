// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __VIEW_FRUSTUM_H_INCLUDED__
#define __VIEW_FRUSTUM_H_INCLUDED__

#include "EzEnumTypes.h"
#include "EzPlane3d.h"
#include "EzPoint3.h"
#include "EzAabbox3d.h"
#include "EzMatrix4.h"

//! Defines the view frustum. That's the space visible by the camera.
/** The view frustum is enclosed by 6 planes. These six planes share
four points. A bounding box around these four points is also stored in
this structure.
*/
struct SViewFrustum
{
	enum VFPLANES
	{
		//! Far plane of the frustum. That is the plane farest away from the eye.
		VF_FAR_PLANE = 0,
		//! Near plane of the frustum. That is the plane nearest to the eye.
		VF_NEAR_PLANE,
		//! Left plane of the frustum.
		VF_LEFT_PLANE,
		//! Right plane of the frustum.
		VF_RIGHT_PLANE,
		//! Bottom plane of the frustum.
		VF_BOTTOM_PLANE,
		//! Top plane of the frustum.
		VF_TOP_PLANE,

		//! Amount of planes enclosing the view frustum. Should be 6.
		VF_PLANE_COUNT
	};

	//! Hold a copy of important transform matrices
	enum E_TRANSFORMATION_STATE_3
	{
		ETS_VIEW_PROJECTION_3 = ETS_PROJECTION + 1,
		ETS_VIEW_MODEL_INVERSE_3,
		ETS_CURRENT_3,
		ETS_COUNT_3
	};

	//! Default Constructor
	SViewFrustum() {}

	//! Copy Constructor
	SViewFrustum(const SViewFrustum& other);

	//! This constructor creates a view frustum based on a projection and/or view matrix.
	SViewFrustum(const EzMatrix4& mat);

	//! This constructor creates a view frustum based on a projection and/or view matrix.
	inline void setFrom(const EzMatrix4& mat);

	//! transforms the frustum by the matrix
	/** \param mat: Matrix by which the view frustum is transformed.*/
	void transform(const EzMatrix4& mat);

	//! returns the point which is on the far left upper corner inside the the view frustum.
	EzPoint3 getFarLeftUp() const;

	//! returns the point which is on the far left bottom corner inside the the view frustum.
	EzPoint3 getFarLeftDown() const;

	//! returns the point which is on the far right top corner inside the the view frustum.
	EzPoint3 getFarRightUp() const;

	//! returns the point which is on the far right bottom corner inside the the view frustum.
	EzPoint3 getFarRightDown() const;

	//! returns a bounding box enclosing the whole view frustum
	const EzAabbox3d &getBoundingBox() const;

	//! recalculates the bounding box member based on the planes
	inline void recalculateBoundingBox();

	//! update the given state's matrix
	void setTransformState( ETRANSFORMATION_STATE state);

	//! the position of the camera
	EzPoint3 cameraPosition;

	//! all planes enclosing the view frustum.
	EzPlane3d planes[VF_PLANE_COUNT];

	//! bounding box around the view frustum
	EzAabbox3d boundingBox;

	//! Hold a copy of important transform matrices
	EzMatrix4 Matrices[ETS_COUNT_3];
};


inline SViewFrustum::SViewFrustum(const SViewFrustum& other)
{
	cameraPosition=other.cameraPosition;
	boundingBox=other.boundingBox;

	unsigned int i;
	for (i=0; i<VF_PLANE_COUNT; ++i)
		planes[i]=other.planes[i];

	for (i=0; i<VF_PLANE_COUNT; ++i)
		Matrices[i]=other.Matrices[i];
}

inline SViewFrustum::SViewFrustum(const EzMatrix4& mat)
{
	setFrom ( mat );
}


inline void SViewFrustum::transform(const EzMatrix4& mat)
{
	for (unsigned int i=0; i<VF_PLANE_COUNT; ++i)
		mat.transformPlane(planes[i]);

	mat.transformVect(cameraPosition);
	recalculateBoundingBox();
}


inline EzPoint3 SViewFrustum::getFarLeftUp() const
{
	EzPoint3 p;
	planes[VF_FAR_PLANE].getIntersectionWithPlanes(
		planes[VF_TOP_PLANE],
		planes[VF_LEFT_PLANE], p);

	return p;
}

inline EzPoint3 SViewFrustum::getFarLeftDown() const
{
	EzPoint3 p;
	planes[VF_FAR_PLANE].getIntersectionWithPlanes(
		planes[VF_BOTTOM_PLANE],
		planes[VF_LEFT_PLANE], p);

	return p;
}

inline EzPoint3 SViewFrustum::getFarRightUp() const
{
	EzPoint3 p;
	planes[VF_FAR_PLANE].getIntersectionWithPlanes(
		planes[VF_TOP_PLANE],
		planes[VF_RIGHT_PLANE], p);

	return p;
}

inline EzPoint3 SViewFrustum::getFarRightDown() const
{
	EzPoint3 p;
	planes[VF_FAR_PLANE].getIntersectionWithPlanes(
		planes[VF_BOTTOM_PLANE],
		planes[VF_RIGHT_PLANE], p);

	return p;
}

inline const EzAabbox3d &SViewFrustum::getBoundingBox() const
{
	return boundingBox;
}

inline void SViewFrustum::recalculateBoundingBox()
{
	boundingBox.reset ( cameraPosition );

	boundingBox.addInternalPoint(getFarLeftUp());
	boundingBox.addInternalPoint(getFarRightUp());
	boundingBox.addInternalPoint(getFarLeftDown());
	boundingBox.addInternalPoint(getFarRightDown());
}


//! This constructor creates a view frustum based on a projection
//! and/or view matrix.
inline void SViewFrustum::setFrom(const EzMatrix4& mat)
{
	// left clipping plane
	planes[SViewFrustum::VF_LEFT_PLANE].Normal.X = -(mat(0,3) + mat(0,0));
	planes[SViewFrustum::VF_LEFT_PLANE].Normal.Y = -(mat(1,3) + mat(1,0));
	planes[SViewFrustum::VF_LEFT_PLANE].Normal.Z = -(mat(2,3) + mat(2,0));
	planes[SViewFrustum::VF_LEFT_PLANE].D = -(mat(3,3) + mat(3,0));

	// right clipping plane
	planes[SViewFrustum::VF_RIGHT_PLANE].Normal.X = -(mat(0,3) - mat(0,0));
	planes[SViewFrustum::VF_RIGHT_PLANE].Normal.Y = -(mat(1,3) - mat(1,0));
	planes[SViewFrustum::VF_RIGHT_PLANE].Normal.Z = -(mat(2,3) - mat(2,0));
	planes[SViewFrustum::VF_RIGHT_PLANE].D =        -(mat(3,3) - mat(3,0));

	// top clipping plane
	planes[SViewFrustum::VF_TOP_PLANE].Normal.X = -(mat(0,3) - mat(0,1));
	planes[SViewFrustum::VF_TOP_PLANE].Normal.Y = -(mat(1,3) - mat(1,1));
	planes[SViewFrustum::VF_TOP_PLANE].Normal.Z = -(mat(2,3) - mat(2,1));
	planes[SViewFrustum::VF_TOP_PLANE].D =        -(mat(3,3) - mat(3,1));

	// bottom clipping plane
	planes[SViewFrustum::VF_BOTTOM_PLANE].Normal.X = -(mat(0,3) + mat(0,1));
	planes[SViewFrustum::VF_BOTTOM_PLANE].Normal.Y = -(mat(1,3) + mat(1,1));
	planes[SViewFrustum::VF_BOTTOM_PLANE].Normal.Z = -(mat(2,3) + mat(2,1));
	planes[SViewFrustum::VF_BOTTOM_PLANE].D =        -(mat(3,3) + mat(3,1));

	// near clipping plane
	planes[SViewFrustum::VF_NEAR_PLANE].Normal.X = -mat(0,2);
	planes[SViewFrustum::VF_NEAR_PLANE].Normal.Y = -mat(1,2);
	planes[SViewFrustum::VF_NEAR_PLANE].Normal.Z = -mat(2,2);
	planes[SViewFrustum::VF_NEAR_PLANE].D =        -mat(3,2);

	// far clipping plane
	planes[SViewFrustum::VF_FAR_PLANE].Normal.X = -(mat(0,3) - mat(0,2));
	planes[SViewFrustum::VF_FAR_PLANE].Normal.Y = -(mat(1,3) - mat(1,2));
	planes[SViewFrustum::VF_FAR_PLANE].Normal.Z = -(mat(2,3) - mat(2,2));
	planes[SViewFrustum::VF_FAR_PLANE].D =        -(mat(3,3) - mat(3,2));
	// normalize normals

	for (int i=0; i<6; ++i)
	{
		const float len = reciprocal_squareroot(
				planes[i].Normal.getLengthSQ() );
		planes[i].Normal *= len;
		planes[i].D *= len;
	}

	// make bounding box
	recalculateBoundingBox();
}


inline void SViewFrustum::setTransformState(ETRANSFORMATION_STATE state)
{
	switch ( state )
	{
		case ETS_VIEW:
			Matrices[ETS_VIEW_PROJECTION_3].setbyproduct_nocheck(
					Matrices[ETS_PROJECTION],
					Matrices[ETS_VIEW]);
			Matrices[ETS_VIEW_MODEL_INVERSE_3] = Matrices[ETS_VIEW];
			Matrices[ETS_VIEW_MODEL_INVERSE_3].makeInverse();
			break;

		case ETS_WORLD:
			Matrices[ETS_CURRENT_3].setbyproduct(
					Matrices[ETS_VIEW_PROJECTION_3 ],
					Matrices[ETS_WORLD]);
			break;
		default:
			break;
	}
}




#endif

