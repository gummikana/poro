#ifndef INC_MEMORY_TRACKER_H
#define INC_MEMORY_TRACKER_H

#if 0
#include <atomic>

class AtomicInteger
{
public:
	AtomicInteger();
	AtomicInteger( int initial_value );
	AtomicInteger( const AtomicInteger& other );

	void operator+=( int );
	void operator-=( int );
	int operator++( int );
	int operator--( int );
	int operator=( int value ) { Set( value ); return value; }
	
	int Get() const;
	void Set( int value );

	AtomicInteger& operator =( const AtomicInteger& other );

private:
	std::atomic<int> mValue;

};



// Quick hax class to keep track of how many instances there of a class
// used for debug purpouses
template< class T >
class CAutoCounter
{
public:
	virtual ~CAutoCounter()
	{
		myCounter--;
	}

	static int Count()
	{
		return myCounter.Get();
	}


protected:
	CAutoCounter()
	{ 
		myCounter++;
	}

private:
	static AtomicInteger myCounter;
};


template< class T >
AtomicInteger CAutoCounter< T >::myCounter;


// Quick hax class to keep track of how many instances there of a class
// used for debug purpouses
template< class T >
class MemoryTracker
{
public:
	virtual ~MemoryTracker()
	{
		myCounter--;
		myMemoryAllocs -= myMemoryChunk;
	}

	static int Count()
	{
		return myCounter.Get();
	}

	static int DEBUG_GetMemoryAllocs()
	{
		return myMemoryAllocs.Get();
	}


protected:
	MemoryTracker()
	{ 
		myMemoryChunk = 0;
		myCounter++;
	}

	void SetMemoryChunk( int memory_chunk )
	{
		myMemoryChunk = memory_chunk;
		myMemoryAllocs += myMemoryChunk;
	}

private:
	static AtomicInteger myCounter;
	static AtomicInteger myMemoryAllocs;
	int myMemoryChunk;
};

template< class T >
AtomicInteger MemoryTracker< T >::myCounter;

template< class T >
AtomicInteger MemoryTracker< T >::myMemoryAllocs;

#endif

#endif
