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

#include <vector>
#include <mutex>
#include "poro_types.h"

// define PORO_CONSERVATIVE if you only need a single-threaded API
// #define PORO_CONSERVATIVE 1


namespace poro {
    namespace impl { struct FileImContext;  }

	// ================================

	namespace StreamWriteMode
	{
		enum Enum 
		{ 
			// Clear and rewrite the file.
			Recreate = 1,
			// Write to the end of the existing file contents.
			Append = 2,

			// these allow to define binary or ascii, if none is defined binary is used
			Binary = 4,
			Ascii = 8
		};
	}

    // ================================

	namespace StreamStatus
	{
		enum Enum
		{
			// Stream operation succeeded.
			NoError = 1,
			// Stream operation failed.
			AccessFailed = 0,
		};
	}

    // ================================

	namespace FileLocation
	{
		enum Enum
		{ 
			// User documents directory. i.e. "My Documents" on windows.
			UserDocumentsDirectory,
			// Current working directory of the executable.
			WorkingDirectory,
			UserDirectory = WorkingDirectory, // HACK: temp hack for WIP code
		};
	}

	struct FileDataTempReleaser
	{
		virtual void Release( const char* data ) { }
	};

	struct FileDataTemp
	{
		inline ~FileDataTemp()
		{
			if ( releaser )
				releaser->Release( data );
			data = NULL;
			data_size_bytes = 0;
		}

		template<typename TReleaser>
		void Init( const char* data, uint32 data_size_bytes )
		{
			static_assert( sizeof( TReleaser ) <= sizeof( mem ), "TReleaser doesnt fit in the buffer we've reserved for it" );
			this->data = data;
			this->data_size_bytes = data_size_bytes;
			releaser = new( mem )TReleaser(); // placement new to avoid temp allocation
		}

		inline bool IsValid()
		{
			return data != NULL && data_size_bytes > 0;
		}

		const char* data = NULL;
		uint32 data_size_bytes = 0;
	private:
		FileDataTempReleaser* releaser = NULL;
		char mem[4];
	};

    // ================================

	namespace platform_impl
	{
		struct IStreamInternal
		{
			virtual ~IStreamInternal() { }
			virtual StreamStatus::Enum Read( char* out_buffer, poro::types::Uint32 buffer_capacity_bytes, poro::types::Uint32* out_bytes_read ) = 0;
			virtual StreamStatus::Enum ReadTextLine( std::string* out_text ) = 0;
			virtual void ReadWholeFileTemp( FileDataTemp* out_data ) = 0;
			virtual StreamStatus::Enum Write( const char* buffer, poro::types::Uint32 buffer_size_bytes ) = 0;
			virtual StreamStatus::Enum WriteLineEnding() = 0;
			virtual void SeekToBeginning() = 0;
			virtual void FlushWrites() = 0;
			virtual void Close() = 0;

			long mSize = 0;
		};
	}

	// ================================

	class IFileDevice
	{
        friend class ReadStream;
        friend class WriteStream;
	public:
		virtual ~IFileDevice() { }
		virtual void Destroy() { }
        virtual ReadStream OpenRead( const std::string& path ) = 0;
		virtual ReadStream OpenRead( FileLocation::Enum location, const std::string& path_relative_to_location ) = 0;
        virtual WriteStream OpenWrite( FileLocation::Enum location, const std::string& read_path_relative_to_location, poro::types::Uint32 write_mode = StreamWriteMode::Enum::Recreate ) = 0;
		virtual std::wstring GetFullPath( const std::string& path_relative_to_device_root ) = 0;
		virtual void GetFiles( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_files ) { }
		virtual bool DoesExist( const std::string& path ) = 0;
		virtual std::string GetDateForFile( const std::string& filename ) { return ""; }
		virtual bool SupportsFileDates() { return false;  }
		virtual bool SupportsLegacyFileDates() { return false; }
		virtual bool SupportsGetFiles( FileLocation::Enum location, const std::string& path_relative_to_location ) { return false; } // this being false makes the code fall back to the non IFileDevice path 
	};

    // ================================

	// Provides write access to a file.
	// The stream is automatically closed at the end of the scope in which it was created.
	class WriteStream
	{
	    friend class IFileDevice;
        friend class FileSystem;
        friend class DiskFileDevice;
        friend struct poro::impl::FileImContext;
	public:
		WriteStream() { mDevice = NULL; mStreamImpl = NULL; }
		WriteStream( const WriteStream& other ) { operator=( other ); }
        WriteStream& operator= ( const WriteStream& other );
		
		// write 'length_bytes' bytes from 'data' to the stream.
		StreamStatus::Enum Write( const char* data, poro::types::Uint32 length_bytes );
		
		// write 'text' to the stream.
		StreamStatus::Enum Write( const std::string& text );
		
		// write 'text' to the stream, then a line ending.
		StreamStatus::Enum WriteLine( const std::string& text );
		
		// write a line ending to the stream.
		StreamStatus::Enum WriteEndOfLine();
		
		// flush all buffered writes to the file
		StreamStatus::Enum FlushWrites();
		
		// Returns true if the stream was succesfully opened and has not been closed.
		bool IsValid() { return mStreamImpl != NULL; }
		
		// close the stream and clean up all resources used by it.
        ~WriteStream();
	private:
		void Close();
        // data
        IFileDevice* mDevice;
        platform_impl::IStreamInternal* mStreamImpl;
	};

    // ================================

	// Provides read access to a file.
	// The stream is automatically closed at the end of the scope in which it was created.
	class ReadStream
	{
		friend class IFileDevice;
		friend class FileSystem;
		friend class DiskFileDevice;
		friend struct poro::impl::FileImContext;
	public:
		ReadStream( IFileDevice* device, platform_impl::IStreamInternal* stream ) { mDevice = device; mStreamImpl = stream; }
		ReadStream() { mDevice = NULL; mStreamImpl = NULL; }
		ReadStream( const ReadStream& other ) { operator=( other ); }
		ReadStream& operator= ( const ReadStream& other );
		
		// Read 'buffer_capacity_bytes' bytes from the stream to 'out_buffer'. Actual number of bytes read is written to 'out_bytes_read'.
		StreamStatus::Enum Read( char* out_buffer, poro::types::Uint32 buffer_capacity_bytes, poro::types::Uint32* out_bytes_read );
		
		// Read one line of text from the stream to to 'out_buffer'. Actual number of bytes read is written to 'out_bytes_read'. 'buffer_capacity' specifies the maximum capacity of the buffer.
		StreamStatus::Enum ReadTextLine( char* out_buffer, poro::types::Uint32 buffer_capacity, poro::types::Uint32* out_length_read );
		
		// Read one line of text from the stream to 'out_text'.
		StreamStatus::Enum ReadTextLine( std::string& out_text );
		
		// Reads the whole file. Does no allocations if possible.
		virtual void ReadWholeFileTemp( FileDataTemp* out_data );

		// Returns true if the stream was succesfully opened and has not been closed.
		bool IsValid() { return mStreamImpl != NULL; }
		
		// Close the stream and clean up all resources used by it.
		~ReadStream();
	private:
		StreamStatus::Enum ReadWholeFile( char*& out_buffer, poro::types::Uint32* out_bytes_read, bool add_null_terminate );
		void GetPointerToWholeFile( char*& out_buffer, poro::types::Uint32* out_size_bytes );
		StreamStatus::Enum ReadWholeTextFile( std::string& out_text );
		void Close();
		// data
		IFileDevice* mDevice = NULL;
		platform_impl::IStreamInternal* mStreamImpl = NULL;
	};

	// ================================

	// FileSystem provides access to reading, writing and enumeration of files in various locations.
	// The API is thread safe if SetDeviceList and GetDeviceList are used correctly. In normal usage scenarious they'd be called at the initialization of a program, before any file operations take place.
	// Please see the documentation of SetDeviceList and GetDeviceList for more details.
	// Any ReadStream and WriteStream instances returned by this API should be used ONLY in the thread in which they were created.
	//
	// SetDeviceList and GetDeviceList are intended to be used to change the locations where files are searched from. This can be used for example to build a game that supports multiple mods at the same time.
	// Each device provided via SetDeviceList is used as a "root" directory when searching for a file inside a _Read_ call.
	// All functions take relative file paths unless otherwise specified.
	class FileSystem
	{
	public:
		// === reading API ===
		// Open a file at 'relative_path' for reading. File is looked up from each FileDevice in current device list, starting with the first device.
		ReadStream OpenRead( const std::string& relative_path );

		// Open a file at 'relative_path' for reading. File is opened using the default file device.
		ReadStream OpenRead( poro::FileLocation::Enum location, const std::string& relative_path );

		// Open all files matching 'relative_path'. File is looked up from each FileDevice in current device list, starting with the first device. Device list can be modified using GetDeviceList and SetDeviceList.
		void OpenReadOnAllMatchingFiles( const std::string& relative_path, std::vector<ReadStream>* out_files );

		void RemoveFile( const std::string& relative_path, FileLocation::Enum location = FileLocation::UserDocumentsDirectory, bool report_errors = true );

		bool RenameFile( const std::string& file, const std::string& new_name, poro::FileLocation::Enum location = FileLocation::UserDocumentsDirectory );

		// === immediate mode reading api ===
		// Read 'buffer_capacity_bytes' bytes from the file in 'relative_path' to 'out_buffer'. Actual number of bytes read is written to 'out_bytes_read'
		StreamStatus::Enum Read( const std::string& relative_path, char* out_buffer, poro::types::Uint32 buffer_capacity_bytes, poro::types::Uint32* out_bytes_read );

		// Read the entire contents of file at 'relative_path' to 'out_buffer'. 'out_buffer' is allocated during the call using 'malloc' and must be released by the user using 'free' when they're done with it. Number of bytes read is written to 'out_bytes_read'
		StreamStatus::Enum ReadWholeFile( const std::string& relative_path, char*& out_buffer, poro::types::Uint32* out_bytes_read );
		
		// Reads the entire file and adds a '\0' to the end
		StreamStatus::Enum ReadWholeFileAndNullTerminate( const std::string& relative_path, char*& out_buffer, poro::types::Uint32* out_bytes_read );

		// Read the entire contents of file at 'relative_path'. Any allocated buffers are automatically freed when the returned object is destructed. Doesn't do any allocations if possible.
		void ReadWholeFileTemp( const std::string& relative_path, FileDataTemp* out_data );

		// Reads one line of text from the file at 'relative_path' to 'out_buffer'. consecutive calls to this function from current thread will continue reading from the same stream.
		StreamStatus::Enum ReadTextLine( const std::string& relative_path, char* out_buffer, poro::types::Uint32 buffer_capacity, poro::types::Uint32* out_length_read );
		
		// Reads one line of text from the file at 'relative_path' to 'out_text'. consecutive calls to this function from current thread will continue reading from the same stream.
		StreamStatus::Enum ReadTextLine( const std::string& relative_path, std::string& out_text );
		
		// Reads the entire contents of the file at 'relative_path' to 'out_text'.
		StreamStatus::Enum ReadWholeTextFile( const std::string& relative_path, std::string& out_text );

		// Reads the entire contents of the file at 'relative_path' to 'out_text_lines'.
		void ReadTextLines( const std::string& relative_path, std::vector<std::string>& out_text_lines );
		
		// Reads the entire contents of the file at 'relative_path' to a vector and returns the vector.
		std::vector<std::string> ReadTextLines( const std::string& relative_path );

		// === writing API ===

		// Open a file at 'relative_path' for reading.
		WriteStream OpenWrite( const std::string& relative_path, poro::types::Uint32 write_mode = StreamWriteMode::Recreate, FileLocation::Enum location = FileLocation::UserDocumentsDirectory );

		// === immediate mode writing api ===

		// Write 'length_bytes' bytes from data to the file at 'relative_path'.
		StreamStatus::Enum WriteWholeFile( const std::string& relative_path, const char* data, poro::types::Uint32 length_bytes, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );
		// Write 'text' to the file at 'relative_path'.
		StreamStatus::Enum WriteWholeTextFile( const std::string& relative_path, const std::string& text, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );

		// === file browsing API ===

		// Get the full location of a file.
		std::wstring GetFullPathFromRelativePath( FileLocation::Enum location, const std::string& relative_path );
		std::wstring CompleteReadPath( const std::string& path, const std::wstring& root_path );
		std::wstring CompleteWritePath( const poro::FileLocation::Enum to_location, const std::string& path );

		// Get a list of files at the location specified by the parameters.
		void GetFiles( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_files );
		std::vector<std::string> GetFiles( FileLocation::Enum location, const std::string& path_relative_to_location );

		// Get a list of directories at the location specified by the parameters.
		void GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_directories );
		std::vector<std::string> GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location );
		
		// returns true if file at 'relative_path' exists in some of the file devices
		bool DoesExist( const std::string& path_relative_to_device_root );

		// returns true if file at 'relative_path' exists in some in location
		bool DoesExist( poro::FileLocation::Enum location, const std::string& path_relative_to_device_root );

		// ---
		std::string GetDateForFile( const std::string& path_relative_to_device_root );

		// === initialization API ===

		// Inits working directory and sets the name of the folder that contains user data, inside user data directory. Should be provided without slashes. For example "poro_game".
		void InitAndSetUserDataFolderName( const std::string& name );

		// Adds a path proxy. A proxy should start with "??" followed by a three-letter name, for example "??SAV"
		void SetPathProxy( const std::string& name, const poro::FileLocation::Enum to_location, const std::string& to_path );

		void RemovePathProxy( const std::string& name );

		// Get the list of file devices currently available NOTE: this shouldn't be called while any file operations started via this API are in progress. Only devices created by the user should be destroyed by the user.
		std::vector<IFileDevice*> GetDeviceList();
		
		// set the list of file devices currently available. NOTE: this shouldn't be called while any file operations started via this API are in progress.
		void SetDeviceList( std::vector<IFileDevice*> devices );

        FileSystem();
        ~FileSystem();
	private:
		friend class DiskFileDevice;


		struct PathProxy
		{
			std::string name;
			poro::FileLocation::Enum location;
			std::string path;
		};

        // data
        std::vector<IFileDevice*> mDevices;
		std::vector<PathProxy> mPathProxies;
		std::mutex mMutex;
        IFileDevice* mDefaultDevice;
		IFileDevice* mDefaultDevice2;
	};

	// ================================

	class DiskFileDevice : public IFileDevice
	{
    public:
		DiskFileDevice( const std::string& read_root_path_full );
		DiskFileDevice( FileLocation::Enum read_location, const std::string& read_root_path_relative_to_location = "" );

        virtual ReadStream OpenRead( const std::string& path_relative_to_device_root ) override;
		virtual ReadStream OpenRead( FileLocation::Enum location, const std::string& path_relative_to_location ) override;
        virtual WriteStream	OpenWrite( FileLocation::Enum location, const std::string& path_relative_to_location, poro::types::Uint32 write_mode = StreamWriteMode::Enum::Recreate ) override;
		virtual std::wstring GetFullPath( const std::string& path_relative_to_device_root ) override;
		virtual bool DoesExist( const std::string& path ) override;
		virtual bool SupportsLegacyFileDates() override;

    private:
        std::wstring mReadRootPath;
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