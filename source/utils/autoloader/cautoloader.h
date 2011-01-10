#ifndef INC_CAUTOLOADER_H
#define INC_CAUTOLOADER_H

#include "autoloader_utils.h"

namespace ceng {

//-----------------------------------------------------------------------------

class IAutoLoaderHelper
{
public:

	virtual ~IAutoLoaderHelper() { }

	std::string mRootElement;
	std::string mFilename;
	std::string mTimeStamp;

	bool Empty() const { return ( GetPtr() == 0 ); }

	virtual void* GetPtr() const = 0;
	virtual void LoadFromFile() = 0;
	virtual void SaveToFile( const std::string& filename ) = 0;

	std::string GetTimeStamp() const { return mTimeStamp; }
	void SetTimeStamp( const std::string& time_stamp ) { mTimeStamp = time_stamp; }

	std::string GetFilename() const { return mFilename; }
};

//-----------------------------------------------------------------------------

template< typename T >
class CAutoLoaderHelper : public IAutoLoaderHelper
{
public:

	CAutoLoaderHelper( T* load_me, const std::string& filename, const std::string& root_element ) :
		mLoadMe( load_me )
	{
		mFilename = filename;
		mRootElement = root_element;
	}

	virtual void* GetPtr() const
	{
		return (void*)mLoadMe;
	}

	virtual void LoadFromFile() 
	{
		cassert( mLoadMe );
		ceng::XmlLoadFromFile( *mLoadMe, mFilename, mRootElement );
	}

	virtual void SaveToFile( const std::string& filename ) 
	{
		cassert( mLoadMe );
		ceng::XmlSaveToFile( *mLoadMe, filename, mRootElement );
	}

	T* mLoadMe;
};

//-----------------------------------------------------------------------------

// autoloader 
class CAutoLoader
{
public:

	CAutoLoader() { }
	~CAutoLoader() { Clear(); }

	void Clear()
	{
		for( std::size_t i = 0; i < mLoaders.size(); ++i )
		{
			delete mLoaders[ i ];
		}

		mLoaders.clear();
	}

	void Update()
	{
		for( std::size_t i = 0; i < mLoaders.size() ;++i )
		{
			cassert( mLoaders[ i ] );
			cassert( mLoaders[ i ]->Empty() == false );
			cassert( mLoaders[ i ]->GetFilename().empty() == false );

			std::string time_stamp = GetTimeStampForFile( mLoaders[ i ]->GetFilename() );

			if( time_stamp > mLoaders[ i ]->GetTimeStamp() )
			{
				mLoaders[ i ]->LoadFromFile();
				mLoaders[ i ]->SetTimeStamp( time_stamp );
			}
		}
	}
	
	void LoadEverything()
	{
		for( std::size_t i = 0; i < mLoaders.size() ;++i )
		{
			cassert( mLoaders[ i ] );
			cassert( mLoaders[ i ]->Empty() == false );
			cassert( mLoaders[ i ]->GetFilename().empty() == false );
			mLoaders[ i ]->LoadFromFile();
			mLoaders[ i ]->SetTimeStamp( GetTimeStampForFile( mLoaders[ i ]->GetFilename() ) );
		}
	}
	
	void SaveEverything( const std::string& extra_filename )
	{
		for( std::size_t i = 0; i < mLoaders.size() ;++i )
		{
			cassert( mLoaders[ i ] );
			cassert( mLoaders[ i ]->Empty() == false );
			cassert( mLoaders[ i ]->GetFilename().empty() == false );

			std::string filename = mLoaders[ i ]->GetFilename() + extra_filename;
			mLoaders[ i ]->SaveToFile( filename );
		}
	}

	template< typename T >
	void AddFile( T* load_me, const std::string& filename, const std::string& root_element )
	{
		cassert( load_me );
		ceng::XmlLoadFromFile( *load_me, filename, root_element );

		IAutoLoaderHelper* load_helper = new CAutoLoaderHelper< T >( load_me, filename, root_element );
		load_helper->SetTimeStamp( GetTimeStampForFile( filename ) );
		mLoaders.push_back( load_helper );

	}

	template< typename T >
	void RemoveLoader( T* remove_me )
	{
		void* ptr = (void*)remove_me;

		for( std::size_t i = 0; i < mLoaders.size(); )
		{
			cassert( mLoaders[ i ] );
			if( mLoaders[ i ]->GetPtr() == ptr )
			{
				delete mLoaders[ i ];
				mLoaders[ i ] = mLoaders[ mLoaders.size() ];
				mLoaders.pop_back();
			}
			else
			{
				++i;
			}
		}
	}

	std::string GetTimeStampForFile( const std::string& filename )
	{
		return ceng::GetDateForFile( filename );
	}

	std::vector< IAutoLoaderHelper* > mLoaders;
};

//-----------------------------------------------------------------------------

} // end of namespace ceng

#endif