///////////////////////////////////////////////////////////////////////////////
//
// CWidgetFactory
// ==============
//
// A Automated widget factory for serializing widgets of different types
//
//
// Created 31.12.2005 by Pete
//
//=============================================================================
//.............................................................................
#ifndef INC_UI_CWIDGETFACTORY_H
#define INC_UI_CWIDGETFACTORY_H

#include <map>
#include "ui_utils.h"


namespace ceng {
namespace ui {

class CWidget;

namespace impl 
{
	///////////////////////////////////////////////////////////////////////////

	class IGenericMaker
	{
	public:
		virtual ~IGenericMaker() { }

		virtual CWidget* GetWidget() = 0;
	};

	///////////////////////////////////////////////////////////////////////////

	template< class T >
	class CGenericMakerImpl : public IGenericMaker
	{
	public:
		CWidget* GetWidget() 
		{
			return new T;
		}

		static std::string name;
	};

	template< class T > std::string CGenericMakerImpl< T >::name;
	///////////////////////////////////////////////////////////////////////////
}

class CWidgetFactory : public CStaticSingleton< CWidgetFactory >
{
public:
	
	template< class T >
	class CWidgetRegisterer
	{
	public:
		CWidgetRegisterer( T* t, const std::string& name )
		{
			CWidgetFactory::GetSingletonPtr()->RegisterWidget( name, t );
		}
	};

	~CWidgetFactory() 
	{ 
		while ( !myRegistery.empty() )
		{
			delete myRegistery.begin()->second;
			myRegistery.erase( myRegistery.begin() );
		}
	}

	//=========================================================================

	CWidget* CreateWidget( const std::string& type )
	{
		std::map< std::string, impl::IGenericMaker* >::iterator i;

		i = myRegistery.find( type );

		if ( i != myRegistery.end() )
		{
			
			return i->second->GetWidget();

		} else {
			return NULL;
		}	
	}

	//=========================================================================

	void RegisterHandCraftedWidget( const std::string& name, impl::IGenericMaker* t )
	{
		myRegistery.insert( std::pair< std::string, impl::IGenericMaker* >( name, t ) );
	}

	//.........................................................................

	template< class T >
	void RegisterWidget( const std::string& name, T* type )
	{
		impl::CGenericMakerImpl< T >::name = name;

		std::map< std::string, impl::IGenericMaker* >::iterator i = myRegistery.find( name );
		
		if( i == myRegistery.end() )
			myRegistery.insert( std::pair< std::string, impl::IGenericMaker* >( name, new impl::CGenericMakerImpl< T > ) );
	}

	//.........................................................................


	//=========================================================================

	template< class T >
	std::string GetName( const T& t )
	{
		return impl::CGenericMakerImpl< T >::name;
	}

	///////////////////////////////////////////////////////////////////////////
private:
	CWidgetFactory() { }

	std::map< std::string, impl::IGenericMaker* > myRegistery;

	friend class CStaticSingleton< CWidgetFactory >;
};

// template< typename T > std::string CWidgetFactory::CGenericMakerImpl< T >::name;

void InitUI();

} // end of namespace ui
} // end of namespace ceng

#define WIDGET_REGISTER( x ) ::ceng::ui::CWidgetFactory::CWidgetRegisterer< x > CENG_Join( hidden_widget_registerer, __LINE__ )( NULL, #x )

#endif