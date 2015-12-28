#pragma once


#include "irrString.h"


namespace irr
{
namespace jay
{


class Type 
{
public:
	Type(const core::stringc & type_name, const Type * par_type );
	~Type();

	core::stringc GetTypeName() const;

	bool IsSameType ( const Type & compare_to ) const;
	bool IsDerivedType ( const Type & compare_to ) const;
	bool operator<( const Type & compare_to ) const;
private:
	core::stringc name;
	const Type * base_type;
};

}
}