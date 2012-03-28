#ifndef INC_CXMLPTR_SERIALIZER_H
#define INC_CXMLPTR_SERIALIZER_H

#include <map>

#include "../xml/cxml.h"
#include "../singleton/csingletonptr.h"


namespace ceng {
//-----------------------------------------------------------------------------


// WARNING! When saving the pointers need to exist through out the whole saving process
// 
// used through ceng::GetSingletonPtr< CXmlPtrSerializer< T > >() 
template< typename T >
class CXmlPtrSerializer /*: public CSingletonPtr< CXmlPtrSerializer< T > >*/
{
public:

	CXmlPtrSerializer() { }
	~CXmlPtrSerializer() { }

	int SavePointer( T* ptr )
	{
		if( ptr == NULL ) return -1;

		// check if we've already stored the pointer and return it's id
		for( typename std::map< int, T* >::iterator i = mPointers.begin(); i != mPointers.end(); ++i )
			if( i->second == ptr ) return i->first;

		// we didn't find it, create a new entry for it
		int id = mPointers.size();
		mPointers[ id ] = ptr;

		return id;
	}

	T* LoadPointer( int handle )
	{
		if( handle == -1 ) return NULL;

		cassert( mPointers.empty() == false );
		// cassert( mPointers[ handle ] );
		
		// create an "empty" pointer if there isn't a pointer there already
		if( mPointers[ handle ] == NULL ) {
			logger << "Warning! Loading " << GetItemName() << " from CXmlPtrSerializer with the id: " << handle << " before that has been created, might cause errors later on" << std::endl;  
			mPointers[ handle ] = new T;
		}

		return mPointers[ handle ];
	}

	template< class PtrType >
	struct PtrHelper 
	{
		PtrHelper() : id( 0 ), ptr( NULL ) { }
		PtrHelper( int id, PtrType* ptr ) : id( id ), ptr( ptr ) { }

		int id;
		PtrType* ptr;


		void Serialize( ceng::CXmlFileSys* filesys )
		{
#ifdef _DEBUG
			std::stringstream ss;
			ss << ptr;
			std::string debug_address = ss.str();
			XML_BindAttributeAlias( filesys, debug_address, "__id_debug_address__" );
#endif

			XML_BindAttributeAlias( filesys, id, "__id_ptr__" );

			if( ptr ) 
			ptr->Serialize( filesys );
		}
	};

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		if( filesys->IsWriting() )
		{
			std::string item_name = GetItemName();
			for( typename std::map< int, T* >::iterator i = mPointers.begin(); i != mPointers.end(); ++i )
			{
				PtrHelper< T > ptr( i->first, i->second );
				XML_BindAlias( filesys, ptr, item_name );
			}

			// should we clean after our selves and delete ourselves? 
			// or at least clean the mPointers so that we can be checked that we've been saved
		}
		else if ( filesys->IsReading() )
		{
			// we should make sure mPointers is empty
			cassert( mPointers.empty() );

			std::string item_name = GetItemName();

			int i = 0;
			for( i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
			{
				cassert( filesys->GetNode()->GetChild( i )->GetName() == item_name );
				if( filesys->GetNode()->GetChild( i )->GetName() == item_name )
				{
					
					// we're using NULL here so we can load the id first and then the actual pointer
					// the reason for this is to make sure we don't have to delete the pointer 
					// we've created in case there's already a pointer created for it, because it was 
					// needed before we had the chance to load it
					PtrHelper< T > ptr( -1, NULL );
					ceng::XmlConvertTo( filesys->GetNode()->GetChild( i ), ptr );
					
					cassert( ptr.id >= 0 );

					// this happens in the cases were the pointer was needed before it was loaded
					if( mPointers[ ptr.id ] ) {
						ptr.ptr = mPointers[ ptr.id ];
						ceng::XmlConvertTo( filesys->GetNode()->GetChild( i ), ptr );
					} else {
						ptr.ptr = new T;
						ceng::XmlConvertTo( filesys->GetNode()->GetChild( i ), ptr );
					}

					mPointers[ ptr.id ] = ptr.ptr;
				}
			}
		}
	}

	std::string GetName() 
	{
		// we trust that these are unique
		std::string class_name = GetItemName();
		return class_name + "_PTR_COLLECTION"; 
	}
	
private:
	
	std::string GetItemName() 
	{
		// we trust that these are unique
		std::string class_name;
		if( typeid( T ).name() ) class_name = typeid( T ).name();

		cassert( class_name != "" );

		class_name = ceng::StringReplace( "::", "_", class_name ); 
		class_name = ceng::StringReplace( "class ", "", class_name );
		class_name = ceng::StringReplace( "`anonymous namespace'", "", class_name );
		class_name = ceng::Lowercase( class_name );
		if( class_name.empty() == false ) class_name[ 0 ] = toupper( class_name[ 0 ] );

		return class_name; 
	}

	std::map< int, T* > mPointers;

};

//-----------------------------------------------------------------------------


template< class T >
int CXmlPtrSerializer_Save( T* ptr ) 
{
	return ceng::GetSingletonPtr< ceng::CXmlPtrSerializer< T > >()->SavePointer( ptr );
}

template< class T >
T* CXmlPtrSerializer_Load( T* ptr, int handle ) 
{
	return ceng::GetSingletonPtr< ceng::CXmlPtrSerializer< T > >()->LoadPointer( handle );
}

//-----------------------------------------------------------------------------

#define XML_BindPtrAlias( x_filesys, y_ptr, z_name ) \
		if( x_filesys && x_filesys->IsWriting() ) { \
			int id = ceng::CXmlPtrSerializer_Save( y_ptr ); \
			XML_BindAlias( x_filesys, id, z_name ); \
		} else if( x_filesys && x_filesys->IsReading() ) { \
			int id = -1; \
			XML_BindAlias( x_filesys, id, z_name ); \
			y_ptr = ceng::CXmlPtrSerializer_Load( y_ptr, id ); \
		}

#define XML_BindPtr( x_filesys, y_ptr ) XML_BindPtrAlias( x_filesys, y_ptr, #y_ptr )

//-----------------------------------------------------------------------------

#define XML_LOADPOINTERS_Begin( x_filesys, y_class_type ) \
	if( x_filesys && filesys->IsReading() ) { \
		ceng::CXmlPtrSerializer< y_class_type >* t = ceng::GetSingletonPtr< ceng::CXmlPtrSerializer< y_class_type > >(); \
		XML_BindAlias( x_filesys, *t, t->GetName() ); \
	}

	
#define XML_SAVEPOINTERS_End( x_filesys, y_class_type ) \
	if( x_filesys && x_filesys->IsWriting() ) { \
		ceng::CXmlPtrSerializer< y_class_type >* t = ceng::GetSingletonPtr< ceng::CXmlPtrSerializer< y_class_type > >(); \
		XML_BindAlias( x_filesys, *t, t->GetName() ); \
	} \
	ceng::CSingletonPtr< ceng::CXmlPtrSerializer< y_class_type > >::Delete();

//-----------------------------------------------------------------------------

} // end of namespace ceng

#endif 
