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

#ifndef PORO_CONSERVATIVE
#include <mutex>
#endif

#include <fstream>
#include <sstream>
#include <algorithm>
#include <utils/filesystem/filesystem.h>

#ifdef PORO_PLAT_WINDOWS
#include "external/poro_windows.h"
#endif

#include "poro_macros.h"

typedef poro::types::Uint32 u32;


namespace poro {

namespace platform_impl
{
	void Init();
	void GetFiles( const std::string& full_path, std::vector<std::string>* out_files );
	void GetDirectories( const std::string& full_path, std::vector<std::string>* out_directories );
	std::string GetFullPath( FileLocation::Enum location, const std::string& relative_path );
	std::string GetFullPath( FileLocation::Enum location );
	std::string CombinePath( std::string root, std::string relative_to_root );



	struct StreamInternal
	{
		StreamInternal( std::string filename, bool read, StreamStatus::Enum* out_status, u32 write_mode )
		{
			*out_status = StreamStatus::AccessFailed;

			if ( read )
			{
				mWriteBuffer = NULL;
				mFile.open( filename.c_str(), std::ios::in | std::ios::binary );
			}
			else
			{
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
		void Init()
		{

		}

		void ListFiles( const std::string& full_path, std::vector<std::string>* out_files, bool list_directories )
		{
			poro_assert( out_files );

			struct _finddata_t file_info;
			int file_handle;
			std::string find_path = CombinePath( full_path, "*.*" );
			// find first file...
			if( (file_handle = _findfirst( find_path.c_str(), &file_info )) != -1 )
			{
				do {
					if( (file_info.attrib & _A_SUBDIR ) )
					{
						if( list_directories ) 
							out_files->push_back( CombinePath( full_path, file_info.name ) );
					}
					else
					{
						if( list_directories == false ) 
							out_files->push_back( CombinePath( full_path, file_info.name ) );
					}

				} while( _findnext( file_handle, &file_info ) == 0 );
				_findclose( file_handle );
			}
			else
			{
				// nothing found, or an error happened
			}
		}

		/*void GetDirectories( const std::string& full_path, std::vector<std::string>* out_directories )
		{
			// TODO:
		}*/

		std::string GetFullPath( FileLocation::Enum location, const std::string& relative_path )
		{
			std::string a = GetFullPath( location );
			return CombinePath( a, relative_path );
		}

		std::string GetFullPath( FileLocation::Enum location )
		{
			TCHAR path[ MAX_PATH ];
			std::string result;
			if ( location == FileLocation::UserDocumentsDirectory )
			{
				if ( SUCCEEDED( SHGetFolderPath( NULL,
					CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
					NULL,
					0,
					path ) ) )
				{
					result = std::string( path );
				}
			}
			else
			{
				GetCurrentDirectory( MAX_PATH, path );
				result = std::string( path );
			}
		
			// clean the path
			std::replace( result.begin(), result.end(), '\\', '/' );
			if ( result[ result.length() - 1 ] != '/' )
				result = result + '/';
			return result;
		}

		static bool IsSeparator( char c ) { return c == '\\' ||c == '/'; }
		static char GetSeparator( const std::string& path )
		{
			for( std::size_t i = 0; i < path.size(); ++i )
			{
				if(path[i] == '\\' ) return '\\';
				if(path[i] == '/' ) return '/';
			}
			return '\\';
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

			// ideally we would use this but because we won't because winapi designers are horrible people.
			//if ( PathIsRelative( relative_to_root.c_str() ) == false )
			//	return relative_to_root;
			if( root.size() > 1 && IsSeparator( root.back() ) && IsSeparator( relative_to_root[0] ) ) 
				root.pop_back();

			if( !IsSeparator( root.back() ) && !IsSeparator( relative_to_root[0] ) ) 
				return root + GetSeparator( root ) + relative_to_root;
			
			poro_assert( IsSeparator( root.back() ) || IsSeparator( relative_to_root[0] ));

			return root + relative_to_root;
		}
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
	struct ImContext
	{
		ReadStream            readStream;
		std::string           readPath;
	};

	PORO_THREAD_LOCAL ImContext* gImCtx;

	void UpdateReadImContext( FileSystem* file_system, const std::string& new_path )
	{
		if ( gImCtx == NULL )
			gImCtx = new ImContext();

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
			gImCtx = new ImContext();

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

	return mStreamImpl->Write( const_cast<char*>( text.c_str() ), text.length() );
	return StreamStatus::NoError;
}

StreamStatus::Enum WriteStream::WriteLine( const std::string& text )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	StreamStatus::Enum status = mStreamImpl->Write( const_cast<char*>( text.c_str() ), text.length() );
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
		mDevice->Close( this );
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
		mDevice->Close( this );
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
	for ( size_t i = 0; i < mDevices.size(); i++ )
	{
		IFileDevice* device = mDevices[i];
		ReadStream stream = device->OpenRead( path );
		if ( stream.mStreamImpl )
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
	for ( size_t i = 0; i < mDevices.size(); i++ )
	{
		IFileDevice* device = mDevices[ i ];
		ReadStream stream = device->OpenRead( path );
		if ( stream.mStreamImpl )
			out_files->push_back( stream );
	}
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

std::string FileSystem::GetFullPathFromRelativePath( const std::string& relative_path )
{
	// use first device that is able to find a matching file
	ReadStream result = ReadStream();
	for ( size_t i = 0; i < mDevices.size(); i++ )
	{
		IFileDevice* device = mDevices[ i ];
		const std::string full_path = device->GetFullPath( relative_path );
		if ( ceng::DoesExist( full_path ) )
			return full_path;
	}
	return "";
}


std::vector<std::string> FileSystem::GetFiles( FileLocation::Enum location, const std::string& path_relative_to_location )
{
	std::vector<std::string> result;
	GetFiles( location, path_relative_to_location, &result );
	return result;
}

std::vector<std::string> FileSystem::GetFiles( std::string full_path )
{
	std::vector<std::string> result;
	GetFiles( full_path, &result );
	return result;
}

std::vector<std::string> FileSystem::GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location )
{
	std::vector<std::string> result;
	GetDirectories( location, path_relative_to_location, &result );
	return result;
}

std::vector<std::string> FileSystem::GetDirectories( std::string full_path )
{
	std::vector<std::string> result;
	GetDirectories( full_path, &result );
	return result;
}

void FileSystem::GetFiles( FileLocation::Enum location, std::vector<std::string>* out_files )
{
	const std::string full_path = platform_impl::GetFullPath( location );
	platform_impl::ListFiles( full_path, out_files, false );
}

void FileSystem::GetFiles( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_files )
{
	const std::string full_path = platform_impl::GetFullPath( location, path_relative_to_location );
	platform_impl::ListFiles( full_path, out_files, false );
}

void FileSystem::GetFiles( std::string full_path, std::vector<std::string>* out_files )
{
	platform_impl::ListFiles( full_path, out_files, false );
}

void FileSystem::GetDirectories( FileLocation::Enum location, std::vector<std::string>* out_directories )
{
	const std::string full_path = platform_impl::GetFullPath( location );
	platform_impl::ListFiles( full_path, out_directories, true );
}

void FileSystem::GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_directories )
{
	const std::string full_path = platform_impl::GetFullPath( location, path_relative_to_location );
	platform_impl::ListFiles( full_path, out_directories, true );
}

void FileSystem::GetDirectories( std::string full_path, std::vector<std::string>* out_directories )
{
	platform_impl::ListFiles( full_path, out_directories, true );
}

// ===

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
	mReadRootPath = read_root_path_full;
}

DiskFileDevice::DiskFileDevice( FileLocation::Enum read_location, const std::string& read_root_path_relative_to_location )
{
	mReadRootPath = platform_impl::GetFullPath( read_location, read_root_path_relative_to_location );
}

// ===

ReadStream DiskFileDevice::OpenRead( const std::string& path_relative_to_device_root )
{
	const std::string full_path = GetFullPath( path_relative_to_device_root );
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
	const std::string full_path = platform_impl::GetFullPath( location, path_relative_to_location );
	StreamStatus::Enum status;
	WriteStream result;
	result.mDevice = this;
	result.mStreamImpl = new platform_impl::StreamInternal( full_path, false, &status, write_mode );// TODO: allocate from a pool
	if ( status != StreamStatus::NoError )
	{
		delete result.mStreamImpl;
		result.mStreamImpl = NULL;
	}
	return result;
}

std::string DiskFileDevice::GetReadRootPath()
{
	return mReadRootPath;
}

std::string DiskFileDevice::GetFullPath( const std::string& path_relative_to_device_root )
{
	return platform_impl::CombinePath( mReadRootPath, path_relative_to_device_root );
}

// ===
	
void DiskFileDevice::Close( WriteStream* stream )
{
	// do nothing
}

void DiskFileDevice::Close( ReadStream* stream )
{
	// do nothing
}

} // end of namespace