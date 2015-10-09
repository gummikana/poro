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

#ifndef INC_FILEIO_H
#define INC_FILEIO_H

#include <vector>
#include "poro_types.h"


// TODO: get rid of u32 typedef

//#define PORO_CONSERVATIVE 1


#ifdef PORO_CONSERVATIVE
    #define PORO_THREAD_LOCAL static
#else
    #define PORO_THREAD_LOCAL __declspec(thread)
#endif


namespace poro {
    typedef poro::types::Uint32 u32;
    namespace impl { struct ImContext;  }
    namespace platform_impl { struct StreamInternal; }
    // ================================

	namespace StreamWriteMode
	{
		enum Enum 
		{ 
			Recreate, // clear and rewrite the file
			Append,   // write to the end of the existing file contents
		};
	}

    // ================================

	namespace StreamStatus
	{
		enum Enum { 
			NoError = 1,      // stream operation succeeded
			AccessFailed = 0, // stream operation failed
		};
	}

    // ================================

	namespace FileLocation
	{
		enum Enum { 
			UserDocumentsDirectory, // user documents directory. i.e. "My Documents" on windows
			WorkingDirectory,       // current working directory of the executable
		};
	}

    // ================================

	class IFileDevice
	{
        friend class ReadStream;
        friend class WriteStream;
	public:
        virtual ReadStream  OpenRead( const std::string& path ) = 0;
        virtual WriteStream OpenWrite( FileLocation::Enum location, const std::string& read_path_relative_to_location, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate ) = 0;
        virtual std::string GetReadRootPath() = 0;
	protected:
        virtual void Close( WriteStream* stream ) = 0;
        virtual void Close( ReadStream* stream ) = 0;
	};

    // ================================

	class WriteStream
	{
	    friend class IFileDevice;
        friend class FileSystem;
        friend class DiskFileDevice;
        friend struct poro::impl::ImContext;
	public:
		WriteStream( const WriteStream& other ) { operator=( other ); }
        WriteStream& operator= ( const WriteStream& other );
		// write 'length_bytes' bytes from 'data' to the stream.
		StreamStatus::Enum Write( char* data, u32 length_bytes );
		// write 'text' to the stream.
		StreamStatus::Enum Write( const std::string& text );
		// write 'text' to the stream, then a line ending.
		StreamStatus::Enum WriteLine( const std::string& text );
		// close the stream and clean up all resources used by it.
        ~WriteStream();
	private:
		void Close();
        WriteStream() { mDevice = NULL; mStreamImpl = NULL; }
        // data
        IFileDevice*                   mDevice;
        platform_impl::StreamInternal* mStreamImpl;
	};

    // ================================

	class ReadStream
	{
        friend class IFileDevice;
        friend class FileSystem;
        friend class DiskFileDevice;
        friend struct poro::impl::ImContext;
	public:
		ReadStream( const ReadStream& other ) { operator=( other ); }
		ReadStream& operator= ( const ReadStream& other );
		// read 'buffer_capacity_bytes' bytes from the stream to 'out_buffer'. actual number of bytes read is written to 'out_bytes_read'.
		StreamStatus::Enum Read             ( char* out_buffer, u32 buffer_capacity_bytes, u32* out_bytes_read );
		// read one line of text from the stream to to 'out_buffer'. actual number of bytes read is written to 'out_bytes_read'. 'buffer_capacity' specifies the maximum capacity of the buffer.
		StreamStatus::Enum ReadTextLine     ( char* out_buffer, u32 buffer_capacity, u32* out_length_read );
		// read one line of text from the stream to 'out_text'.
		StreamStatus::Enum ReadTextLine     ( std::string& out_text );
		// close the stream and clean up all resources used by it.
        ~ReadStream();
	private:
		StreamStatus::Enum ReadWholeFile( char*& out_buffer, u32* out_bytes_read );
		StreamStatus::Enum ReadWholeTextFile( std::string& out_text );
		void Close();
        ReadStream() { mDevice = NULL; mStreamImpl = NULL; }
        // data
		IFileDevice*                   mDevice;
        platform_impl::StreamInternal* mStreamImpl;
	};

    // ================================

	class FileSystem
	{
	public:
		// === reading API ===
		// open a file at 'relative_path' for reading. file is looked up from each FileDevice in current device list, starting with the first device.
		ReadStream OpenRead            ( const std::string& relative_path );
		// open all files matching 'relative_path'. file is looked up from each FileDevice in current device list, starting with the first device. device list can be modified using GetDeviceList and SetDeviceList
		void       OpenReadOnAllMatchingFiles( const std::string& relative_path, std::vector<ReadStream>* out_files );

		// === immediate mode reading api ===
		// read 'buffer_capacity_bytes' bytes from the file in 'relative_path' to 'out_buffer'. actual number of bytes read is written to 'out_bytes_read'
		StreamStatus::Enum		 Read             ( const std::string& relative_path, char* out_buffer, u32 buffer_capacity_bytes, u32* out_bytes_read );
		// read the entire contents of file at 'relative_path' to out_buffer. out_buffer is allocated during the call and must be released by user using 'free' when they're done with it. number of bytes read is written to 'out_bytes_read'
		StreamStatus::Enum		 ReadWholeFile    ( const std::string& relative_path, char*& out_buffer, u32* out_bytes_read );
		// reads one line of text from the file at 'relative_path' to 'out_buffer'. consecutive calls to this function from current thread will continue reading from the same stream.
		StreamStatus::Enum		 ReadTextLine     ( const std::string& relative_path, char* out_buffer, u32 buffer_capacity, u32* out_length_read );
		// reads one line of text from the file at 'relative_path' to 'out_text'. consecutive calls to this function from current thread will continue reading from the same stream.
		StreamStatus::Enum		 ReadTextLine     ( const std::string& relative_path, std::string& out_text );
		// reads the entire contents of the file at 'relative_path' to 'out_text'.
		StreamStatus::Enum		 ReadWholeTextFile( const std::string& relative_path, std::string& out_text );

		// reads the entire contents of the file at 'relative_path' to 'out_text_lines'.
		void					 ReadTextLines    ( const std::string& relative_path, std::vector<std::string>& out_text_lines );
		// reads the entire contents of the file at 'relative_path' to a vector and returns the vector.
		std::vector<std::string> ReadTextLines    ( const std::string& relative_path );

		// === writing API ===
		// open a file at 'relative_path' for reading.
		WriteStream OpenWrite( const std::string& relative_path, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );

		// === immediate mode writing api ===
		// write 'length_bytes' bytes from data to the file at 'relative_path'
		StreamStatus::Enum WriteWholeTextFile( const std::string& relative_path, char* data, u32 length_bytes, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );
		// write 'text' to the file at 'relative_path'
		StreamStatus::Enum WriteWholeTextFile( const std::string& relative_path, const std::string& text,      StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );

		// === file browsing API ===
		// get a list of files at the location specified by the parameters.
		std::vector<std::string> GetFiles      ( FileLocation::Enum location, const std::string& path_relative_to_location = "" );
		// get a list of files at the location specified by the parameters.
		std::vector<std::string> GetFiles      ( std::string full_path );
		// get a list of directories at the location specified by the parameters.
		std::vector<std::string> GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location = "" );
		// get a list of directories at the location specified by the parameters.
		std::vector<std::string> GetDirectories( std::string full_path );

		// get a list of files at the location specified by the parameters.
		void 					 GetFiles      ( FileLocation::Enum location, std::vector<std::string>* out_files );
		// get a list of files at the location specified by the parameters.
		void 					 GetFiles      ( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_files );
		// get a list of files at the location specified by 'full_path'.
		void                     GetFiles      ( std::string full_path, std::vector<std::string>* out_files );
		// get a list of directories at the location specified by the parameters.
        void                     GetDirectories( FileLocation::Enum location, std::vector<std::string>* out_directories );
		// get a list of directories at the location specified by the parameters.
		void                     GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_directories );
		// get a list of directories at the location specified by 'full_path'.
		void                     GetDirectories( std::string full_path, std::vector<std::string>* out_directories );

		// === initialization API ===
		// get the list of file devices currently in use. NOTE: this shouldn't be called while any file operations started via this API are in progress. only devices created by the user should be destroyed by the user.
		std::vector<IFileDevice*> GetDeviceList();
		// set the list of file devices currently in use. NOTE: this shouldn't be called while any file operations started via this API are in progress.
		void SetDeviceList( std::vector<IFileDevice*> devices );

        FileSystem();
        ~FileSystem();
	private:
        // data
        std::vector<IFileDevice*> mDevices;
        IFileDevice* mDefaultDevice;
		IFileDevice* mDefaultDevice2;
	};

	// ================================

	class DiskFileDevice : public IFileDevice
	{
    public:
		DiskFileDevice( const std::string& read_root_path_full );
		DiskFileDevice( FileLocation::Enum read_location, const std::string& read_root_path_relative_to_location = "" );

        virtual ReadStream    OpenRead( const std::string& path_relative_to_root ) override;
        virtual WriteStream   OpenWrite( FileLocation::Enum location, const std::string& path_relative_to_location, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate ) override;
        virtual std::string   GetReadRootPath() override;
    protected:
        virtual void Close( WriteStream* stream ) override;
        virtual void Close( ReadStream* stream ) override;
    private:
        std::string mReadRootPath;
	};
}

#endif