///////////////////////////////////////////////////////////////////////////////
//
// config_macro.h
// ==================
//
//	This is a collection of macros that are used to create configuration classes
//	extremely easily and as little repeated lines as possible
//	You can check the test file for more examples
//
//  Here's an example how to use these macros
/*
	
	#define CONFIG_TEST(list_) \
		list_(float, player_speed,	10)                 

	DEFINE_CONFIG( ConfigPlayer, CONFIG_TEST )
	
*/
//
//
//	Here's the output of the macro example above:
/*
	class ConfigPlayer {
	public:

		float speed;

		ConfigPlayer() {
			speed = 10;
		}

		void Serialize( ceng::CXmlFileSys* filesys ) {
			XML_BindAttribute( filesys, speed );
		}
	};
*/
//	
//	As you can see the macros are there to make it faster and more efficient
//	to do configuration classes. 
//
//
// Created 23.10.2010 by Petri Purho (petri.purho@gmail.com)
//.............................................................................
//
//=============================================================================
#ifndef INC_CONFIG_MACRO_H
#define INC_CONFIG_MACRO_H

namespace ceng {

#define CONFIG_HEADER_DECLARE(type, name, value) \
    type name;

#define CONFIG_XML_BIND_DECLARE(type, name, value) \
	XML_BindAttribute( filesys, name );

#define CONFIG_INIT_MEMBERS(type, name, value) \
	name = value;


#define DEFINE_CONFIG(name, list)                 \
    class name { \
	public: \
		name() { \
			list(CONFIG_INIT_MEMBERS) \
		} \
		\
        list(CONFIG_HEADER_DECLARE)   \
		\
		void Serialize( ceng::CXmlFileSys* filesys ) \
		{ \
			list(CONFIG_XML_BIND_DECLARE) \
		} \
	};


} // end of namespace ceng

#endif
