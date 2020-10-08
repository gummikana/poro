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


#include "carray2d.h"

namespace ceng {

#ifdef DEBUG_ARRAY2D_MEM_LEAKS


AtomicInteger::AtomicInteger()
{
	Set( 0 );
}

AtomicInteger::AtomicInteger(int initial_value)
{
	Set( initial_value );
}

AtomicInteger::AtomicInteger( const AtomicInteger& other )
{
	Set( other.Get() );
}

void AtomicInteger::operator+=( int value )
{
	mValue.fetch_add( value );
}

void AtomicInteger::operator-=( int value )
{
	mValue.fetch_add( -value );
}

int AtomicInteger::operator++(int)
{
	return mValue.fetch_add( 1 );
}

int AtomicInteger::operator--(int)
{
	return mValue.fetch_add( -1 );
}


int AtomicInteger::Get() const
{
	return mValue.load();
}

void AtomicInteger::Set( int value )
{
	mValue.store( value );
}

AtomicInteger& AtomicInteger::operator =( const AtomicInteger& other )
{
	mValue.store( other.mValue.load() );
	return *this;
}

AtomicInteger CArray2DLeaks::mCount = 0;

#endif

};