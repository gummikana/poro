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

#include <mutex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <utils/filesystem/filesystem.h>

#ifdef PORO_PLAT_WINDOWS
#include "external/poro_windows.h"
#endif


namespace poro {

namespace platform_impl
{
	void Init();
    void GetFiles( const std::string& full_path, std::vector<std::string>* out_files );
    void GetDirectories( const std::string& full_path, std::vector<std::string>* out_directories );
    std::string GetFullPath( FileLocation::Enum location, const std::string& relative_path );
    std::string GetFullPath( FileLocation::Enum location );
    std::string CombinePath( const std::string& a, const std::string& b );

    struct StreamInternal
    {
        StreamInternal( std::string filename, bool read, StreamStatus::Enum* out_status )
        {
            *out_status = StreamStatus::AccessFailed;
            if ( read )
                mFile.open( filename.c_str(), std::ios::in | std::ios::binary );
            else
                mFile.open( filename.c_str(), std::ios::out | std::ios::binary );

            if ( mFile.is_open() == 0 )
                return;

			*out_status = StreamStatus::NoError;
            mSize = ceng::ReadFileSize( mFile );
            mPosition = 0;
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

		StreamStatus::Enum Write( char* buffer, u32 buffer_size_bytes )
		{
			if ( mFile.good() == false )
				return StreamStatus::AccessFailed;

			mFile.write( buffer, buffer_size_bytes );
			return StreamStatus::NoError;
		}

		StreamStatus::Enum WriteLineEnding()
		{
			const char* LINE_ENDING = "\r\n";
			mFile.write( LINE_ENDING, 2 );
			return StreamStatus::NoError;
		}

        void Close()
        {
            mFile.close();
        }


        std::fstream mFile;
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

    void GetFiles( const std::string& full_path, std::vector<std::string>* out_files )
    {
        // TODO:
    }

    void GetDirectories( const std::string& full_path, std::vector<std::string>* out_directories )
    {
        // TODO:
    }

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

    std::string CombinePath( const std::string& a, const std::string& b )
    {
		return a + b;
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

StreamStatus::Enum ReadStream::ReadWholeFile( char*& out_buffer, u32* out_bytes_read )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	out_bytes_read = 0;
	out_buffer = (char*)malloc( mStreamImpl->mSize );
	return mStreamImpl->Read( out_buffer, mStreamImpl->mSize, out_bytes_read );
}

StreamStatus::Enum ReadStream::ReadTextLine( char* out_buffer, u32 buffer_capacity, u32* out_length_read )
{
	if ( mStreamImpl == NULL ) return StreamStatus::AccessFailed;

	// TODO: optimize
	std::string text;
	StreamStatus::Enum status = mStreamImpl->ReadTextLine( &text );
	if ( status != StreamStatus::NoError )
		return status;

	int len = std::min( buffer_capacity, text.length() );
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

    std::stringstream text;
    char c;
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
    StreamStatus::Enum result = impl::gImCtx->readStream.ReadWholeFile( out_buffer, out_bytes_read );
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
    impl::UpdateReadImContext( this, path );
    StreamStatus::Enum result = impl::gImCtx->readStream.ReadWholeTextFile( out_text );
    impl::CloseReadImContext( this );
    return result;
}

void FileSystem::ReadTextLines( const std::string& path, std::vector<std::string>& out_text_lines )
{
	impl::UpdateReadImContext( this, path );
	std::string line;
	while ( impl::gImCtx->readStream.ReadTextLine( line ) )
		out_text_lines.push_back( line );
	return;
}

std::vector<std::string> FileSystem::ReadTextLines( const std::string& path )
{
	impl::UpdateReadImContext( this, path );
	std::vector<std::string> text_lines;
	std::string line;
	while ( impl::gImCtx->readStream.ReadTextLine( line ) )
		text_lines.push_back( line );
	return text_lines;
}

// ===

WriteStream FileSystem::OpenWrite( const std::string& path, StreamWriteMode::Enum write_mode, FileLocation::Enum location )
{
    return mDefaultDevice->OpenWrite( location, path, write_mode );
}

// ===
StreamStatus::Enum FileSystem::WriteWholeTextFile( const std::string& path, char* data, u32 length_bytes, StreamWriteMode::Enum write_mode, FileLocation::Enum location )
{
	WriteStream stream = OpenWrite( path, write_mode, location );
	return stream.Write( data, length_bytes );
}

StreamStatus::Enum FileSystem::WriteWholeTextFile( const std::string& path, const std::string& text, StreamWriteMode::Enum write_mode, FileLocation::Enum location )
{
	WriteStream stream = OpenWrite( path, write_mode, location );
	return stream.Write( text );
}

// ===

std::vector<std::string> FileSystem::GetFiles( FileLocation::Enum location, const std::string& path_relative_to_location )
{
    std::vector<std::string> result;
    const std::string full_path = platform_impl::GetFullPath( location, path_relative_to_location );
    platform_impl::GetFiles( full_path, &result );
    return result;
}

std::vector<std::string> FileSystem::GetFiles( std::string full_path )
{
    std::vector<std::string> result;
    platform_impl::GetFiles( full_path, &result );
    return result;
}

std::vector<std::string> FileSystem::GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location )
{
    std::vector<std::string> result;
    const std::string full_path = platform_impl::GetFullPath( location, path_relative_to_location );
    platform_impl::GetDirectories( full_path, &result );
    return result;
}

std::vector<std::string> FileSystem::GetDirectories( std::string full_path )
{
    std::vector<std::string> result;
    platform_impl::GetDirectories( full_path, &result );
    return result;
}

void FileSystem::GetFiles( FileLocation::Enum location, std::vector<std::string>* out_files )
{
    const std::string full_path = platform_impl::GetFullPath( location );
    platform_impl::GetFiles( full_path, out_files );
}

void FileSystem::GetFiles( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_files )
{
    const std::string full_path = platform_impl::GetFullPath( location, path_relative_to_location );
    platform_impl::GetFiles( full_path, out_files );
}

void FileSystem::GetFiles( std::string full_path, std::vector<std::string>* out_files )
{
    platform_impl::GetFiles( full_path, out_files );
}

void FileSystem::GetDirectories( FileLocation::Enum location, std::vector<std::string>* out_directories )
{
    const std::string full_path = platform_impl::GetFullPath( location );
    platform_impl::GetDirectories( full_path, out_directories );
}

void FileSystem::GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_directories )
{
    const std::string full_path = platform_impl::GetFullPath( location, path_relative_to_location );
    platform_impl::GetDirectories( full_path, out_directories );
}

void FileSystem::GetDirectories( std::string full_path, std::vector<std::string>* out_directories )
{
    platform_impl::GetDirectories( full_path, out_directories );
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

ReadStream DiskFileDevice::OpenRead( const std::string& path_relative_to_root )
{
    const std::string full_path = platform_impl::CombinePath( mReadRootPath, path_relative_to_root );
	StreamStatus::Enum status;
    ReadStream result;
    result.mDevice = this;
    result.mStreamImpl = new platform_impl::StreamInternal( full_path, true, &status ); // TODO: allocate from a pool
	if ( status != StreamStatus::NoError )
	{
		delete result.mStreamImpl;
		result.mStreamImpl = NULL;
	}
    return result;
}

WriteStream DiskFileDevice::OpenWrite( FileLocation::Enum location, const std::string& path_relative_to_location, StreamWriteMode::Enum write_mode )
{
    const std::string full_path = platform_impl::GetFullPath( location, path_relative_to_location );
	StreamStatus::Enum status;
    WriteStream result;
    result.mDevice = this;
    result.mStreamImpl = new platform_impl::StreamInternal( full_path, false, &status );// TODO: allocate from a pool
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