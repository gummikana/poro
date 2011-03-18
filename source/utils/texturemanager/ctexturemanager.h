///////////////////////////////////////////////////////////////////////////////
//
// CTextureManager
// ===============
//
// A generic buffer for textures.
//
//
// Created 19.04.2006 by Pete
//=============================================================================
//.............................................................................
#pragma warning ( disable : 4503)
#ifndef INC_CTEXTUREMANAGER_H
#define INC_CTEXTUREMANAGER_H

#include "../debug.h"
#include "../singleton/csingleton.h"

#include <string>
#include <map>

namespace ceng {

class CTextureManagerBasicReleaser
{
public:
	template< class T >
	void operator()( T& p )
	{
		delete p;
		p = 0;
	}
};

template< class Type, class Releaser = CTextureManagerBasicReleaser >
class CTextureManager : public CSingleton< CTextureManager< Type, Releaser > >
{
public:
	
	struct CTextureHelpStruct
	{
		CTextureHelpStruct() : 
		myPointer( Type() ),
		myReferenceCount( 0 ),
		myPreloaded( false )
		{
			
		}
		
		Type			myPointer;
		unsigned int	myReferenceCount;
		bool			myPreloaded;
	};
	
	~CTextureManager() 
	{ 
		if( myLogErrors )	
		{
			if( myTextureMap.empty() == false )
			{
				logger << "CTextureManager unreleased garbage!" << std::endl;
				// LOG_FUNCTION();

				typename std::map< std::string, CTextureHelpStruct >::iterator i;
				for( i = myTextureMap.begin(); i != myTextureMap.end(); ++i )
				{
					logger << i->first << "\t" << i->second.myReferenceCount << ", " << i->second.myPreloaded << std::endl;
				}
			}
			else
			{
				logger << "CTextureManager died gracefully" << std::endl;
			}
		}
	}

	void SetLogErrors( bool log_errors ) { myLogErrors = log_errors; }


	//! Checks if we have the given file
	bool HasFile( const std::string& file ) const
	{
		typename std::map< std::string, CTextureHelpStruct >::const_iterator i;
		i = myTextureMap.find( file );
		return ( i != myTextureMap.end() );
	}

	//! Adds a new pointer to our buffer, sets its reference count to 1
	void AddNew( const std::string& file, Type pointer, bool preloaded = false )
	{	
		if( HasFile( file ) == false )
		{
			CTextureHelpStruct temp;
			temp.myPointer = pointer;
			temp.myReferenceCount = 1;
			temp.myPreloaded = preloaded;

			myTextureMap.insert( std::pair< std::string, CTextureHelpStruct >( file, temp ) );
		}
		else
		{
			if( myLogErrors )
				logger << "CTextureManager::AddNew() - trying to add a new texture, but we already have one of the same name: "  << file << std::endl;
		}

	}

	//! Returns a pointer to the given file, increases the referencecount by one
	Type GetPointer( const std::string& file )
	{
		typename std::map< std::string, CTextureHelpStruct >::iterator i;
		i = myTextureMap.find( file );

		if( i != myTextureMap.end() )
		{
			i->second.myReferenceCount++;
			return i->second.myPointer;
		}
		else
		{
			if( myLogErrors )
				logger << "CTextureManager::GetPointer() - trying to get pointer to a file that doesn't exist: " << file << std::endl;
			return Type();
		}
	}

	//! Releases a the given pointer, decreases its referencecount
	/*!
		If the reference count is decreased to zero, the pointer is released 
		through	the Releaser opeatorion. 
	*/
	void ReleasePointer( Type pointer )
	{
		if( pointer == NULL ) 
			return;

		const bool debug = false;

		if( debug )
		{
			logger << "<CTextureManager::ReleasePointer>" << std::endl;
			
			logger << "pointer: " << pointer << std::endl;
		}
		
		typename std::map< std::string, CTextureHelpStruct >::iterator i;
		for( i = myTextureMap.begin(); i != myTextureMap.end(); ++i )
		{
			if( i->second.myPointer == pointer ) 
			{
				if( debug )
					logger << "found pointer: " << pointer << std::endl;
				break;
			}
		}

		if( i != myTextureMap.end() )
		{
			if( debug )
				logger << "i != myTextureMap.end()" << std::endl;
			i->second.myReferenceCount--;
			if( i->second.myReferenceCount <= 0 && i->second.myPreloaded == false )
			{
				Releaser r;
				r( i->second.myPointer );
				myTextureMap.erase( i );
			}
		}
		else
		{
			if( debug )
				logger << "i == myTextureMap.end()" << std::endl;
			/*if( myLogErrors )
				logger_warning << "CTextureManager::ReleasePointer() - trying to release a pointer that doens't exist" << std::endl;
			*/
			Releaser r;
			r( pointer );
		}
	}

	//! Increases the reference count by one. 
	/*! 
		Returns true if the pointer is safe to use.
		If AddReference returns a false, then the pointer wasn't found in the 
		data structure, suggesting a corruption and possibly and likely causing 
		a crash later in the program's life.
	*/
	bool AddReference( Type pointer )
	{
		if( pointer == NULL )
			return false;

		typename std::map< std::string, CTextureHelpStruct >::iterator i;
		for( i = myTextureMap.begin(); i != myTextureMap.end(); ++i )
		{
			if( i->second.myPointer == pointer ) 
				break;
		}

		if( i != myTextureMap.end() )
		{
			if( i->second.myReferenceCount == 0 )
			{
				logger << "CTextureManager::AddReference() - trying add a reference to a pointer no longer in the memory: " << i->first << std::endl;
				return false;
			}

			i->second.myReferenceCount++;

			return true;

		}
		else
		{
			logger << "CTextureManager::AddReference() - trying add a reference to a pointer no longer in the memory: " << i->first << std::endl;
			return false;
		}
	}

	//! Returns filename of the pointer
	std::string GetFilename( Type pointer )
	{
		typename std::map< std::string, CTextureHelpStruct >::iterator i;
		for( i = myTextureMap.begin(); i != myTextureMap.end(); ++i )
		{
			if( i->second.myPointer == pointer ) 
				return i->first;
		}

		return "";
	}

	//! Unloads a preloaded file
	void UnloadFile( const std::string& file )
	{
		typename std::map< std::string, CTextureHelpStruct >::iterator i;
		i = myTextureMap.find( file );

		if( i != myTextureMap.end() )
		{
			i->second.myPreloaded = false;
			if( i->second.myReferenceCount <= 0 && i->second.myPreloaded == false )
			{
				Releaser r;
				r( i->second.myPointer );
				myTextureMap.erase( i );
			}
			
		}
		else
		{
			if( myLogErrors )
				logger << "CTextureManager::UnloadFile() - trying to unload a file that doesn't exist: " << file  << std::endl;
			return;
		}
	}

private:

	std::map< std::string, CTextureHelpStruct >	myTextureMap;
	bool myLogErrors;

	CTextureManager() : myTextureMap(), myLogErrors( true ) { }

	
	friend class CSingleton< CTextureManager< Type, Releaser > >;
};

} // end of namespace ceng


#endif