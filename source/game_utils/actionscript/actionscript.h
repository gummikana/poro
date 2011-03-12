#ifndef INC_ACTIONSCRIPT_H
#define INC_ACTIONSCRIPT_H

#include <iostream>

#include "../../types.h"
#include "actionscript_types.h"
#include "eventdispatcher.h"
#include "vector.h"
#include "as_math.h"
#include "event.h"
#include "sprite.h"
#include "textsprite.h"
#include "displayobjectcontainer.h"

#include "../../utils/random/random.h"

namespace as {

void trace( const std::string& what );

inline void trace( const std::string& what )
{
	std::cout << what << std::endl;
}

class Rndm
{
public:

	Number Float( Number min_value, Number max_value ) { return (Number)ceng::Randomf( (float)min_value, (float)max_value ); }
	Number Random() { return  ceng::Randomf( 0.f, 1.f ); }
	int Integer( int min_value, int max_value ) { return ceng::Random( min_value, max_value ); }

	// float
	// random
	// integer

};

poro::ITexture* GetTexture( const std::string& filename );
as::Sprite*		LoadSprite( const std::string& filename );

} // end of namespace as

#endif
