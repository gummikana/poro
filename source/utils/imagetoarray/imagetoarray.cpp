#include "imagetoarray.h"

#include <utils/color/ccolor.h>
#include <poro/iplatform.h>
#include <poro/igraphics.h>


//-----------------------------------------------------------------------------
void LoadImage( const std::string& filename, ceng::CArray2D< poro::types::Uint32 >& out_array2d, bool include_alpha )
{
	using namespace imagetoarray;

	TempTexture* surface = GetTexture(filename);

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
void SaveImage( const std::string& filename, const ceng::CArray2D< poro::types::Uint32 >& image_data )
{
	using namespace imagetoarray;

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

	//stbi_write_png( filename.c_str(), w, h, 4, pixels, w * 4 );
	Poro()->GetGraphics()->ImageSave( filename.c_str(), w, h, 4, pixels, w * 4 );

	delete [] pixels;
	pixels = NULL;
}


namespace imagetoarray {


TempTexture* GetTexture( const std::string& filename )
{
	TempTexture* result = new TempTexture;
	result->data = Poro()->GetGraphics()->ImageLoad( filename.c_str(), &result->width, &result->height, &result->bpp, 4 );
	if( result->data == NULL ) std::cout << "GetTexture - Couldn't load file: " << filename << std::endl;
	result->filename = filename;
	return result;
}

uint32 GetPixel(TempTexture* surface, int x, int y, bool include_alpha )
{
	if( surface == NULL ) return 0;
	return surface->GetPixel( x, y, include_alpha );
}


} // End of namespace imagetoarray