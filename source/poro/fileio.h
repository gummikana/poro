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

// TODO: all functions should use full file path if given one.


#ifndef INC_FILEIO_H
#define INC_FILEIO_H

#include <iostream>

#include <vector>
#include "poro_types.h"

// define PORO_CONSERVATIVE if you only need a single-threaded API
// #define PORO_CONSERVATIVE 1


namespace poro {
    namespace impl { struct ImContext;  }
    namespace platform_impl { struct StreamInternal; }
    // ================================

	namespace StreamWriteMode
	{
		enum Enum 
		{ 
			// Clear and rewrite the file.
			Recreate,
			// Write to the end of the existing file contents.
			Append,
		};
	}

    // ================================

	namespace StreamStatus
	{
		enum Enum {
			// Stream operation succeeded.
			NoError = 1,
			// Stream operation failed.
			AccessFailed = 0,
		};
	}

    // ================================

	namespace FileLocation
	{
		enum Enum { 
			// User documents directory. i.e. "My Documents" on windows.
			UserDocumentsDirectory,
			// Current working directory of the executable.
			WorkingDirectory,
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
		virtual std::string GetFullPath( const std::string& path_relative_to_device_root ) = 0;
	protected:
        virtual void Close( WriteStream* stream ) = 0;
        virtual void Close( ReadStream* stream ) = 0;
	};

    // ================================

	// Provides write access to a file.
	// The stream is automatically closed at the end of the scope in which it was created.
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
		StreamStatus::Enum Write( char* data, poro::types::Uint32 length_bytes );
		// write 'text' to the stream.
		StreamStatus::Enum Write( const std::string& text );
		// write 'text' to the stream, then a line ending.
		StreamStatus::Enum WriteLine( const std::string& text );
		// write a line ending to the stream.
		StreamStatus::Enum WriteEndOfLine();
		// Returns true if the stream was succesfully opened and has not been closed.
		bool IsValid() { return mStreamImpl != NULL; }
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

	// Provides read access to a file.
	// The stream is automatically closed at the end of the scope in which it was created.
	class ReadStream
	{
        friend class IFileDevice;
        friend class FileSystem;
        friend class DiskFileDevice;
        friend struct poro::impl::ImContext;
	public:
		ReadStream( const ReadStream& other ) { operator=( other ); }
		ReadStream& operator= ( const ReadStream& other );
		// Read 'buffer_capacity_bytes' bytes from the stream to 'out_buffer'. Actual number of bytes read is written to 'out_bytes_read'.
		StreamStatus::Enum Read             ( char* out_buffer, poro::types::Uint32 buffer_capacity_bytes, poro::types::Uint32* out_bytes_read );
		// Read one line of text from the stream to to 'out_buffer'. Actual number of bytes read is written to 'out_bytes_read'. 'buffer_capacity' specifies the maximum capacity of the buffer.
		StreamStatus::Enum ReadTextLine     ( char* out_buffer, poro::types::Uint32 buffer_capacity, poro::types::Uint32* out_length_read );
		// Read one line of text from the stream to 'out_text'.
		StreamStatus::Enum ReadTextLine     ( std::string& out_text );
		// Returns true if the stream was succesfully opened and has not been closed.
		bool IsValid() { return mStreamImpl != NULL; }
		// Close the stream and clean up all resources used by it.
        ~ReadStream();
	private:
		StreamStatus::Enum ReadWholeFile( char*& out_buffer, poro::types::Uint32* out_bytes_read );
		StreamStatus::Enum ReadWholeTextFile( std::string& out_text );
		void Close();
        ReadStream() { mDevice = NULL; mStreamImpl = NULL; }
        // data
		IFileDevice*                   mDevice;
        platform_impl::StreamInternal* mStreamImpl;
	};

    // ================================

	// FileSystem provides access to reading, writing and enumeration of files in various locations.
	// The API is thread safe if SetDeviceList and GetDeviceList are used correctly. In normal usage scenarious they'd be called at the initialization of a program, before any file operations take place.
	// Please see the documentation of SetDeviceList and GetDeviceList for more details.
	// Any ReadStream and WriteStream instances returned by this API should only be used ONLY in the thread in which they were created.
	//
	// SetDeviceList and GetDeviceList are intended to be used to change the locations where files are searched from. This can be used for example to build a game that supports multiple mods at the same time.
	// Each device provided via SetDeviceList is used as a "root" directory when searching for a file inside a _Read_ call.
	// All functions take relative file paths unless otherwise specified.
	class FileSystem
	{
	public:
		// === reading API ===
		// Open a file at 'relative_path' for reading. file is looked up from each FileDevice in current device list, starting with the first device.
		ReadStream OpenRead            ( const std::string& relative_path );
		// Open all files matching 'relative_path'. File is looked up from each FileDevice in current device list, starting with the first device. Device list can be modified using GetDeviceList and SetDeviceList.
		void       OpenReadOnAllMatchingFiles( const std::string& relative_path, std::vector<ReadStream>* out_files );

		// === immediate mode reading api ===
		// Read 'buffer_capacity_bytes' bytes from the file in 'relative_path' to 'out_buffer'. Actual number of bytes read is written to 'out_bytes_read'
		StreamStatus::Enum		 Read             ( const std::string& relative_path, char* out_buffer, poro::types::Uint32 buffer_capacity_bytes, poro::types::Uint32* out_bytes_read );
		// Read the entire contents of file at 'relative_path' to 'out_buffer'. 'out_buffer' is allocated during the call using 'malloc' and must be released by the user using 'free' when they're done with it. Number of bytes read is written to 'out_bytes_read'
		StreamStatus::Enum		 ReadWholeFile    ( const std::string& relative_path, char*& out_buffer, poro::types::Uint32* out_bytes_read );
		// Reads one line of text from the file at 'relative_path' to 'out_buffer'. consecutive calls to this function from current thread will continue reading from the same stream.
		StreamStatus::Enum		 ReadTextLine     ( const std::string& relative_path, char* out_buffer, poro::types::Uint32 buffer_capacity, poro::types::Uint32* out_length_read );
		// Reads one line of text from the file at 'relative_path' to 'out_text'. consecutive calls to this function from current thread will continue reading from the same stream.
		StreamStatus::Enum		 ReadTextLine     ( const std::string& relative_path, std::string& out_text );
		// Reads the entire contents of the file at 'relative_path' to 'out_text'.
		StreamStatus::Enum		 ReadWholeTextFile( const std::string& relative_path, std::string& out_text );

		// Reads the entire contents of the file at 'relative_path' to 'out_text_lines'.
		void					 ReadTextLines    ( const std::string& relative_path, std::vector<std::string>& out_text_lines );
		// Reads the entire contents of the file at 'relative_path' to a vector and returns the vector.
		std::vector<std::string> ReadTextLines    ( const std::string& relative_path );

		// === writing API ===
		// Open a file at 'relative_path' for reading.
		WriteStream OpenWrite( const std::string& relative_path, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );

		// === immediate mode writing api ===
		// Write 'length_bytes' bytes from data to the file at 'relative_path'.
		StreamStatus::Enum WriteWholeFile( const std::string& relative_path, char* data, poro::types::Uint32 length_bytes, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );
		// Write 'text' to the file at 'relative_path'.
		StreamStatus::Enum WriteWholeTextFile( const std::string& relative_path, const std::string& text,                      StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );

		// === file browsing API ===
		// Get the full location of a file.
		std::string GetFullPathFromRelativePath( const std::string& relative_path );

		// Get a list of files at the location specified by the parameters.
		std::vector<std::string> GetFiles      ( FileLocation::Enum location, const std::string& path_relative_to_location = "" );
		// Get a list of files at the location specified by the parameters.
		std::vector<std::string> GetFiles      ( std::string full_path );
		// Get a list of directories at the location specified by the parameters.
		std::vector<std::string> GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location = "" );
		// Get a list of directories at the location specified by the parameters.
		std::vector<std::string> GetDirectories( std::string full_path );

		// Get a list of files at the location specified by the parameters.
		void 					 GetFiles      ( FileLocation::Enum location, std::vector<std::string>* out_files );
		// Get a list of files at the location specified by the parameters.
		void 					 GetFiles      ( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_files );
		// Get a list of files at the location specified by 'full_path'.
		void                     GetFiles      ( std::string full_path, std::vector<std::string>* out_files );
		// Get a list of directories at the location specified by the parameters.
        void                     GetDirectories( FileLocation::Enum location, std::vector<std::string>* out_directories );
		// Get a list of directories at the location specified by the parameters.
		void                     GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_directories );
		// Get a list of directories at the location specified by 'full_path'.
		void                     GetDirectories( std::string full_path, std::vector<std::string>* out_directories );

		// === initialization API ===
		// Get the list of file devices currently in use. NOTE: this shouldn't be called while any file operations started via this API are in progress. Only devices created by the user should be destroyed by the user.
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

        virtual ReadStream    OpenRead( const std::string& path_relative_to_device_root ) override;
        virtual WriteStream   OpenWrite( FileLocation::Enum location, const std::string& path_relative_to_location, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate ) override;
        virtual std::string   GetReadRootPath() override;
		virtual std::string   GetFullPath( const std::string& path_relative_to_device_root ) override;
    protected:
        virtual void Close( WriteStream* stream ) override;
        virtual void Close( ReadStream* stream ) override;
    private:
        std::string mReadRootPath;
	};

	// ================================

	struct CharBufferAutoFree
	{
		char*				memory;
		poro::types::Uint32 size_bytes;

		CharBufferAutoFree()
		{
			memory = NULL;
			size_bytes = 0;
		}

		~CharBufferAutoFree()
		{
			if ( memory )
				free( memory );
			memory = NULL;
			size_bytes = 0;
		}
	};
}

#endif