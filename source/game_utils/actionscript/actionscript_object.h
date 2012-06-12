#ifndef INC_ACTIONSCRIPT_OBJECT_H
#define INC_ACTIONSCRIPT_OBJECT_H

#include <iostream>
#include <map>
#include "../../utils/debug.h"

// this needs to be defined for Object to keep track of how and were it's been used in
// #define ACTIONSCRIPT_TRACK_OBJECTS

namespace as 
{

//-----------------------------------------------------------------------------

#ifndef ACTIONSCRIPT_TRACK_OBJECTS
	
class Object
{
public:
	Object() { }
	virtual ~Object() {  }
};

#endif

//-----------------------------------------------------------------------------
	
template< class T >
class Mem
{
public:

	static std::map< T*, int > freed_pointers;
	static std::map< T*, int > new_pointers;
	static bool break_me;

	static void FreePointer( T* what_ever )
	{
		// return;
		typename std::map< T*, int >::iterator i = freed_pointers.find( what_ever );
		if( i != freed_pointers.end() ) 
		{
			if( ( freed_pointers[ what_ever ] + 1 ) != new_pointers[ what_ever ] )
			{
				std::cout << "Found a pointer that we're freeing 2 times: " << what_ever << " in freed " << freed_pointers[ what_ever ] << ", in new " << new_pointers[ what_ever ] << std::endl;
				if( break_me )
					cassert( false );
			}

			//cassert( false );
		}

		freed_pointers[ what_ever ]++;

		// delete what_ever;
	}


	static void NewPointer( T* what_ever )
	{	
		// return;
		typename std::map< T*, int >::iterator i = new_pointers.find( what_ever );
		if( i != new_pointers.end() ) 
		{
			if( freed_pointers[ what_ever ] != new_pointers[ what_ever ] )
			{
				std::cout << "Created a pointer a again: " << what_ever << " in freed " << freed_pointers[ what_ever ] << ", in new " << new_pointers[ what_ever ] << std::endl;
				if( break_me )
					cassert( false );
			}
		}

		new_pointers[ what_ever ]++;

	}
};

template< typename T >  std::map< T*, int > Mem< T >::freed_pointers;
template< typename T >  std::map< T*, int > Mem< T >::new_pointers;
template< typename T >  bool Mem< T >::break_me = 0;

template< class T >
void FreePointer( T* what_ever )
{
	Mem< T >::FreePointer( what_ever );
}

template< class T >
void NewPointer( T* what_ever )
{
	Mem< T >::NewPointer( what_ever );
}

//-----------------------------------------------------------------------------

#ifdef ACTIONSCRIPT_TRACK_OBJECTS

class Object
{
public:
	Object() { NewPointer< Object >( this ); }
	virtual ~Object() { FreePointer< Object >( this ); }
};

#endif

} // end of namespace actionscript
//-----------------------------------------------------------------------------

#endif
