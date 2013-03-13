#include <iostream>
#include <sstream>

#include "../../../source/game_utils/font/cfont.h"
#include "../../../source/utils/vector_utils/vector_utils.h"
#include "../../../source/utils/filesystem/filesystem.h"

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

//-----------------------------------------------------------------------------

void CreateFont(
	const std::string& ttf_file,
	float size,
	const std::string& xml_filename,
	const std::string& texture_file,
	int width,
	int height )
{
	unsigned char ttf_buffer[1<<25];
	unsigned char* bmap = (unsigned char*)malloc(width*height);
	
	fread(ttf_buffer, 1, 1<<25, fopen(ttf_file.c_str(), "rb"));

	stbtt_bakedchar g_cdata[96]; // ASCII 32..126 is 95 glyphs
	stbtt_BakeFontBitmap( ttf_buffer, 0, size, bmap, width, height, 32, 96, g_cdata);

	CFont* font = new CFont;
	font->SetLineHeight( size );
	font->SetTextureFilename( texture_file );
	
	for( int i = 0; i < 96; ++i ) 
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
	}

	// int result = poro::ImageSave( icon_file.c_str(), w, h, 3, pixels, w * 3 );

	// convert this into a 32bit image
	unsigned char* pixels = (unsigned char*)malloc(width*height*4);

	for( int y = 0; y < height; ++y )
	{
		for( int x = 0; x < width; ++x )
		{
			int p = ( 4 * x + 4 * width * y);
			// int p2 = (x + (pos_x * 3)) + ( 3 * width * ( y + pos_y));
			// int p1 = ( x + w * y )
			pixels[ p + 0 ] = 255;
			pixels[ p + 1 ] = 255;
			pixels[ p + 2 ] = 255;
			pixels[ p + 3 ] = bmap[ x + width * y ];
		}
	}

	stbi_write_png( texture_file.c_str(), width, height, 4, pixels, width * 4 );
	ceng::XmlSaveToFile( *font, xml_filename, "Font" );
}
// #endif

int main( int inargc, char* inargs[] ) 
{
	std::vector< std::string > args = ceng::ArgsToVector( inargc, inargs );

	std::string font_file = GetArgumentParam( "-font", args );

	if( font_file.empty() ) 
	{
		std::cout << "Font builder creates xml & png for poro, from a given ttf -file. Parameters are: " << std::endl
		<< "\t -font path_to_ttf_file.ttf " << std::endl
		<< "\t -size 16 " << std::endl
		<< "\t -xml path_to_xml_file.xml " << std::endl
		<< "\t -texture path_to_texture_file.png " << std::endl
		<< "\t -w 128 " << std::endl
		<< "\t -h 128 " << std::endl;
		return 0;
	}

	std::string font_name = ceng::GetFilenameWithoutExtension( font_file );

	int width = ceng::CastFromString< int >( GetArgumentParam( "-w", args, "128" ) );
	int height = ceng::CastFromString< int >( GetArgumentParam( "-h", args, "128" ) );
	float size = ceng::CastFromString< float >( GetArgumentParam( "-size", args, "16" ) );
	font_name = "data/fonts/" + font_name + "_" + ceng::CastToString( size );
	std::string texture_filename = GetArgumentParam( "-texture", args, font_name + ".png" );
	std::string xml_filename = GetArgumentParam( "-xml", args, font_name + ".xml" );

	CreateFont( font_file, size, xml_filename, texture_filename, width, height );

	return 0;
}
