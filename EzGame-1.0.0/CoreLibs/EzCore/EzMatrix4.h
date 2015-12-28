// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Nerilicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __MATRIX_H_INCLUDED__
#define __MATRIX_H_INCLUDED__

#include "EzPoint3.h"
#include "EzPoint2.h"
#include "EzPlane3d.h"
#include "EzAabbox3d.h"
#include "EzRect.h"
#include "EzMemObject.h"


//! 4x4 matrix. Mostly used as transformation matrix for 3d calculations.
/** The matrix is a D3D style matrix, row major with translations in the 4th row. */
class EZCORE_ENTRY EzMatrix4 : public EzMemObject
{
	public:

		//! Constructor Flags
		enum eConstructor
		{
			EM4CONST_NOTHING = 0,
			EM4CONST_COPY,
			EM4CONST_IDENTITY,
			EM4CONST_TRANSPOSED,
			EM4CONST_INVERSE,
			EM4CONST_INVERSE_TRANSPOSED
		};

		//! Default constructor
		/** \param constructor Choose the initialization style */
		EzMatrix4( eConstructor constructor = EM4CONST_IDENTITY );
		//! Copy constructor
		/** \param other Other matrix to copy from
		\param constructor Choose the initialization style */
		EzMatrix4( const EzMatrix4& other,eConstructor constructor = EM4CONST_COPY);

		//! Simple operator for directly accessing every element of the matrix.
		float& operator()(const int row, const int col) { definitelyIdentityMatrix=false; return M[ row * 4 + col ]; }

		//! Simple operator for directly accessing every element of the matrix.
		const float& operator()(const int row, const int col) const { return M[row * 4 + col]; }

		//! Simple operator for linearly accessing every element of the matrix.
		float& operator[](unsigned int index) { definitelyIdentityMatrix=false; return M[index]; }

		//! Simple operator for linearly accessing every element of the matrix.
		const float& operator[](unsigned int index) const { return M[index]; }

		//! Sets this matrix equal to the other matrix.
		inline EzMatrix4& operator=(const EzMatrix4 &other);

		//! Sets all elements of this matrix to the value.
		inline EzMatrix4& operator=(const float& scalar);

		//! Returns pointer to internal array
		const float* pointer() const { return M; }
		float* pointer() { definitelyIdentityMatrix=false; return M; }

		//! Returns true if other matrix is equal to this matrix.
		bool operator==(const EzMatrix4 &other) const;

		//! Returns true if other matrix is not equal to this matrix.
		bool operator!=(const EzMatrix4 &other) const;

		//! Add another matrix.
		EzMatrix4 operator+(const EzMatrix4& other) const;

		//! Add another matrix.
		EzMatrix4& operator+=(const EzMatrix4& other);

		//! Subtract another matrix.
		EzMatrix4 operator-(const EzMatrix4& other) const;

		//! Subtract another matrix.
		EzMatrix4& operator-=(const EzMatrix4& other);

		//! set this matrix to the product of two matrices
		inline EzMatrix4& setbyproduct(const EzMatrix4& other_a,const EzMatrix4& other_b );

		//! Set this matrix to the product of two matrices
		/** no optimization used,
		use it if you know you never have a identity matrix */
		EzMatrix4& setbyproduct_nocheck(const EzMatrix4& other_a,const EzMatrix4& other_b );

		//! Multiply by another matrix.
		EzMatrix4 operator*(const EzMatrix4& other) const;

		//! Multiply by another matrix.
		EzMatrix4& operator*=(const EzMatrix4& other);

		//! Multiply by scalar.
		EzMatrix4 operator*(const float& scalar) const;

		//! Multiply by scalar.
		EzMatrix4& operator*=(const float& scalar);

		//! Set matrix to identity.
		inline EzMatrix4& makeIdentity();

		//! Returns true if the matrix is the identity matrix
		inline bool isIdentity() const;

		//! Returns true if the matrix is the identity matrix
		bool isIdentity_integer_base () const;

		//! Set the translation of the current matrix. Will erase any previous values.
		EzMatrix4& setTranslation( const EzPoint3& translation );

		//! Gets the current translation
		EzPoint3 getTranslation() const;

		//! Set the inverse translation of the current matrix. Will erase any previous values.
		EzMatrix4& setInverseTranslation( const EzPoint3& translation );

		//! Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
		inline EzMatrix4& setRotationRadians( const EzPoint3& rotation );

		//! Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
		EzMatrix4& setRotationDegrees( const EzPoint3& rotation );

		//! Returns the rotation, as set by setRotation().
		/** This code was orginally written by by Chev. */
		EzPoint3 getRotationDegrees() const;

		//! Make an inverted rotation matrix from Euler angles.
		/** The 4th row and column are unmodified. */
		inline EzMatrix4& setInverseRotationRadians( const EzPoint3& rotation );

		//! Make an inverted rotation matrix from Euler angles.
		/** The 4th row and column are unmodified. */
		EzMatrix4& setInverseRotationDegrees( const EzPoint3& rotation );

		//! Set Scale
		EzMatrix4& setScale( const EzPoint3& scale );

		//! Set Scale
		EzMatrix4& setScale( const float scale ) { return setScale(EzPoint3(scale,scale,scale)); }

		//! Get Scale
		EzPoint3 getScale() const;

		//! Translate a vector by the inverse of the translation part of this matrix.
		void inverseTranslateVect( EzPoint3& vect ) const;

		//! Rotate a vector by the inverse of the rotation part of this matrix.
		void inverseRotateVect( EzPoint3& vect ) const;

		//! Rotate a vector by the rotation part of this matrix.
		void rotateVect( EzPoint3& vect ) const;

		//! An alternate transform vector method, writing into a second vector
		void rotateVect(EzPoint3& out, const EzPoint3& in) const;

		//! An alternate transform vector method, writing into an array of 3 floats
		void rotateVect(float *out,const EzPoint3 &in) const;

		//! Transforms the vector by this matrix
		void transformVect( EzPoint3& vect) const;

		//! Transforms input vector by this matrix and stores result in output vector
		void transformVect( EzPoint3& out, const EzPoint3& in ) const;

		//! An alternate transform vector method, writing into an array of 4 floats
		void transformVect(float *out,const EzPoint3 &in) const;

		//! Translate a vector by the translation part of this matrix.
		void translateVect( EzPoint3& vect ) const;

		//! Transforms a plane by this matrix
		void transformPlane( EzPlane3d &plane) const;

		//! Transforms a plane by this matrix
		void transformPlane( const EzPlane3d &in, EzPlane3d &out) const;

		//! Transforms a axis aligned bounding box
		/** The result box of this operation may not be accurate at all. For
		correct results, use transformBoxEx() */
		void transformBox(EzAabbox3d& box) const;

		//! Transforms a axis aligned bounding box
		/** The result box of this operation should by accurate, but this operation
		is slower than transformBox(). */
		void transformBoxEx(EzAabbox3d& box) const;

		//! Multiplies this matrix by a 1x4 matrix
		void multiplyWith1x4Matrix(float* matrix) const;

		//! Calculates inverse of matrix. Slow.
		/** \return Returns false if there is no inverse matrix.*/
		bool makeInverse();


		//! Inverts a primitive matrix which only contains a translation and a rotation
		/** \param out: where result matrix is written to. */
		bool getInversePrimitive ( EzMatrix4& out ) const;

		//! Gets the inversed matrix of this one
		/** \param out: where result matrix is written to.
		\return Returns false if there is no inverse matrix. */
		bool getInverse(EzMatrix4& out) const;

		//! Builds a right-handed perspective projection matrix based on a field of view
		EzMatrix4& buildProjectionMatrixPerspectiveFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

		//! Builds a left-handed perspective projection matrix based on a field of view
		EzMatrix4& buildProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

		//! Builds a right-handed perspective projection matrix.
		EzMatrix4& buildProjectionMatrixPerspectiveRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

		//! Builds a left-handed perspective projection matrix.
		EzMatrix4& buildProjectionMatrixPerspectiveLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

		//! Builds a left-handed orthogonal projection matrix.
		EzMatrix4& buildProjectionMatrixOrthoLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

		//! Builds a right-handed orthogonal projection matrix.
		EzMatrix4& buildProjectionMatrixOrthoRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

		//! Builds a left-handed look-at matrix.
		EzMatrix4& buildCameraLookAtMatrixLH(
				const EzPoint3& position,
				const EzPoint3& target,
				const EzPoint3& upVector);

		//! Builds a right-handed look-at matrix.
		EzMatrix4& buildCameraLookAtMatrixRH(
				const EzPoint3& position,
				const EzPoint3& target,
				const EzPoint3& upVector);

		//! Builds a matrix that flattens geometry into a plane.
		/** \param light: light source
		\param plane: plane into which the geometry if flattened into
		\param point: value between 0 and 1, describing the light source.
		If this is 1, it is a point light, if it is 0, it is a directional light. */
		EzMatrix4& buildShadowMatrix(const EzPoint3& light, EzPlane3d plane, float point=1.0f);

		//! Builds a matrix which transforms a normalized Device Coordinate to Device Coordinates.
		/** Used to scale <-1,-1><1,1> to viewport, for example from von <-1,-1> <1,1> to the viewport <0,0><0,640> */
		EzMatrix4& buildNDCToDCMatrix( const EzRect& area, float zScale);

		//! Creates a new matrix as interpolated matrix from two other ones.
		/** \param b: other matrix to interpolate with
		\param time: Must be a value between 0 and 1. */
		EzMatrix4 interpolate(const EzMatrix4& b, float time) const;

		//! Gets transposed matrix
		EzMatrix4 getTransposed() const;

		//! Gets transposed matrix
		inline void getTransposed( EzMatrix4& dest ) const;

		/*
			construct 2D Texture transformations
			rotate about center, scale, and transform.
		*/
		//! Set to a texture transformation matrix with the given parameters.
		EzMatrix4& buildTextureTransform( float rotateRad,
				const EzPoint2 &rotatecenter,
				const EzPoint2 &translate,
				const EzPoint2 &scale);

		//! Set texture transformation rotation
		/** Rotate about z axis, recenter at (0.5,0.5).
		Doesn't clear other elements than those affected
		\param radAngle Angle in radians
		\return Altered matrix */
		EzMatrix4& setTextureRotationCenter( float radAngle );

		//! Set texture transformation translation
		/** Doesn't clear other elements than those affected.
		\param x Offset on x axis
		\param y Offset on y axis
		\return Altered matrix */
		EzMatrix4& setTextureTranslate( float x, float y );

		//! Set texture transformation translation, using a transposed representation
		/** Doesn't clear other elements than those affected.
		\param x Offset on x axis
		\param y Offset on y axis
		\return Altered matrix */
		EzMatrix4& setTextureTranslateTransposed( float x, float y );

		//! Set texture transformation scale
		/** Doesn't clear other elements than those affected.
		\param sx Scale factor on x axis
		\param sy Scale factor on y axis
		\return Altered matrix. */
		EzMatrix4& setTextureScale( float sx, float sy );

		//! Set texture transformation scale, and recenter at (0.5,0.5)
		/** Doesn't clear other elements than those affected.
		\param sx Scale factor on x axis
		\param sy Scale factor on y axis
		\return Altered matrix. */
		EzMatrix4& setTextureScaleCenter( float sx, float sy );

		//! Sets all matrix data members at once
		EzMatrix4& setM(const float* data);

		//! Sets if the matrix is definitely identity matrix
		void setDefinitelyIdentityMatrix( bool isDefinitelyIdentityMatrix);

		//! Gets if the matrix is definitely identity matrix
		bool getDefinitelyIdentityMatrix() const;

	private:
		//! Matrix data, stored in row-major order
		float M[16];
		//! Flag is this matrix is identity matrix
		mutable bool definitelyIdentityMatrix;
};

// Default constructor
inline EzMatrix4::EzMatrix4( eConstructor constructor ) : definitelyIdentityMatrix(false)
{
	switch ( constructor )
	{
		case EM4CONST_NOTHING:
		case EM4CONST_COPY:
			break;
		case EM4CONST_IDENTITY:
		case EM4CONST_INVERSE:
		default:
			makeIdentity();
			break;
	}
}

// Copy constructor
inline EzMatrix4::EzMatrix4( const EzMatrix4& other, eConstructor constructor) : definitelyIdentityMatrix(false)
{
	switch ( constructor )
	{
		case EM4CONST_IDENTITY:
			makeIdentity();
			break;
		case EM4CONST_NOTHING:
			break;
		case EM4CONST_COPY:
			*this = other;
			break;
		case EM4CONST_TRANSPOSED:
			other.getTransposed(*this);
			break;
		case EM4CONST_INVERSE:
			if (!other.getInverse(*this))
				memset(M, 0, 16*sizeof(float));
			break;
		case EM4CONST_INVERSE_TRANSPOSED:
			if (!other.getInverse(*this))
				memset(M, 0, 16*sizeof(float));
			else
				*this=getTransposed();
			break;
	}
}

//! Add another matrix.
inline EzMatrix4 EzMatrix4::operator+(const EzMatrix4& other) const
{
	EzMatrix4 temp ( EM4CONST_NOTHING );

	temp[0] = M[0]+other[0];
	temp[1] = M[1]+other[1];
	temp[2] = M[2]+other[2];
	temp[3] = M[3]+other[3];
	temp[4] = M[4]+other[4];
	temp[5] = M[5]+other[5];
	temp[6] = M[6]+other[6];
	temp[7] = M[7]+other[7];
	temp[8] = M[8]+other[8];
	temp[9] = M[9]+other[9];
	temp[10] = M[10]+other[10];
	temp[11] = M[11]+other[11];
	temp[12] = M[12]+other[12];
	temp[13] = M[13]+other[13];
	temp[14] = M[14]+other[14];
	temp[15] = M[15]+other[15];

	return temp;
}

//! Add another matrix.
inline EzMatrix4& EzMatrix4::operator+=(const EzMatrix4& other)
{
	M[0]+=other[0];
	M[1]+=other[1];
	M[2]+=other[2];
	M[3]+=other[3];
	M[4]+=other[4];
	M[5]+=other[5];
	M[6]+=other[6];
	M[7]+=other[7];
	M[8]+=other[8];
	M[9]+=other[9];
	M[10]+=other[10];
	M[11]+=other[11];
	M[12]+=other[12];
	M[13]+=other[13];
	M[14]+=other[14];
	M[15]+=other[15];

	return *this;
}

//! Subtract another matrix.
inline EzMatrix4 EzMatrix4::operator-(const EzMatrix4& other) const
{
	EzMatrix4 temp ( EM4CONST_NOTHING );

	temp[0] = M[0]-other[0];
	temp[1] = M[1]-other[1];
	temp[2] = M[2]-other[2];
	temp[3] = M[3]-other[3];
	temp[4] = M[4]-other[4];
	temp[5] = M[5]-other[5];
	temp[6] = M[6]-other[6];
	temp[7] = M[7]-other[7];
	temp[8] = M[8]-other[8];
	temp[9] = M[9]-other[9];
	temp[10] = M[10]-other[10];
	temp[11] = M[11]-other[11];
	temp[12] = M[12]-other[12];
	temp[13] = M[13]-other[13];
	temp[14] = M[14]-other[14];
	temp[15] = M[15]-other[15];

	return temp;
}

//! Subtract another matrix.
inline EzMatrix4& EzMatrix4::operator-=(const EzMatrix4& other)
{
	M[0]-=other[0];
	M[1]-=other[1];
	M[2]-=other[2];
	M[3]-=other[3];
	M[4]-=other[4];
	M[5]-=other[5];
	M[6]-=other[6];
	M[7]-=other[7];
	M[8]-=other[8];
	M[9]-=other[9];
	M[10]-=other[10];
	M[11]-=other[11];
	M[12]-=other[12];
	M[13]-=other[13];
	M[14]-=other[14];
	M[15]-=other[15];

	return *this;
}

//! Multiply by scalar.
inline EzMatrix4 EzMatrix4::operator*(const float& scalar) const
{
	EzMatrix4 temp ( EM4CONST_NOTHING );

	temp[0] = M[0]*scalar;
	temp[1] = M[1]*scalar;
	temp[2] = M[2]*scalar;
	temp[3] = M[3]*scalar;
	temp[4] = M[4]*scalar;
	temp[5] = M[5]*scalar;
	temp[6] = M[6]*scalar;
	temp[7] = M[7]*scalar;
	temp[8] = M[8]*scalar;
	temp[9] = M[9]*scalar;
	temp[10] = M[10]*scalar;
	temp[11] = M[11]*scalar;
	temp[12] = M[12]*scalar;
	temp[13] = M[13]*scalar;
	temp[14] = M[14]*scalar;
	temp[15] = M[15]*scalar;

	return temp;
}

//! Multiply by scalar.
inline EzMatrix4& EzMatrix4::operator*=(const float& scalar)
{
	M[0]*=scalar;
	M[1]*=scalar;
	M[2]*=scalar;
	M[3]*=scalar;
	M[4]*=scalar;
	M[5]*=scalar;
	M[6]*=scalar;
	M[7]*=scalar;
	M[8]*=scalar;
	M[9]*=scalar;
	M[10]*=scalar;
	M[11]*=scalar;
	M[12]*=scalar;
	M[13]*=scalar;
	M[14]*=scalar;
	M[15]*=scalar;

	return *this;
}

//! Multiply by another matrix.
inline EzMatrix4& EzMatrix4::operator*=(const EzMatrix4& other)
{
	// do checks on your own in order to avoid copy creation
	if ( !other.isIdentity() )
	{
		if ( this->isIdentity() )
		{
			return (*this = other);
		}
		else
		{
			EzMatrix4 temp( *this );
			return setbyproduct_nocheck( temp, other );
		}
	}
	return *this;
}

//! multiply by another matrix
// set this matrix to the product of two other matrices
// goal is to reduce stack use and copy
inline EzMatrix4& EzMatrix4::setbyproduct_nocheck(const EzMatrix4& other_a,const EzMatrix4& other_b )
{
	const float *m1 = other_a.M;
	const float *m2 = other_b.M;

	M[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
	M[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
	M[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
	M[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

	M[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
	M[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
	M[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
	M[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

	M[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
	M[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
	M[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
	M[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

	M[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
	M[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
	M[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
	M[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
	definitelyIdentityMatrix=false;
	return *this;
}


//! multiply by another matrix
// set this matrix to the product of two other matrices
// goal is to reduce stack use and copy
inline EzMatrix4& EzMatrix4::setbyproduct(const EzMatrix4& other_a, const EzMatrix4& other_b )
{
	if ( other_a.isIdentity () )
		return (*this = other_b);
	else
	if ( other_b.isIdentity () )
		return (*this = other_a);
	else
		return setbyproduct_nocheck(other_a,other_b);
}

//! multiply by another matrix
inline EzMatrix4 EzMatrix4::operator*(const EzMatrix4& m2) const
{
	// Testing purpose..
	if ( this->isIdentity() )
		return m2;
	if ( m2.isIdentity() )
		return *this;

	EzMatrix4 m3 ( EM4CONST_NOTHING );

	const float *m1 = M;

	m3[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
	m3[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
	m3[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
	m3[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

	m3[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
	m3[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
	m3[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
	m3[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

	m3[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
	m3[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
	m3[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
	m3[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

	m3[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
	m3[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
	m3[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
	m3[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
	return m3;
}

inline EzPoint3 EzMatrix4::getTranslation() const
{
	return EzPoint3(M[12], M[13], M[14]);
}


inline EzMatrix4& EzMatrix4::setTranslation( const EzPoint3& translation )
{
	M[12] = translation.X;
	M[13] = translation.Y;
	M[14] = translation.Z;
	definitelyIdentityMatrix=false;
	return *this;
}

inline EzMatrix4& EzMatrix4::setInverseTranslation( const EzPoint3& translation )
{
	M[12] = -translation.X;
	M[13] = -translation.Y;
	M[14] = -translation.Z;
	definitelyIdentityMatrix=false;
	return *this;
}

inline EzMatrix4& EzMatrix4::setScale( const EzPoint3& scale )
{
	M[0] = scale.X;
	M[5] = scale.Y;
	M[10] = scale.Z;
	definitelyIdentityMatrix=false;
	return *this;
}

inline EzPoint3 EzMatrix4::getScale() const
{
	return EzPoint3(M[0],M[5],M[10]);
}

inline EzMatrix4& EzMatrix4::setRotationDegrees( const EzPoint3& rotation )
{
	return setRotationRadians( rotation * DEGTORAD );
}

inline EzMatrix4& EzMatrix4::setInverseRotationDegrees( const EzPoint3& rotation )
{
	return setInverseRotationRadians( rotation * DEGTORAD );
}

inline EzMatrix4& EzMatrix4::setRotationRadians( const EzPoint3& rotation )
{
	const double cr = cos( rotation.X );
	const double sr = sin( rotation.X );
	const double cp = cos( rotation.Y );
	const double sp = sin( rotation.Y );
	const double cy = cos( rotation.Z );
	const double sy = sin( rotation.Z );

	M[0] = (float)( cp*cy );
	M[1] = (float)( cp*sy );
	M[2] = (float)( -sp );

	const double srsp = sr*sp;
	const double crsp = cr*sp;

	M[4] = (float)( srsp*cy-cr*sy );
	M[5] = (float)( srsp*sy+cr*cy );
	M[6] = (float)( sr*cp );

	M[8] = (float)( crsp*cy+sr*sy );
	M[9] = (float)( crsp*sy-sr*cy );
	M[10] = (float)( cr*cp );
	definitelyIdentityMatrix=false;
	return *this;
}


//! Returns a rotation that is equivalent to that set by setRotationDegrees().
/** This code was sent in by Chev.  Note that it does not necessarily return
the *same* Euler angles as those set by setRotationDegrees(), but the rotation will
be equivalent, i.e. will have the same result when used to rotate a vector or node. */
inline EzPoint3 EzMatrix4::getRotationDegrees() const
{
	const EzMatrix4 &mat = *this;

	double Y = -asin(mat(0,2));
	const double C = cos(Y);
	Y *= RADTODEG64;

	double rotx, roty, X, Z;

	if (fabs(C)>ROUNDING_ERROR_64)
	{
		const float invC = (float)(1.0/C);
		rotx = mat(2,2) * invC;
		roty = mat(1,2) * invC;
		X = atan2( roty, rotx ) * RADTODEG64;
		rotx = mat(0,0) * invC;
		roty = mat(0,1) * invC;
		Z = atan2( roty, rotx ) * RADTODEG64;
	}
	else
	{
		X = 0.0;
		rotx = mat(1,1);
		roty = -mat(1,0);
		Z = atan2( roty, rotx ) * RADTODEG64;
	}

	// fix values that get below zero
	// before it would set (!) values to 360
	// that where above 360:
	if (X < 0.0) X += 360.0;
	if (Y < 0.0) Y += 360.0;
	if (Z < 0.0) Z += 360.0;

	return EzPoint3((float)X, (float)Y, (float)Z);
}


inline EzMatrix4& EzMatrix4::setInverseRotationRadians( const EzPoint3& rotation )
{
	double cr = cos( rotation.X );
	double sr = sin( rotation.X );
	double cp = cos( rotation.Y );
	double sp = sin( rotation.Y );
	double cy = cos( rotation.Z );
	double sy = sin( rotation.Z );

	M[0] = (float)( cp*cy );
	M[4] = (float)( cp*sy );
	M[8] = (float)( -sp );

	double srsp = sr*sp;
	double crsp = cr*sp;

	M[1] = (float)( srsp*cy-cr*sy );
	M[5] = (float)( srsp*sy+cr*cy );
	M[9] = (float)( sr*cp );

	M[2] = (float)( crsp*cy+sr*sy );
	M[6] = (float)( crsp*sy-sr*cy );
	M[10] = (float)( cr*cp );
	definitelyIdentityMatrix=false;
	return *this;
}


/*!
*/
inline EzMatrix4& EzMatrix4::makeIdentity()
{
	memset(M, 0, 16*sizeof(float));
	M[0] = M[5] = M[10] = M[15] = (float)1;
	definitelyIdentityMatrix=true;
	return *this;
}


/*
	check identity with epsilon
	solve floating range problems..
*/
inline bool EzMatrix4::isIdentity() const
{
	if (definitelyIdentityMatrix)
		return true;
	if (!equals( M[ 0], (float)1 ) ||
			!equals( M[ 5], (float)1 ) ||
			!equals( M[10], (float)1 ) ||
			!equals( M[15], (float)1 ))
		return false;

	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			if ((j != i) && (!iszero((*this)(i,j))))
				return false;

	definitelyIdentityMatrix=true;
	return true;
}

/*
	doesn't solve floating range problems..
	but takes care on +/- 0 on translation because we are changing it..
	reducing floating point branches
	but it needs the floats in memory..
*/
inline bool EzMatrix4::isIdentity_integer_base() const
{
	if (definitelyIdentityMatrix)
		return true;
	if(IR(M[0])!=FLOAT_VALUE_1)	return false;
	if(IR(M[1])!=0)			return false;
	if(IR(M[2])!=0)			return false;
	if(IR(M[3])!=0)			return false;

	if(IR(M[4])!=0)			return false;
	if(IR(M[5])!=FLOAT_VALUE_1)	return false;
	if(IR(M[6])!=0)			return false;
	if(IR(M[7])!=0)			return false;

	if(IR(M[8])!=0)			return false;
	if(IR(M[9])!=0)			return false;
	if(IR(M[10])!=FLOAT_VALUE_1)	return false;
	if(IR(M[11])!=0)		return false;

	if(IR(M[12])!=0)		return false;
	if(IR(M[13])!=0)		return false;
	if(IR(M[13])!=0)		return false;
	if(IR(M[15])!=FLOAT_VALUE_1)	return false;
	definitelyIdentityMatrix=true;
	return true;
}


inline void EzMatrix4::rotateVect( EzPoint3& vect ) const
{
	EzPoint3 tmp = vect;
	vect.X = tmp.X*M[0] + tmp.Y*M[4] + tmp.Z*M[8];
	vect.Y = tmp.X*M[1] + tmp.Y*M[5] + tmp.Z*M[9];
	vect.Z = tmp.X*M[2] + tmp.Y*M[6] + tmp.Z*M[10];
}

//! An alternate transform vector method, writing into a second vector
inline void EzMatrix4::rotateVect(EzPoint3& out, const EzPoint3& in) const
{
	out.X = in.X*M[0] + in.Y*M[4] + in.Z*M[8];
	out.Y = in.X*M[1] + in.Y*M[5] + in.Z*M[9];
	out.Z = in.X*M[2] + in.Y*M[6] + in.Z*M[10];
}

//! An alternate transform vector method, writing into an array of 3 floats
inline void EzMatrix4::rotateVect(float *out, const EzPoint3& in) const
{
	out[0] = in.X*M[0] + in.Y*M[4] + in.Z*M[8];
	out[1] = in.X*M[1] + in.Y*M[5] + in.Z*M[9];
	out[2] = in.X*M[2] + in.Y*M[6] + in.Z*M[10];
}

inline void EzMatrix4::inverseRotateVect( EzPoint3& vect ) const
{
	EzPoint3 tmp = vect;
	vect.X = tmp.X*M[0] + tmp.Y*M[1] + tmp.Z*M[2];
	vect.Y = tmp.X*M[4] + tmp.Y*M[5] + tmp.Z*M[6];
	vect.Z = tmp.X*M[8] + tmp.Y*M[9] + tmp.Z*M[10];
}

inline void EzMatrix4::transformVect( EzPoint3& vect) const
{
	float vector[3];

	vector[0] = vect.X*M[0] + vect.Y*M[4] + vect.Z*M[8] + M[12];
	vector[1] = vect.X*M[1] + vect.Y*M[5] + vect.Z*M[9] + M[13];
	vector[2] = vect.X*M[2] + vect.Y*M[6] + vect.Z*M[10] + M[14];

	vect.X = vector[0];
	vect.Y = vector[1];
	vect.Z = vector[2];
}

inline void EzMatrix4::transformVect( EzPoint3& out, const EzPoint3& in) const
{
	out.X = in.X*M[0] + in.Y*M[4] + in.Z*M[8] + M[12];
	out.Y = in.X*M[1] + in.Y*M[5] + in.Z*M[9] + M[13];
	out.Z = in.X*M[2] + in.Y*M[6] + in.Z*M[10] + M[14];
}


inline void EzMatrix4::transformVect(float *out, const EzPoint3 &in) const
{
	out[0] = in.X*M[0] + in.Y*M[4] + in.Z*M[8] + M[12];
	out[1] = in.X*M[1] + in.Y*M[5] + in.Z*M[9] + M[13];
	out[2] = in.X*M[2] + in.Y*M[6] + in.Z*M[10] + M[14];
	out[3] = in.X*M[3] + in.Y*M[7] + in.Z*M[11] + M[15];
}


//! Transforms a plane by this matrix
inline void EzMatrix4::transformPlane( EzPlane3d &plane) const
{
	EzPoint3 member;
	// Fully transform the plane member point, i.e. rotate, translate and scale it.
	transformVect(member, plane.getMemberPoint());

	EzPoint3 normal = plane.Normal;
	normal.normalize();

	// The normal needs to be rotated and inverse scaled, but not translated.
	const EzPoint3 scale = getScale();

	if(!equals(scale.X, 0.f) && !equals(scale.Y, 0.f) && !equals(scale.Z, 0.f)
		&& (!equals(scale.X, 1.f) || !equals(scale.Y, 1.f) || !equals(scale.Z, 1.f)))
	{
		// Rotating the vector will also apply the scale, so we have to invert it twice.
		normal /= (scale * scale);
	}

	rotateVect(normal);

	normal.normalize();
	plane.setPlane(member, normal);
}

//! Transforms a plane by this matrix
inline void EzMatrix4::transformPlane( const EzPlane3d &in, EzPlane3d &out) const
{
	out = in;
	transformPlane( out );
}

//! Transforms a axis aligned bounding box
inline void EzMatrix4::transformBox(EzAabbox3d& box) const
{
	if (isIdentity())
		return;

	transformVect(box.MinEdge);
	transformVect(box.MaxEdge);
	box.repair();
}

//! Transforms a axis aligned bounding box more accurately than transformBox()
inline void EzMatrix4::transformBoxEx(EzAabbox3d& box) const
{
	const float Amin[3] = {box.MinEdge.X, box.MinEdge.Y, box.MinEdge.Z};
	const float Amax[3] = {box.MaxEdge.X, box.MaxEdge.Y, box.MaxEdge.Z};

	float Bmin[3];
	float Bmax[3];

	Bmin[0] = Bmax[0] = M[12];
	Bmin[1] = Bmax[1] = M[13];
	Bmin[2] = Bmax[2] = M[14];

	const EzMatrix4 &m = *this;

	for (unsigned int i = 0; i < 3; ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			const float a = m(j,i) * Amin[j];
			const float b = m(j,i) * Amax[j];

			if (a < b)
			{
				Bmin[i] += a;
				Bmax[i] += b;
			}
			else
			{
				Bmin[i] += b;
				Bmax[i] += a;
			}
		}
	}

	box.MinEdge.X = Bmin[0];
	box.MinEdge.Y = Bmin[1];
	box.MinEdge.Z = Bmin[2];

	box.MaxEdge.X = Bmax[0];
	box.MaxEdge.Y = Bmax[1];
	box.MaxEdge.Z = Bmax[2];
}


//! Multiplies this matrix by a 1x4 matrix
inline void EzMatrix4::multiplyWith1x4Matrix(float* matrix) const
{
	/*
	0  1  2  3
	4  5  6  7
	8  9  10 11
	12 13 14 15
	*/

	float mat[4];
	mat[0] = matrix[0];
	mat[1] = matrix[1];
	mat[2] = matrix[2];
	mat[3] = matrix[3];

	matrix[0] = M[0]*mat[0] + M[4]*mat[1] + M[8]*mat[2] + M[12]*mat[3];
	matrix[1] = M[1]*mat[0] + M[5]*mat[1] + M[9]*mat[2] + M[13]*mat[3];
	matrix[2] = M[2]*mat[0] + M[6]*mat[1] + M[10]*mat[2] + M[14]*mat[3];
	matrix[3] = M[3]*mat[0] + M[7]*mat[1] + M[11]*mat[2] + M[15]*mat[3];
}

inline void EzMatrix4::inverseTranslateVect( EzPoint3& vect ) const
{
	vect.X = vect.X-M[12];
	vect.Y = vect.Y-M[13];
	vect.Z = vect.Z-M[14];
}

inline void EzMatrix4::translateVect( EzPoint3& vect ) const
{
	vect.X = vect.X+M[12];
	vect.Y = vect.Y+M[13];
	vect.Z = vect.Z+M[14];
}


inline bool EzMatrix4::getInverse(EzMatrix4& out) const
{
	/// Calculates the inverse of this Matrix
	/// The inverse is calculated using Cramers rule.
	/// If no inverse exists then 'false' is returned.

	if ( this->isIdentity() )
	{
		out=*this;
		return true;
	}

	const EzMatrix4 &m = *this;

	float d = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) -
		(m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
		(m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)) +
		(m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) -
		(m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
		(m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0));

	if( iszero ( d ) )
		return false;

	d = reciprocal ( d );

	out(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) +
			m(1, 2) * (m(2, 3) * m(3, 1) - m(2, 1) * m(3, 3)) +
			m(1, 3) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)));
	out(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(0, 3) * m(3, 2)) +
			m(2, 2) * (m(0, 3) * m(3, 1) - m(0, 1) * m(3, 3)) +
			m(2, 3) * (m(0, 1) * m(3, 2) - m(0, 2) * m(3, 1)));
	out(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) +
			m(3, 2) * (m(0, 3) * m(1, 1) - m(0, 1) * m(1, 3)) +
			m(3, 3) * (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)));
	out(0, 3) = d * (m(0, 1) * (m(1, 3) * m(2, 2) - m(1, 2) * m(2, 3)) +
			m(0, 2) * (m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1)) +
			m(0, 3) * (m(1, 2) * m(2, 1) - m(1, 1) * m(2, 2)));
	out(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) +
			m(1, 3) * (m(2, 2) * m(3, 0) - m(2, 0) * m(3, 2)) +
			m(1, 0) * (m(2, 3) * m(3, 2) - m(2, 2) * m(3, 3)));
	out(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(0, 3) * m(3, 0)) +
			m(2, 3) * (m(0, 2) * m(3, 0) - m(0, 0) * m(3, 2)) +
			m(2, 0) * (m(0, 3) * m(3, 2) - m(0, 2) * m(3, 3)));
	out(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) +
			m(3, 3) * (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) +
			m(3, 0) * (m(0, 3) * m(1, 2) - m(0, 2) * m(1, 3)));
	out(1, 3) = d * (m(0, 2) * (m(1, 3) * m(2, 0) - m(1, 0) * m(2, 3)) +
			m(0, 3) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
			m(0, 0) * (m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2)));
	out(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0)) +
			m(1, 0) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
			m(1, 1) * (m(2, 3) * m(3, 0) - m(2, 0) * m(3, 3)));
	out(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(0, 1) * m(3, 0)) +
			m(2, 0) * (m(0, 1) * m(3, 3) - m(0, 3) * m(3, 1)) +
			m(2, 1) * (m(0, 3) * m(3, 0) - m(0, 0) * m(3, 3)));
	out(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) +
			m(3, 0) * (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) +
			m(3, 1) * (m(0, 3) * m(1, 0) - m(0, 0) * m(1, 3)));
	out(2, 3) = d * (m(0, 3) * (m(1, 1) * m(2, 0) - m(1, 0) * m(2, 1)) +
			m(0, 0) * (m(1, 3) * m(2, 1) - m(1, 1) * m(2, 3)) +
			m(0, 1) * (m(1, 0) * m(2, 3) - m(1, 3) * m(2, 0)));
	out(3, 0) = d * (m(1, 0) * (m(2, 2) * m(3, 1) - m(2, 1) * m(3, 2)) +
			m(1, 1) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
			m(1, 2) * (m(2, 1) * m(3, 0) - m(2, 0) * m(3, 1)));
	out(3, 1) = d * (m(2, 0) * (m(0, 2) * m(3, 1) - m(0, 1) * m(3, 2)) +
			m(2, 1) * (m(0, 0) * m(3, 2) - m(0, 2) * m(3, 0)) +
			m(2, 2) * (m(0, 1) * m(3, 0) - m(0, 0) * m(3, 1)));
	out(3, 2) = d * (m(3, 0) * (m(0, 2) * m(1, 1) - m(0, 1) * m(1, 2)) +
			m(3, 1) * (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) +
			m(3, 2) * (m(0, 1) * m(1, 0) - m(0, 0) * m(1, 1)));
	out(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) +
			m(0, 1) * (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) +
			m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)));
	out.definitelyIdentityMatrix = definitelyIdentityMatrix;
	return true;
}


//! Inverts a primitive matrix which only contains a translation and a rotation
//! \param out: where result matrix is written to.
inline bool EzMatrix4::getInversePrimitive ( EzMatrix4& out ) const
{
	out.M[0 ] = M[0];
	out.M[1 ] = M[4];
	out.M[2 ] = M[8];
	out.M[3 ] = 0;

	out.M[4 ] = M[1];
	out.M[5 ] = M[5];
	out.M[6 ] = M[9];
	out.M[7 ] = 0;

	out.M[8 ] = M[2];
	out.M[9 ] = M[6];
	out.M[10] = M[10];
	out.M[11] = 0;

	out.M[12] = (float)-(M[12]*M[0] + M[13]*M[1] + M[14]*M[2]);
	out.M[13] = (float)-(M[12]*M[4] + M[13]*M[5] + M[14]*M[6]);
	out.M[14] = (float)-(M[12]*M[8] + M[13]*M[9] + M[14]*M[10]);
	out.M[15] = 1;
	out.definitelyIdentityMatrix = definitelyIdentityMatrix;
	return true;
}

/*!
*/
inline bool EzMatrix4::makeInverse()
{
	if (definitelyIdentityMatrix)
		return true;

	EzMatrix4 temp ( EM4CONST_NOTHING );

	if (getInverse(temp))
	{
		*this = temp;
		return true;
	}

	return false;
}


inline EzMatrix4& EzMatrix4::operator=(const EzMatrix4 &other)
{
	if (this==&other)
		return *this;
	memcpy(M, other.M, 16*sizeof(float));
	definitelyIdentityMatrix=other.definitelyIdentityMatrix;
	return *this;
}


inline EzMatrix4& EzMatrix4::operator=(const float& scalar)
{
	for (int i = 0; i < 16; ++i)
		M[i]=scalar;
	definitelyIdentityMatrix=false;
	return *this;
}


inline bool EzMatrix4::operator==(const EzMatrix4 &other) const
{
	if (definitelyIdentityMatrix && other.definitelyIdentityMatrix)
		return true;
	for (int i = 0; i < 16; ++i)
		if (M[i] != other.M[i])
			return false;

	return true;
}


inline bool EzMatrix4::operator!=(const EzMatrix4 &other) const
{
	return !(*this == other);
}


// Builds a right-handed perspective projection matrix based on a field of view
inline EzMatrix4& EzMatrix4::buildProjectionMatrixPerspectiveFovRH(
		float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const double h = 1.0/tan(fieldOfViewRadians/2.0);
	const float w = (float) (h / aspectRatio);

	M[0] = w;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (float)h;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (float)(zFar/(zNear-zFar)); // DirectX version
//		M[10] = (float)(zFar+zNear/(zNear-zFar)); // OpenGL version
	M[11] = -1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (float)(zNear*zFar/(zNear-zFar)); // DirectX version
//		M[14] = (float)(2.0f*zNear*zFar/(zNear-zFar)); // OpenGL version
	M[15] = 0;
	definitelyIdentityMatrix=false;
	return *this;
}


// Builds a left-handed perspective projection matrix based on a field of view
inline EzMatrix4& EzMatrix4::buildProjectionMatrixPerspectiveFovLH(
		float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const double h = 1.0/tan(fieldOfViewRadians/2.0);
	const float w = (float)(h / aspectRatio);

	M[0] = w;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (float) h;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (zFar/(zFar-zNear));
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (-zNear*zFar/(zFar-zNear));
	M[15] = 0;
	definitelyIdentityMatrix=false;
	return *this;
}


// Builds a left-handed orthogonal projection matrix.
inline EzMatrix4& EzMatrix4::buildProjectionMatrixOrthoLH(
		float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	M[0] = (float)(2/widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (float)(2/heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (float)(1/(zFar-zNear));
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = (float)(zNear/(zNear-zFar));
	M[15] = 1;
	definitelyIdentityMatrix=false;
	return *this;
}


// Builds a right-handed orthogonal projection matrix.
inline EzMatrix4& EzMatrix4::buildProjectionMatrixOrthoRH(
		float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	M[0] = (float)(2/widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (float)(2/heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (float)(1/(zNear-zFar));
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = (float)(zNear/(zNear-zFar));
	M[15] = -1;
	definitelyIdentityMatrix=false;
	return *this;
}


// Builds a right-handed perspective projection matrix.
inline EzMatrix4& EzMatrix4::buildProjectionMatrixPerspectiveRH(
		float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	M[0] = (float)(2*zNear/widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (float)(2*zNear/heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (float)(zFar/(zNear-zFar));
	M[11] = -1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (float)(zNear*zFar/(zNear-zFar));
	M[15] = 0;
	definitelyIdentityMatrix=false;
	return *this;
}


// Builds a left-handed perspective projection matrix.
inline EzMatrix4& EzMatrix4::buildProjectionMatrixPerspectiveLH(
		float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	M[0] = (float)(2*zNear/widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (float)(2*zNear/heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (float)(zFar/(zFar-zNear));
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (float)(zNear*zFar/(zNear-zFar));
	M[15] = 0;
	definitelyIdentityMatrix=false;
	return *this;
}


// Builds a matrix that flattens geometry into a plane.
inline EzMatrix4& EzMatrix4::buildShadowMatrix(const EzPoint3& light, EzPlane3d plane, float point)
{
	plane.Normal.normalize();
	const float d = plane.Normal.dotProduct(light);

	M[ 0] = (float)(-plane.Normal.X * light.X + d);
	M[ 1] = (float)(-plane.Normal.X * light.Y);
	M[ 2] = (float)(-plane.Normal.X * light.Z);
	M[ 3] = (float)(-plane.Normal.X * point);

	M[ 4] = (float)(-plane.Normal.Y * light.X);
	M[ 5] = (float)(-plane.Normal.Y * light.Y + d);
	M[ 6] = (float)(-plane.Normal.Y * light.Z);
	M[ 7] = (float)(-plane.Normal.Y * point);

	M[ 8] = (float)(-plane.Normal.Z * light.X);
	M[ 9] = (float)(-plane.Normal.Z * light.Y);
	M[10] = (float)(-plane.Normal.Z * light.Z + d);
	M[11] = (float)(-plane.Normal.Z * point);

	M[12] = (float)(-plane.D * light.X);
	M[13] = (float)(-plane.D * light.Y);
	M[14] = (float)(-plane.D * light.Z);
	M[15] = (float)(-plane.D * point + d);
	definitelyIdentityMatrix=false;
	return *this;
}

// Builds a left-handed look-at matrix.
inline EzMatrix4& EzMatrix4::buildCameraLookAtMatrixLH(
			const EzPoint3& position,
			const EzPoint3& target,
			const EzPoint3& upVector)
{
	EzPoint3 zaxis = target - position;
	zaxis.normalize();

	EzPoint3 xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	EzPoint3 yaxis = zaxis.crossProduct(xaxis);

	M[0] = (float)xaxis.X;
	M[1] = (float)yaxis.X;
	M[2] = (float)zaxis.X;
	M[3] = 0;

	M[4] = (float)xaxis.Y;
	M[5] = (float)yaxis.Y;
	M[6] = (float)zaxis.Y;
	M[7] = 0;

	M[8] = (float)xaxis.Z;
	M[9] = (float)yaxis.Z;
	M[10] = (float)zaxis.Z;
	M[11] = 0;

	M[12] = (float)-xaxis.dotProduct(position);
	M[13] = (float)-yaxis.dotProduct(position);
	M[14] = (float)-zaxis.dotProduct(position);
	M[15] = 1;
	definitelyIdentityMatrix=false;
	return *this;
}


// Builds a right-handed look-at matrix.
inline EzMatrix4& EzMatrix4::buildCameraLookAtMatrixRH(
			const EzPoint3& position,
			const EzPoint3& target,
			const EzPoint3& upVector)
{
	EzPoint3 zaxis = position - target;
	zaxis.normalize();

	EzPoint3 xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	EzPoint3 yaxis = zaxis.crossProduct(xaxis);

	M[0] = (float)xaxis.X;
	M[1] = (float)yaxis.X;
	M[2] = (float)zaxis.X;
	M[3] = 0;

	M[4] = (float)xaxis.Y;
	M[5] = (float)yaxis.Y;
	M[6] = (float)zaxis.Y;
	M[7] = 0;

	M[8] = (float)xaxis.Z;
	M[9] = (float)yaxis.Z;
	M[10] = (float)zaxis.Z;
	M[11] = 0;

	M[12] = (float)-xaxis.dotProduct(position);
	M[13] = (float)-yaxis.dotProduct(position);
	M[14] = (float)-zaxis.dotProduct(position);
	M[15] = 1;
	definitelyIdentityMatrix=false;
	return *this;
}


// creates a new matrix as interpolated matrix from this and the passed one.
inline EzMatrix4 EzMatrix4::interpolate(const EzMatrix4& b, float time) const
{
	EzMatrix4 mat ( EM4CONST_NOTHING );

	for (unsigned int i=0; i < 16; i += 4)
	{
		mat.M[i+0] = (float)(M[i+0] + ( b.M[i+0] - M[i+0] ) * time);
		mat.M[i+1] = (float)(M[i+1] + ( b.M[i+1] - M[i+1] ) * time);
		mat.M[i+2] = (float)(M[i+2] + ( b.M[i+2] - M[i+2] ) * time);
		mat.M[i+3] = (float)(M[i+3] + ( b.M[i+3] - M[i+3] ) * time);
	}
	return mat;
}


// returns transposed matrix
inline EzMatrix4 EzMatrix4::getTransposed() const
{
	EzMatrix4 t ( EM4CONST_NOTHING );
	getTransposed ( t );
	return t;
}


// returns transposed matrix
inline void EzMatrix4::getTransposed( EzMatrix4& o ) const
{
	o[ 0] = M[ 0];
	o[ 1] = M[ 4];
	o[ 2] = M[ 8];
	o[ 3] = M[12];

	o[ 4] = M[ 1];
	o[ 5] = M[ 5];
	o[ 6] = M[ 9];
	o[ 7] = M[13];

	o[ 8] = M[ 2];
	o[ 9] = M[ 6];
	o[10] = M[10];
	o[11] = M[14];

	o[12] = M[ 3];
	o[13] = M[ 7];
	o[14] = M[11];
	o[15] = M[15];
	o.definitelyIdentityMatrix=definitelyIdentityMatrix;
}


// used to scale <-1,-1><1,1> to viewport
inline EzMatrix4& EzMatrix4::buildNDCToDCMatrix( const EzRect& viewport, float zScale)
{
	const float scaleX = (viewport.getWidth() - 0.75f ) / 2.0f;
	const float scaleY = -(viewport.getHeight() - 0.75f ) / 2.0f;

	const float dx = -0.5f + ( (viewport.UpperLeftCorner.X + viewport.LowerRightCorner.X ) / 2.0f );
	const float dy = -0.5f + ( (viewport.UpperLeftCorner.Y + viewport.LowerRightCorner.Y ) / 2.0f );

	makeIdentity();
	M[12] = (float)dx;
	M[13] = (float)dy;
	return setScale(EzPoint3((float)scaleX, (float)scaleY, (float)zScale));
}

/*!
	Generate texture coordinates as linear functions so that:
		u = Ux*x + Uy*y + Uz*z + Uw
		v = Vx*x + Vy*y + Vz*z + Vw
	The matrix M for this case is:
		Ux  Vx  0  0
		Uy  Vy  0  0
		Uz  Vz  0  0
		Uw  Vw  0  0
*/


inline EzMatrix4& EzMatrix4::buildTextureTransform( float rotateRad,
		const EzPoint2 &rotatecenter,
		const EzPoint2 &translate,
		const EzPoint2 &scale)
{
	const float c = cosf(rotateRad);
	const float s = sinf(rotateRad);

	M[0] = (float)(c * scale.X);
	M[1] = (float)(s * scale.Y);
	M[2] = 0;
	M[3] = 0;

	M[4] = (float)(-s * scale.X);
	M[5] = (float)(c * scale.Y);
	M[6] = 0;
	M[7] = 0;

	M[8] = (float)(c * scale.X * rotatecenter.X + -s * rotatecenter.Y + translate.X);
	M[9] = (float)(s * scale.Y * rotatecenter.X +  c * rotatecenter.Y + translate.Y);
	M[10] = 1;
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = 0;
	M[15] = 1;
	definitelyIdentityMatrix=false;
	return *this;
}


// rotate about z axis, center ( 0.5, 0.5 )
inline EzMatrix4& EzMatrix4::setTextureRotationCenter( float rotateRad )
{
	const float c = cosf(rotateRad);
	const float s = sinf(rotateRad);
	M[0] = (float)c;
	M[1] = (float)s;

	M[4] = (float)-s;
	M[5] = (float)c;

	M[8] = (float)(0.5f * ( s - c) + 0.5f);
	M[9] = (float)(-0.5f * ( s + c) + 0.5f);
	definitelyIdentityMatrix = definitelyIdentityMatrix && (rotateRad==0.0f);
	return *this;
}


inline EzMatrix4& EzMatrix4::setTextureTranslate ( float x, float y )
{
	M[8] = (float)x;
	M[9] = (float)y;
	definitelyIdentityMatrix = definitelyIdentityMatrix && (x==0.0f) && (y==0.0f);
	return *this;
}


inline EzMatrix4& EzMatrix4::setTextureTranslateTransposed ( float x, float y )
{
	M[2] = (float)x;
	M[6] = (float)y;
	definitelyIdentityMatrix = definitelyIdentityMatrix && (x==0.0f) && (y==0.0f) ;
	return *this;
}

inline EzMatrix4& EzMatrix4::setTextureScale ( float sx, float sy )
{
	M[0] = (float)sx;
	M[5] = (float)sy;
	definitelyIdentityMatrix = definitelyIdentityMatrix && (sx==1.0f) && (sy==1.0f);
	return *this;
}


inline EzMatrix4& EzMatrix4::setTextureScaleCenter( float sx, float sy )
{
	M[0] = (float)sx;
	M[5] = (float)sy;
	M[8] = (float)(0.5f - 0.5f * sx);
	M[9] = (float)(0.5f - 0.5f * sy);
	definitelyIdentityMatrix = definitelyIdentityMatrix && (sx==1.0f) && (sy==1.0f);
	return *this;
}


// sets all matrix data members at once
inline EzMatrix4& EzMatrix4::setM(const float* data)
{
	memcpy(M,data, 16*sizeof(float));

	definitelyIdentityMatrix = false;
	return *this;
}


// sets if the matrix is definitely identity matrix
inline void EzMatrix4::setDefinitelyIdentityMatrix( bool isDefinitelyIdentityMatrix)
{
	definitelyIdentityMatrix = isDefinitelyIdentityMatrix;
}


// gets if the matrix is definitely identity matrix
inline bool EzMatrix4::getDefinitelyIdentityMatrix() const
{
	return definitelyIdentityMatrix;
}


// Multiply by scalar.
inline EzMatrix4 operator*(const float scalar, const EzMatrix4& mat)
{
	return mat*scalar;
}


//! global const identity matrix
EZCORE_ENTRY extern const EzMatrix4 IdentityMatrix;


#endif

