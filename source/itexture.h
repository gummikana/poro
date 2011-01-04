#ifndef INC_ITEXTURE_H
#define INC_ITEXTURE_H

#include <assert.h>
#include "poro_types.h"

namespace poro {

class ITexture 
{
public:
	virtual ~ITexture() { }

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	
	virtual void SetUVCoords( float x1, float y1, float x2, float y2 ) {
		assert( false );
		// this is actually used, probably for a texture atlas,
		// you should implement this
	}

	virtual void SetExternalSize(int width, int height){
		assert(false);
		//Must be implemented if used
	}

	virtual types::string GetFilename() const = 0;
	/*{ 
		assert( false ); 
		// Implement this ! 
	}*/

};

} // end o namespace poro

#endif