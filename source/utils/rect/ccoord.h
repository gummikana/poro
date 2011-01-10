#ifndef INC_CCOORD_H
#define INC_CCOORD_H

#include "cpoint.h"
namespace ceng {
//! 2d coordination class. Basic POD class
/*
class CCoord
{
public:

	CCoord() : x( -1 ), y( -1 ) { }
    CCoord( int inx, int iny ) : x( inx ), y( iny ) { }

	bool empty() { return ( x == -1 && y == -1 ); }
	int x, y;
};
*/

typedef CPoint< int > CCoord;

}
#endif