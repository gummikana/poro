/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ***************************************************************************/


#ifndef INC_CMEMORYPOOL_H
#define INC_CMEMORYPOOL_H

#include "../../poro/platform_defs.h"

#ifdef PORO_PLAT_MAC
	#include <malloc/malloc.h> // mac os x
#elif defined(PORO_PLAT_IPHONE)
	#include <malloc/malloc.h> // mac os x
#else
	#include "malloc.h" // linux, windows
#endif

//#include <malloc.h>
#include <vector>
#include "../singleton/csingletonptr.h"

namespace ceng {

typedef std::size_t size_type;

template< class T, size_type number_of_units = 50 >
class CSimpleMemoryPool
{
private:
	struct Unit;

	struct Unit
	{
		Unit* prev;
		Unit* next;
	};

	void* myMemoryBlock;
	Unit* myAllocatedMemoryBlock;
	Unit* myFreeMemoryBlock;

	const size_type myUnitSize;
	const size_type myBlockSize;
	size_type		myNumOfAllocatedObjects;

public:
	CSimpleMemoryPool() :
		myMemoryBlock( NULL ),
		myAllocatedMemoryBlock( NULL ),
		myFreeMemoryBlock( NULL ),
		myUnitSize( sizeof( T ) ),
		myBlockSize( number_of_units * ( sizeof( T ) + sizeof( Unit ) ) ),
		myNumOfAllocatedObjects( 0 )
	{
		myMemoryBlock = malloc( myBlockSize );

		if( myMemoryBlock != NULL )
		{
			for( size_type i=0; i< number_of_units; i++ )
			{
				Unit* cur_unit = (Unit *)( (char *)myMemoryBlock + i * ( myUnitSize + sizeof( Unit ) ) );

				cur_unit->prev = NULL;
				cur_unit->next = myFreeMemoryBlock;

				if( myFreeMemoryBlock != NULL)
				{
					myFreeMemoryBlock->prev = cur_unit;
				}
				myFreeMemoryBlock = cur_unit;
			}
		}
	}

	~CSimpleMemoryPool()
	{
		free( myMemoryBlock );
	}

	// T* NewObject() { return GetMem( sizeof( T ) ); }
    void* GetMem( size_type sizeo )
	{
		if(	sizeo > myUnitSize || NULL == myMemoryBlock   || NULL == myFreeMemoryBlock)
		{
			return malloc( sizeo );
			// return new T;
		}

		Unit* cur_unit = myFreeMemoryBlock;

		myFreeMemoryBlock = cur_unit->next;
		if( myFreeMemoryBlock )
		{
			myFreeMemoryBlock->prev = NULL;
		}

		cur_unit->next = myAllocatedMemoryBlock;

		if( myAllocatedMemoryBlock )
		{
			myAllocatedMemoryBlock->prev = cur_unit;
		}

		myAllocatedMemoryBlock = cur_unit;

		void* result = reinterpret_cast< void* >( ( (char *)cur_unit + sizeof( Unit ) ) );
		myNumOfAllocatedObjects++;
		// result->Init();
		return result;
	}

    void Free( void* p )
	{
		if( HasPointer( p ) )
		{
			Unit* cur_unit = (Unit *)((char *)p - sizeof(Unit) );

			myAllocatedMemoryBlock = cur_unit->next;
			if( myAllocatedMemoryBlock )
			{
				myAllocatedMemoryBlock->prev = NULL;
			}

			cur_unit->next = myFreeMemoryBlock;
			if( myFreeMemoryBlock )
			{
				myFreeMemoryBlock->prev = cur_unit;
			}

			myFreeMemoryBlock = cur_unit;
			if( myNumOfAllocatedObjects > 0 )
				myNumOfAllocatedObjects--;
		}
		else
		{
			free( p );
			// delete p;
		}
	}

	bool HasPointer( void* void_p ) const
	{
		if( void_p >= myMemoryBlock && void_p < (void *)((char *)myMemoryBlock + myBlockSize ) )
			return true;

		return false;
	}

	bool IsEmpty() const
	{
		return ( myNumOfAllocatedObjects == 0 );
	}

	bool IsFull() const
	{
		return ( myFreeMemoryBlock == NULL );
	}

	size_type GetNumOfAllocatedObjects() const { return myNumOfAllocatedObjects; }
};

//---------------------------------------------------------------------------------------

template< class T, size_type single_pool_size = 50 >
class CMemoryPoolForObjects
{
public:
	typedef CSimpleMemoryPool< T, single_pool_size > PoolType;

	CMemoryPoolForObjects() :
		myPools(),
		myFreePool( NULL ),
		myCleanThePoolCount( 0 )
	{
		myPools.push_back( new PoolType );
		myFreePool = myPools[ 0 ];
	}

	~CMemoryPoolForObjects()
	{
		Clear();
	}

	void Clear()
	{
		for( unsigned int i = 0; i < myPools.size(); ++i )
			delete myPools[ i ];
		myPools.clear();
		myFreePool = NULL;
		myCleanThePoolCount  = 0;
	}

    void* GetMem( size_type sizeo )
	{
		if( myFreePool == NULL || myFreePool->IsFull() )
		{
			size_type lowest_count = single_pool_size + 1;
			bool found_anything = false;
			for( unsigned int i = 0; i < myPools.size(); ++i )
			{
				if( myPools[ i ]->GetNumOfAllocatedObjects() < lowest_count )
				{
					myFreePool = myPools[ i ];
					found_anything = true;
				}
			}

			if( found_anything == false )
			{
				myFreePool = new PoolType;
				myPools.push_back( myFreePool );
			}
		}

		return myFreePool->GetMem( sizeo );
	}

    void Free( void* p )
	{
		myCleanThePoolCount++;


		if( myCleanThePoolCount > (int)single_pool_size )
		{
			size_type clean_count = myFreePool->GetNumOfAllocatedObjects();
			myCleanThePoolCount = 0;
			for( unsigned int i = 0; i < myPools.size(); )
			{
				if( myPools[ i ] != myFreePool && myPools[ i ]->IsEmpty() )
				{
					delete myPools[ i ];
					myPools[ i ] = myPools[ myPools.size() - 1 ];
					myPools.pop_back();
				}
				else if( myPools[ i ]->GetNumOfAllocatedObjects() < clean_count )
				{
					myFreePool = myPools[ i ];
					clean_count = myPools[ i ]->GetNumOfAllocatedObjects();
				}
				else
				{
					++i;
				}
			}

		}

		if( myFreePool->HasPointer( p ) )
		{
			myFreePool->Free( p );
			return;
		}

		for( unsigned int i = 0; i < myPools.size(); )
		{
			if( myPools[ i ]->HasPointer( p ) )
			{
				myPools[ i ]->Free( p );
				return;
			}
			else if( myPools[ i ] != myFreePool && myPools[ i ]->IsEmpty() )
			{
				delete myPools[ i ];
				myPools[ i ] = myPools[ myPools.size() - 1 ];
				myPools.pop_back();
			}
			else
			{
				++i;
			}
		}
	}

	void DumpCountToLog()
	{
		size_type objects_count = 0;
		for( unsigned int i = 0; i < myPools.size(); ++i )
		{
			objects_count += myPools[ i ]->GetNumOfAllocatedObjects();
		}

		// ceng::logger_debug << "CMemoryPool Dump: " << myPools.size() << " / " << objects_count << std::endl;
	}

	std::vector< PoolType* >	myPools;
	PoolType*					myFreePool;
	int							myCleanThePoolCount;

};

//----------------------------------------------------------------------------------

template< class T, size_type size_o_pool = 50 >
class CMemoryPoolObject
{
public:
	virtual ~CMemoryPoolObject() { }

	void* operator new( size_type sizeo )
	{
		return ceng::GetSingletonPtr< ceng::CMemoryPoolForObjects< T, size_o_pool > >()->GetMem( sizeo );
	}

	void operator delete( void* pointer )
	{
		ceng::GetSingletonPtr< ceng::CMemoryPoolForObjects< T, size_o_pool > >()->Free( pointer );
	}
};

} // end o namespace ceng

#endif
