#include "csingleton.h"

namespace ceng {
	
class T;
// Added template<> here. - Jani Hast/3.6.2009
template<>
T* CStaticSingleton< T >::myInstance = 0;

}