#ifndef INC_CCSSOBJECT_INFO_H
#define INC_CCSSOBJECT_INFO_H

#include "signalslot_libs.h"

namespace ceng {
namespace css {

template< class T >
class CSSObject : public CStaticSingleton< CSSObject< T > >
{
public:
	std::string name;
	bool is_a_css_object;

private:
	CSSObject() { }

	friend class CStaticSingleton< CSSObject< T > >;
};

} // end of namespace css
} // end of namespace ceng

#endif