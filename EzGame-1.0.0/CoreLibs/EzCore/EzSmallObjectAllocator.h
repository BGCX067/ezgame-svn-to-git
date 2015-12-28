#ifndef __EZFIXEDALLOCATPR_H_INCLUDE__
#define __EZFIXEDALLOCATPR_H_INCLUDE__

//////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////


#include "EzCriticalSection.h"


#ifndef EZSMALLOBJECTALLOCATOR_H
#define EZSMALLOBJECTALLOCATOR_H
#endif

#ifndef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE				25600
#endif

#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE			256
#endif


class EZCORE_ENTRY EzFixedAllocator
{
private:
    struct EZCORE_ENTRY SChunk
    {
    public:
        void init(size_t stBlockSize, unsigned char ucBlocks);
        void* allocate(size_t stBlockSize);
        void deallocate(void* p, size_t stBlockSize);
        void reset(size_t stBlockSize, unsigned char ucBlocks);
        void release();
        bool hasAvailable(unsigned char ucNumBlocks) const;
        bool hasBlock(unsigned char * p, size_t stChunkLength) const;
        bool isFilled( void ) const;

        unsigned char* m_pucData;
        unsigned char	m_ucFirstAvailableBlock;
        unsigned char	m_ucBlocksAvailable;
    };

    void push_Back(SChunk& Chunk);
    void pop_Back();
    void reserve(size_t stNewSize);
        
    // Internal functions        
    void doDeallocate(void* p);
    SChunk* vicinityFind(void* p);
    
    // Data 
    size_t m_stBlockSize;
    unsigned char m_ucNumBlocks;
    
    SChunk* m_pChunks;
    size_t m_stNumChunks;
    size_t m_stMaxNumChunks;
    SChunk* m_pAllocChunk;
    SChunk* m_pDeallocChunk;
    SChunk* m_pEmptyChunk;

	EzCriticalSection m_CriticalSection;

    
public:
	EzFixedAllocator(void);
	~EzFixedAllocator(void);

    // Create a EzFixedAllocator able to manage blocks of 'blockSize' size
    void init(size_t stBlockSize);

    // Allocate a memory block
    void* allocate();
    // Deallocate a memory block previously allocated with Allocate()
    // (if that's not the case, the behavior is undefined)
    void deallocate(void* p);
    // Returns the block size with which the EzFixedAllocator was initialized
    size_t blockSize() const
    { return m_stBlockSize; }
    // Comparison operator for sorting 
    bool operator<(size_t rhs) const
    { return blockSize() < rhs; }
    // Returns the number of chunks allocated
    size_t getNumChunks() const
    { return m_stNumChunks;}
    
};



class EZCORE_ENTRY EzSmallObjectAllocator
{
public:
    EzSmallObjectAllocator(size_t stChunkSize = DEFAULT_CHUNK_SIZE);

    void* allocate(size_t stNumBytes);
    void deallocate(void* p, size_t stSize);

    EzFixedAllocator* getFixedAllocatorForSize(size_t stNumBytes);

private:
    EzSmallObjectAllocator(const EzSmallObjectAllocator&)
	{
	}
    
	EzSmallObjectAllocator& operator=(const EzSmallObjectAllocator&)
	{
        return *(this);
	}
    
    EzFixedAllocator m_Pool[MAX_SMALL_OBJECT_SIZE];
    size_t m_stChunkSize;

};


#endif
