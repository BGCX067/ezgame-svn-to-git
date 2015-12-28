#include "EzStringPalette.h"

EzStringPalette::EzStringPalette(void)
{
}

EzStringPalette::~EzStringPalette(void)
{
}

const char* EzStringPalette::getPaletteString() const
{
	return palette.c_str();

}

void EzStringPalette::setPaletteString( const char* n )
{
	palette = n;
}

const char* EzStringPalette::getSubStr( short offset ) const
{
	static EzStringc out;
	out = "";
	
	// -1 is a null offset
	if ( offset == -1 ) 
	{
		return out.c_str();
	}

	for ( unsigned int i = offset; i < palette.size(); ++i ) 
	{
		if ( palette[i] == '\0' ) 
		{
			break;
		}

		out.append(palette[i]);
	}

	return out.c_str();

}

unsigned int EzStringPalette::addSubStr( const char* n )
{
	//Search for the EzStringc
   //  When searching for strings also search for ending null.
	unsigned int offset = (unsigned int)palette.find(n);
	
	//If EzStringc was not found, append it
	if ( offset == 0xFFFFFFFF )
	{
		offset = (unsigned int)palette.size();
		palette.append( n );
		palette.append('\0' );

	}

	//Return the offset where the EzStringc was found or appended
	return offset;

}
