/***************************************************************************
*
* Copyright (c) 2015 Olli Harjola, Petri Purho
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

#include "fileio.h"
#include "iplatform.h"

#ifndef PORO_CONSERVATIVE
#include <mutex>
#endif

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <utils/filesystem/filesystem.h>

#ifdef PORO_PLAT_WINDOWS
#include "external/poro_windows.h"

#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
//#pragma comment(lib,"Pathcch.lib")
#include "shlobj.h"
#include "strsafe.h"

#endif

#include "poro_macros.h"

typedef poro::types::Uint32 u32;


namespace poro {

namespace platform_impl
{
	void Init();
	void InitAndSetUserDataFolderName( const std::string& name );
	void ListFiles( const std::wstring& full_path, std::vector<std::string>* out_files, bool list_directories );
	std::wstring GetFullPath( FileLocation::Enum location, const std::string& relative_path );
	std::wstring GetFullPath( FileLocation::Enum location );
	std::wstring CombinePath( std::wstring root, std::wstring relative_to_root );
	std::string CombinePath( std::string root, std::string relative_to_root );


	struct StreamInternal
	{
		StreamInternal( const std::wstring& filename, bool read, StreamStatus::Enum* out_status, u32 write_mode )
		{
			*out_status = StreamStatus::AccessFailed;

			if ( read )
			{
				mWriteBuffer = NULL;
				mFile.open( filename.c_str(), std::ios::in | std::ios::binary );
			}
			else
			{
				auto directory_path = filename;
				const size_t last_slash_index = filename.rfind( '\\' );
				if ( std::string::npos != last_slash_index )
					directory_path = filename.substr( 0, last_slash_index );

				const int code  = SHCreateDirectoryExW( NULL, directory_path.c_str(), NULL );
				//if ( code != ERROR_SUCCESS )
				//	std::cout << "Path creation failed - " << code;	

				mWriteBuffer = (char*)malloc( WRITE_BUFFER_SIZE_BYTES );
				mWriteBufferPos = 0;

				if( write_mode & StreamWriteMode::Ascii )
				{
					mFile.open( filename.c_str(), 
						( (write_mode & StreamWriteMode::Append ) ? std::ios::app :  std::ios::out ) );
				}
				else
				{
					mFile.open( filename.c_str(), 
						( (write_mode & StreamWriteMode::Append ) ? std::ios::app :  std::ios::out ) 
						| std::ios::binary );
				}
			}

			if ( mFile.is_open() == 0 )
				return;

			*out_status = StreamStatus::NoError;
			mSize = ceng::ReadFileSize( mFile );
			mPosition = 0;
			// BUG 2018.11.28 - crashed here (in ntdll.dll!76fae486())
		}

		StreamInternal() : 
			mWriteBuffer( NULL ),
			mWriteBufferPos( 0 ),
			mSize( 0 ),
			mPosition( 0 )
		{
		}

		StreamStatus::Enum Read( char* out_buffer, u32 buffer_capacity_bytes, u32* out_bytes_read )
		{
			if (mFile.good() == false )
				return StreamStatus::AccessFailed;

			long read_size = std::min( mSize - mPosition, (long)buffer_capacity_bytes );
			mPosition += read_size;
			mFile.read( out_buffer, read_size );
			*out_bytes_read = (u32)read_size;
			return StreamStatus::NoError;
		}

		StreamStatus::Enum ReadTextLine( std::string* out_text )
		{
			if ( mFile.good() == false )
				return StreamStatus::AccessFailed;

			bool first = true;
			const int buffer_size = 1024;
			char buffer[ buffer_size ];
			int count = 0;
			char c;
			while ( mFile.get( c ).good() )
			{
				if ( c == '\n' )
					break;
				if ( c != '\r')
					buffer[ count++ ] = c;

				if ( count == buffer_size )
				{
					*out_text = first ? std::string( buffer, count ) : ( *out_text + std::string( buffer, count ) );
					count = 0;
				}
			}

			if (count > 0)
				*out_text = first ? std::string( buffer, count ) : ( *out_text + std::string( buffer, count ) );
			else if ( first )
				*out_text = "";

			return StreamStatus::NoError;
		}

		StreamStatus::Enum Write( const char* buffer, u32 buffer_size_bytes )
		{
			if ( mFile.good() == false )
				return StreamStatus::AccessFailed;

			if ( buffer_size_bytes > WRITE_BUFFER_SIZE_BYTES )
			{
				if ( mWriteBufferPos > 0 )
				{
					mFile.write( mWriteBuffer, mWriteBufferPos );
					mWriteBufferPos = 0;
				}

				mFile.write( buffer, buffer_size_bytes );
			}
			else
			{
				if ( mWriteBufferPos + buffer_size_bytes > WRITE_BUFFER_SIZE_BYTES )
				{
					mFile.write( mWriteBuffer, mWriteBufferPos );
					mWriteBufferPos = 0;
				}

				memcpy( mWriteBuffer + mWriteBufferPos, buffer, buffer_size_bytes );
				mWriteBufferPos += buffer_size_bytes;
			}

			//mFile.write( buffer, buffer_size_bytes );
			return StreamStatus::NoError;
		}

		StreamStatus::Enum WriteLineEnding()
		{
			const char* LINE_ENDING = "\r\n";
			return Write( LINE_ENDING, 2 );
		}

		void SeekToBeginning()
		{
			mFile.seekg( 0, std::ios::beg );
			mWriteBufferPos = 0;
		}

		void FlushWrites()
		{
			if ( mWriteBufferPos > 0 )
			{
				mFile.write( mWriteBuffer, mWriteBufferPos );
				mWriteBufferPos = 0;
			}

			mFile.flush();
		}

		void Close()
		{
			if ( mWriteBuffer )
			{
				FlushWrites();

				free( mWriteBuffer );
				mWriteBuffer = NULL;
			}
			mFile.close();
		}


		const unsigned int WRITE_BUFFER_SIZE_BYTES = 8192;

		std::fstream mFile;
		char* mWriteBuffer;
		u32   mWriteBufferPos;
		long mSize;
		long mPosition;
	};
}

#ifdef PORO_PLAT_WINDOWS

	namespace platform_impl
	{
		// utils

		void NormalizeSeparators( std::wstring& path )
		{
			for ( size_t i = 0; i < path.size(); i++ )
			{
				if ( path[i] == L'/' )
					path[i] = L'\\';
			}
		}

		void NormalizeSeparators( std::string& path )
		{
			for ( size_t i = 0; i < path.size(); i++ )
			{
				if ( path[i] == '/' )
					path[i] = '\\';
			}
		}

		void NormalizeSeparatorsOut( std::string& path )
		{
			for ( size_t i = 0; i < path.size(); i++ )
			{
				if ( path[i] == '\\' )
					path[i] = '/';
			}
		}

		std::wstring StringToWString( const std::string& str )
		{
			std::wstring result( str.length(), L' ' );
			std::copy( str.begin(), str.end(), result.begin() );
			return result;
		}

		std::string WStringToString( const std::wstring& str )
		{
			return std::string ( str.begin(), str.end() );
		}

		std::wstring CleanPath( std::wstring& str )
		{
			return str;
			if ( str.empty() == false )
			{
				NormalizeSeparators( str );
				if ( str[str.length() - 1] != L'/' )
					str = str + L'/';
			}

			return str;
		}

		static bool IsSeparator( char c )
		{
			return c == '\\' || c == '/';
		}

		static bool IsSeparator( WCHAR c )
		{
			return c == L'\\' || c == L'/';
		}

		static WCHAR GetSeparator( const std::wstring& path )
		{
			return L'/';
		}

		static char GetSeparator( const std::string& path )
		{
			return '/';
		}

		static std::wstring GetWorkingDirectory()
		{
			WCHAR path[MAX_PATH];
			GetCurrentDirectoryW( MAX_PATH, path );
			return CleanPath( std::wstring( path ) );
		}


		// impl

		#pragma warning (push)
		#pragma warning(disable:4995)

		std::wstring user_directory;
		std::wstring working_directory = GetWorkingDirectory();


		void Init()
		{
			working_directory = GetWorkingDirectory();
		}

		void InitAndSetUserDataFolderName( const std::string& name )
		{
			WCHAR path[MAX_PATH];
			path[0] = '\0';

			PWSTR appdata_path_ptr = NULL;

			if ( SUCCEEDED( SHGetKnownFolderPath( FOLDERID_LocalAppDataLow, CSIDL_FLAG_CREATE, NULL, &appdata_path_ptr ) ) )
			{
				auto namew = StringToWString( name );
				StringCchCatW( path,MAX_PATH, appdata_path_ptr );
				PathAppendW( path, namew.c_str() );
				StringCchCatW( path, MAX_PATH, L"\\" );

				CoTaskMemFree( appdata_path_ptr );
			}

			user_directory = CleanPath( std::wstring( path ) );
		}

		void ListFiles( const std::wstring& full_path_where, const std::string& relative_path_where, std::vector<std::string>* out_files, bool list_directories )
		{
			poro_assert( out_files );

			std::wstring find_path = CombinePath( full_path_where, L"*" );

			WIN32_FIND_DATAW file_info;
			auto file_handle = FindFirstFileW( find_path.c_str(), &file_info );
			if ( file_handle != INVALID_HANDLE_VALUE )
			{
				do 
				{
					if ( std::wstring( file_info.cFileName ) == L"." || std::wstring( file_info.cFileName ) == L".." )
					{
						continue;
					}

					auto out_file = CombinePath( relative_path_where, WStringToString( file_info.cFileName ) );
					NormalizeSeparatorsOut( out_file );

					if( (file_info.dwFileAttributes & _A_SUBDIR) )
					{
						if( list_directories ) 
							out_files->push_back( out_file );
					}
					else
					{
						if( list_directories == false ) 
							out_files->push_back( out_file );
					}
				} 
				while( FindNextFileW( file_handle, &file_info ) );

				FindClose( file_handle );
			}
			else
			{
				// nothing found, or an error occurred
			}
		}

		std::wstring GetFullPath( FileLocation::Enum location, const std::string& relative_path )
		{
			std::wstring a = GetFullPath( location );
			return CombinePath( a, StringToWString( relative_path ) );
		}

		std::wstring GetFullPath( FileLocation::Enum location )
		{
			if ( location == FileLocation::UserDocumentsDirectory )
				return user_directory;
			else
				return working_directory;
		}

		std::wstring CombinePath( std::wstring root, std::wstring relative_to_root )
		{
			WCHAR path[MAX_PATH];
			path[0] = '\0';
			PathAppendW( path, root.c_str() );
			PathAppendW( path, relative_to_root.c_str() );

			auto result = std::wstring( path );
			NormalizeSeparators( result );
			return result;

			// check if the paths are combined properly
			/*if ( root.empty() ) return relative_to_root;
			if ( relative_to_root.empty() ) return relative_to_root;

			// detect full path
			WCHAR c = 0;
			WCHAR prev = 0;
			for ( size_t i = 0; i < relative_to_root.length(); i++ )
			{
				c = relative_to_root[i];
				if ( prev == L':' && c == L'\\' )
					return relative_to_root;
				prev = c;
			}

			// ideally we would use this but WinAPI wants more
			//if ( PathIsRelative( relative_to_root.c_str() ) == false )
			//	return relative_to_root;
			if ( root.size() > 1 && IsSeparator( root.back() ) && IsSeparator( relative_to_root[0] ) )
				root.pop_back();

			if ( !IsSeparator( root.back() ) && !IsSeparator( relative_to_root[0] ) )
				return root + GetSeparator( root ) + relative_to_root;

			poro_assert( IsSeparator( root.back() ) || IsSeparator( relative_to_root[0] ) );

			auto result = root + relative_to_root;
			NormalizeSeparators( result );
			return result;*/
		}

		std::wstring CombinePath( std::wstring root, std::string relative_to_root_ )
		{
			std::wstring relative_to_root = StringToWString( relative_to_root_ );
			WCHAR path[MAX_PATH];
			path[0] = '\0';
			PathAppendW( path, root.c_str() );
			PathAppendW( path, relative_to_root.c_str() );

			auto result = std::wstring( path );
			NormalizeSeparators( result );
			return result;

			// check if the paths are combined properly
			/*if ( root.empty() ) return relative_to_root;
			if ( relative_to_root.empty() ) return relative_to_root;

			// detect full path
			WCHAR c = 0;
			WCHAR prev = 0;
			for ( size_t i = 0; i < relative_to_root.length(); i++ )
			{
				c = relative_to_root[i];
				if ( prev == L':' && c == L'\\' )
					return relative_to_root;
				prev = c;
			}

			// ideally we would use this but WinAPI wants more
			//if ( PathIsRelative( relative_to_root.c_str() ) == false )
			//	return relative_to_root;
			if ( root.size() > 1 && IsSeparator( root.back() ) && IsSeparator( relative_to_root[0] ) )
				root.pop_back();

			if ( !IsSeparator( root.back() ) && !IsSeparator( relative_to_root[0] ) )
				return root + GetSeparator( root ) + relative_to_root;

			poro_assert( IsSeparator( root.back() ) || IsSeparator( relative_to_root[0] ) );

			auto result = root + relative_to_root;
			NormalizeSeparators( result );
			return result;*/
		}

		std::string CombinePath( std::string root, std::string relative_to_root )
		{
			// check if the paths are combined properly
			if( root.empty() ) return relative_to_root;
			if( relative_to_root.empty() ) return relative_to_root;
			
			// detect full path
			char c = 0;
			char prev = 0;
			for ( size_t i = 0; i < relative_to_root.length(); i++ )
			{
				c = relative_to_root[ i ];
				if ( prev == ':' && c == '\\' )
					return relative_to_root;
				prev = c;
			}

			// ideally we would use this but WinAPI wants more
			//if ( PathIsRelative( relative_to_root.c_str() ) == false )
			//	return relative_to_root;
			if( root.size() > 1 && IsSeparator( root.back() ) && IsSeparator( relative_to_root[0] ) ) 
				root.pop_back();

			if( !IsSeparator( root.back() ) && !IsSeparator( relative_to_root[0] ) ) 
				return root + GetSeparator( root ) + relative_to_root;
			
			poro_assert( IsSeparator( root.back() ) || IsSeparator( relative_to_root[0] ));

			auto result = root + relative_to_root;
			NormalizeSeparators( result );
			return result;
		}

		void DeleteFileImpl( const std::wstring& path )
		{
			const int result = DeleteFileW( path.c_str() );

			if ( result == 0 )
			{
				LogError << "DeleteFileImp(" << WStringToString( path ) << ") file failed - error code: " << GetLastError() << "\n";
			}
		}

		std::string GetDateForFile( const std::wstring& filename )
		{
			std::string result;

			WIN32_FIND_DATAW file_info;
			auto file_handle = FindFirstFileW( filename.c_str(), &file_info );
			if ( file_handle == INVALID_HANDLE_VALUE )
			{
				return "";
			}
			else
			{
				std::stringstream ss;

				FILETIME file_time = file_info.ftLastWriteTime;
				if ( file_time.dwHighDateTime == 0 && file_time.dwLowDateTime == 0 )
					file_time = file_info.ftCreationTime;
				if ( file_time.dwHighDateTime == 0 && file_time.dwLowDateTime == 0 )
					file_time = file_info.ftLastAccessTime;

				SYSTEMTIME system_time = { 0 };
				SYSTEMTIME local_time = { 0 };

				FileTimeToSystemTime( &file_time, &system_time );
				SystemTimeToTzSpecificLocalTime( NULL, &system_time, &local_time );

				ss << std::setfill( '0' ) << std::setw( 2 ) << ( local_time.wYear - 2000 ); // "-2000" so the output matches the format used by ceng::GetDateForFile()
				ss << std::setfill( '0' ) << std::setw( 2 ) << local_time.wMonth;
				ss << std::setfill( '0' ) << std::setw( 2 ) << local_time.wDay;
				ss << std::setfill( '0' ) << std::setw( 2 ) << local_time.wHour;
				ss << std::setfill( '0' ) << std::setw( 2 ) << local_time.wMinute;
				ss << std::setfill( '0' ) << std::setw( 2 ) << local_time.wSecond;

				result = ss.str();

				FindClose( file_handle );

				#ifdef WIZARD_DEBUG
					const auto cengresult = ceng::GetDateForFile( platform_impl::WStringToString( filename  ) );
					poro_assert( result == cengresult );
				#endif
			}

			return result;
		}

		bool RenameFile( const std::wstring& file, const std::wstring new_name )
		{
			/* add | MOVEFILE_WRITE_THROUGH  if you want to make this block until the operation is done*/
			int result = MoveFileExW( file.c_str(), new_name.c_str(), MOVEFILE_REPLACE_EXISTING );
			if ( result == 0 )
			{
				LogError << "RenameFile(" << WStringToString( file ) << ", " << WStringToString( new_name ) << ") failed - error code: " << GetLastError() << "\n";
			}
			return result == 1 ? true : false;
		}

		#pragma warning(pop)
	}
#endif

#ifdef PORO_PLAT_MAC
	// TODO: implement
#endif

#ifdef PORO_PLAT_LINUX
	// TODO: implement
#endif

namespace impl
{
	// path handling
	// thread local storage for immediate mode API
	struct FileImContext
	{
		ReadStream            readStream;
		std::string           readPath;
	};

	PORO_THREAD_LOCAL FileImContext* gImCtx;

	void UpdateReadImContext( FileSystem* file_system, const std::string& new_path )
	{
		if ( gImCtx == NULL )
			gImCtx = new FileImContext();

		if ( new_path != gImCtx->readPath )
		{
			gImCtx->readStream.~ReadStream();
			gImCtx->readPath = new_path;
			gImCtx->readStream = file_system->OpenRead( new_path );
		}
	}

	void CloseReadImContext( FileSystem* file_system )
	{
		if ( gImCtx == NULL )
			gImCtx = new FileImContext();

		if ( gImCtx->readPath != "" )
		{
			gImCtx->readStream.~ReadStream();
			gImCtx->readPath = "";
		}
	}
}


// === WriteStream ================================

StreamStatus::Enum WriteStream::Write( char* data, u32 length_bytes )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;
	
	return mStreamImpl->Write( data, length_bytes );
}

StreamStatus::Enum WriteStream::Write( const std::string& text )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	return mStreamImpl->Write( text.c_str(), text.length() );
	return StreamStatus::NoError;
}

StreamStatus::Enum WriteStream::WriteLine( const std::string& text )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	StreamStatus::Enum status = mStreamImpl->Write( text.c_str(), text.length() );
	if ( status != StreamStatus::NoError )
		return status;
	return mStreamImpl->WriteLineEnding();
}

StreamStatus::Enum WriteStream::WriteEndOfLine()
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	return mStreamImpl->WriteLineEnding();
}

StreamStatus::Enum WriteStream::FlushWrites()
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	mStreamImpl->FlushWrites();
	return StreamStatus::NoError;
}

// ===

void WriteStream::Close()
{
	if ( mStreamImpl )
	{
		mStreamImpl->Close();
		delete mStreamImpl;
		mStreamImpl = NULL;
	}
}

WriteStream::~WriteStream()
{
	Close();
}

WriteStream& WriteStream::operator= ( const WriteStream& other )
{
	if ( &other == this ) return *this;
	this->mDevice = other.mDevice;
	this->mStreamImpl = other.mStreamImpl;
	WriteStream& other2 = const_cast<WriteStream&>( other );
	other2.mDevice = NULL;
	other2.mStreamImpl = NULL;
	return *this;
}


// === ReadStream ================================

StreamStatus::Enum ReadStream::Read( char* out_buffer, u32 buffer_capacity_bytes, u32* out_bytes_read )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	return mStreamImpl->Read( out_buffer, buffer_capacity_bytes, out_bytes_read );
}

StreamStatus::Enum ReadStream::ReadWholeFile( char*& out_buffer, u32* out_bytes_read, bool add_null_terminate )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	mStreamImpl->SeekToBeginning();
	auto buffer_size = mStreamImpl->mSize;
	if( add_null_terminate ) 
		++buffer_size;

	out_buffer = (char*)malloc( buffer_size );
	StreamStatus::Enum status = mStreamImpl->Read( out_buffer, mStreamImpl->mSize, out_bytes_read );
	if ( status != StreamStatus::NoError )
	{
		free( out_buffer );
		out_buffer = NULL;
	}
	else
	{
		if( add_null_terminate )
		{
			poro_assert( out_buffer );
			++(*out_bytes_read);
			out_buffer[ mStreamImpl->mSize ] = '\0';
		}
	}
	return status;
}

StreamStatus::Enum ReadStream::ReadTextLine( char* out_buffer, u32 buffer_capacity, u32* out_length_read )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	// TODO: optimize
	std::string text;
	StreamStatus::Enum status = mStreamImpl->ReadTextLine( &text );
	if ( status != StreamStatus::NoError )
		return status;

	int len = std::min( (size_t)buffer_capacity, text.length() );
	for ( int i = 0; i < len; i++ )
		out_buffer[ i ] = text[ i ];

	return status;
}

StreamStatus::Enum ReadStream::ReadTextLine( std::string& out_text )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	return mStreamImpl->ReadTextLine( &out_text );
}

StreamStatus::Enum ReadStream::ReadWholeTextFile( std::string& out_text )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	mStreamImpl->SeekToBeginning();
	std::stringstream text;
	char c;
	// TODO! This really should be used, this is pretty slow...
	while ( mStreamImpl->mFile.get( c ).good() )
		text.put( c );

	out_text = text.str();
	return StreamStatus::NoError;
}

// ===

void ReadStream::Close()
{ 
	if ( mStreamImpl )
	{
		mStreamImpl->Close();
		delete mStreamImpl;
		mStreamImpl = NULL;
	}
}

ReadStream::~ReadStream()
{
	Close();
}

ReadStream& ReadStream::operator= ( const ReadStream& other )
{
	if ( &other == this ) return *this;
	this->mDevice = other.mDevice;
	this->mStreamImpl = other.mStreamImpl;
	ReadStream& other2 = const_cast<ReadStream&>( other );
	other2.mDevice = NULL;
	other2.mStreamImpl = NULL;
	return *this;
}


// === FileSystem ================================

ReadStream FileSystem::OpenRead( const std::string& path )
{
	// use first device that is able to find a matching file
	ReadStream result = ReadStream();

	for ( auto& device : mDevices )
	{
		ReadStream stream = device->OpenRead( path );
		if ( stream.IsValid() )
		{
			result = stream;
			break;
		}
	}

	return result;
}

// ===

void FileSystem::OpenReadOnAllMatchingFiles( const std::string& path, std::vector<ReadStream>* out_files )
{
	ReadStream result = ReadStream();

	for ( auto& device : mDevices )
	{
		ReadStream stream = device->OpenRead( path );
		if ( stream.IsValid() )
			out_files->push_back( stream );
	}
}

// ===

void FileSystem::RemoveFile( const std::string& relative_path, FileLocation::Enum location )
{
	auto file = CompleteWritePath( location, relative_path );
	platform_impl::DeleteFileImpl( file );
}

bool FileSystem::RenameFile( const std::string& file_, const std::string& new_name_, FileLocation::Enum location )
{
	auto file = CompleteWritePath( location, file_ );
	auto new_name = CompleteWritePath( location, new_name_ );

	return platform_impl::RenameFile( file, new_name );
}

// ===

StreamStatus::Enum FileSystem::Read( const std::string& path, char* out_buffer, u32 buffer_capacity_bytes, u32* out_bytes_read )
{
	impl::UpdateReadImContext( this, path );
	return impl::gImCtx->readStream.Read( out_buffer, buffer_capacity_bytes, out_bytes_read );
}

StreamStatus::Enum FileSystem::ReadWholeFile( const std::string& path, char*& out_buffer, u32* out_bytes_read )
{
	impl::UpdateReadImContext( this, path );
	StreamStatus::Enum result = impl::gImCtx->readStream.ReadWholeFile( out_buffer, out_bytes_read, false );
	impl::CloseReadImContext( this );
	return result;
}

StreamStatus::Enum FileSystem::ReadWholeFileAndNullTerminate( const std::string& path, char*& out_buffer, poro::types::Uint32* out_bytes_read )
{
	impl::UpdateReadImContext( this, path );
	StreamStatus::Enum result = impl::gImCtx->readStream.ReadWholeFile( out_buffer, out_bytes_read, true );
	impl::CloseReadImContext( this );
	return result;
}

StreamStatus::Enum FileSystem::ReadTextLine( const std::string& path, char* out_buffer, u32 buffer_capacity, u32* out_length_read )
{
	impl::UpdateReadImContext( this, path );
	return impl::gImCtx->readStream.ReadTextLine( out_buffer, buffer_capacity, out_length_read );
}

StreamStatus::Enum FileSystem::ReadTextLine( const std::string& path, std::string& out_text )
{
	impl::UpdateReadImContext( this, path );
	return impl::gImCtx->readStream.ReadTextLine( out_text );
}

StreamStatus::Enum FileSystem::ReadWholeTextFile( const std::string& path, std::string& out_text )
{
	impl::CloseReadImContext( this );
	impl::UpdateReadImContext( this, path );
	StreamStatus::Enum result = impl::gImCtx->readStream.ReadWholeTextFile( out_text );
	impl::CloseReadImContext( this );
	return result;
}

void FileSystem::ReadTextLines( const std::string& path, std::vector<std::string>& out_text_lines )
{
	impl::CloseReadImContext( this );
	impl::UpdateReadImContext( this, path );
	std::string line;
	while ( impl::gImCtx->readStream.ReadTextLine( line ) )
		out_text_lines.push_back( line );
	impl::CloseReadImContext( this );
	return;
}

std::vector<std::string> FileSystem::ReadTextLines( const std::string& path )
{
	impl::CloseReadImContext(this);
	impl::UpdateReadImContext( this, path );
	std::vector<std::string> text_lines;
	std::string line;
	while ( impl::gImCtx->readStream.ReadTextLine( line ) )
		text_lines.push_back( line );
	impl::CloseReadImContext( this );
	return text_lines;
}

// ===

WriteStream FileSystem::OpenWrite( const std::string& path, u32 write_mode, FileLocation::Enum location )
{
	return mDefaultDevice->OpenWrite( location, path, write_mode );
}

// ===
StreamStatus::Enum FileSystem::WriteWholeFile( const std::string& path, char* data, u32 length_bytes, StreamWriteMode::Enum write_mode, FileLocation::Enum location )
{
	WriteStream stream = OpenWrite( path, write_mode, location );
	return stream.Write( data, length_bytes );
}

StreamStatus::Enum FileSystem::WriteWholeTextFile( const std::string& path, const std::string& text, StreamWriteMode::Enum write_mode, FileLocation::Enum location )
{
	WriteStream stream = OpenWrite( path, write_mode | StreamWriteMode::Ascii , location );
	return stream.Write( text );
}

// ===

std::wstring FileSystem::GetFullPathFromRelativePath( FileLocation::Enum location, const std::string& relative_path )
{
	return platform_impl::GetFullPath( location, relative_path );
}

void FileSystem::GetFiles( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_files )
{
	poro_assert( out_files );
	const std::wstring full_path = CompleteWritePath( location, path_relative_to_location );
	platform_impl::ListFiles( full_path, path_relative_to_location, out_files, false );
}

std::vector<std::string> FileSystem::GetFiles( FileLocation::Enum location, const std::string& path_relative_to_location )
{
	std::vector<std::string> result;
	GetFiles( location, path_relative_to_location, &result );
	return result;
}

void FileSystem::GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_directories )
{
	poro_assert( out_directories );
	const std::wstring full_path = CompleteWritePath( location, path_relative_to_location );
	platform_impl::ListFiles( full_path, path_relative_to_location, out_directories, true );
}

std::vector<std::string> FileSystem::GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location )
{
	std::vector<std::string> result;
	GetDirectories( location, path_relative_to_location, &result );
	return result;
}

// --

bool FileSystem::DoesExist( const std::string& path_relative_to_device_root )
{
	bool result = false;

	for ( auto& device : mDevices )
	{
		ReadStream stream = device->OpenRead( path_relative_to_device_root );
		if ( stream.IsValid() )
		{
			result = true;
			break;
		}
	}

	return result;
}

std::string FileSystem::GetDateForFile( const std::string& path_relative_to_device_root )
{
	bool result = false;

	for ( auto& device : mDevices )
	{
		ReadStream stream = device->OpenRead( path_relative_to_device_root );
		if ( stream.IsValid() )
		{
			const auto full_path = device->GetFullPath( path_relative_to_device_root );
			return platform_impl::GetDateForFile( full_path );
		}
	}

	cassert( false && "not implemented" );
	return "";
}

// ===

std::wstring FileSystem::CompleteReadPath( const std::string& path, const std::wstring& root_path )
{
	if ( path.empty() )
		return platform_impl::StringToWString( path );

	if ( path.size() > 5 && path[0] == '?' && path[1] == '?' )
	{
		std::lock_guard<std::mutex> lock( mMutex );

		const auto name = path.substr( 0, 5 );

		for ( auto& proxy : mPathProxies )
		{
			if ( proxy.name == name )
			{
				const std::string new_path = proxy.path + path.substr( 5, path.size() - 5 );
				return platform_impl::GetFullPath( proxy.location, new_path );
			}
		}

		poro_assert( false && "Path proxy not found" );
	}

	return platform_impl::CombinePath( root_path, path );
}

std::wstring FileSystem::CompleteWritePath( const poro::FileLocation::Enum location, const std::string& path )
{
	if ( path.empty() )
		return platform_impl::StringToWString( path );

	if ( path.size() > 5 && path[0] == '?' && path[1] == '?' )
	{
		std::lock_guard<std::mutex> lock( mMutex );

		const auto name = path.substr( 0, 5 );

		for ( auto& proxy : mPathProxies )
		{
			if ( proxy.name == name )
			{
				const std::string new_path = proxy.path + path.substr( 5, path.size() - 5 );
				return platform_impl::GetFullPath( proxy.location, new_path );
			}
		}

		poro_assert( false && "Path proxy not found" );
	}

	return platform_impl::GetFullPath( location, path );
}

// ===

void FileSystem::InitAndSetUserDataFolderName( const std::string& name )
{
	platform_impl::InitAndSetUserDataFolderName( name );
}

void FileSystem::SetPathProxy( const std::string& name, const poro::FileLocation::Enum to_location, const std::string& to_path )
{
	std::lock_guard<std::mutex> lock( mMutex );

	poro_assert( name.size() == 5 );
	poro_assert( name[0] == '?' );
	poro_assert( name[1] == '?' );

	PathProxy* proxy = NULL;

	for ( uint32 i = 0; i < mPathProxies.size(); i++ )
	{
		if ( mPathProxies[i].name == name )
		{
			proxy = &mPathProxies[i];
			break;
		}
	}

	if ( proxy == NULL )
	{
		mPathProxies.push_back( PathProxy() );
		proxy = &mPathProxies.back();
	}

	proxy->name = name;
	proxy->location = to_location;
	proxy->path = to_path;
}

void FileSystem::RemovePathProxy( const std::string& name )
{
	std::lock_guard<std::mutex> lock( mMutex );

	for ( uint32 i = 0; i < mPathProxies.size(); i++ )
	{
		if ( mPathProxies[i].name == name )
		{
			mPathProxies.erase( mPathProxies.begin()+i );
			break;
		}
	}
}

std::vector<IFileDevice*> FileSystem::GetDeviceList()
{
	return mDevices;
}

void FileSystem::SetDeviceList( std::vector<IFileDevice*> devices )
{
	mDevices = devices;
}

FileSystem::FileSystem()
{
	platform_impl::Init();
	mDefaultDevice = new DiskFileDevice( FileLocation::WorkingDirectory );

	mDefaultDevice2 = new DiskFileDevice( FileLocation::WorkingDirectory );
	mDevices.push_back( mDefaultDevice2 );
}

FileSystem::~FileSystem()
{
	delete mDefaultDevice;
	delete mDefaultDevice2;
	mDevices.clear();
}

// === DiskFileDevice ================================

DiskFileDevice::DiskFileDevice( const std::string& read_root_path_full )
{
	mReadRootPath = platform_impl::StringToWString( read_root_path_full );
}

DiskFileDevice::DiskFileDevice( FileLocation::Enum read_location, const std::string& read_root_path_relative_to_location )
{
	mReadRootPath = platform_impl::GetFullPath( read_location, read_root_path_relative_to_location );
}

// ===

ReadStream DiskFileDevice::OpenRead( const std::string& path_relative_to_device_root )
{
	const std::wstring full_path = Poro()->GetFileSystem()->CompleteReadPath( path_relative_to_device_root, mReadRootPath );

	StreamStatus::Enum status;
	ReadStream result;
	result.mDevice = this;
	result.mStreamImpl = new platform_impl::StreamInternal( full_path, true, &status, StreamWriteMode::Recreate ); // TODO: allocate from a pool
	if ( status != StreamStatus::NoError )
	{
		delete result.mStreamImpl;
		result.mStreamImpl = NULL;
	}
	return result;
}

WriteStream DiskFileDevice::OpenWrite( FileLocation::Enum location, const std::string& path_relative_to_location, u32 write_mode )
{
	const std::wstring full_path = Poro()->GetFileSystem()->CompleteWritePath( location, path_relative_to_location );

	StreamStatus::Enum status;
	WriteStream result;
	result.mDevice = this;
	result.mStreamImpl = new platform_impl::StreamInternal( full_path, false, &status, write_mode ); // TODO: allocate from a pool
	if ( status != StreamStatus::NoError )
	{
		delete result.mStreamImpl;
		result.mStreamImpl = NULL;
	}
	return result;
}

std::wstring DiskFileDevice::GetFullPath( const std::string& path_relative_to_device_root )
{
	return Poro()->GetFileSystem()->CompleteReadPath( path_relative_to_device_root, mReadRootPath );
}

} // end of namespace