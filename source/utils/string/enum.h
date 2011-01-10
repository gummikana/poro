///////////////////////////////////////////////////////////////////////////////
//
// Macro magic
//
// Easy way to create a enum that can be converted into a string and from a 
// string
// 
//.............................................................................
/*

  #define ENUM_LIST(_)                           \
	_(RED,   0)                                  \
	_(GREEN, 1)                                  \
	_(BLUE,  87)

	class Pertti
	{
	public:

		DEFINE_ENUM(Color, ENUM_LIST)
	
	};

*/
//
// Created 20.02.2005 Pete
//
//.............................................................................
//
//=============================================================================
#ifndef INC_ENUM_H
#define INC_ENUM_H

//  by Chad Austin  
//  on flipcode.com forum
//  http://www.flipcode.com/cgi-bin/fcmsg.cgi?thread_show=24481

#include <string>


namespace ceng {

	// Infrastructure.

#define ENUM_BODY(name, value) \
    name = value,

#define AS_STRING_CASE(name, value) \
    case name: return #name;

#define FROM_STRING_CASE(name, value) \
    if ( str == #name) {			  \
        return name;                  \
    }

#define DEFINE_ENUM(name, list)                 \
    enum name {                                 \
        list(ENUM_BODY)                         \
    };                                          \
	static std::string asString(int n) {       \
        switch (n) {                            \
            list(AS_STRING_CASE)                \
            default: return "";                 \
        }                                       \
    }                                           \
    static name fromString(const std::string& str ) {   \
        list(FROM_STRING_CASE)                  \
        return name(); /* assert? throw? */     \
    }


} // end of namespace ceng
#endif
