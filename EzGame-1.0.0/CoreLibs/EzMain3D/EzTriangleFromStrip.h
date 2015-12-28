#pragma once

#include "../EzCore/EzCore.h"



class EzTriangleFromStrip
{
public:
	EzTriangleFromStrip(void);
	~EzTriangleFromStrip(void);
	bool setNvTriangles( const EzArray<EzTriangle16> & in );
	bool setTSTriangles( const EzArray<EzTriangle16> & in );

	const unsigned short& getStripCount() const
	{ 
		return m_usNumStrips; 
	}
	const unsigned short& getStripLength(unsigned short usStripIdx)  const
	{
		return m_aStripLengths[usStripIdx]; 
	}
	const unsigned short& getStrip(unsigned short usStripIdx, unsigned short usIdx)  const
	{ 
		return m_aStrips[usStripIdx][usIdx]; 
	}
	
	const unsigned short& getTrianglesCount() const
	{ 
		return m_usNumTriangles; 
	}

	const unsigned short getStripLengthSum() const
	{
		return m_usNumTriangles + 2 * m_usNumStrips;
	}


protected:
	void setStrip( int index, const EzArray<unsigned short> & in );
	void setStripCount(unsigned short usIdx);
	unsigned short calcTriangleCount() const;

	/*! Number of triangles. */
	unsigned short m_usNumTriangles;
	/*! Number of OpenGL triangle strips that are present. */
	mutable unsigned short m_usNumStrips;
	/*! The number of points in each triangle strip. */
	EzArray<unsigned short> m_aStripLengths;

	/*!
	 * The points in the Triangle strips.  Size is the sum of all entries in Strip
	 * Lengths.
	 */
	EzArray< EzArray<unsigned short> > m_aStrips;

	// temp;
	EzArray<EzTriangle16>	m_aTriangles;
};
