#include "NvTriStrip/NvTriStrip.h"
#include "TriStripper/tri_stripper.h"
#include "EzTriangleFromStrip.h"

using namespace NvTriStrip;
using namespace triangle_stripper;

// Helper methods
typedef EzArray<unsigned short> TriStrip;
typedef	EzList<TriStrip> TriStrips;

EzTriangleFromStrip::EzTriangleFromStrip(void)
{
}

EzTriangleFromStrip::~EzTriangleFromStrip(void)
{
	m_aStrips.clear();
	m_aStripLengths.clear();
}


void EzTriangleFromStrip::setStripCount(unsigned short usIdx)
{
	m_aStrips.set_used( usIdx );
	m_aStripLengths.set_used( usIdx );

	m_usNumStrips = usIdx;
	m_usNumTriangles = 0;

}

void EzTriangleFromStrip::setStrip( int index, const EzArray<unsigned short> & in )
{
	m_aStrips[index] = in;
	m_aStripLengths[index] = in.size();
}

bool EzTriangleFromStrip::setTSTriangles( const EzArray<EzTriangle16> & in )
{
	if ( in.size() > 65535 || in.size() < 0 ) 
	{
		return false;
	}

	m_aStrips.clear();
	m_usNumTriangles = 0;

	TriStrips strips;
	triangle_stripper::indices idcs(in.size()*3);

	unsigned int i, j;
   
	for (i=0; i<in.size(); i++)
	{
		idcs[i * 3 + 0] = in[i][0];
		idcs[i * 3 + 1] = in[i][1];
		idcs[i * 3 + 2] = in[i][2];
	}

   tri_stripper stripper(idcs);

   primitive_vector groups;
   stripper.Strip(&groups);

   // triangles left over
   EzArray<EzTriangle16> stris;

   for (i=0; i<groups.size(); i++)
   {
      if (groups[i].Type == TRIANGLE_STRIP)
      {			
         strips.push_back( TriStrip( (unsigned short)(groups[i].Indices.size()) ) );
		 TriStrip &strip = (*strips.getLast());

         for (j=0; j<groups[i].Indices.size(); j++)
            strip[j] = groups[i].Indices[j];
      } else
      {
         unsigned int size = stris.size();
		 stris.set_used(size + groups[i].Indices.size()/3);
		 stris.reallocate(size + groups[i].Indices.size()/3);

         for (j=(size>0)?(size-1):0; j<stris.size(); j++)
         {
            stris[j][0] = groups[i].Indices[j*3+0];
            stris[j][1] = groups[i].Indices[j*3+1];
            stris[j][2] = groups[i].Indices[j*3+2];
         }
      }
   }

   if (stris.size())
   {
      // stitch em
      TriStrip strip;
      if (strips.getSize() > 0)
      {
		 strip.push_back((*strips.getLast())[strips.getLast()->size()-1]);
         strip.push_back(stris[0][0]);
      }
      for (i=0; i<stris.size(); i++)
      {
         if (i > 0)
         {
            strip.push_back(stris[i][0]);
            strip.push_back(stris[i][0]);
         }

         strip.push_back(stris[i][0]);
         strip.push_back(stris[i][1]);
         strip.push_back(stris[i][2]);
         if (i < stris.size()-1)
            strip.push_back(stris[i][2]);
      }
      strips.push_back(strip);
   }

   if (strips.getSize() > 0)
   {
	  setStripCount( int(strips.getSize()) );

      int i = 0;
	  TriStrips::ConstIterator it;
      for (it=strips.begin(); it!=strips.end(); ++it)
         setStrip(i++, *it);
   }

   //Recalculate Triangle Count
   m_usNumTriangles = calcTriangleCount();

   return true;

}

bool EzTriangleFromStrip::setNvTriangles( const EzArray<EzTriangle16> & in )
{
	if ( in.size() > 65535 || in.size() < 0 ) 
	{
		return false;
	}
   
	m_aStrips.clear();
	m_usNumTriangles = 0;

	unsigned short *data = new unsigned short[in.size() * 3 * 2];
	for (unsigned int i=0; i< in.size(); i++) 
	{
		data[i * 3 + 0] = in[i][0];
		data[i * 3 + 1] = in[i][1];
		data[i * 3 + 2] = in[i][2];
	}

	PrimitiveGroup * groups = 0;
	unsigned short numGroups = 0;

	// GF 3+
	SetCacheSize(CACHESIZE_GEFORCE3);
	// don't generate hundreds of strips
	SetStitchStrips(true);
	GenerateStrips(data, int(in.size()*3), &groups, &numGroups);

	delete [] data;

	if (!groups)
		  return false;

	setStripCount(numGroups);
	for (unsigned short g=0; g<numGroups; g++) 
	{
		if (groups[g].type == PT_STRIP)
		{
			EzArray<unsigned short> strip;
			strip.set_used(groups[g].numIndices);
			
			for (unsigned short s=0; s<groups[g].numIndices; s++)
				strip[s] = groups[g].indices[s];

			setStrip(g, strip);
      }
   }

   delete [] groups;

   //Recalculate Triangle Count
   m_usNumTriangles = calcTriangleCount();

   return true;

}

unsigned short EzTriangleFromStrip::calcTriangleCount() const
{

	//Calculate number of triangles
	//Sum of length of each strip - 2
	unsigned short numTriangles = 0;
	for ( unsigned int i = 0; i < m_aStrips.size(); ++i ) 
	{
		numTriangles += (unsigned short)(m_aStrips[i].size() - 2);
	}

	return numTriangles;
}
