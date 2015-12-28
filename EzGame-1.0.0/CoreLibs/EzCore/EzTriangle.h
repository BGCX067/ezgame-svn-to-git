#ifndef __TRIANGLE_H_INCLUDED__
#define __TRIANGLE_H_INCLUDED__

#include "EzMemObject.h"

template <class T>
class EZCORE_ENTRY EzTriangle : public EzMemObject
{

public:
	T v1; /*!< The index of the first vertex. */ 
	T v2; /*!< The index of the second vertex. */ 
	T v3; /*!< The index of the third vertex. */ 

	/*! Default constructor */
	EzTriangle() {}

	EzTriangle(T v1, T v2, T v3) {
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
	}

	void set(T v1, T v2, T v3) {
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
	}

	T & operator[](int n)
	{
		switch (n) 
		{
			case 0: return v1; break;
			case 1: return v2; break;
			case 2: return v3; break;
		};

		return v1;
	}
	const T & operator[](int n) const 
	{
		switch (n) 
		{
			case 0: return v1; break;
			case 1: return v2; break;
			case 2: return v3; break;
		};

		return v1;
	}

	const EzTriangle<T>& operator=(const EzTriangle<T>& other)
	{
		v1 = other.v1;
		v2 = other.v2;
		v3 = other.v3;
		return *this;
	}
};


typedef EzTriangle<unsigned short> EzTriangle16;
typedef EzTriangle<unsigned int> EzTriangle32;




#endif