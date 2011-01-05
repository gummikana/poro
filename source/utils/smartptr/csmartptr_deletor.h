#ifndef INC_CSMARTPTR_DELETOR_H
#define INC_CSMARTPTR_DELETOR_H

namespace ceng {

template< class T >
struct CSmartPtrDefaultDeletor
{
	void operator()( T* pointer ) { delete pointer; }
};

}

#endif
