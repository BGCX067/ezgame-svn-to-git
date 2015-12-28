#include "EzVariant.h"


EzVariant::EzVariant(void)
: m_eDataType(EDT_NONE)
{
}

EzVariant::EzVariant( EDataType eType)
: m_eDataType(EDT_NONE)
{
	changeType( eType );

}

EzVariant::EzVariant(const EzVariant& other)
: m_eDataType(EDT_NONE)
{
	operator=( other );

}

EzVariant::~EzVariant(void)
{
	clear();
}


void EzVariant::operator=( const EzVariant & other )
{
	if ( m_eDataType != other.m_eDataType )
		changeType( other.m_eDataType );

	switch ( m_eDataType )
	{
	case EDT_COLORF:
		*static_cast<EzColor*>( m_val.data ) = *static_cast<EzColor*>( other.m_val.data );
		return;

	case EDT_VECTOR2:
		*static_cast<EzPoint2*>( m_val.data ) = *static_cast<EzPoint2*>( other.m_val.data );
		return;

	case EDT_VECTOR3:
		*static_cast<EzPoint3*>( m_val.data ) = *static_cast<EzPoint3*>( other.m_val.data );
		return;

	case EDT_MATRIX3:
		*static_cast<EzMatrix3*>( m_val.data ) = *static_cast<EzMatrix3*>( other.m_val.data );
		return;

	case EDT_MATRIX4:
		*static_cast<EzMatrix4*>( m_val.data ) = *static_cast<EzMatrix4*>( other.m_val.data );
		return;

	case EDT_QUATERNTION:
		*static_cast<EzQuaternion*>( m_val.data ) = *static_cast<EzQuaternion*>( other.m_val.data );
		return;

	default:
		m_val = other.m_val;
		return;
	}
}

void EzVariant::clear()
{
	switch ( m_eDataType )
	{
	case EDT_COLORF:
		EzDelete static_cast<EzColor*>( m_val.data );
		return;

	case EDT_VECTOR2:
		EzDelete static_cast<EzPoint2*>( m_val.data );
		return;

	case EDT_VECTOR3:
		EzDelete static_cast<EzPoint3*>( m_val.data );
		return;

	case EDT_MATRIX3:
		EzDelete static_cast<EzMatrix3*>( m_val.data );
		return;

	case EDT_MATRIX4:
		EzDelete static_cast<EzMatrix4*>( m_val.data );
		return;

	case EDT_QUATERNTION:
		EzDelete static_cast<EzQuaternion*>( m_val.data );
		return;
	}

	m_eDataType = EDT_NONE;
	m_val.Double = 0;

}


void EzVariant::changeType( EDataType eType )
{
	if ( m_eDataType == eType )
		return;

	if ( m_eDataType != EDT_NONE )
		clear();

	switch ( (m_eDataType = eType) )
	{
	case EDT_COLORF:
		m_val.data = EzNew EzColor;
		return;

	case EDT_VECTOR2:
		m_val.data = EzNew EzPoint2;
		return;

	case EDT_VECTOR3:
		m_val.data = EzNew EzPoint3;
		return;

	case EDT_MATRIX3:
		m_val.data = EzNew EzMatrix3;
		return;

	case EDT_MATRIX4:
		m_val.data = EzNew EzMatrix4;
		return;

	case EDT_QUATERNTION:
		m_val.data = EzNew EzQuaternion;
		return;
	default:
		m_val.Double = 0;
		break;
	}

}

const unsigned short EzVariant::size()
{
	switch(m_eDataType)
	{
	case EDT_BOOL:
		return sizeof(bool);
	case EDT_BYTE:
		return sizeof(unsigned char);

	case EDT_SHORT:
		return sizeof(unsigned short);
	case EDT_UINT:
		return sizeof(unsigned int);
	case EDT_INT:
		return sizeof(int);
	case EDT_FLOAT:
		return sizeof(float);
	case EDT_DOUBLE:
		return sizeof(double);
	case EDT_COLORF:
		return sizeof(EzColor);
	case EDT_VECTOR2:
		return sizeof(EzPoint2);
	case EDT_VECTOR3:
		return sizeof(EzPoint3);
	case EDT_MATRIX3:
		return sizeof(EzMatrix3);
	case EDT_MATRIX4:
		return sizeof(EzMatrix4);
	case EDT_QUATERNTION:
		return sizeof(EzQuaternion);
	}

	return 0;
}

