#pragma once


#include "EzPoint3.h"
#include "EzQuaternion.h"
#include "EzMemObject.h"

// result = mat*pt
#define EzMatrix3TimesPoint(result,mat,pt) \
{ \
    result.X = mat[0] * (pt).X + \
               mat[3] * (pt).Y + \
               mat[6] * (pt).Z;  \
    result.Y = mat[1] * (pt).X + \
               mat[4] * (pt).Y + \
               mat[7] * (pt).Z;  \
    result.Z = mat[2] * (pt).X + \
               mat[5] * (pt).Y + \
               mat[8] * (pt).Z;  \
}

// result = mat*pt0+pt1
#define EzMatrix3TimesPointPlusPoint(result,mat,pt0,pt1) \
{ \
    result.X = mat[0] * (pt0).X +          \
               mat[3] * (pt0).Y +          \
               mat[6] * (pt0).Z + (pt1).X; \
    result.Y = mat[1] * (pt0).X +          \
               mat[4] * (pt0).Y +          \
               mat[7] * (pt0).Z + (pt1).Y; \
    result.Z = mat[2] * (pt0).X +          \
               mat[5] * (pt0).Y +          \
               mat[8] * (pt0).Z + (pt1).Z; \
}

// result = pt*transpose(mat)
#define EzPointTimesMatrix(result,pt,mat) \
{ \
    result.X = mat[0] * (pt).X + \
               mat[1] * (pt).Y + \
               mat[2] * (pt).Z;  \
    result.Y = mat[3] * (pt).X + \
               mat[4] * (pt).Y + \
               mat[5] * (pt).Z;  \
    result.Z = mat[6] * (pt).X + \
               mat[7] * (pt).Y + \
               mat[8] * (pt).Z;  \
}


//! 3x3 matrix. Mostly used as transformation matrix for 3d calculations.
class EZCORE_ENTRY EzMatrix3 : public EzMemObject
{
public:
	EzMatrix3(void) { makeIdentity(); }
	~EzMatrix3(void){}

	//! Sets this matrix equal to the other matrix.
	inline EzMatrix3& operator=(const EzMatrix3 &other);

	//! Simple operator for directly accessing every element of the matrix.
	float& operator()(const int row, const int col) { definitelyIdentityMatrix=false; return M[ row * 3 + col ]; }

	//! Simple operator for directly accessing every element of the matrix.
	const float& operator()(const int row, const int col) const { return M[row * 3 + col]; }

	//! Simple operator for linearly accessing every element of the matrix.
	float& operator[](unsigned int index) { definitelyIdentityMatrix=false; return M[index]; }

	//! Simple operator for linearly accessing every element of the matrix.
	const float& operator[](unsigned int index) const { return M[index]; }

	bool operator==( const EzMatrix3 & other ) const
	{
		return 
			(M[0] == other[0] ) &&
			(M[1] == other[1] ) &&
			(M[2] == other[2] ) &&
			(M[3] == other[3] ) &&
			(M[4] == other[4] ) &&
			(M[5] == other[5] ) &&
			(M[6] == other[6] ) &&
			(M[7] == other[7] ) &&
			(M[8] == other[8] );
	}

	bool operator!=( const EzMatrix3 & other ) const
	{
		return !(*this==other);
	}


	EzMatrix3 operator+( const EzMatrix3 & other ) const
	{
		EzMatrix3 result = *this;

		result[0] += other[0];
		result[1] += other[1];
		result[2] += other[2];
		result[3] += other[3];
		result[4] += other[4];
		result[5] += other[5];
		result[6] += other[6];
		result[7] += other[7];
		result[8] += other[8];

		return result;
	}

	EzMatrix3 operator-( const EzMatrix3 & other ) const
	{
		EzMatrix3 result = *this;

		result[0] -= other[0];
		result[1] -= other[1];
		result[2] -= other[2];
		result[3] -= other[3];
		result[4] -= other[4];
		result[5] -= other[5];
		result[6] -= other[6];
		result[7] -= other[7];
		result[8] -= other[8];

		return result;
	}



	EzMatrix3 operator*( const EzMatrix3 & other ) const
	{
		EzMatrix3 result;

		result[0] = M[0]*other[0] + M[3]*other[1] + M[6]*other[2];
		result[1] = M[1]*other[0] + M[4]*other[1] + M[7]*other[2];
		result[2] = M[2]*other[0] + M[5]*other[1] + M[8]*other[2];

		result[3] = M[0]*other[3] + M[3]*other[4] + M[6]*other[5];
		result[4] = M[1]*other[3] + M[4]*other[4] + M[7]*other[5];
		result[5] = M[2]*other[3] + M[5]*other[4] + M[8]*other[5];

		result[6] = M[0]*other[6] + M[3]*other[7] + M[6]*other[8];
		result[7] = M[1]*other[6] + M[4]*other[7] + M[7]*other[8];
		result[8] = M[2]*other[6] + M[5]*other[7] + M[8]*other[8];

		return result;
	}
	EzMatrix3 operator*( const float fScalar) const
	{
		EzMatrix3 result;
		const float *m1 = M;
		result[0] = m1[0] * fScalar;
		result[1] = m1[1] * fScalar;
		result[2] = m1[2] * fScalar;
		
		result[3] = m1[3] * fScalar;
		result[4] = m1[4] * fScalar;
		result[5] = m1[5] * fScalar;

		result[6] = m1[6] * fScalar;
		result[7] = m1[7] * fScalar;
		result[8] = m1[8] * fScalar;

		return result;
	}

	EzPoint3 operator*( const EzPoint3& v ) const
	{
		//return EzPoint3((float)M[0]*v.X + M[1]*v.Y + M[2]*v.Z,
		//	(float) M[3]*v.X + M[4]*v.Y + M[5]*v.Z,
		//	(float) M[6]*v.X + M[7]*v.Y + M[8]*v.Z );

		return EzPoint3((float)M[0]*v.X + M[3]*v.Y + M[6]*v.Z,
			(float) M[1]*v.X + M[4]*v.Y + M[7]*v.Z,
			(float) M[2]*v.X + M[5]*v.Y + M[8]*v.Z );
	}


	inline EzMatrix3& makeIdentity();
	inline EzMatrix3 transpose();
	
	inline bool EzMatrix3::isIdentity() const;

	inline EzMatrix3& setRotationDegrees( const EzPoint3& rotation );
	inline EzMatrix3& setRotationRadians( const EzPoint3& rotation );

	inline EzPoint3 getRotationDegrees() const;

	inline void fromQuat( const EzQuaternion & q );
	inline EzQuaternion EzMatrix3::toQuat() const;
	


protected:
	float M[9];
	//! Flag is this matrix is identity matrix
	mutable bool definitelyIdentityMatrix;


};


inline bool EzMatrix3::isIdentity() const
{
	if (definitelyIdentityMatrix)
		return true;

	return false;
}

inline EzMatrix3& EzMatrix3::makeIdentity()
{
	memset(M, 0, 9*sizeof(float));
	M[0] = M[4] = M[8] = (float)1;
	definitelyIdentityMatrix=true;
	return *this;
}

inline EzMatrix3 EzMatrix3::transpose()
{

	EzMatrix3 m3;

	m3[0] = M[0];
	m3[3] = M[1];
	m3[6] = M[2];

	m3[1] = M[3];
	m3[4] = M[4];
	m3[7] = M[5];

	m3[2] = M[6];
	m3[5] = M[7];
	m3[8] = M[8];

	return m3;

}


inline EzMatrix3& EzMatrix3::operator=(const EzMatrix3 &other)
{
	if (this==&other)
		return *this;

	memcpy(M, other.M, 9*sizeof(float));
	definitelyIdentityMatrix=other.definitelyIdentityMatrix;
	return *this;
}


inline EzMatrix3& EzMatrix3::setRotationDegrees( const EzPoint3& rotation )
{
	return setRotationRadians( rotation * DEGTORAD );
}

inline EzMatrix3& EzMatrix3::setRotationRadians( const EzPoint3& rotation )
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

	M[3] = (float)( srsp*cy-cr*sy );
	M[4] = (float)( srsp*sy+cr*cy );
	M[5] = (float)( sr*cp );

	M[6] = (float)( crsp*cy+sr*sy );
	M[7] = (float)( crsp*sy-sr*cy );
	M[8] = (float)( cr*cp );
	definitelyIdentityMatrix=false;
	return *this;
}

inline EzPoint3 EzMatrix3::getRotationDegrees() const
{
	const EzMatrix3 &mat = *this;

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

	return EzPoint3((float)X,(float)Y,(float)Z);
}

void EzMatrix3::fromQuat( const EzQuaternion & q )
{
	M[0] = 1.0f - 2.0f*q.Y*q.Y - 2.0f*q.Z*q.Z;
	M[3] = 2.0f*q.X*q.Y + 2.0f*q.Z*q.W;
	M[6] = 2.0f*q.X*q.Z - 2.0f*q.Y*q.W;
	M[1] = 2.0f*q.X*q.Y - 2.0f*q.Z*q.W;
	M[4] = 1.0f - 2.0f*q.X*q.X - 2.0f*q.Z*q.Z;
	M[7] = 2.0f*q.Z*q.Y + 2.0f*q.X*q.W;
	M[2] = 2.0f*q.X*q.Z + 2.0f*q.Y*q.W;
	M[5] = 2.0f*q.Z*q.Y - 2.0f*q.X*q.W;
	M[8] = 1.0f - 2.0f*q.X*q.X - 2.0f*q.Y*q.Y;

	definitelyIdentityMatrix=false;
}



EzQuaternion EzMatrix3::toQuat() const
{
	EzQuaternion  q;
	const EzMatrix3 &m = *this;
	
	const float diag = m(0,0) + m(1,1) + m(2,2) + 1;

	if( diag > 0.0f )
	{
		const float scale = sqrtf(diag) * 2.0f; // get scale from diagonal

		// TODO: speed this up
		q.X = ( m(2,1) - m(1,2)) / scale;
		q.Y = ( m(0,2) - m(2,0)) / scale;
		q.Z = ( m(1,0) - m(0,1)) / scale;
		q.W = 0.25f * scale;
	}
	else
	{
		if ( m(0,0) > m(1,1) && m(0,0) > m(2,2))
		{
			// 1st element of diag is greatest value
			// find scale according to 1st element, and double it
			const float scale = sqrtf( 1.0f + m(0,0) - m(1,1) - m(2,2)) * 2.0f;

			// TODO: speed this up
			q.X = 0.25f * scale;
			q.Y = (m(0,1) + m(1,0)) / scale;
			q.Z = (m(2,0) + m(0,2)) / scale;
			q.W = (m(2,1) - m(1,2)) / scale;
		}
		else if ( m(1,1) > m(2,2))
		{
			// 2nd element of diag is greatest value
			// find scale according to 2nd element, and double it
			const float scale = sqrtf( 1.0f + m(1,1) - m(0,0) - m(2,2)) * 2.0f;

			// TODO: speed this up
			q.X = (m(0,1) + m(1,0) ) / scale;
			q.Y = 0.25f * scale;
			q.Z = (m(1,2) + m(2,1) ) / scale;
			q.W = (m(0,2) - m(2,0) ) / scale;
		}
		else
		{
			// 3rd element of diag is greatest value
			// find scale according to 3rd element, and double it
			const float scale = sqrtf( 1.0f + m(2,2) - m(0,0) - m(1,1)) * 2.0f;

			// TODO: speed this up
			q.X = (m(0,2) + m(2,0)) / scale;
			q.Y = (m(1,2) + m(2,1)) / scale;
			q.Z = 0.25f * scale;
			q.W = (m(1,0) - m(0,1)) / scale;
		}
	}

	return q;
}

// Use this operator to compute Transpose(VECTOR)*MATRIX.
inline EzPoint3 operator*(const EzPoint3 &v, const EzMatrix3& mat) 
{ 
	return EzPoint3(
		v.X * mat[0] + v.Y * mat[3] + v.Z * mat[6], 
		v.X * mat[1] + v.Y * mat[4] + v.Z * mat[7], 
		v.X * mat[2] + v.Y * mat[5] + v.Z * mat[8] 
	);
}



