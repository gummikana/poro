#include "fileio.h"

#include <mutex>
#include <fstream>
#include <sstream>
#include <utils/filesystem/filesystem.h>
#include <algorithm>

namespace poro {

namespace platform_impl
{
    void GetFiles( const std::string& full_path, std::vector<std::string>* out_files );
    void GetDirectories( const std::string& full_path, std::vector<std::string>* out_directories );
    std::string GetFullPath( FileLocation::Enum location, const std::string& relative_path );
    std::string GetFullPath( FileLocation::Enum location );
    std::string CombinePath( const std::string& a, const std::string& b );

    struct StreamInternal
    {
        StreamInternal( std::string filename, bool read )
        {
            mStatus = StreamStatus::AccessFailed;
            if ( read )
                mFile.open( filename.c_str(), std::ios::in | std::ios::binary );
            else
                mFile.open( filename.c_str(), std::ios::out | std::ios::binary );

            if ( mFile.is_open() == 0 )
                return;

            mStatus = StreamStatus::NoError;
            mSize = ceng::ReadFileSize( mFile );
            mPosition = 0;
        }

        void Read( char* out_buffer, u32 buffer_capacity_bytes, u32* out_bytes_read )
        {
            long read_size = std::min( mSize - mPosition, (long)buffer_capacity_bytes );
            mPosition += read_size;
            mFile.read( out_buffer, read_size );
            *out_bytes_read = (u32)read_size;
        }

        /*void ReadText( char* out_buffer, u32 buffer_capacity_bytes, u32* out_bytes_read )
        {
        long read_size = std::min( mSize - mPosition, (long)buffer_capacity_bytes );
        long count = 0;
        char c;
        while ( count < read_size && mFile.get( c ).good() )
        out_buffer[ count++ ] = c;
        mPosition += read_size;
        *out_bytes_read = (u32)read_size;
        }*/

        void Close()
        {
            mFile.close();
        }


        StreamStatus::Enum mStatus;
        std::fstream mFile;
        long mSize;
        long mPosition;
    };
}

#ifdef PORO_PLAT_WINDOWS
namespace platform_impl
{
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
        // TODO:
        return "";
    }

    std::string GetFullPath( FileLocation::Enum location )
    {
        // TODO:
        return "";
    }

    std::string CombinePath( const std::string& a, const std::string& b )
    {
        // TODO:
        return "";
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
        ReadStream               readStream;
        std::string              readPath;

        WriteStream              writeStream;
        std::string              writePath;
        FileLocation::Enum       writeLocation;
        StreamWriteMode::Enum    writeMode;
    };

    PORO_THREAD_LOCAL ImContext* gImCtx;
    std::vector<WriteStream> gWriteStreams;
#ifndef PORO_CONSERVATIVE
    std::mutex gWriteStreamsMutex; // the immediate mode API is not thread safe in case PORO_CONSERVATIVE is defined
#endif

    void UpdateReadImContext( FileSystem* file_system, const std::string& new_path )
    {
        if ( gImCtx == NULL )
            gImCtx = new ImContext();

        if ( new_path != gImCtx->readPath )
        {
            gImCtx->writeStream.~WriteStream();
            gImCtx->readPath = new_path;
            gImCtx->readStream = file_system->Read( new_path );
        }
    }

    void UpdateWriteImContext( FileSystem* file_system, const std::string& new_path, FileLocation::Enum new_location, StreamWriteMode::Enum write_mode )
    {
        if ( gImCtx == NULL )
            gImCtx = new ImContext();

        if ( new_path != gImCtx->writePath || new_location != gImCtx->writeLocation || write_mode != gImCtx->writeMode )
        {
            gImCtx->writeStream.~WriteStream();
            gImCtx->writePath = new_path;
            gImCtx->writeLocation = new_location;
            gImCtx->writeMode = write_mode;
            gImCtx->writeStream = file_system->Write( new_path, write_mode, new_location );

            {
#ifndef PORO_CONSERVATIVE
                std::unique_lock< std::mutex >( gWriteStreamsMutex );
#endif
                gWriteStreams.push_back( gImCtx->writeStream );
            }
        }
    }

    void OnFrameEndUpdateImContexts()
    {
        if ( gImCtx == NULL )
            gImCtx = new ImContext();

        {
#ifndef PORO_CONSERVATIVE
            std::unique_lock< std::mutex >( gWriteStreamsMutex );
#endif
            for ( size_t i = 0; i < gWriteStreams.size(); i++ )
                gWriteStreams[ i ].~WriteStream();
            gWriteStreams.clear();
        }
    }
}


// === WriteStream ================================

StreamStatus::Enum WriteStream::Write( char* data, u32 length_bytes )
{
    // TODO:
    return StreamStatus::NoError;
}

StreamStatus::Enum WriteStream::Write( const std::string& text )
{
    // TODO:
    return StreamStatus::NoError;
}

// ===

void WriteStream::Close()
{
    if ( mStream )
    {
        mDevice->Close( this );
        mStream->Close();
        delete mStream;
        mStream = NULL;
    }
}

WriteStream::~WriteStream()
{
    Close();
}


// === ReadStream ================================

StreamStatus::Enum ReadStream::Read( char* out_buffer, u32 buffer_capacity_bytes, u32* out_bytes_read )
{
    if ( mStream == NULL )                           return StreamStatus::AccessFailed;
    if ( mStream->mStatus != StreamStatus::NoError ) return mStream->mStatus;
    mStream->Read( out_buffer, buffer_capacity_bytes, out_bytes_read );
    return StreamStatus::NoError;
}

StreamStatus::Enum ReadStream::ReadWholeFile( char*& out_buffer, u32* out_bytes_read )
{
    if ( mStream == NULL )                           return StreamStatus::AccessFailed;
    if ( mStream->mStatus != StreamStatus::NoError ) return mStream->mStatus;
    // TODO: implement
    return StreamStatus::NoError;
}

StreamStatus::Enum ReadStream::ReadTextLine( char* out_buffer, u32 buffer_capacity, u32* out_length_read )
{
    if ( mStream == NULL )                           return StreamStatus::AccessFailed;
    if ( mStream->mStatus != StreamStatus::NoError ) return mStream->mStatus;
    mStream->Read( out_buffer, buffer_capacity, out_length_read );
    return StreamStatus::NoError;
}

StreamStatus::Enum ReadStream::ReadTextLine( std::string* out_text )
{
    if ( mStream == NULL )                           return StreamStatus::AccessFailed;
    if ( mStream->mStatus != StreamStatus::NoError ) return mStream->mStatus;

    if ( mStream->mFile.good() == false )
        return StreamStatus::AccessFailed;

    std::string line;
    std::getline( mStream->mFile, line );
    *out_text = line;
    return StreamStatus::NoError;
}

StreamStatus::Enum ReadStream::ReadWholeTextFile( char* out_buffer, u32 buffer_capacity, u32* out_length_read )
{
    if ( mStream == NULL )                           return StreamStatus::AccessFailed;
    if ( mStream->mStatus != StreamStatus::NoError ) return mStream->mStatus;
    mStream->Read( out_buffer, buffer_capacity, out_length_read );
    return StreamStatus::NoError;
}

StreamStatus::Enum ReadStream::ReadWholeTextFile( char*& out_buffer, u32* out_length_read )
{
    if ( mStream == NULL )                           return StreamStatus::AccessFailed;
    if ( mStream->mStatus != StreamStatus::NoError ) return mStream->mStatus;
    // TODO: implement
    return StreamStatus::NoError;
}

StreamStatus::Enum ReadStream::ReadWholeTextFile( std::string& out_text )
{
    if ( mStream == NULL )                           return StreamStatus::AccessFailed;
    if ( mStream->mStatus != StreamStatus::NoError ) return mStream->mStatus;

    std::stringstream text;
    char c;
    while ( mStream->mFile.get( c ).good() )
        text.put( c );

    out_text = text.str();
    return StreamStatus::NoError;
}

// ===

void ReadStream::Close()
{ 
    if ( mStream )
    {
        mDevice->Close( this );
        mStream->Close();
        delete mStream;
        mStream = NULL;
    }
}

ReadStream::~ReadStream()
{
    Close();
}


// === FileSystem ================================

ReadStream FileSystem::Read( const std::string& path )
{
    ReadStream result = ReadStream();
    for ( size_t i = 0; i < mDevices.size(); i++ )
    {
        IFileDevice* device = mDevices[i];
        ReadStream stream = device->OpenRead( path );
        if ( stream.mStream->mStatus == StreamStatus::NoError )
        {
            result = stream;
            break;
        }
    }
    return result;
}

// ===

void FileSystem::ReadAllMatchingFiles( const std::string& path, std::vector<ReadStream>* out_files )
{
    ReadStream result = ReadStream();
    for ( size_t i = 0; i < mDevices.size(); i++ )
    {
        IFileDevice* device = mDevices[ i ];
        ReadStream stream = device->OpenRead( path );
        if ( stream.mStream->mStatus == StreamStatus::NoError )
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
    return impl::gImCtx->readStream.ReadWholeFile( out_buffer, out_bytes_read );
}

StreamStatus::Enum FileSystem::ReadTextLine( const std::string& path, char* out_buffer, u32 buffer_capacity, u32* out_length_read )
{
    impl::UpdateReadImContext( this, path );
    return impl::gImCtx->readStream.ReadTextLine( out_buffer, buffer_capacity, out_length_read );
}

StreamStatus::Enum FileSystem::ReadTextLine( const std::string& path, std::string* out_text )
{
    impl::UpdateReadImContext( this, path );
    return impl::gImCtx->readStream.ReadTextLine( out_text );
}

StreamStatus::Enum FileSystem::ReadWholeTextFile( const std::string& path, char* out_buffer, u32 buffer_capacity, u32* out_length_read )
{
    impl::UpdateReadImContext( this, path );
    return impl::gImCtx->readStream.ReadWholeTextFile( out_buffer, buffer_capacity, out_length_read );
}

StreamStatus::Enum FileSystem::ReadWholeTextFile( const std::string& path, char*& out_buffer, u32* out_length_read )
{
    impl::UpdateReadImContext( this, path );
    return impl::gImCtx->readStream.ReadWholeTextFile( out_buffer, out_length_read );
}

StreamStatus::Enum FileSystem::ReadWholeTextFile( const std::string& path, std::string& out_text )
{
    impl::UpdateReadImContext( this, path );
    return impl::gImCtx->readStream.ReadWholeTextFile( out_text );
}

// ===

WriteStream FileSystem::Write( const std::string& path, StreamWriteMode::Enum write_mode, FileLocation::Enum location )
{
    return mDefaultDevice->OpenWrite( location, path, write_mode );
}


// ===

StreamStatus::Enum FileSystem::WriteSome( const std::string& path, char* data, u32 length_bytes, StreamWriteMode::Enum write_mode, FileLocation::Enum location )
{
    impl::UpdateWriteImContext( this, path, location, write_mode );
    return impl::gImCtx->writeStream.Write( data, length_bytes );
}
StreamStatus::Enum FileSystem::WriteSome( const std::string& path, const std::string& text, StreamWriteMode::Enum write_mode, FileLocation::Enum location )
{
    impl::UpdateWriteImContext( this, path, location, write_mode );
    return impl::gImCtx->writeStream.Write( text );
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
    mDefaultDevice = new DiskFileDevice( FileLocation::WorkingDirectory );
    mDevices.push_back( mDefaultDevice );
}

FileSystem::~FileSystem()
{
    mDevices.clear();
    delete mDefaultDevice;
}

void FileSystem::OnFrameEnd()
{
    impl::OnFrameEndUpdateImContexts();
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
    bool file_opened_successfully = true;
    ReadStream result;
    result.mDevice = this;
    result.mStream = new platform_impl::StreamInternal( full_path, true ); // TODO: allocate from a pool
    return result;
}

WriteStream DiskFileDevice::OpenWrite( FileLocation::Enum location, const std::string& path_relative_to_location, StreamWriteMode::Enum write_mode )
{
    const std::string full_path = platform_impl::GetFullPath( location, path_relative_to_location );
    WriteStream result;
    result.mDevice = this;
    result.mStream = new platform_impl::StreamInternal( full_path, false );// TODO: allocate from a pool
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