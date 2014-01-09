#include <iostream>
#include <sstream>

#include "../../../source/utils/string/string.h"
#include "../../../source/utils/xml/cxml.h"
#include "../../../source/utils/vector_utils/vector_utils.h"
#include "../../../source/utils/filesystem/filesystem.h"
#include "../../../source/game_utils/font/cfont.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// -------- params parsing ----------

bool HasArgument( const std::string& argument_name, const std::vector< std::string >& args )
{
	for( std::size_t i = 0; i < args.size(); ++i ) 
		if( ceng::RemoveWhiteSpaceAndEndings( args[ i ] ) == argument_name ) return true;

	return false;
}

std::string GetArgumentParam( const std::string& argument_name, const std::vector< std::string >& args, const std::string& default_value = "" )
{
	for( std::size_t i = 0; i < args.size(); ++i ) 
	{
		if( ceng::RemoveWhiteSpaceAndEndings( args[ i ] ) == argument_name && i + 1 < args.size() )
		{
			if( args[ i + 1 ].empty() == false &&
				args[ i + 1 ][0] != '-' ) 
				return args[ i + 1 ];
		}	
	}

	return default_value;
}

std::string GetArgumentParam( int i, const std::vector< std::string >& args, const std::string& default_value = "" )
{
	if( args.size() <= i ) 
	{
		return default_value;
	}

	return args[ i ];
}

//-----------------------------------------------------------------------------
unsigned char ttf_buffer[1<<25];

const int width = 4096;
const int height = 20480;

unsigned char bmap[ width*height ];

void CreateFont(
	const std::string& ttf_file,
	float size,
	const std::string& xml_filename,
	const std::string& texture_file )
{
	memset( bmap, 0, width*height );
	memset( ttf_buffer, 0, 1 << 25 );
	FILE* fptr = fopen(ttf_file.c_str(), "rb");

	if( fptr == NULL ) 
	{
		std::cout << "Error reading file: " << ttf_file << std::endl;
		return;
	}

	fread(ttf_buffer, 1, 1<<25, fptr);

	stbtt_bakedchar g_cdata[95]; // ASCII 32..126 is 95 glyphs
	stbtt_BakeFontBitmap( ttf_buffer, 0, size, bmap, width, height, 32, 95, g_cdata);

	CFont* font = new CFont;
	font->SetLineHeight( size );
	font->SetTextureFilename( texture_file );
	
	int tsize_x = 0;
	int tsize_y = 0;

	for( int i = 0; i < 95; ++i ) 
	{
		font->SetCharQuad( i + 32, 
			new CFont::CharQuad( 
				types::rect( 
					(float)g_cdata[ i ].x0, 
					(float)g_cdata[ i ].y0,
					(float)(g_cdata[ i ].x1 - g_cdata[ i ].x0),
					(float)(g_cdata[ i ].y1 - g_cdata[ i ].y0) ),
				types::vector2( 
					g_cdata[ i ].xoff, 
					g_cdata[ i ].yoff ),
				g_cdata[ i ].xadvance ) );

		if( g_cdata[ i ].x1 > tsize_x ) tsize_x = g_cdata[ i ].x1;
		if( g_cdata[ i ].y1 > tsize_y ) tsize_y = g_cdata[ i ].y1;
	}

	// int result = poro::ImageSave( icon_file.c_str(), w, h, 3, pixels, w * 3 );

	tsize_x += 1;
	tsize_y += 1;

	// convert this into a 32bit image
	unsigned char* pixels = (unsigned char*)malloc(width*height*4);

	for( int y = 0; y < tsize_y; ++y )
	{
		for( int x = 0; x < tsize_x; ++x )
		{
			int p = ( 4 * x + 4 * tsize_x * y);
			// int p2 = (x + (pos_x * 3)) + ( 3 * width * ( y + pos_y));
			// int p1 = ( x + w * y )
			pixels[ p + 0 ] = 255;
			pixels[ p + 1 ] = 255;
			pixels[ p + 2 ] = 255;
			pixels[ p + 3 ] = bmap[ x + width * y ];
		}
	}

	std::cout << "Creating file: " << texture_file << " ... ";
	stbi_write_png( texture_file.c_str(), tsize_x, tsize_y, 4, pixels, tsize_x * 4 );
	std::cout << "Done" << std::endl;
	
	std::cout << "Creating file: " << xml_filename << " ... ";
	ceng::XmlSaveToFile( *font, xml_filename, "Font" );
	std::cout << "Done" << std::endl;
}
// #endif

int main( int inargc, char* inargs[] ) 
{
	std::vector< std::string > args = ceng::ArgsToVector( inargc, inargs );

	std::string font_file = GetArgumentParam( 0, args );

	// TEST CASE
	// CreateFont( "GARA.ttf", 512.f, "gara.xml", "gara.png" );

	if( font_file.empty() ) 
	{
		std::cout << "Font builder creates xml & png for poro, from a given ttf -file. Usage: " << std::endl
		<< "font_build.exe times.ttf 18" << std::endl
		<< "\t this will create data/fonts/times_18.xml and data/fonts/times_18.png files" << std::endl
		<< "\t default size is 16" << std::endl
		<< std::endl
		<< "for more options: " << std::endl
		<< "\t -xml path_to_xml_file.xml " << std::endl
		<< "\t -texture path_to_texture_file.png " << std::endl;
		return 0;
	}

	std::string font_name = ceng::GetFilenameWithoutExtension( font_file );
	float size = ceng::CastFromString< float >( GetArgumentParam( 1, args, "16" ) );
	
	if( size <= 0 ) size = 16;

	font_name = "data/fonts/" + font_name + "_" + ceng::CastToString( size );
	std::string texture_filename = GetArgumentParam( "-texture", args, font_name + ".png" );
	std::string xml_filename = GetArgumentParam( "-xml", args, font_name + ".xml" );

	CreateFont( font_file, size, xml_filename, texture_filename );

	std::cout << "Success" << std::endl;
	return 0;
}
