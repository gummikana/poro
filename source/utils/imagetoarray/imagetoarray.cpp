#include "imagetoarray.h"


#define STBI_HEADER_FILE_ONLY
#include <poro/external/stb_image.h>
#undef STBI_HEADER_FILE_ONLY
#include <sdl.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <poro/iplatform.h>
#include <poro/igraphics.h>
#include <utils/string/string.h>
#include "../game_types.h"


using namespace imagetoarray;


//-----------------------------------------------------------------------------
void LoadImage( const std::string& filename, ceng::CArray2D< Uint32 >& out_array2d, bool include_alpha )
{
	TempTexture* surface = GetTexture( filename );

	if( surface == NULL || surface->data == NULL )
	{
		std::cout << "LoadImage() - Failed to load image: " << filename << std::endl;
		return;		
	}
	
	out_array2d.Resize( surface->w, surface->h );

	if( surface )
	{
		for( int y = 0; y < surface->h; ++y )
		{
			for( int x = 0; x < surface->w; ++x )
			{
				out_array2d.Rand( x, y ) = GetPixel( surface, x, y, include_alpha );
			}
		}
	}

	delete surface;
}

//-----------------------------------------------------------------------------
void SaveImage( const std::string& filename, const ceng::CArray2D< uint32 >& image_data )
{
	// do the file and save it
	const int w = image_data.GetWidth();
	const int h = image_data.GetHeight();
	
	unsigned char* pixels = NULL;
	pixels = new unsigned char[ 4 * w * h ];	

	ceng::CColorUint8 c;
	for( int y = 0; y < h; ++y )
	{
		for( int x = 0; x < w; ++x )
		{
			c.Set32( image_data.Rand( x, y ) );

			int p = ( 4 * x + 4 * w * y);
			pixels[ p + 0 ] = c.GetR();
			pixels[ p + 1 ] = c.GetG();
			pixels[ p + 2 ] = c.GetB();
			pixels[ p + 3 ] = c.GetA();
		}
	}

	poro::IPlatform::Instance()->GetGraphics()->ImageSave( filename.c_str(), w, h, 4, pixels, w * 4 );

	delete [] pixels;
	pixels = NULL;
}


namespace imagetoarray {
uint32 GetPixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
	case 1:
		return *p;

	case 2:
		return *(Uint16 *)p;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;

	case 4:
		return *(uint32 *)p;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}


TempTexture* GetTexture( const std::string& filename )
{
	TempTexture* result = new TempTexture;
	result->data = stbi_load(filename.c_str(), &result->width, &result->height, &result->bpp, 4);
	if( result->data == NULL ) std::cout << "LoadLevel - Couldn't load file: " << filename << std::endl;
	result->filename = filename;
	return result;
}

uint32 GetPixel(TempTexture* surface, int x, int y, bool include_alpha )
{
	if( surface == NULL ) return 0;
	return surface->GetPixel( x, y, include_alpha );
}




} // End of namespace