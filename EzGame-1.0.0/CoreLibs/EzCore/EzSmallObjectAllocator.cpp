#include "EzSmallObjectAllocator.h"
#include "EzDebugHelper.h"
#include "EzMemoryDefines.h"
#include <algorithm>

//---------------------------------------------------------------------------
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above 
//     copyright notice appear in all copies and that both that copyright 
//     notice and this permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
//---------------------------------------------------------------------------


void EzFixedAllocator::SChunk::init(size_t stBlockSize, 
    unsigned char ucBlocks)
{
    EZASSERT(stBlockSize > 0);
    EZASSERT(ucBlocks > 0);
    // Overflow check
    EZASSERT((stBlockSize * ucBlocks) / stBlockSize == ucBlocks);
    
    m_pucData = (unsigned char*)EzExternalAlignedMalloc(
        sizeof(unsigned char)*stBlockSize * ucBlocks, EZ_MEM_ALIGNMENT_DEFAULT);
    reset(stBlockSize, ucBlocks);
}

//---------------------------------------------------------------------------
// EzFixedAllocator::SChunk::Reset
// Clears an already allocated chunk
//---------------------------------------------------------------------------

void EzFixedAllocator::SChunk::reset(size_t stBlockSize,
    unsigned char ucBlocks)
{
    EZASSERT(stBlockSize > 0);
    EZASSERT(ucBlocks > 0);
    // Overflow check
    EZASSERT((stBlockSize * ucBlocks) / stBlockSize == ucBlocks);

    m_ucFirstAvailableBlock = 0;
    m_ucBlocksAvailable = ucBlocks;

    unsigned char i = 0;
    unsigned char* p = m_pucData;
    for (; i != ucBlocks; p += stBlockSize)
    {
        *p = ++i;
    }
}

//---------------------------------------------------------------------------
// EzFixedAllocator::SChunk::Release
// Releases the data managed by a chunk
//---------------------------------------------------------------------------

void EzFixedAllocator::SChunk::release()
{
    EzExternalAlignedFree(m_pucData);
}

//---------------------------------------------------------------------------
// EzFixedAllocator::SChunk::Allocate
// Allocates a block from a chunk
//---------------------------------------------------------------------------

void* EzFixedAllocator::SChunk::allocate(size_t stBlockSize)
{
    if (!m_ucBlocksAvailable) 
        return 0;
    
    EZASSERT((m_ucFirstAvailableBlock * stBlockSize) / stBlockSize == 
        m_ucFirstAvailableBlock);

    unsigned char* pucResult =
        m_pucData + (m_ucFirstAvailableBlock * stBlockSize);
    m_ucFirstAvailableBlock = *pucResult;
    --m_ucBlocksAvailable;
    
    return pucResult;
}

//---------------------------------------------------------------------------
// EzFixedAllocator::SChunk::Deallocate
// Dellocates a block from a chunk
//---------------------------------------------------------------------------

void EzFixedAllocator::SChunk::deallocate(void* p, size_t stBlockSize)
{
    EZASSERT(p >= m_pucData);

    unsigned char* toRelease = static_cast<unsigned char*>(p);
    // Alignment check
    EZASSERT((toRelease - m_pucData) % stBlockSize == 0);

    *toRelease = m_ucFirstAvailableBlock;
    m_ucFirstAvailableBlock = static_cast<unsigned char>(
        (toRelease - m_pucData) / stBlockSize);
    // Truncation check
    EZASSERT(m_ucFirstAvailableBlock == (toRelease - m_pucData) / stBlockSize);

    ++m_ucBlocksAvailable;
}

//---------------------------------------------------------------------------
// EzFixedAllocator::SChunk::HasAvailable
//---------------------------------------------------------------------------
bool EzFixedAllocator::SChunk::hasAvailable(unsigned char ucNumBlocks) const
{
    return m_ucBlocksAvailable == ucNumBlocks;
}

//---------------------------------------------------------------------------
// EzFixedAllocator::SChunk::HasAvailable
//---------------------------------------------------------------------------
bool EzFixedAllocator::SChunk::hasBlock(unsigned char * p, 
    size_t stChunkLength) const
{
    return ( m_pucData <= p ) && ( p < m_pucData + stChunkLength ); 
}

//---------------------------------------------------------------------------
// EzFixedAllocator::SChunk::HasAvailable
//---------------------------------------------------------------------------
bool EzFixedAllocator::SChunk::isFilled( void ) const
{
    return ( 0 == m_ucBlocksAvailable );
}

//---------------------------------------------------------------------------
// EzFixedAllocator::EzFixedAllocator
//---------------------------------------------------------------------------

EzFixedAllocator::EzFixedAllocator()
    : m_stBlockSize(0)
    , m_pChunks(0)
    , m_stNumChunks(0)
    , m_stMaxNumChunks(0)
    , m_pAllocChunk(0)
    , m_pDeallocChunk(0)
    , m_pEmptyChunk(0)
	, m_ucNumBlocks(0)
{
}

//---------------------------------------------------------------------------
// Creates a EzFixedAllocator object of a fixed block size
//---------------------------------------------------------------------------
void EzFixedAllocator::init(size_t stBlockSize)
{
    m_stNumChunks  = 0;
    m_stMaxNumChunks = 0;
    m_pChunks = NULL;
    m_pAllocChunk = NULL;
    m_stBlockSize = stBlockSize;
    size_t stNumBlocks = DEFAULT_CHUNK_SIZE / stBlockSize;

    if (stNumBlocks > UCHAR_MAX) 
        stNumBlocks = UCHAR_MAX;
    else if (stNumBlocks == 0) 
        stNumBlocks = 8 * stBlockSize;
    
    m_ucNumBlocks = static_cast<unsigned char>(stNumBlocks);
    EZASSERT(m_ucNumBlocks == stNumBlocks);
}

//---------------------------------------------------------------------------
// EzFixedAllocator::~EzFixedAllocator
//---------------------------------------------------------------------------

EzFixedAllocator::~EzFixedAllocator()
{  
    for (size_t i = 0; i < m_stNumChunks; ++i)
    {
       m_pChunks[i].release();
    }

    EzExternalFree(m_pChunks);
}

//---------------------------------------------------------------------------
// EzFixedAllocator::Push_Back
// Adds a chunk to the end of the chunks array
//---------------------------------------------------------------------------
void EzFixedAllocator::push_Back(SChunk& Chunk)
{
    size_t stCount = m_stNumChunks;
    reserve(m_stNumChunks+1);
    m_pChunks[stCount] = Chunk;
    m_stNumChunks++;
}

//---------------------------------------------------------------------------
// EzFixedAllocator::Push_Back
// Adds a chunk to the end of the chunks array
//---------------------------------------------------------------------------
void EzFixedAllocator::pop_Back()
{
    --m_stNumChunks;
}

//---------------------------------------------------------------------------
// EzFixedAllocator::Reserve
// Guarantees space for a certain number of chunks
//---------------------------------------------------------------------------
void EzFixedAllocator::reserve(size_t stNewSize)
{
    if (stNewSize > m_stMaxNumChunks)
    {
        m_pChunks = (SChunk*)EzExternalRealloc(m_pChunks, 
            stNewSize*sizeof(SChunk));
        m_stMaxNumChunks = stNewSize;
    }
}

//---------------------------------------------------------------------------
// EzFixedAllocator::Allocate
// Allocates a block of fixed size
//---------------------------------------------------------------------------

void* EzFixedAllocator::allocate()
{
    m_CriticalSection.lock();
    // Prove that the empty chunk points to either nothing or
    // a chunk with no elements allocated.
    EZASSERT( (NULL == m_pEmptyChunk) || 
            (m_pEmptyChunk->hasAvailable(m_ucNumBlocks)));

    if (m_pAllocChunk && m_pAllocChunk->isFilled())
        m_pAllocChunk = NULL;

    // Recycle the empty chunk if possible
    if (NULL != m_pEmptyChunk)
    {
        m_pAllocChunk = m_pEmptyChunk;
        m_pEmptyChunk = NULL;
    }

    if (m_pAllocChunk == 0)
    {
        for (size_t i = 0; i < m_stNumChunks; ++i)
        {
            if (!m_pChunks[i].isFilled())
            {
                m_pAllocChunk = &m_pChunks[i];
                break;
            }
        }

        // If no alloc chunk has space,
        // add an alloc chunk
        if (NULL == m_pAllocChunk)
        {
            // Initialize
            reserve(m_stNumChunks + 1);
            SChunk newChunk;
            newChunk.init(m_stBlockSize, m_ucNumBlocks);
            push_Back(newChunk);
            m_pAllocChunk = &m_pChunks[m_stNumChunks - 1];
            m_pDeallocChunk = &m_pChunks[0];
        }
    }
    EZASSERT(m_pAllocChunk != 0);
    EZASSERT(!m_pAllocChunk->isFilled() );

    // Prove that the empty chunk points to either nothing or
    // a chunk with no elements allocated.
    EZASSERT( (NULL == m_pEmptyChunk) || 
            (m_pEmptyChunk->hasAvailable(m_ucNumBlocks)));

    
    void* pvMem = m_pAllocChunk->allocate(m_stBlockSize);
    m_CriticalSection.unlock();
    return pvMem;
}

//---------------------------------------------------------------------------
// EzFixedAllocator::Deallocate
// Deallocates a block previously allocated with Allocate
// (undefined behavior if called with the wrong pointer)
//---------------------------------------------------------------------------

void EzFixedAllocator::deallocate(void* p)
{
    m_CriticalSection.lock();
    EZASSERT(m_stNumChunks != 0);
    EZASSERT(&m_pChunks[0] <= m_pDeallocChunk);
    EZASSERT(&m_pChunks[m_stNumChunks - 1] >= m_pDeallocChunk);
    
    m_pDeallocChunk  = vicinityFind(p);
    EZASSERT(m_pDeallocChunk);

    doDeallocate(p);
    m_CriticalSection.unlock();
}

//---------------------------------------------------------------------------
// EzFixedAllocator::VicinityFind (internal)
// Finds the chunk corresponding to a pointer, using an efficient search
//---------------------------------------------------------------------------

EzFixedAllocator::SChunk* EzFixedAllocator::vicinityFind(void* p)
{
    EZASSERT(m_stNumChunks != 0);
    EZASSERT(m_pDeallocChunk);

    const size_t stChunkLength = m_ucNumBlocks * m_stBlockSize;

    SChunk* lo = m_pDeallocChunk;
    SChunk* hi = m_pDeallocChunk + 1;
    SChunk* loBound = &m_pChunks[0];
    SChunk* hiBound = &m_pChunks[m_stNumChunks - 1] + 1;

    if (hi == hiBound)
        hi = 0;

    for (;;)
    {
        if (lo)
        {
            if ( lo->hasBlock( (unsigned char*)p, stChunkLength ) )
                return lo;

            if ( lo == loBound )
            {
                lo = NULL;
                if ( NULL == hi ) 
                    break;
            }
            else 
                --lo;

        }
        
        if (hi)
        {
            if ( hi->hasBlock( (unsigned char*)p, stChunkLength ) ) 
                return hi;
            if ( ++hi == hiBound )
            {
                hi = NULL;
                if ( NULL == lo )
                    break;
            }

        }
    }
    
    EZASSERT(!"Could not find pointer p in EzFixedAllocator::VicinityFind()");
    return 0;
}

//---------------------------------------------------------------------------
// EzFixedAllocator::DoDeallocate (internal)
// Performs deallocation. Assumes m_pkDeallocChunk points to the correct chunk
//---------------------------------------------------------------------------

void EzFixedAllocator::doDeallocate(void* p)
{
    EZASSERT(m_pDeallocChunk->m_pucData <= p);
    EZASSERT( m_pDeallocChunk->hasBlock(static_cast<unsigned char *>(p),
        m_ucNumBlocks * m_stBlockSize));

    // prove either m_pkEmptyChunk points nowhere, or points to a truly empty 
    // SChunk.
    EZASSERT((NULL == m_pEmptyChunk) || 
           (m_pEmptyChunk->hasAvailable(m_ucNumBlocks)));

    // call into the chunk, will adjust the inner EzList but won't release memory
    m_pDeallocChunk->deallocate(p, m_stBlockSize);

    if (m_pDeallocChunk->hasAvailable(m_ucNumBlocks))
    {
        EZASSERT(m_pEmptyChunk != m_pDeallocChunk);

        // m_pkDeallocChunk is empty, but a SChunk is only released if there 
        // are 2 empty chunks.  Since m_pkEmptyChunk may only point to a 
        // previously cleared SChunk, if it points to something else 
        // besides m_pkDeallocChunk, then FixedAllocator currently has 2 empty
        // Chunks.
        if (NULL != m_pEmptyChunk)
        {
            // If last SChunk is empty, just change what m_pkDeallocChunk
            // points to, and release the last.  Otherwise, swap an empty
            // SChunk with the last, and then release it.
            SChunk* pLastChunk = &m_pChunks[m_stNumChunks - 1];
            
            if (pLastChunk == m_pDeallocChunk)
                m_pDeallocChunk = m_pEmptyChunk;
            else if (pLastChunk != m_pEmptyChunk )
                std::swap( *m_pEmptyChunk, *pLastChunk);

            EZASSERT(pLastChunk->hasAvailable(m_ucNumBlocks));
            pLastChunk->release();
            pop_Back();
            m_pAllocChunk = m_pDeallocChunk;
        }
        m_pEmptyChunk = m_pDeallocChunk;
    }

    // prove either m_pkEmptyChunk points nowhere, or points to a truly empty 
    // SChunk.
    EZASSERT((NULL == m_pEmptyChunk) || 
           (m_pEmptyChunk->hasAvailable(m_ucNumBlocks)));

}

//------------------------------------------------------
//---------------------------------------------------------------------------
// EzSmallObjectAllocator::EzSmallObjectAllocator
// Creates an allocator for small objects given chunk size and maximum 'small'
//     object size
//---------------------------------------------------------------------------

EzSmallObjectAllocator::EzSmallObjectAllocator(size_t stChunkSize)
    : m_stChunkSize(stChunkSize)
{   
    for (size_t st = 1; st <= MAX_SMALL_OBJECT_SIZE; st++)
    {
        m_Pool[st-1].init(st);
    }
}

//---------------------------------------------------------------------------
// EzSmallObjectAllocator::Allocate
// Allocates 'numBytes' memory
// Uses an internal pool of NiFixedAllocator objects for small objects  
//---------------------------------------------------------------------------

void* EzSmallObjectAllocator::allocate(size_t stNumBytes)
{
    EZASSERT(stNumBytes != 0);
    EZASSERT(stNumBytes <= MAX_SMALL_OBJECT_SIZE);
    EZASSERT(m_Pool[stNumBytes-1].blockSize() == stNumBytes);
    return m_Pool[stNumBytes-1].allocate();  
}

//---------------------------------------------------------------------------
// EzSmallObjectAllocator::Deallocate
// Deallocates memory previously allocated with Allocate
// (undefined behavior if you pass any other pointer)
//---------------------------------------------------------------------------

void EzSmallObjectAllocator::deallocate(void* p, size_t stNumBytes)
{
    EZASSERT(stNumBytes != 0);
    EZASSERT(stNumBytes <= MAX_SMALL_OBJECT_SIZE);
    EZASSERT(m_Pool[stNumBytes-1].blockSize() == stNumBytes);
    return m_Pool[stNumBytes-1].deallocate(p);  
}

//---------------------------------------------------------------------------
// EzSmallObjectAllocator::GetFixedAllocatorForSize
// Returns the EzFixedAllocator for the given input byte size
//---------------------------------------------------------------------------
EzFixedAllocator* EzSmallObjectAllocator::getFixedAllocatorForSize(
    size_t stNumBytes)
{
    EZASSERT(stNumBytes != 0);
    EZASSERT(stNumBytes <= MAX_SMALL_OBJECT_SIZE);
    EZASSERT(m_Pool[stNumBytes-1].blockSize() == stNumBytes);
    return &m_Pool[stNumBytes-1];
}
