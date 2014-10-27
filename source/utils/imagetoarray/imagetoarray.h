#ifndef INC_IMAGE_TO_ARRAY_H
#define INC_IMAGE_TO_ARRAY_H

#include <poro/poro_types.h>
#include <utils/array2d/carray2d.h>

//-----------------------------------------------------------------------------

// typedef uint32 
typedef poro::types::Uint32 uint32;


void	LoadImage( const std::string& filename, ceng::CArray2D< poro::types::Uint32 >& out_array2d, bool include_alpha );
void	SaveImage( const std::string& filename, const ceng::CArray2D< poro::types::Uint32 >& image_data );


namespace imagetoarray {

struct TempTexture
{
	TempTexture() : w( width ), h( height ) { }
	~TempTexture() 
	{
		delete [] data;
		data = NULL;
	}

	int width, height, bpp;
	unsigned char *data;
	std::string filename;

	// aliases
	int& w;
	int& h;

	int ColorClamp( int i, int size )
	{
		if( i < 0 ) 
			i += size * (int)(((-i) / size) + 1);
		
		if( i >= size ) i = i % size;
		
		return i;
	}

	poro::types::Uint32 GetPixel( int x, int y, bool include_alpha )
	{
		if( data == NULL ) return 0;

		x = ColorClamp( x, width );
		y = ColorClamp( y, height );
		bpp = 4;
		int co = ( y * width ) * bpp + x * bpp;
		
		/*int co0 = data[co];
		int co1 = data[co + 1];
		int co2 = data[co + 2];
		int co3 = data[co + 3];
		
		std::cout << "CO( " << x << ", " << y << " ): " << co0 << ", " << co1 << ", " << co2 << ", " << co3 << std::endl;*/

		poro::types::Uint32 result = 
			data[ co ] << 16 |
			data[ co+1 ] << 8 |
			data[ co+2 ] << 0; // |
			/*data[ co+3 ] << 24*/
			//0xff << 24;

		if( include_alpha ) 
			result |= data[ co+3 ] << 24;


		return result;
	}

};

//-----------------------------------------------------------------------------
TempTexture* GetTexture( const std::string& filename );
poro::types::Uint32 GetPixel(TempTexture* surface, int x, int y, bool include_alpha = false );

//-----------------------------------------------------------------------------

} // End of namespace

#endif
