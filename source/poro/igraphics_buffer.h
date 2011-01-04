#ifndef INC_IGRAPHICSBUFFER_H
#define INC_IGRAPHICSBUFFER_H

#include "igraphics.h"

namespace poro {
	
	class IGraphicsBuffer : public IGraphics 
	{
	public:
		virtual ~IGraphicsBuffer() { }
		virtual ITexture*	GetTexture() = 0;
		virtual void		Release() {};
		
		//IGraphics
		virtual bool		Init( int width, int height, bool fullscreen = false, const types::string& caption = "" ) = 0;
		
	};
		
} // end o namespace poro

#endif
