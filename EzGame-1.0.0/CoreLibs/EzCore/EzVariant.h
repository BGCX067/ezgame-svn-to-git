#pragma once
#include "EzCoreLibType.h"
#include "EzCore.h"


class EZCORE_ENTRY EzVariant
{
public:
	enum EDataType
	{
		// all count types should come between tBool and tUInt
		EDT_NONE,
		EDT_BOOL,
		EDT_BYTE,
		EDT_SHORT,
		EDT_UINT,
		EDT_INT,
		EDT_FLOAT,
		EDT_DOUBLE,
		EDT_COLORF,
		EDT_VECTOR2,
		EDT_VECTOR3,
		EDT_MATRIX3,
		EDT_MATRIX4,
		EDT_QUATERNTION
	};

	EzVariant(void);
	EzVariant( EDataType eType);
	EzVariant(const EzVariant& other);
	~EzVariant(void);


	bool isBool() const { return m_eDataType == EDT_BOOL; }
	bool isByte() const { return m_eDataType == EDT_BYTE; }
	bool isInt() const { return m_eDataType == EDT_INT; }
	bool isUInt() const { return m_eDataType == EDT_UINT; }
	bool isShort() const { return m_eDataType == EDT_SHORT; }
	bool isFloat() const { return m_eDataType == EDT_FLOAT; }
	bool isDouble() const { return m_eDataType == EDT_DOUBLE; }
	bool isColorf() const { return m_eDataType == EDT_COLORF; }
	bool isMatrix3() const { return m_eDataType == EDT_MATRIX3; }
	bool isMatrix4() const { return m_eDataType == EDT_MATRIX4; }
	bool isQuat() const { return m_eDataType == EDT_QUATERNTION; }
	bool isVector3() const { return m_eDataType == EDT_VECTOR3; }
	bool isVector2() const { return m_eDataType == EDT_VECTOR2; }
	bool isValid() const { return m_eDataType != EDT_NONE; }

	void operator=( const EzVariant & other );

	template <typename T> T get() const;
	template <typename T> void set( const T & x );

	void clear();
	void changeType( EDataType eType );
	const unsigned short size();

	const unsigned char* EzVariant::getPointer() const;

	

protected:


	union Value 
	{
		bool    			Boolean;
		unsigned char		Byte;
		unsigned short		UShort;
		unsigned int		UInt;
		int					Int;
		float				Float;
		double				Double;
		void *				data;
	};

	Value		m_val;
	EDataType	m_eDataType;
};


template <> inline void EzVariant::set( const bool & b ) 
{ 
	changeType(EDT_BOOL);
	m_val.Boolean = b;
}

template <> inline bool EzVariant::get() const
{
	if(isBool())
	{
		return m_val.Boolean;
	}
	else
	{
		return false;
	}
}

template <> inline void EzVariant::set( const unsigned char & data ) 
{ 
	changeType(EDT_BYTE);
	m_val.Byte = data;

}

template <> inline unsigned char EzVariant::get() const
{
	if(isByte())
	{
		return m_val.Byte;
	}
	else
	{
		return 0;
	}
}


template <> inline void EzVariant::set( const unsigned short & data ) 
{ 
	changeType(EDT_SHORT);
	m_val.UShort = data;
}

template <> inline unsigned short EzVariant::get() const
{
	if(isShort())
	{
		return m_val.UShort;
	}
	else
	{
		return 0;
	}
}

template <> inline void EzVariant::set( const unsigned int & data ) 
{ 
	changeType(EDT_UINT);
	m_val.UInt = data;
}

template <> inline unsigned int EzVariant::get() const
{
	if(isUInt())
	{
		return m_val.UInt;
	}
	else
	{
		return 0;
	}
}


template <> inline void EzVariant::set( const int & data ) 
{ 
	changeType(EDT_INT);
	m_val.Int = data;
}

template <> inline int EzVariant::get() const
{
	if(isInt())
	{
		return m_val.Int;
	}
	else
	{
		return 0;
	}
}

template <> inline void EzVariant::set( const float & data ) 
{ 
	changeType(EDT_FLOAT);
	m_val.Float = data;
}

template <> inline float EzVariant::get() const
{
	if(isFloat())
	{
		return m_val.Float;
	}
	else
	{
		return 0;
	}
}

template <> inline void EzVariant::set( const double & data ) 
{ 
	changeType(EDT_DOUBLE);
	m_val.Double = data;
}

template <> inline double EzVariant::get() const
{
	if(isDouble())
	{
		return m_val.Double;
	}
	else
	{
		return 0;
	}
}

template <> inline void EzVariant::set( const EzColor & data ) 
{ 
	changeType(EDT_COLORF);
	*static_cast<EzColor*>(m_val.data) = data;
}

template <> inline EzColor EzVariant::get() const
{
	if(isColorf())
	{
		return *static_cast<EzColor*>(m_val.data);
	}
	else
	{
		return EzColor();
	}
}

template <> inline void EzVariant::set( const EzMatrix3 & data ) 
{ 
	changeType(EDT_MATRIX3);
	*static_cast<EzMatrix3*>(m_val.data) = data;
}

template <> inline EzMatrix3 EzVariant::get() const
{
	if(isMatrix3())
	{
		return *static_cast<EzMatrix3*>(m_val.data);
	}
	else
	{
		return EzMatrix3();
	}
}

template <> inline void EzVariant::set( const EzMatrix4 & data ) 
{ 
	changeType(EDT_MATRIX4);
	*static_cast<EzMatrix4*>(m_val.data) = data;
}

template <> inline EzMatrix4 EzVariant::get() const
{
	if(isMatrix4())
	{
		return *static_cast<EzMatrix4*>(m_val.data);
	}
	else
	{
		return EzMatrix4();
	}
}

template <> inline void EzVariant::set( const EzQuaternion & data ) 
{ 
	changeType(EDT_QUATERNTION);
	*static_cast<EzQuaternion*>(m_val.data) = data;
}

template <> inline EzQuaternion EzVariant::get() const
{
	if(isQuat())
	{
		return *static_cast<EzQuaternion*>(m_val.data);
	}
	else
	{
		return EzQuaternion();
	}
}

template <> inline void EzVariant::set( const EzPoint2 & data ) 
{ 
	changeType(EDT_VECTOR2);
	*static_cast<EzPoint2*>(m_val.data) = data;
}

template <> inline EzPoint2 EzVariant::get() const
{
	if(isVector2())
	{
		return *static_cast<EzPoint2*>(m_val.data);
	}
	else
	{
		return EzPoint2();
	}
}


template <> inline void EzVariant::set( const EzPoint3 & data ) 
{ 
	changeType(EDT_VECTOR3);
	*static_cast<EzPoint3*>(m_val.data) = data;
}

template <> inline EzPoint3 EzVariant::get() const
{
	if(isVector3())
	{
		return *static_cast<EzPoint3*>(m_val.data);
	}
	else
	{
		return EzPoint3();
	}
}


inline const unsigned char* EzVariant::getPointer() const
{
	return (unsigned char*) m_val.data;
}

