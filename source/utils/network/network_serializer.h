/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
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


#ifndef INC_NETWORK_SERIALIZER_H
#define INC_NETWORK_SERIALIZER_H

#include "network_utils.h"

#include <vector>

namespace network_utils 
{
	class ISerializer
	{
	public:
		virtual ~ISerializer() { }

		virtual bool IsSaving() const = 0;
		virtual bool IsLoading() const { return !IsSaving(); }
		virtual bool HasOverflowed() const = 0;
		
		virtual void IO( uint8		&value ) = 0;
		virtual void IO( int8		&value ) = 0;
		virtual void IO( uint16		&value ) = 0;
		virtual void IO( int16		&value ) = 0;
		virtual void IO( uint32		&value ) = 0;
		virtual void IO( int32		&value ) = 0;
		virtual void IO( uint64		&value ) = 0;
		virtual void IO( int64		&value ) = 0;
		virtual void IO( float32	&value ) = 0;
		virtual void IO( double64	&value ) = 0;
		virtual void IO( bool		&value ) = 0;
		virtual void IO( types::ustring& str ) = 0;
	
#if 0
		template< typename T >
		void IO( std::vector< T >& vector )
		{
			uint32 size = vector.size();
			IO( size );
			if( HasOverflowed() ) return;

			if( IsLoading() )
				vector.resize( size );

			for( uint32 i = 0; i < size; ++i )
			{
				T temp_var = vector[ i ];
				this->IO( temp_var );
				if( IsLoading() )
					vector[ i ] = temp_var;
			}
		}
#endif

#ifdef NETWORK_USE_VECTOR2
		template< typename T >
		void IO( ceng::math::CVector2< T >& vector2 )
		{
			T x = vector2.x;
			T y = vector2.y;
			IO( x );
			IO( y );

			if( IsLoading() )
			{
				vector2.x = x;
				vector2.y = y;
			}
		}
#endif

	};

	//-------------------------------------------------------------------------


	class CSerialSaver : virtual public ISerializer
	{
	protected:
		types::ustring mBuffer;
		uint32 mLength;
		uint32 mBytesUsed;
		bool mHasOverflowed;

	public:
		CSerialSaver()
		{
			// buffer=buf; length=size; bytesUsed=0; bHasOverflowed=false;
			mLength = 1024;
			mBuffer.reserve( 1024 );
			mBytesUsed = 0;
			mHasOverflowed = false;
		}

		void IO( uint8& value ) override
		{
			if ( mHasOverflowed ) return; //stop writing when overflowed
			mBuffer += value;
			mBytesUsed += sizeof( value );
		}

		void IO( int8& value ) override
		{
			if ( mHasOverflowed ) return; //stop writing when overflowed
			mBuffer += value;
			mBytesUsed += sizeof( value );
		}

		void IO( uint16& value ) override
		{
			if ( mHasOverflowed ) return; //stop writing when overflowed
			mBuffer += ConvertUint16ToHex( value );
			mBytesUsed += sizeof( value );
		}

		void IO( int16& value ) override
		{
			if ( mHasOverflowed ) return; //stop writing when overflowed
			mBuffer += ConvertUint16ToHex( value );
			mBytesUsed += sizeof(value);
		}

		void IO( uint32& value ) override
		{
			if( mHasOverflowed ) return; //stop writing when overflowed
			mBuffer += ConvertUint32ToHex( value );
			mBytesUsed += sizeof( value );
		}

		void IO( int32&	value ) override
		{
			if( mHasOverflowed ) return; //stop writing when overflowed
			mBuffer += ConvertInt32ToHex( value );
			mBytesUsed += sizeof( value );
		}

		void IO( uint64& value )  override
		{
			if( mHasOverflowed ) return; //stop writing when overflowed
			mBuffer += ConvertUint64ToHex( value );
			mBytesUsed += sizeof( value );
		}

		void IO( int64& value ) override
		{
			if( mHasOverflowed ) return; //stop writing when overflowed
			mBuffer += ConvertInt64ToHex( value );
			mBytesUsed += sizeof( value );
		}

		void IO( float32& value ) override
		{
			if( mHasOverflowed ) return; //stop writing when overflowed
			mBuffer += FloatToHexString( value );
			mBytesUsed += sizeof( value );
		}
		
		void IO( double64& value ) override
		{
			if( mHasOverflowed ) return; //stop writing when overflowed
			mBuffer += DoubleToHexString( value );
			mBytesUsed += sizeof( value );
		}
			
		void IO( bool& value ) override
		{
			if( mHasOverflowed ) return; //stop writing when overflowed
			uint8 v = (value)?1:0;
			IO( v );
		}

		void IO( types::ustring& str ) override
		{
			uint32 l = str.length();
			IO(l);
			if( mHasOverflowed ) return;
			
			mBuffer += str;
			mBytesUsed +=l;
		}

		void IO( const types::ustring& str )
		{
			uint32 l = str.length();
			IO(l);
			if( mHasOverflowed ) return;
			
			mBuffer += str;
			mBytesUsed +=l;
		}

		bool HasOverflowed() const { return mHasOverflowed; }
		bool IsSaving() const { return true; }

		const types::ustring& GetData() const { return mBuffer; }
	};

	//-------------------------------------------------------------------------

	class CSerialLoader : virtual public ISerializer
	{
	protected:
		types::ustring mBuffer;
		uint32 mBytesUsed;
		uint32 mLength;
		bool mHasOverflowed;
	public:

		CSerialLoader( const types::ustring& buf )
		{
			mBuffer = buf;
			mBytesUsed = 0; 
			mHasOverflowed = false;
			mLength = mBuffer.size();
		}

		void Reset()
		{
			mBytesUsed = 0;
			mHasOverflowed = false;
		}

		void IO( uint8& value ) override
		{
			if ( mHasOverflowed ) return;
			if ( mBytesUsed + sizeof( value ) > mLength ) { mHasOverflowed = true; return; }
			value = mBuffer[mBytesUsed];
			mBytesUsed += sizeof( value );
		}

		void IO( int8& value ) override
		{
			if( mHasOverflowed ) return; 
			if( mBytesUsed + sizeof( value ) > mLength ) { mHasOverflowed = true; return; }
			value = mBuffer[ mBytesUsed ];
			mBytesUsed += sizeof( value );
		}

		void IO( uint16& value ) override
		{
			if ( mHasOverflowed ) return;
			if ( mBytesUsed + sizeof(value) > mLength ) { mHasOverflowed = true; return; }
			value = ConvertHexToUint16( mBuffer.substr( mBytesUsed, sizeof( value ) ) );
			mBytesUsed += sizeof( value );
		}

		void IO( int16&	value ) override
		{
			if ( mHasOverflowed ) return;
			if ( mBytesUsed + sizeof( value ) > mLength ) { mHasOverflowed = true; return; }
			value = ConvertHexToInt16( mBuffer.substr( mBytesUsed, sizeof( value ) ) );
			mBytesUsed += sizeof( value );
		}

		void IO( uint32& value ) override
		{
			if( mHasOverflowed ) return; 
			if( mBytesUsed + sizeof( value ) > mLength ) { mHasOverflowed = true; return; }
			value = ConvertHexToUint32( mBuffer.substr( mBytesUsed, sizeof( value ) ) );
			mBytesUsed += sizeof( value );
		}

		void IO( int32&	value ) override
		{
			if( mHasOverflowed ) return; 
			if( mBytesUsed + sizeof( value ) > mLength ) { mHasOverflowed = true; return; }
			value = ConvertHexToInt32( mBuffer.substr( mBytesUsed, sizeof( value ) ) );
			mBytesUsed += sizeof( value );
		}

		void IO( uint64& value ) override
		{
			if( mHasOverflowed ) return; 
			if( mBytesUsed + sizeof( value ) > mLength ) { mHasOverflowed = true; return; }
			value = ConvertHexToUint64( mBuffer.substr( mBytesUsed, sizeof( value ) ) );
			mBytesUsed += sizeof( value );
		}

		void IO( int64&	value ) override
		{
			if( mHasOverflowed ) return; 
			if( mBytesUsed + sizeof( value ) > mLength ) { mHasOverflowed = true; return; }
			value = ConvertHexToInt64( mBuffer.substr( mBytesUsed, sizeof( value ) ) );
			mBytesUsed += sizeof( value );
		}

		void IO( float32& value ) override
		{
			if( mHasOverflowed ) return; 
			if( mBytesUsed + sizeof( value ) > mLength ) { mHasOverflowed = true; return; }
			value = HexStringToFloat( mBuffer.substr( mBytesUsed, sizeof( value ) ) );
			mBytesUsed += sizeof( value );
		}

		void IO( double64& value ) override
		{
			if( mHasOverflowed ) return; //stop writing when overflowed
			if( mBytesUsed + sizeof( value ) > mLength ) { mHasOverflowed = true; return; }
			value = HexStringToDouble( mBuffer.substr( mBytesUsed, sizeof( value ) ) );
			mBytesUsed += sizeof( value );
		}
			
		void IO( bool& value ) override
		{
			uint8 v = 0;
			IO( v );
			if( mHasOverflowed ) 
				return; 
			value = (v != 0)?1:0;
		}

		void IO( types::ustring& str ) override
		{
			uint32 len = 0;
			IO( len );
			if( mHasOverflowed ) return;
			if( mBytesUsed + len > mLength ) { mHasOverflowed = true; return; }
			str = mBuffer.substr( mBytesUsed, len );
			mBytesUsed += len;
		}

		bool HasOverflowed() const { return mHasOverflowed; }
		bool IsSaving() const { return false; }
	};

	//-------------------------------------------------------------------------
} // end o namespace network utils


// -------------- types ---------------------------

namespace types { 
	typedef network_utils::ISerializer		bit_serialize;
} // end of namespace types

#endif
