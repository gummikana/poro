#ifndef INC_CINFINITEGRID_H
#define INC_CINFINITEGRID_H

#include <map>
#include "infinite_types.h"
#include "../array2d/carray2d.h"
#include "cgridpoint.h"

//=============================================================================

namespace ceng {

template< class ArrayGridTypes  >
class CInfiniteGrid
{
public:

	typedef typename ArrayGridTypes									T;
	typedef typename ceng::CArray2D< typename ArrayGridTypes >		Array2D;
	typedef typename CGridPoint< int >								GPoint;
	typedef typename ceng::math::CVector2< int >					Point2D;
	typedef typename std::map< typename GPoint, typename Array2D* >	GridMap;

	//-------------------------------------------------------------------------

	CInfiniteGrid( int number_of_bits = 7 ) :
		mHashUpperMask( 0 ),
		mHashLowerMask( 0 ),
		mNumberOfBits( 0 ),
		mArray2DSize( 0 ),
		mWorlds(),
		mWorldBounds(),
		mBufferedGetWorldFirst(),
		mBufferedGetWorldSecond( NULL )
	{
		SetNumberOfBits( number_of_bits );
	}
	

	~CInfiniteGrid()
	{
		Clear();
	}

	//-------------------------------------------------------------------------

	void SetNumberOfBits( int n ) 
	{
		mNumberOfBits = n;

		int mask = 0xFFFFFFF;

		mask = mask << mNumberOfBits;
		
		mHashUpperMask = mask;
		mHashLowerMask = ~mask; 

		mArray2DSize = (int)pow( 2.0, mNumberOfBits );
	}

	//-------------------------------------------------------------------------

	inline GPoint CreateGPoint( int x, int y ) const {

		int masked_x = x & mHashUpperMask;
		int masked_y = y & mHashUpperMask;
	
		masked_x = masked_x >> mNumberOfBits;
		masked_y = masked_y >> mNumberOfBits;

		return GPoint( masked_x, masked_y );
	}

	inline Point2D CreateArrayPoint( int x, int y ) const {
		int masked_x = x & mHashLowerMask;
		int masked_y = y & mHashLowerMask;
	
		return Point2D( masked_x, masked_y );
	}
	

	//-------------------------------------------------------------------------

	inline T& At( int x, int y ) {
		GPoint g = CreateGPoint( x, y );
		Array2D* world = GetWorld( g );

		cassert( world );

		Point2D p = CreateArrayPoint( x, y );
		
		cassert( p.x >= 0 );
		cassert( p.x < world->GetWidth() );
		cassert( p.y >= 0 );
		cassert( p.y < world->GetHeight() );

		return world->Rand( p.x, p.y );
	}

	inline T& At( const Point2D& p ) { return At( (int)p.x, (int)p.y ); }

	//-------------------------------------------------------------------------

	inline const T& At( int x, int y ) const {
		GPoint g = CreateGPoint( x, y );
		const Array2D* world = GetWorld( g );
		
		static T null_object = T();
		if( world == NULL ) return null_object;

		static T temp_null = T();
		if( world == NULL ) return temp_null;

		Point2D p = CreateArrayPoint( x, y );
		
		cassert( p.x >= 0 );
		cassert( p.x < world->GetWidth() );
		cassert( p.y >= 0 );
		cassert( p.y < world->GetHeight() );

		return world->Rand( p.x, p.y );
	}

	inline const T& At( const Point2D& p ) const { return At( (int)p.x, (int)p.y ); }

	//-------------------------------------------------------------------------

	Array2D* CreateNewWorld( const GPoint& gpoint )
	{
		int x = gpoint.GetX();
		int y = gpoint.GetY();

		if( x < mWorldBounds.mini.x )
			mWorldBounds.mini.x = x;

		if( y < mWorldBounds.mini.y )
			mWorldBounds.mini.y = y;

		if( x > mWorldBounds.maxi.x )
			mWorldBounds.maxi.x = x;

		if( y > mWorldBounds.maxi.y )
			mWorldBounds.maxi.y = y;

		Array2D* result = new Array2D( mArray2DSize, mArray2DSize );

		mWorlds[ gpoint ] =  result;
		return result;
	}
	
	//-------------------------------------------------------------------------
	// GetWorld
	// return a 2D array that's at the given gpoint
	// if the 2D array doesn't exist and we're not calling const GetWorld will
	// create a new empty 2D array and return that

	inline const Array2D* GetWorld( const GPoint& gpoint ) const
	{
		GridMap::const_iterator i = mWorlds.find( gpoint );

		if( i == mWorlds.end() ) 
			return NULL;
		else
			return i->second;
	}

	//.........................................................................
	//
	// GetWorld()
	// this isn't thread safe!
	// Could be converted to be thread safe if the buffering would be locked
	// or disabled.
	//
	// The buffering increases the speed of this pretty much, since it skips
	// a lot of the find operations. Most of the time (I'm guessing) we're 
	// looping through an area. Looping through an area means that you'll be
	// hitting the same world / 2DArray most of the time and that means
	// we could buffer the results. Which saves "a lot" of time.
	//
	inline Array2D* GetWorld( const GPoint& gpoint )
	{
		if( mBufferedGetWorldFirst == gpoint && mBufferedGetWorldSecond != NULL ) return mBufferedGetWorldSecond;

		GridMap::const_iterator i = mWorlds.find( gpoint );

		mBufferedGetWorldFirst = gpoint;

		if( i == mWorlds.end() ) {

			mBufferedGetWorldSecond = CreateNewWorld( gpoint );
			return mBufferedGetWorldSecond;
		}
		else {
			mBufferedGetWorldSecond = i->second;
			return i->second;
		}
	}
	
	//=========================================================================

	void Clear() 
	{
		GridMap::iterator i;
		for( i = mWorlds.begin(); i != mWorlds.end(); ++i )
		{
			delete i->second;
		}

		mWorlds.clear();
		mWorldBounds.mini.Set( 0, 0 );
		mWorldBounds.maxi.Set( 0, 0 );
		mBufferedGetWorldSecond = NULL;
	}

	types::iaabb	GetWorldBounds() const { return mWorldBounds; }


private:

	//-------------------------------------------------------------------------

	types::int32 mHashUpperMask;
	types::int32 mHashLowerMask; // ~mHashUpperMask
	int mNumberOfBits;
	int mArray2DSize;

	//-------------------------------------------------------------------------
	 
	GridMap			mWorlds; // std::map< CGridPoint, CArray2D* >
	types::iaabb	mWorldBounds;

	//-------------------------------------------------------------------------
	// buffered things, these things make this less thread safe
	typename GPoint mBufferedGetWorldFirst;
	typename Array2D* mBufferedGetWorldSecond;

	//-------------------------------------------------------------------------
};

} // end of namespace ceng
//=============================================================================

#endif
