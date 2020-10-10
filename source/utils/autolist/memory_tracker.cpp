#include "memory_tracker.h"


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