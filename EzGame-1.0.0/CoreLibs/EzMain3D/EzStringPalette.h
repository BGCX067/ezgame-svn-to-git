#pragma once

#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzCore.h"



class EzStringPalette : public EzRefObject
{
public:
	EzStringPalette(void);
	~EzStringPalette(void);

	const char* getPaletteString() const;
	void setPaletteString( const char* n );

	const char* getSubStr( short offset ) const;

	unsigned int addSubStr(const char* n);


protected:
	/*! A bunch of 0x00 seperated strings. */
	EzStringc palette;
};

