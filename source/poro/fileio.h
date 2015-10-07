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
		enum Enum { Recreate, Append, };
	}

    // ================================

	namespace StreamStatus
	{
		enum Enum { NoError = 1, AccessFailed = 0, };
	}

    // ================================

	namespace FileLocation
	{
		enum Enum { UserDocumentsDirectory, WorkingDirectory, };
	}

    // ================================

	class IFileDevice
	{
        friend class ReadStream;
        friend class WriteStream;
	public:
		virtual ReadStream  OpenRead( const std::string& path );
		virtual WriteStream OpenWrite( FileLocation::Enum location, const std::string& read_path_relative_to_location, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate );
		virtual std::string GetReadRootPath();
	protected:
		virtual void Close( WriteStream* stream );
		virtual void Close( ReadStream* stream );
	};

    // ================================

	class WriteStream
	{
	    friend class IFileDevice;
        friend class FileSystem;
        friend class DiskFileDevice;
        friend struct poro::impl::ImContext;
	public:
        WriteStream( const WriteStream& other ) { mDevice = other.mDevice; mStream = other.mStream; }
		StreamStatus::Enum Write( char* data, u32 length_bytes );
		StreamStatus::Enum Write( const std::string& text );
        ~WriteStream();
	private:
		void Close();
        WriteStream() { mDevice = NULL; mStream = NULL; }
        // data
        IFileDevice*                   mDevice;
        platform_impl::StreamInternal* mStream;
	};

    // ================================

	class ReadStream
	{
        friend class IFileDevice;
        friend class FileSystem;
        friend class DiskFileDevice;
        friend struct poro::impl::ImContext;
	public:
        ReadStream( const ReadStream& other ) { mDevice = other.mDevice; mStream = other.mStream; }
		StreamStatus::Enum Read             ( char* out_buffer, u32 buffer_capacity_bytes, u32* out_bytes_read );
		StreamStatus::Enum ReadWholeFile    ( char** out_buffer, u32* out_bytes_read );
		StreamStatus::Enum ReadTextLine     ( char* out_buffer, u32 buffer_capacity, u32* out_length_read );
		StreamStatus::Enum ReadTextLine     ( std::string* out_text );
		StreamStatus::Enum ReadWholeTextFile( char* out_buffer, u32 buffer_capacity, u32* out_length_read );
		StreamStatus::Enum ReadWholeTextFile( char* out_buffer, u32* out_length_read );
		StreamStatus::Enum ReadWholeTextFile( std::string& out_text );
        ~ReadStream();
	private:
		void Close();
        ReadStream() { mDevice = NULL; mStream = NULL; }
        // data
		IFileDevice*                   mDevice;
        platform_impl::StreamInternal* mStream;
	};

    // ================================

	class FileSystem
	{
	public:
		// reading API
		ReadStream Read                ( const std::string& path );
		void       ReadAllMatchingFiles( const std::string& path, std::vector<ReadStream>* out_files );
		//StreamStatus::Enum ReadAllMatchingFiles( const std::string& path, std::function<void(ReadStream&, const std::string&)> reader_function );  

		// immediate mode reading api
		StreamStatus::Enum Read             ( const std::string& path, char* out_buffer, u32 buffer_capacity_bytes, u32* out_bytes_read );
		StreamStatus::Enum ReadWholeFile    ( const std::string& path, char** out_buffer, u32* out_bytes_read );
		StreamStatus::Enum ReadTextLine     ( const std::string& path, char* out_buffer, u32 buffer_capacity, u32* out_length_read );
		StreamStatus::Enum ReadTextLine     ( const std::string& path, std::string* out_text );
		StreamStatus::Enum ReadWholeTextFile( const std::string& path, char* out_buffer, u32 buffer_capacity, u32* out_length_read );
		StreamStatus::Enum ReadWholeTextFile( const std::string& path, char* out_buffer, u32* out_length_read );
		StreamStatus::Enum ReadWholeTextFile( const std::string& path, std::string& out_text );

		// writing API
		WriteStream Write( const std::string& path, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );

		// immediate mode writing api
		StreamStatus::Enum WriteSome( const std::string& path, char* data, u32 length_bytes, StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );
		StreamStatus::Enum WriteSome( const std::string& path, const std::string& text,      StreamWriteMode::Enum write_mode = StreamWriteMode::Enum::Recreate, FileLocation::Enum location = FileLocation::Enum::UserDocumentsDirectory );

		// file browsing API
		virtual std::vector<std::string> GetFiles      ( FileLocation::Enum location, const std::string& path_relative_to_location = "" );
		virtual std::vector<std::string> GetFiles      ( std::string full_path );
		virtual std::vector<std::string> GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location = "" );
		virtual std::vector<std::string> GetDirectories( std::string full_path );

		virtual void 					 GetFiles      ( FileLocation::Enum location, std::vector<std::string>* out_files );
		virtual void 					 GetFiles      ( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_files );
		virtual void                     GetFiles      ( std::string full_path, std::vector<std::string>* out_files );
        virtual void                     GetDirectories( FileLocation::Enum location, std::vector<std::string>* out_directories );
		virtual void                     GetDirectories( FileLocation::Enum location, const std::string& path_relative_to_location, std::vector<std::string>* out_directories );
		virtual void                     GetDirectories( std::string full_path, std::vector<std::string>* out_directories );

		// initialization API
		std::vector<IFileDevice*> GetDeviceList();
		void SetDeviceList( std::vector<IFileDevice*> devices );
	private:
		FileSystem();
		~FileSystem();
		void OnFrameEnd();
        // data
        std::vector<IFileDevice*> mDevices;
        IFileDevice* mDefaultDevice;
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