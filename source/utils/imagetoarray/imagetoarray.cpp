#include "imagetoarray.h"


#include <poro/iplatform.h>
#include <poro/igraphics.h>
#include <utils/color/ccolor.h>
#include <utils/debug.h>
#include <poro/external/stb_image_write.h>


//-----------------------------------------------------------------------------

void LoadImage( const std::string& filename, ceng::CArray2D< poro::types::Uint32 >& out_array2d, bool include_alpha )
{
	int w = 0;
	int h = 0;
	int bpp = 0;
	const int req_comp = 4;

	poro::FileDataTemp file;
	Poro()->GetFileSystem()->ReadWholeFileTemp( filename, &file );
	if ( file.IsValid() == false )
	{
		logger_error << "LoadImage() - Failed to load image: " << filename << "\n";
		return;
	}

	unsigned char* result = stbi_load_from_memory( (stbi_uc*)file.data, file.data_size_bytes, &w, &h, &bpp, req_comp );

	// error reading the file, probably not image file
	if ( result == NULL )
	{
		logger_error << "LoadImage() - Failed to load image: " << filename << "\n";
		return;
	}

	poro_assert( bpp == 4 );
	poro_assert( w > 0 );
	poro_assert( h > 0 );

	out_array2d.mySize = w*h;
	out_array2d.myWidth = w;
	out_array2d.myHeight = h;
	out_array2d.GetData().data = (uint32*)result;
	out_array2d.GetData()._size = out_array2d.mySize;


	poro_assert( include_alpha );
	if( include_alpha == false )
	{
		logger_error << "LoadImage() include_alpha == false in: " << filename << "\n";
	}

#if 0
#define PORO_SWAP_RED_AND_BLUE
#ifdef PORO_SWAP_RED_AND_BLUE
		if ( comp && *comp == 4 )
		{
			int width = *x;
			int height = *y;
			int bpp = *comp;

			for ( int ix = 0; ix < width; ++ix )
			{
				for ( int iy = 0; iy < height; ++iy )
				{
					int i = ( iy * width ) * bpp + ix * bpp;
					// color = ((color & 0x000000FF) << 16) | ((color & 0x00FF0000) >> 16) | (color & 0xFF00FF00);
					/*data[ co ] << 16 |
					data[ co+1 ] << 8 |
					data[ co+2 ] << 0; // |*/

					unsigned char temp = result[i + 2];
					result[i + 2] = result[i];
					result[i] = temp;
				}
			}
		}
#endif
#endif

}

#if 0 
void LoadImage( const std::string& filename, ceng::CArray2D< poro::types::Uint32 >& out_array2d, bool include_alpha )
{
	using namespace imagetoarray;

	// Cacheing this is a bit silly, use LoadImageConst for cached version
	TempTexture* surface = GetTexture(filename);
	if( surface == NULL || surface->data == NULL )
	{
		logger_error << "LoadImage() - Failed to load image: " << filename << "\n";
		return;		
	}
	
	out_array2d.Resize( surface->w, surface->h );

	if( surface )
	{
		int i = 0;
		const int bpp = 4;
		auto data = surface->data;
		uint32 color = 0;
		if (include_alpha)
		{
			for (int y = 0; y < surface->h; ++y)
			{
				for (int x = 0; x < surface->w; ++x)
				{
					/*int co = (y * surface->width) * bpp + x * bpp;
					cassert(co == i);*/

					color = data[i+0] << 16 | data[i+1] << 8 | data[i+2] << 0 | data[i+3] << 24;
					i += 4;
					out_array2d.Rand(x, y) = color;
					// cassert( color == surface->GetPixel( x, y, include_alpha ) );
				}
			}
		}
		else
		{
			for (int y = 0; y < surface->h; ++y)
			{
				for (int x = 0; x < surface->w; ++x)
				{
					color = data[i+0] << 16 | data[i+1] << 8 | data[i+2] << 0;
					i += 4;
					out_array2d.Rand(x, y) = color;
				}
			}
		}
	}

	delete surface;
}
#endif

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

	if( Poro() == NULL || Poro()->GetGraphics() == NULL )
	{
		stbi_write_png( filename.c_str(), w, h, 4, pixels, w * 4 );
	}
	else
	{
		Poro()->GetGraphics()->ImageSave( filename.c_str(), w, h, 4, pixels, w * 4 );
	}

	delete [] pixels;
	pixels = NULL;
}


namespace imagetoarray {

TempTexture::~TempTexture()
{
	Poro()->GetGraphics()->ImageFree( data );
	data = NULL;
}

TempTexture* GetTexture( const std::string& filename )
{
	TempTexture* result = new TempTexture;
	result->data = Poro()->GetGraphics()->ImageLoad( filename.c_str(), &result->width, &result->height, &result->bpp, 4 );
	if( result->data == NULL ) 
		logger_error << "GetTexture - Couldn't load file: " << filename << "\n";
	result->filename = filename;
	return result;
}

uint32 GetPixel(TempTexture* surface, int x, int y, bool include_alpha )
{
	if( surface == NULL ) return 0;
	return surface->GetPixel( x, y, include_alpha );
}


} // End of namespace imagetoarray
