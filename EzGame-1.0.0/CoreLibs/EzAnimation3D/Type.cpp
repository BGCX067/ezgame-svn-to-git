#include "Type.h"

namespace irr
{
namespace jay
{


Type::Type (const core::stringc & type_name, const Type * par_type ) : name(type_name), base_type(par_type) {} 

Type::~Type() {}

bool Type::operator<( const Type & compare_to ) const {
	return (this < &compare_to);
}

bool Type::IsSameType( const Type & compare_to ) const {
	return &compare_to == this;
}

bool Type::IsDerivedType( const Type & compare_to ) const {

	const Type * search = this;
	while ( search != NULL ) {
		if ( search == &compare_to ) {
			return true;
		}
		search = search->base_type;
	}
	return false;
}

core::stringc Type::GetTypeName() const {
	return name;
}


}
}