#pragma once

#include "EzDebugHelper.h"
#include "EzRTTI.h"


template <typename T> 
class EZCORE_ENTRY EzSingleton
{
protected:

    static T* ms_pSingleton;

public:
    EzSingleton( void )
    {
		EZASSERT( !ms_pSingleton );
	    ms_pSingleton = static_cast< T* >( this );
	}

    
	~EzSingleton( void )
	{
		EZASSERT( ms_pSingleton );  
		ms_pSingleton = NULL;
		

	}
    
	static T& get( void )
	{	
		EZASSERT( ms_pSingleton );  
		return ( *ms_pSingleton ); 
	}

	static bool isInitialized()
	{
		return ms_pSingleton != 0;
	}

};

