/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ***************************************************************************/

#include "sprite.h"
#include <map>
#include <climits>

// LoadSprite requires these
#include "../../poro/iplatform.h"
#include "../../poro/igraphics.h"
#include "../../poro/ishader.h"

#include "../../utils/singleton/csingletonptr.h"
#include "../../utils/math/point_inside.h"
#include "../../utils/filesystem/filesystem.h"
#include "../../utils/imagetoarray/imagetoarray.h"

#include "../tween/tween_utils.h"

#include "asset_loading/Animations.h"
#include "asset_loading/AnimationUpdater.h"


namespace as { 

#ifdef WIZARD_DEBUG_SPRITES
bool Sprite::DEBUG_REPORT_LEAKY_SPRITES = false;
bool Sprite::DEBUG_FIX_LEAKY_SPRITES = false;
#endif

typedef poro::types::Uint32 Uint32;

namespace
{
	struct TextureBuffer
	{
		TextureBuffer() :
			texture(NULL),
			image_data(NULL),
			time_stamp("")
		{ }

		TextureBuffer(poro::ITexture* texture, ceng::CArray2D< Uint32 >*image_data, const std::string& time_stamp) :
			texture(texture),
			image_data(image_data),
			time_stamp(time_stamp)
		{ }


		poro::ITexture* texture;
		ceng::CArray2D< Uint32 >* image_data;
		std::string time_stamp;
	};

	TextureBuffer* GetTextureBuffer(const std::string& filename);
}

namespace impl {

#ifdef WIZARD_DEBUG_SPRITES

std::vector< std::string > DEBUG_SavedImages;

std::map< std::string, 
	std::pair<
		std::vector< as::Sprite::RectAnimation >,
		std::vector< as::Sprite::RectAnimation >
		> 
	> DEBUG_RectAnimations;
#endif


struct SpriteLoadHelper
{
	SpriteLoadHelper() 
	{
		Init();
	}

	~SpriteLoadHelper()
	{
		ceng::VectorClearPointers(child_sprites);
	}

	void Init()
	{
		name = "";
		color.resize(4);
		for (std::size_t i = 0; i < color.size(); ++i) 
			color[i] = 1.f;
		filename = "";
		offset.Set(0, 0);
		scale.Set(1, 1);
		position.Set(0, 0);
		angle = 0;
		default_animation = "";

		rect_animations.clear();
		ceng::VectorClearPointers(child_sprites);
	}


	/*bool is_text;
	std::string text;*/
	// position,?
	// angle?
	std::string name;
	std::vector< float > color;

	std::string		filename;
	types::vector2	offset;
	types::vector2	scale;
	types::vector2	position;
	float			angle;
	std::string		default_animation;

	std::vector< Sprite::RectAnimation > rect_animations;
	std::vector< SpriteLoadHelper* > child_sprites;

	// these are kept around for saving these out
	// TODO - these aren't really needed to be kept around...
	// they're only needed to serialize the sprites back which is done when #WIZARD_DEBUG_SPRITES is defined
// #ifdef WIZARD_DEBUG_SPRITES
	std::string	hotspots_filename;
	std::vector< Sprite::Hotspot > hotspots;
	std::string	xml_filename;
// #endif

	// impl time_stamp
	std::string		time_stamp;

	void SpriteLoadHelper::Serialize(ceng::CXmlFileSys* filesys)
	{
#ifdef WIZARD_DEBUG_SPRITES

		if( filesys->IsWriting() )
		{
			SpriteLoadHelper default_values;
			default_values.Init();


			if( filename != default_values.filename )
				XML_BindAttributeAlias(filesys, filename, "filename");

			if( name != default_values.name )
				XML_BindAttributeAlias(filesys, name, "name");

			if( offset.x != default_values.offset.x ||  
				offset.y != default_values.offset.y )
			{
				XML_BindAttributeAlias(filesys, offset.x, "offset_x");
				XML_BindAttributeAlias(filesys, offset.y, "offset_y");
			}

			if( scale.x != default_values.scale.x ||  
				scale.y != default_values.scale.y )
			{
				XML_BindAttributeAlias(filesys, scale.x, "scale_x");
				XML_BindAttributeAlias(filesys, scale.y, "scale_y");
			}

			if( position.x != default_values.position.x ||  
				position.y != default_values.position.y )
			{
				XML_BindAttributeAlias(filesys, position.x, "position_x");
				XML_BindAttributeAlias(filesys, position.y, "position_y");
			}

			if( angle != default_values.angle )
				XML_BindAttributeAlias(filesys, angle, "angle");

			if( default_animation != default_values.default_animation )
				XML_BindAttributeAlias(filesys, default_animation, "default_animation");


			if( color[0] != default_values.color[0] ||  
				color[1] != default_values.color[1] ||
				color[2] != default_values.color[2] ||
				color[3] != default_values.color[3] )
			{
				XML_BindAttributeAlias(filesys, color[0], "color_r");
				XML_BindAttributeAlias(filesys, color[1], "color_g");
				XML_BindAttributeAlias(filesys, color[2], "color_b");
				XML_BindAttributeAlias(filesys, color[3], "color_a");
			}

			std::vector< Sprite::RectAnimation > temp_rect_animations = rect_animations;
			ceng::VectorXmlSerializerObjects< Sprite::RectAnimation > rect_serializer(temp_rect_animations, "RectAnimation");
			rect_serializer.Serialize(filesys);

			ceng::VectorXmlSerializer< as::impl::SpriteLoadHelper > child_sprite_serializer(child_sprites, "Sprite");
			child_sprite_serializer.Serialize(filesys);

			if( hotspots_filename != default_values.hotspots_filename)
				XML_BindAttributeAlias(filesys, hotspots_filename, "hotspots_filename");

			// std::vector< Sprite::Hotspot > hotspots;
			ceng::VectorXmlSerializerObjects< Sprite::Hotspot > hotspot_serializer(hotspots, "Hotspot");
			hotspot_serializer.Serialize( filesys );

			return;
		}
#endif

		{
			// XML_BindAttributeAlias(filesys, is_text, "is_text");
			XML_BindAttributeAlias(filesys, filename, "filename");
			XML_BindAttributeAlias(filesys, name, "name");
			XML_BindAttributeAlias(filesys, offset.x, "offset_x");
			XML_BindAttributeAlias(filesys, offset.y, "offset_y");
			XML_BindAttributeAlias(filesys, scale.x, "scale_x");
			XML_BindAttributeAlias(filesys, scale.y, "scale_y");

			XML_BindAttributeAlias(filesys, position.x, "position_x");
			XML_BindAttributeAlias(filesys, position.y, "position_y");
			XML_BindAttributeAlias(filesys, angle, "angle");

			XML_BindAttributeAlias(filesys, default_animation, "default_animation");

			XML_BindAttributeAlias(filesys, color[0], "color_r");
			XML_BindAttributeAlias(filesys, color[1], "color_g");
			XML_BindAttributeAlias(filesys, color[2], "color_b");
			XML_BindAttributeAlias(filesys, color[3], "color_a");

			std::vector< Sprite::RectAnimation > temp_rect_animations = rect_animations;
			ceng::VectorXmlSerializerObjects< Sprite::RectAnimation > rect_serializer(temp_rect_animations, "RectAnimation");
			rect_serializer.Serialize(filesys);

			ceng::VectorXmlSerializer< as::impl::SpriteLoadHelper > child_sprite_serializer(child_sprites, "Sprite");
			child_sprite_serializer.Serialize(filesys);

			XML_BindAttributeAlias(filesys, hotspots_filename, "hotspots_filename");

			// std::vector< Sprite::Hotspot > hotspots;
			ceng::VectorXmlSerializerObjects< Sprite::Hotspot > hotspot_serializer(hotspots, "Hotspot");
			hotspot_serializer.Serialize( filesys );

			// load hotspots
			if ( filesys->IsReading() && hotspots_filename.empty() == false )
			{
				ceng::CArray2D<uint32> hotspots_image;
				LoadImage( hotspots_filename, hotspots_image );
				for ( auto& rect_animation : temp_rect_animations )
				{
					for ( const Sprite::Hotspot& hotspot_config : hotspots )
					{
						Sprite::ResolvedHotspot hotspot;
						hotspot.name = hotspot_config.name;
						rect_animation.mHotspots.push_back( hotspot );
					}

					for ( int frame = 0; frame < rect_animation.mFrameCount; frame++ )
					{
						types::rect rect = rect_animation.FigureOutRectPos( frame );
						types::ivector2 min( (int)rect.x, (int)rect.y );
						types::ivector2 max( (int)rect.x + (int)rect.w, (int)rect.y + (int)rect.h );

						min.x = ceng::math::Min( min.x, hotspots_image.GetWidth() );
						min.y = ceng::math::Min( min.y, hotspots_image.GetHeight() );
						max.x = ceng::math::Min( max.x, hotspots_image.GetWidth() );
						max.y = ceng::math::Min( max.y, hotspots_image.GetHeight() );

						int hotspots_found_this_frame = 0;

						for ( int y = min.y; y<max.y; y++ )
						{
							for ( int x = min.x; x < max.x; x++ )
							{
								const uint32 color = hotspots_image.At( x, y);
								if ( (color & 0xFF000000)>0 )
								{
									int hotspot_i = 0;
									for ( const auto& hotspot_config : hotspots )
									{
										const uint32 color2 = hotspot_config.color;
										uint32 mask = 0;
										{
											mask |= (color2 & 0x00FF0000)>0 ? 0x00FF0000 : 0;
											mask |= (color2 & 0x0000FF00)>0 ? 0x0000FF00 : 0;
											mask |= (color2 & 0x000000FF)>0 ? 0x000000FF : 0;
										}

										if ( ( color & mask ) == color2 )
										{
											rect_animation.mHotspots[hotspot_i].positions.emplace_back( types::ivector2( x, y ) - min );
											if ( ++hotspots_found_this_frame == hotspots.size() )
											{
												x = max.x; // all hotspots found, jump to next frame
												y = max.y;
											}
										}

										hotspot_i++;
									}
								}
							} // x
						} // y
					} // frames
				} // rect anims
			} // if hotspots_filename.empty() == false

			// apply to rect animations
			rect_animations = temp_rect_animations;
		}
		
#ifdef WIZARD_DEBUG_SPRITES
		if( filesys->IsReading() && 
			( as::Sprite::DEBUG_REPORT_LEAKY_SPRITES ||
			as::Sprite::DEBUG_FIX_LEAKY_SPRITES ) 
			)
		{
			DEBUG_CheckForLeaks();
		}
#endif
	}

	// DEBUG - checks if there are pixels that are too close to the rect animations
	bool DEBUG_CheckOutsideRect( const ceng::CArray2D< uint32 >& image, const types::irect& rect )
	{
		/*types::irect rect;
		rect.x = (int)frect.x;
		rect.y = (int)frect.y;
		rect.w = (int)frect.w;
		rect.h = (int)frect.h;
		*/
		bool result = true;
		for( int x = rect.x - 1; x <= rect.x + rect.w; ++x )
		{
			int y = rect.y - 1;
			if( image.IsValid( x, y ) && 
				(image.Rand( x, y ) & 0xFF000000) != 0 )
			{
				logger_error << "Leaking rect at: " << filename << " - at " << x << ", " << y << "\n";
				result = false;
			}

			y = rect.y + rect.h;
			if( image.IsValid( x, y ) && 
				(image.Rand( x, y ) & 0xFF000000) != 0 )
			{
				logger_error << "Leaking rect at: " << filename << " - at " << x << ", " << y << "\n";
				result = false;
			}
		}

		for( int y = rect.y - 1; y <= rect.y + rect.h; ++y )
		{
			int x = rect.x - 1;
			if( image.IsValid( x, y ) && 
				(image.Rand( x, y ) & 0xFF000000) != 0 )
			{
				logger_error << "Leaking rect at: " << filename << " - at " << x << ", " << y << "\n";
				result = false;
			}

			x = rect.x + rect.w;
			if( image.IsValid( x, y ) && 
				(image.Rand( x, y ) & 0xFF000000) != 0 )
			{
				logger_error << "Leaking rect at: " << filename << " - at " << x << ", " << y << "\n";
				result = false;
			}
		}

		return result;
	}
	
	void DEBUG_Blit( const ceng::CArray2D< uint32 >& src_image, const types::irect& src_rect, ceng::CArray2D< uint32 >& dest_image, const types::irect& dest_rect, const std::string& debug_filename )
	{
		for( int y = 0; y < dest_rect.h; ++y )
		{
			for( int x = 0; x < dest_rect.w; ++x  )
			{
				const int dest_x = x + dest_rect.x;
				const int dest_y = y + dest_rect.y;
				dest_image.Rand( dest_x, dest_y ) = 0;
			}
		}

		for( int y = 0; y < src_rect.h; ++y )
		{
			for( int x = 0; x < src_rect.w; ++x )
			{
				int src_x = x + src_rect.x;
				int src_y = y + src_rect.y;

				int dest_x = x + dest_rect.x;
				int dest_y = y + dest_rect.y;

				if( dest_x == 10 && dest_y == 17 )
				{
					int debug_break = 1;
				}
				cassert( dest_image.IsValid( dest_x, dest_y ) );
				// cassert(src_image.IsValid( src_x, src_y ) );
				if( src_image.IsValid( src_x, src_y ) == false)
				{
					logger << "Error - image( " << debug_filename << ") is too tiny for all the frames at (" << src_x << ", " << src_y << ") \n";
				}
				else
				{
					dest_image.Rand( dest_x, dest_y ) = src_image.Rand( src_x, src_y );
				}
			}
		}
		/*
		for( int y = src_rect.y; y < src_rect.y; )
		for( int x = src_rect.x; x < src_rect.x + src_rect.w; ++x )
		{

		}
		*/
	}

#ifdef WIZARD_DEBUG_SPRITES
	template< class TRect >
	inline bool Rects_Collides( const TRect& a, const TRect& b )
	{
		return (
			a.GetRight() - 1 < b.GetLeft() || 
			a.GetBottom() - 1 < b.GetTop() || 
			a.GetLeft() > b.GetRight() - 1  || 
			a.GetTop() > b.GetBottom() - 1 ) == false;

	}

	// std::map< std::string, std::pair< as::Sprite::RectAni

	void DEBUG_CheckForLeaks()
	{
		if( ceng::ContainsString( "data/entities/verlet_chains", filename ) )
		{
			logger << "Skipping verlet - " << filename << "\n";
			return;
		}

		if( ceng::VectorContains( DEBUG_SavedImages, filename ) )
		{
			if( ceng::VectorContains( DEBUG_SavedImages, xml_filename ) == false )
			{
				logger << "Warning! XML FILE: " << xml_filename << " it reuses png: " << filename << "\n";

				std::sort( rect_animations.begin(), rect_animations.end(), []( const as::Sprite::RectAnimation& a, const as::Sprite::RectAnimation& b ) {return a.FigureOutRectPos(0).y < b.FigureOutRectPos(0).y; } );

				if( ceng::VectorCompare( DEBUG_RectAnimations[ filename ].first, rect_animations ) )
				{

					rect_animations = DEBUG_RectAnimations[ filename ].second;
					std::string xml_outfilename = xml_filename;
					if( as::Sprite::DEBUG_FIX_LEAKY_SPRITES )
					{
						ceng::XmlSaveToFile( *this, xml_outfilename, "Sprite", true );
					}
					DEBUG_SavedImages.push_back( xml_filename );
					return;
				}
				else
				{
					logger_error << "ERROR! XML FILE: " << xml_filename << " it reuses png: " << filename << " and we couldn't fix it by replacing rect animations! Needs to be fixed by hand\n";
					return;
				}
			}
			return;
		}

		ceng::CArray2D< uint32 > image;
		LoadImage( filename, image );

		std::sort( rect_animations.begin(), rect_animations.end(), []( const as::Sprite::RectAnimation& a, const as::Sprite::RectAnimation& b ) {return a.FigureOutRectPos(0).y < b.FigureOutRectPos(0).y; } );

		std::vector< int > broken_rect_animations; 
		std::map< int, int > broken_rect_y_pos;
		std::vector< types::irect > existing_rects;

		// check out stuff
		for( size_t i = 0; i < rect_animations.size(); ++i )
		{
			as::Sprite::RectAnimation r = rect_animations[i];
			for( int j = 0; j < r.mFrameCount; ++j )
			{
				types::irect temp_r = r.FigureOutIRectPos( j );
				if( DEBUG_CheckOutsideRect( image, temp_r ) == false )
				{
					broken_rect_y_pos[ temp_r.y ] = 1;
					ceng::VectorAddUnique( broken_rect_animations, (int)i );
				}

				for( size_t k = 0; k < existing_rects.size(); ++k )
				{
					types::irect check_me = existing_rects[k];
					if( temp_r.y == check_me.y && temp_r.GetBottom() == check_me.GetBottom() )
						continue;

					// if these collide it's a bad thing
					if( Rects_Collides( check_me, temp_r) ) 
					{
						logger_error << "ERROR in " << xml_filename << " - animations overlap " 
							<< check_me.GetTop() << ", " << check_me.GetLeft() << " - " << check_me.GetRight() - 1 << ", " << check_me.GetBottom() - 1 
							<< " and "
							<< temp_r.GetTop() << ", " << temp_r.GetLeft() << " - " << temp_r.GetRight() - 1 << ", " << temp_r.GetBottom() - 1
							<< " \n";
					}
				}

				existing_rects.push_back( temp_r );

			}		

			// cassert( r.mPositionX == 0 );
		}



		// fix these to a new image
		if( broken_rect_animations.empty() == false )
		{
			int new_width = 0;
			int new_height = 0;

			// figure out the width, height
			int extra_offset_y = 1;

			std::vector< as::Sprite::RectAnimation > new_rect_animations;
			new_rect_animations.resize( rect_animations.size() );
			std::map< int, int > new_rect_animation_positions;

			for( size_t i = 0; i < rect_animations.size(); ++i )
			{
				as::Sprite::RectAnimation r = rect_animations[i];
				bool is_broken = ceng::VectorContains( broken_rect_animations, (int)i );
				const int y_pos = r.FigureOutIRectPos( 0 ).y;

				// for overlapping
				if( is_broken == false && 
					broken_rect_y_pos.find( y_pos ) != broken_rect_y_pos.end() )
				{
					is_broken = true;
				}
				
				as::Sprite::RectAnimation new_r = r;
				if( is_broken )
				{
					new_r.mWidth++; 
					new_r.mHeight++;
					new_r.mShrinkByOnePixel = true;
				}
				
				if( new_rect_animation_positions.find( y_pos) == new_rect_animation_positions.end() )
				{
					int lowest_point = 0;
					for( size_t j = 0; j < i; ++j )
						lowest_point = ceng::math::Max( new_rect_animations[j].FigureOutIRectPos(  new_rect_animations[j].mFrameCount - 1 ).GetBottom(), lowest_point );

					lowest_point++;
					new_rect_animation_positions[ y_pos ] = lowest_point;
				}

				new_r.mPositionY = new_rect_animation_positions[ y_pos ];
				
				cassert( r.mWidth != 0 );
				new_r.mPositionX = ( r.mPositionX / r.mWidth ) * new_r.mWidth;

				for( int j = 0; j < r.mFrameCount; ++j )
				{
					new_width = ceng::math::Max( new_width, new_r.FigureOutIRectPos( j ).GetRight() );
					new_height = ceng::math::Max( new_height, new_r.FigureOutIRectPos( j ).GetBottom() );
				}

				new_rect_animations[i] = new_r;
			}

			/*
			_emissive.xml <- ? are these the same rect animations?
			_emissive.png
			_uv_scr.png
			_hotspots.png
			*/

			std::vector< std::string > filenames_to_fix;
			filenames_to_fix.push_back( filename );
			filenames_to_fix.push_back( ceng::GetWithoutExtension( filename ) + "_uv_src.png");
			if( hotspots_filename.empty() == false )
				filenames_to_fix.push_back( hotspots_filename );

			for( size_t k = 0; k < filenames_to_fix.size(); ++k )
			{
				if( Poro()->GetFileSystem()->DoesExist( filenames_to_fix[k] ) == false )
					continue;

				const std::string src_filename = filenames_to_fix[k];
				if( ceng::VectorContains( DEBUG_SavedImages, src_filename) ) 
					continue;

				ceng::CArray2D< uint32 > src_image;
				LoadImage( src_filename, src_image );

				cassert( src_image.GetWidth() == image.GetWidth() );
				cassert( src_image.GetHeight() == image.GetHeight() );

				ceng::CArray2D< uint32 > new_image( new_width, new_height );

				for( size_t i = 0; i < rect_animations.size(); ++i )
				{
					as::Sprite::RectAnimation r = rect_animations[i];
					as::Sprite::RectAnimation new_r = new_rect_animations[i];

					for( int j = 0; j < r.mFrameCount; ++j )
					{
						DEBUG_Blit( src_image, r.FigureOutIRectPos( j ), new_image, new_r.FigureOutIRectPos( j ), src_filename );
					}
				}

				// TODO( Petri ): Write the output for this
				// ceng::CopyFileCeng( src_filename, ceng::GetWithoutExtension( src_filename ) + "_orig.png" );
				if( as::Sprite::DEBUG_FIX_LEAKY_SPRITES )
				{
					SaveImage( src_filename, new_image );
				}
				// SaveImage( "temptemp/new_gfx/" + ceng::GetFilename( src_filename ), new_image );
				DEBUG_SavedImages.push_back( src_filename );
				// SaveImage( "temptemp/new_gfx/" + ceng::GetFilename( filename ), new_image );
			}

			DEBUG_RectAnimations[ filename ].first = rect_animations;
			DEBUG_RectAnimations[ filename ].second = new_rect_animations;

			rect_animations = new_rect_animations;
			std::string xml_outfilename = xml_filename;
			if( as::Sprite::DEBUG_FIX_LEAKY_SPRITES )
			{
				ceng::XmlSaveToFile( *this, xml_outfilename, "Sprite", true );
			}

			DEBUG_SavedImages.push_back( filename );
			DEBUG_SavedImages.push_back( xml_filename );
		}

		if( ceng::ContainsString( "enemies_gfx", xml_filename ) )
		{
			// god damn ragdolls
			std::string ragdoll_folder = "data/ragdolls/" + ceng::GetFilenameWithoutExtension( filename );
			if( Poro()->GetFileSystem()->DoesExist( ragdoll_folder ) ) 
			{
				// logger_error << "Found ragdoll: " << ragdoll_folder << "\n";

				int stand_w = 0;
				int stand_h = 0;

				for( size_t i = 0; i < rect_animations.size(); ++i )
				{
					if( rect_animations[i].mName == "stand" )
					{
						stand_w = rect_animations[i].FigureOutIRectPos( 0 ).w;
						stand_h = rect_animations[i].FigureOutIRectPos( 0 ).h;
					}
				}

				auto files = Poro()->GetFileSystem()->GetFiles( poro::FileLocation::WorkingDirectory, ragdoll_folder );
				for ( std::string& file : files )		
				{
					if( ceng::GetFileExtension( file ) != "png" )
						continue;

					ceng::CArray2D< uint32 > rimage;
					LoadImage( file, rimage );
					if( rimage.GetWidth() != stand_w || rimage.GetHeight() != stand_h )
					{
						logger_error << "Found ragdoll problems: " << file 
							<< " - width (" << rimage.GetWidth() 
							<< ") & height (" << rimage.GetHeight() 
							<< ") don't match stand_w (" << stand_w 
							<< ") / stand_h (" << stand_h 
							<< ") of image: " << xml_filename << "\n";
					}
				}
			}
		}
	}
#endif
};

//-----------------------------------------------------------------------------

static void ApplySpriteLoadHelperToSprite(as::Sprite* result, impl::SpriteLoadHelper* sprite_data)
{
	if( sprite_data == NULL )
		return;

	// we need to duplicate rect_animations
	/*std::vector< as::Sprite::RectAnimation* > rect_animations(sprite_data->rect_animations.size());
	for (std::size_t i = 0; i < rect_animations.size(); ++i)
	{
		rect_animations[i] = new as::Sprite::RectAnimation(*(sprite_data->rect_animations[i]));
	}*/

	result->SetRectAnimations( &sprite_data->rect_animations );
	result->SetCenterOffset(sprite_data->offset);
	result->SetScale(sprite_data->scale.x, sprite_data->scale.y);
	result->SetName(sprite_data->name);
	result->SetColor(sprite_data->color);

	if (sprite_data->default_animation.empty() == false)
	{
		result->PlayAnimation(sprite_data->default_animation);
		// result->Update(0);
	}

	TextureBuffer* buffer = GetTextureBuffer(sprite_data->filename);
	if (buffer == NULL) {
		logger_error << "ERROR - couldn't load file: " << sprite_data->filename << "\n";
		return;
	}

	poro::ITexture* image = buffer->texture;
	if (image == NULL) {
		logger_error << "ERROR - couldn't load file: " << sprite_data->filename << "\n";
		return;
	}

	result->SetTexture(image);
	result->SetImageData(buffer->image_data);
	result->SetSize((int)image->GetWidth(), (int)image->GetHeight());

	// this is overwritten in LoadSpriteTo(...)
	result->SetFilename(image->GetFilename());
	result->MoveTo(sprite_data->position);
	result->SetRotation(sprite_data->angle);

	for (std::size_t i = 0; i < sprite_data->child_sprites.size(); ++i)
	{
		Sprite* child = new Sprite;
		ApplySpriteLoadHelperToSprite(child, sprite_data->child_sprites[i]);
		result->addChild(child);
	}

	// sprite_data->rect_animations.clear();
	// sprite_data.child_sprites.clear();
}


} // end of namespace impl

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace {

	typedef std::map< std::string, TextureBuffer* > TTextureBuffer;
	TTextureBuffer mTextureBuffer;

	typedef std::map< std::string, impl::SpriteLoadHelper* > TTSpriteBuffer;
	TTSpriteBuffer mSpriteBuffer;

	// ---------

	impl::SpriteLoadHelper* GetSpriteLoadHelper(const std::string& filename)
	{
		TTSpriteBuffer::iterator i = mSpriteBuffer.find(filename);

		bool load = false;

		impl::SpriteLoadHelper* data = NULL;
		if (i != mSpriteBuffer.end()) 
		{
			data = i->second;
		}
		else 
		{
			data = new impl::SpriteLoadHelper;
			mSpriteBuffer[filename] = data;
			load = true;
		}

		cassert( data ); // this shouldn't be null

#ifdef WIZARD_HOT_LOAD_ENABLED
		// if we want to update sprite files on the fly...
		// check if we should reload
		std::string time_stamp = Poro()->GetFileSystem()->GetDateForFile(filename);
		load |= ( data->time_stamp != time_stamp );
		// logger << "WIZARD_HOT_LOAD_ENABLED!\n";
#else
		std::string time_stamp = "d";	// debug 
#endif

		// reload (or load for the first time)
		if ( load )
		{
			if ( Poro()->GetFileSystem()->DoesExist( filename ) == false )
			{
				logger_error << "ERROR - LoadSpriteTo()... file doesn't exist: " << filename << "\n";
				return NULL;
			}

			if( data->time_stamp.empty() == false )
				logger << "reloading sprite: " << filename << "\n";

			data->xml_filename = filename;
			ceng::XmlLoadFromFile((*data), filename, "Sprite");
			data->time_stamp = time_stamp;

			if( true )
			{
				// check fi the sprite leaks
			}
		}

		return data;
	}

	//----------------------------------------------------------------------------

	ceng::CArray2D< Uint32 >* GetPixelDataCopy( poro::ITexture* image)
	{
		//Copy the raw pixel data
		ceng::CArray2D< Uint32 >* raw_pixel_data_copy;

		int w = image->GetWidth();
		int h = image->GetHeight();
		unsigned char* raw_pixel_data_orig = image->GetPixelData();

		if ( raw_pixel_data_orig == NULL )
			return NULL;


		Uint32* raw_pixel_data = reinterpret_cast< Uint32* >( raw_pixel_data_orig );
		raw_pixel_data_copy = new ceng::CArray2D< Uint32 >( w, h );
		
		int len = w * h;
		for ( int i = 0 ; i < len; ++i )
		{
			Uint32 value = *( raw_pixel_data + i );

			// Swap the bytes. ABGR to ARGB.
			value = ((value & 0x000000FF) << 16) | ((value & 0x00FF0000) >> 16) | (value & 0xFF00FF00);

			int x = i % w;
			int y = (int)floor((double)i / w);
			raw_pixel_data_copy->Rand( x, y ) = value;
		}

		return raw_pixel_data_copy;
	}

	//----------------------------------------------------------------------------

	TextureBuffer* GetTextureBuffer( const std::string& filename )
	{
		TTextureBuffer::iterator i = mTextureBuffer.find( filename );

		if( i == mTextureBuffer.end() )
		{
			if ( Poro()->GetFileSystem()->DoesExist( filename ) == false )
			{
				logger_error << "ERROR - LoadSpriteTo()... file doesn't exist: " << filename << "\n";
				return NULL;
			}

			poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
			poro::ITexture* image = graphics->LoadTexture( filename );

			if ( image == NULL )
				return NULL;

			const std::string time_stamp = Poro()->GetFileSystem()->GetDateForFile(filename);

			TextureBuffer* data = new TextureBuffer( image, NULL, time_stamp );
			mTextureBuffer[ filename ] = data;
			return data;
		}
		else
		{
#ifdef WIZARD_HOT_LOAD_ENABLED
			const std::string time_stamp = Poro()->GetFileSystem()->GetDateForFile( filename );
			if( i->second->time_stamp != time_stamp ) 
			{
				// debug reasons
				logger << "Reloading texture file: " << filename << "\n";

				// release old texture
				poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
				graphics->DestroyTexture( i->second->texture );

				logger << "Release of texture done: " << filename << "\n";

				// release old image data
				if ( i->second->image_data != NULL )
				{
					i->second->image_data->Clear();
					delete i->second->image_data;
					i->second->image_data = NULL;

					logger << "Release of image data done: " << filename << "\n";
				}
			
				// reload
				poro::ITexture* image = graphics->LoadTexture( filename );

				if ( image == NULL )
					return NULL;

				logger << "Loading of new texture done: " << filename << "\n";

				i->second->texture = image;
				i->second->time_stamp = time_stamp;
				i->second->image_data = NULL;
			}
#endif

			return i->second;
		}
	}


} // end of anonymous namespace
//-----------------------------------------------------------------------------

void PreloadTexture( const std::string& filename )
{
	TTextureBuffer::iterator i = mTextureBuffer.find( filename );

	if( i == mTextureBuffer.end() )
	{
		poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
		poro::ITexture* image = graphics->LoadTexture( filename );
		std::string time_stamp = Poro()->GetFileSystem()->GetDateForFile(filename);

		TextureBuffer* data = new TextureBuffer( image, NULL, time_stamp );
		mTextureBuffer[ filename ] = data;
		// return image;
	}
}

void ReleasePreloadedTexture( const std::string& filename )
{
	TTextureBuffer::iterator i = mTextureBuffer.find( filename );

	if( i != mTextureBuffer.end() )
	{
		poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
		graphics->DestroyTexture( i->second->texture );
		delete i->second;

		mTextureBuffer.erase( i );
	}
}

void ReleaseCachedTextures()
{
	auto graphics = Poro()->GetGraphics();

	for ( auto& pair : mTextureBuffer )
	{
		if ( pair.second->texture )
			graphics->DestroyTexture( pair.second->texture );
		delete pair.second->image_data;
		delete pair.second;
	}

	for ( auto& pair : mSpriteBuffer )
	{
		delete pair.second;
	}

	mTextureBuffer.clear();
}

poro::ITexture* GetTexture( const std::string& filename )
{
	TextureBuffer* buffer = GetTextureBuffer( filename );

	if ( buffer == NULL ) return NULL;

	return buffer->texture;
}

ceng::CArray2D< Uint32 >* GetImageData( const std::string& filename, bool load_and_cache_if_needed )
{
	TextureBuffer* buffer = GetTextureBuffer( filename );

	if ( buffer == NULL ) return NULL;

	ceng::CArray2D< Uint32 >* image_data = buffer->image_data;
	if ( load_and_cache_if_needed && image_data == NULL)
	{
		image_data = new ceng::CArray2D< Uint32 >();
		LoadImage( filename, *image_data, true );
		buffer->image_data = image_data;
	}

	return image_data;
}
//-----------------------------------------------------------------------------

void LoadSpriteTo( const std::string& filename, as::Sprite* result )
{
	cassert( result );

	if( filename.size() >= 3 && filename.substr( filename.size() - 3 ) == "xml" )
	{
		// if we're loading an xml file
		using namespace impl;

		SpriteLoadHelper* sprite_data = GetSpriteLoadHelper( filename );
		ApplySpriteLoadHelperToSprite( result, sprite_data );
		result->SetFilename( filename );
		return;
	}
	else
	{
		// we're loading just a texture
		TextureBuffer* buffer = GetTextureBuffer( filename );
		if ( buffer == NULL )
			return;

		poro::ITexture* image = buffer->texture;
		if( image == NULL )
			return;

		result->SetTexture( image );
		result->SetImageData( buffer->image_data );
		result->SetSize( (int)image->GetWidth(), (int)image->GetHeight() );
		result->SetFilename( filename );
		return;
	}
}

//-----------------------------------------------------------------------------

int Sprite::culled_this_frame = 0;
int Sprite::rendered_this_frame = 0;
int Sprite::total_sprites = 0;

//-----------------------------------------------------------------------------

Sprite* LoadSpriteLegacy( const std::string& filename )
{
	Sprite* result = new Sprite;
	LoadSpriteTo( filename, result );
	return result;
}

bool PreloadSprite( const std::string& filename )
{
	if( filename.size() >= 3 && filename.substr( filename.size() - 3 ) == "xml" )
	{
		impl::SpriteLoadHelper* sprite_data = GetSpriteLoadHelper(filename);
		return ( sprite_data ? 1 : 0 );
	}

	TextureBuffer* buffer = GetTextureBuffer( filename );
	return ( buffer ? 1 : 0 );
}

///////////////////////////////////////////////////////////////////////////////

Sprite::Sprite() : 
	DisplayObjectContainer(),
	mClearTweens( true ),
	mBlendMode( poro::BLEND_MODE::NORMAL ),
	mName(),
	mTexture( NULL ),
	mImageData( NULL ),
	mSize( 0, 0 ),
	mCenterOffset( 0, 0 ),
	mXForm(),
	mZ( 100 ),
	mLastFrameRendered( -1 ),
	mColor( 4 ),
	mDead( false ),
	mVisible( true ),
	mRect( NULL ),
	mShader( NULL ),
	mMainTextureFilteringMode( poro::TEXTURE_FILTERING_MODE::NEAREST ),
	mRectAnimations( NULL ),
	mRectAnimation( NULL ),
	mAnimations( NULL ),
	mAnimationUpdater( NULL ),
	mHasAnimationFinished( false )
{
	mColor[ 0 ] = 1.f;
	mColor[ 1 ] = 1.f;
	mColor[ 2 ] = 1.f;
	mColor[ 3 ] = 1.f;

	#ifdef WIZARD_DEBUG
		total_sprites++;
	#endif
}

Sprite::~Sprite()
{
	// for debug reasons
	if( mClearTweens )
		GTweenClearSpriteOfTweens( this );

	mRectAnimation = NULL;

	delete mRect;
	mRect = NULL;

	if( mFather )
		mFather->removeChild( this );

	mFather = NULL;

	Clear();

	#ifdef WIZARD_DEBUG
		total_sprites--;
	#endif
}
//-----------------------------------------------------------------------------

Sprite* Sprite::GetChildByName( const std::string& name, bool recursive ) const
{
	ChildList::const_iterator i;
	for( i = mChildren.begin(); i != mChildren.end(); ++i )
	{
		Sprite* sprite = dynamic_cast< Sprite* >(*i);
		if( sprite )
		{
			if(sprite->GetName() == name )
			{
				return sprite;
			}
			else if( recursive )
			{
				as::Sprite* temp = sprite->GetChildByName( name, recursive );
				if( temp ) return temp;
			}
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Sprite::Clear()
{
	DisplayObjectContainer::ChildList erase_me = mChildren;
	for( DisplayObjectContainer::ChildList::iterator i = erase_me.begin(); i != erase_me.end(); ++i )
		delete *i;
	

	mChildren.clear();
}

//-----------------------------------------------------------------------------

void Sprite::Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform ) 
{ 
	// cull clearly invisible sprites
	if( mVisible == false || this->GetScaleX() == 0 || this->GetScaleY() == 0 )
		return;

	if( mRect ) 
		DrawRect( *mRect, graphics, camera, transform );  
	else 
		DrawRect( types::rect( 0, 0, mSize.x, mSize.y ), graphics, camera, transform ); 
	
	// draw all children
	DrawChildren( graphics, camera, transform );
}
//-----------------------------------------------------------------------------

void Sprite::DrawChildren( poro::IGraphics* graphics, types::camera* camera, Transform& transform )
{
	if ( mChildren.empty() )
		return;

	transform.PushXForm( mXForm, mColor );

	DisplayObjectContainer::ChildList::iterator i;
	Sprite* current = NULL;

	for( i = mChildren.begin(); i != mChildren.end(); )
	{
		cassert( *i );
		cassert( (*i)->GetFather() == this );

		if( (*i)->GetSpriteType() == this->GetSpriteType() )
		{			
			current = dynamic_cast< Sprite* >(*i);
			cassert( current );
			if( current->IsSpriteDead() == false )
			{
				current->Draw( graphics, camera, transform );
				++i;
			}
			else 
			{
				DisplayObjectContainer::ChildList::iterator remove = i;
				++i;
				mChildren.erase( remove );
				// current->SetFather( NULL );
				delete current;
			}
		}
		else
		{
			++i;
		}
	}

	transform.PopXForm();
}	
//-----------------------------------------------------------------------------

void Sprite::DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const Transform& transform )
{
	poro_assert( graphics );

	if( mTexture == NULL )
		return;

	static poro::types::vec2 temp_verts[ 4 ];
	static poro::types::vec2 tex_coords[ 4 ];
	static poro::types::vec2 alpha_tex_coords[ 4 ];

	const types::xform& matrix = transform.GetXForm();
	const std::vector< float >& tcolor = transform.GetColor();

	types::rect dest_rect(rect.x, rect.y, rect.w, rect.h );
	poro::types::fcolor color_me = poro::GetFColor( 
	mColor[ 0 ] * tcolor[ 0 ], 
	mColor[ 1 ] * tcolor[ 1 ], 
	mColor[ 2 ] * tcolor[ 2 ], 
	mColor[ 3 ] * tcolor[ 3 ] );

	temp_verts[ 0 ].x = (float)0 - mCenterOffset.x;
	temp_verts[ 0 ].y = (float)0 - mCenterOffset.y;
	temp_verts[ 1 ].x = (float)0 - mCenterOffset.x;
	temp_verts[ 1 ].y = (float)dest_rect.h - mCenterOffset.y;
	temp_verts[ 3 ].x = (float)(dest_rect.w) - mCenterOffset.x;
	temp_verts[ 3 ].y = (float)0 - mCenterOffset.y;
	temp_verts[ 2 ].x = (float)dest_rect.w - mCenterOffset.x;
	temp_verts[ 2 ].y = (float)dest_rect.h - mCenterOffset.y;

	for( int i = 0; i < 4; ++i )
	{
		temp_verts[ i ] = ceng::math::MulWithScale( mXForm,  temp_verts[ i ] );
		temp_verts[ i ] = ceng::math::MulWithScale( matrix, temp_verts[ i ] );
	}

	// cull the vertices so we can at least stress the graphics driver a bit less
	if ( IsOutsideSreen( graphics, temp_verts ) )
		return;

#ifdef WIZARD_DEBUG
	rendered_this_frame++;
#endif

	mLastFrameRendered = Poro()->GetFrameNum();

	// blend mode
	if( mBlendMode != poro::BLEND_MODE::NORMAL )
		graphics->PushBlendMode( mBlendMode );

	tex_coords[ 0 ].x = dest_rect.x;
	tex_coords[ 0 ].y = dest_rect.y;
	tex_coords[ 1 ].x = dest_rect.x;
	tex_coords[ 1 ].y = dest_rect.y + dest_rect.h;
	tex_coords[ 3 ].x = dest_rect.x + dest_rect.w;
	tex_coords[ 3 ].y = dest_rect.y;
	tex_coords[ 2 ].x = dest_rect.x + dest_rect.w;
	tex_coords[ 2 ].y = dest_rect.y + dest_rect.h;

	// ---
	graphics->SetShader( mShader );
	if ( mShader )
	{
		graphics->Shader_SetParameter( mShader, "tex", mTexture, mMainTextureFilteringMode );
		graphics->Shader_SetParameter( mShader, "tex_size", poro::types::vec2( (float)mTexture->GetDataWidth(), (float)mTexture->GetDataHeight() ) );
	}

	graphics->DrawTexture( mTexture, temp_verts, tex_coords, 4, color_me, poro::DRAW_PRIMITIVE_MODE::TRIANGLE_FAN );

	graphics->SetShader( NULL );
		
	if( mBlendMode != poro::BLEND_MODE::NORMAL )
		graphics->PopBlendMode();
}
///////////////////////////////////////////////////////////////////////////////

void DrawSprite( Sprite* sprite, poro::IGraphics* graphics, types::camera* camera )
{
    Transform t;
    DrawSprite( sprite, graphics, camera, t );
}

void DrawSprite( Sprite* sprite, poro::IGraphics* graphics, types::camera* camera, Transform& transform )
{
	cassert( sprite );
	cassert( graphics );

	sprite->Draw( graphics, camera, transform );
}

bool Sprite::IsOutsideSreen(poro::IGraphics* graphics, const poro::types::vec2* verts)
{
	using namespace std;

	// calculate AABB for the transformed verts
	using namespace std;
	poro::types::vec2 verts_min = verts[0];
	poro::types::vec2 verts_max = verts[0];

	for ( int i = 1; i < 4; ++i )
	{
		verts_min.x = min( verts[i].x, verts_min.x );
		verts_min.y = min( verts[i].y, verts_min.y );
		verts_max.x = max( verts[i].x, verts_max.x );
		verts_max.y = max( verts[i].y, verts_max.y );
	}

	// don't render if outside the screen
	bool outside_screen = verts_max.x < 0 || verts_max.y < 0 || verts_min.x > graphics->GetInternalSize().x || verts_min.y > graphics->GetInternalSize().y;

#ifdef WIZARD_DEBUG
	culled_this_frame += (int)outside_screen;
#endif

	return outside_screen;
}

//=============================================================================

void Sprite::Update( float dt )
{
	mHasAnimationFinished = false;

	if( mRectAnimation ) 
		mRectAnimation->Update( this, dt );

	if( mAnimationUpdater.get() ) 
		mAnimationUpdater->Update( dt );

	// update children as well
	ChildList::iterator i;
	for( i = mChildren.begin(); i != mChildren.end(); ++i )
	{
		Sprite* sprite = dynamic_cast< Sprite* >(*i);
		if( sprite ) 
		{
			cassert( sprite->GetFather() == this );
			sprite->Update( dt );
		}
	}

}
//=============================================================================

void Sprite::SetScale( float w, float h ) { 
	mXForm.scale.x = w; 
	mXForm.scale.y = h; 
}

//=============================================================================

void Sprite::MoveCenterTo( const types::vector2& p ) { 
	MoveTo( p );
}

//=============================================================================

types::rect Sprite::RectAnimation::FigureOutRectPos( int frame ) const
{
	cassert( frame >= 0 );
	cassert( mFramesPerRow != 0 );
	int y_pos = (int)( frame / mFramesPerRow );
	int x_pos = frame % mFramesPerRow;

	if( mShrinkByOnePixel )
	{
		return 
			types::rect( 
			(float)( mPositionX + x_pos * mWidth ), 
			(float)( mPositionY + y_pos * mHeight ), 
			(float)mWidth - 1, 
			(float)mHeight - 1 );

	}
	else
	{
		return 
			types::rect( 
			(float)( mPositionX + x_pos * mWidth ), 
			(float)( mPositionY + y_pos * mHeight ), 
			(float)mWidth, 
			(float)mHeight );
	}
}

types::irect Sprite::RectAnimation::FigureOutIRectPos( int frame ) const
{
	cassert( frame >= 0 );
	cassert( mFramesPerRow != 0 );
	int y_pos = (int)( frame / mFramesPerRow );
	int x_pos = frame % mFramesPerRow;

	if( mShrinkByOnePixel )
	{
		return 
			types::irect( 
			(int)( mPositionX + x_pos * mWidth ), 
			(int)( mPositionY + y_pos * mHeight ), 
			(int)mWidth - 1, 
			(int)mHeight - 1 );
	}
	else
	{
		return 
				types::irect( 
				(int)( mPositionX + x_pos * mWidth  ), 
				(int)( mPositionY + y_pos * mHeight  ), 
				(int)mWidth, 
				(int)mHeight );
	}
}


void Sprite::RectAnimation::Update( Sprite* sprite, float dt ) const
{
	// mPaused
	// mPreviousFrame
	// mCurrentTime
	// mCurrentFrame

	as::Sprite::RectAnimationData* data = &(sprite->mRectAnimationData);
	if( data->mPaused ) 
		dt = 0;

	if ( data->mPreviousFrame > -1 )
		data->mEnteredNewFrame = false;

	data->mPreviousFrame = data->mCurrentFrame;
	int frame = data->mCurrentFrame;
	data->mCurrentTime += dt;

	if( data->mWaitTime > 0 && data->mCurrentTime >= data->mWaitTime )
	{
		float frames = ( data->mCurrentTime / data->mWaitTime );
		int iframes = (int)floor( frames );
		data->mCurrentTime  -= iframes * data->mWaitTime;
		frame += iframes;
		if( frame >= mFrameCount) 
		{
			if( mLoop ) 
			{
				cassert( mFrameCount > 0 );
				frame = frame % mFrameCount;
			}
			else 
			{
				sprite->mHasAnimationFinished = true;
				frame = mFrameCount - 1;

				if( mNextAnimation.empty() == false ) 
				{
					sprite->PlayRectAnimation( mNextAnimation );
					return;
				}
			}
		}

		data->mEnteredNewFrame = true;
	}

	SetFrame( sprite, frame, false );
}
	
void Sprite::RectAnimation::SetFrame( Sprite* sprite, int frame, bool update_anyhow ) const
{
	// is at the end
	as::Sprite::RectAnimationData* data = &(sprite->mRectAnimationData);
	if( frame >= mFrameCount && mLoop == false ) 
	{
		if( mNextAnimation.empty() == false ) 
		{
			sprite->PlayRectAnimation( mNextAnimation );
			return;
		}

		return;
	}

	// --- figure the frame --

	if( frame == 0 || frame != data->mCurrentFrame || update_anyhow ) 
	{
		// if( frame > mFrameCount && mLoop ) frame = frame % mFrameCount;

		cassert( sprite );
		data->mCurrentFrame  = frame;
		sprite->SetRect( FigureOutRectPos( data->mCurrentFrame ) );
		
		if( mHasNewCenterOffset ) sprite->SetCenterOffset( mCenterOffset );

		// update children
		// find the animations with the frame
		for( std::size_t i = 0; i < mChildAnimations.size(); ++i )
		{
			const ChildAnimation* child_anim = &(mChildAnimations[ i ]);
			cassert( child_anim );
			if( child_anim->frame == frame ) 
			{
				Sprite* child = NULL;

				as::Sprite* parent = dynamic_cast< Sprite* >( sprite->getParent() );
				

				if( parent ) child = parent->GetChildByName( child_anim->sprite_name );
				else child = sprite->GetChildByName( child_anim->sprite_name );

				if( child == NULL ) continue;

				child->MoveTo( child_anim->position );
				child->SetRotation( child_anim->rotation );
				child->SetAlpha( child_anim->alpha );
				child->SetScale( child_anim->scale.x, child_anim->scale.y );
				// sprite->ApplyChildAnimation( mChildAnimations[ i ] );
			}
		}
	}
}

void Sprite::RectAnimation::Serialize( ceng::CXmlFileSys* filesys )
{
#ifdef WIZARD_DEBUG_SPRITES
	if( filesys->IsWriting() )
	{
		RectAnimation default_values;

		if( mName != default_values.mName )
			XML_BindAttributeAlias( filesys, mName, "name" );

		if( mFrameCount != default_values.mFrameCount )
			XML_BindAttributeAlias( filesys, mFrameCount, "frame_count" );

		if( mWidth != default_values.mWidth )
			XML_BindAttributeAlias( filesys, mWidth, "frame_width" );

		if( mHeight != default_values.mHeight )
			XML_BindAttributeAlias( filesys, mHeight, "frame_height" );

		if( mFramesPerRow != default_values.mFramesPerRow )
			XML_BindAttributeAlias( filesys, mFramesPerRow, "frames_per_row" );
		
		if( mShrinkByOnePixel != default_values.mShrinkByOnePixel )
			XML_BindAttributeAlias( filesys, mShrinkByOnePixel, "shrink_by_one_pixel" );

		if( mPositionX != default_values.mPositionX ||
			mPositionY != default_values.mPositionY )
		{
			XML_BindAttributeAlias( filesys, mPositionX, "pos_x" );
			XML_BindAttributeAlias( filesys, mPositionY, "pos_y" ); 
		}

		if( mWaitTime != default_values.mWaitTime )
			XML_BindAttributeAlias( filesys, mWaitTime, "frame_wait" );

		if( mHasNewCenterOffset != default_values.mHasNewCenterOffset )
			XML_BindAttributeAlias( filesys, mHasNewCenterOffset, "has_offset" );

		if( mCenterOffset != default_values.mCenterOffset )
		{
			XML_BindAttributeAlias( filesys, mCenterOffset.x, "offset_x" );
			XML_BindAttributeAlias( filesys, mCenterOffset.y, "offset_y" );
		}

		if( mLoop != default_values.mLoop )
			XML_BindAttributeAlias( filesys, mLoop, "loop" );

		if( mNextAnimation != default_values.mNextAnimation )
			XML_BindAttributeAlias( filesys, mNextAnimation, "next_animation" );


		ceng::VectorXmlSerializerObjects< Sprite::ChildAnimation > serializer( mChildAnimations, "ChildAnimations" );
		serializer.Serialize( filesys );

		ceng::VectorXmlSerializerObjects< Sprite::Event > event_serializer( mEvents, "Event" );
		event_serializer.Serialize( filesys );

		return;
	}
#endif

	XML_BindAttributeAlias( filesys, mName, "name" );

	XML_BindAttributeAlias( filesys, mFrameCount, "frame_count" );
	XML_BindAttributeAlias( filesys, mWidth, "frame_width" );
	XML_BindAttributeAlias( filesys, mHeight, "frame_height" );
	XML_BindAttributeAlias( filesys, mFramesPerRow, "frames_per_row" );
	XML_BindAttributeAlias( filesys, mShrinkByOnePixel, "shrink_by_one_pixel" );

	XML_BindAttributeAlias( filesys, mPositionX, "pos_x" );
	XML_BindAttributeAlias( filesys, mPositionY, "pos_y" ); 

	XML_BindAttributeAlias( filesys, mWaitTime, "frame_wait" );

	XML_BindAttributeAlias( filesys, mHasNewCenterOffset, "has_offset" );
	XML_BindAttributeAlias( filesys, mCenterOffset.x, "offset_x" );
	XML_BindAttributeAlias( filesys, mCenterOffset.y, "offset_y" );

	XML_BindAttributeAlias( filesys, mLoop, "loop" );

	XML_BindAttributeAlias( filesys, mNextAnimation, "next_animation" );

	ceng::VectorXmlSerializerObjects< Sprite::ChildAnimation > serializer( mChildAnimations, "ChildAnimations" );
	serializer.Serialize( filesys );

	ceng::VectorXmlSerializerObjects< Sprite::Event > event_serializer( mEvents, "Event" );
	event_serializer.Serialize( filesys );
}

//-------------------------------------------------------------------------

void Sprite::SetRectAnimation( const RectAnimation* animation )
{
	mRectAnimation = animation;
	mRectAnimationData.mPaused = false;
	if( mRectAnimation )
	{
		mRectAnimationData.mWaitTime = mRectAnimation->mWaitTime;
		mRectAnimationData.mCurrentFrame = 0;
		mRectAnimationData.mPreviousFrame = -1;
		mRectAnimationData.mCurrentTime = 0;
		mRectAnimationData.mEnteredNewFrame = true;
	}
	
}
//-------------------------------------------------------------------------

void Sprite::SetRectAnimations( std::vector< RectAnimation >* animations ) 
{
	mRectAnimations = animations;
}
//-----------------------------------------------------------------------------

void Sprite::PlayRectAnimation( const std::string& name )
{
	if( mRectAnimation && mRectAnimation->mName == name )
	{
		mRectAnimationData.mPaused = false;
		return;
	}

	if( mRectAnimations == NULL ) 
		return;

	for( std::size_t i = 0; i < mRectAnimations->size(); ++i ) 
	{
		auto anim = &(mRectAnimations->at(i));
		if( anim->mName == name ) 
		{
			SetRectAnimation( anim );
			// add so that the rect animation doesn't flash for 1 frame
			const bool mFinishedTemp = mHasAnimationFinished;
			Update( 0 );
			mHasAnimationFinished = mFinishedTemp;
			return;
		}
	}
}
//-----------------------------------------------------------------------------

void Sprite::PauseRectAnimation()
{
	if( mRectAnimation ) 
		mRectAnimationData.mPaused = true;
}
//-----------------------------------------------------------------------------

bool Sprite::IsRectAnimationPlaying() const
{
	if( mRectAnimation == NULL ) return false;
	if( mRectAnimation->mLoop )  return true;
	if( mRectAnimationData.mCurrentFrame >= mRectAnimation->mFrameCount - 1 ) return false;

	return true;
}
//-----------------------------------------------------------------------------

bool Sprite::HasRectAnimationJustFinished() const
{
	if( mRectAnimation == NULL ) return false;
	if( mHasAnimationFinished )  return true;
	// this is unneeded
	// if( mRectAnimation->mLoop )  return false; 

	return false;
}
//-----------------------------------------------------------------------------

bool Sprite::HasRectAnimation( const std::string& name ) const
{
	if( mRectAnimation && mRectAnimation->mName == name ) return true;

	if( mRectAnimations == NULL ) 
		return false;

	for( std::size_t i = 0; i < mRectAnimations->size(); ++i ) 
	{
		if( mRectAnimations->at(i).mName == name ) 
			return true;
	}

	return false;
}
//-----------------------------------------------------------------------------

std::string Sprite::GetRectAnimationName() const
{
	if( mRectAnimation ) return mRectAnimation->mName;
	return "";
}
//-----------------------------------------------------------------------------

bool Sprite::IsAnimationPlaying() const
{
	if( mAnimationUpdater.get() == NULL ) return false;
	if( mAnimationUpdater->IsOver() ) return false;

	return true;
}

void Sprite::PlayAnimation( const std::string& animation_name )
{
	PlayRectAnimation( animation_name );

	if( mAnimations == NULL )
	{
		// logger << "Error trying to play animation before AnimationsSheet has been added: " << animation_name << "\n";
		return;
	}

	if( mAnimationUpdater.get() == NULL )
		mAnimationUpdater.reset( new SpriteAnimationUpdater );


	impl::Animation* a = mAnimations->GetAnimation( animation_name );

	if( a == NULL )
	{
		logger_error << "Error animation not found in animation sheet: " << animation_name << "\n";
		mAnimationUpdater.reset( NULL );
		return;
	}

	mAnimationUpdater->Reset();
	mAnimationUpdater->SetAnimation( a  );
	mAnimationUpdater->SetSprite( this );
	mAnimationUpdater->Update( 0 );
}

void Sprite::SetAnimationFrame(int frame) 
{
	if( mAnimationUpdater.get() ) mAnimationUpdater->SetFrame( frame );
	if( mRectAnimation ) mRectAnimation->SetFrame( this, frame, false );
}
//-----------------------------------------------------------------------------

types::vector2 Sprite::GetHotspot( const std::string& name ) const
{
	types::vector2 result = -GetCenterOffset();

	if ( auto rect_animation = GetRectAnimation() )
	{
		for ( auto& hotspot : rect_animation->mHotspots )
		{
			if ( hotspot.name == name )
			{
				const int frame =  mRectAnimationData.mCurrentFrame;
				if ( frame < (int)hotspot.positions.size() )
					result += types::vector2( hotspot.positions[ frame ] );

				break;
			}
		}
	}

	return result;
}
//-----------------------------------------------------------------------------

types::vector2 Sprite::GetScreenPosition()
{
	// types::vector2 result( 0, 0 );
	std::vector< DisplayObjectContainer* > parents;
	getParentTree( parents );

	types::xform xform;
	for( int i = (int)parents.size() - 1; i > 0; --i )
	{
		cassert( parents[ i ] );
		const Sprite* parent = dynamic_cast< const Sprite* >( parents[ i ] );
		if( parent )
			xform = ceng::math::Mul( xform, parent->GetXForm() );
	}

	poro::types::vec2 temp( GetPos().x, GetPos().y );
	temp = ceng::math::MulWithScale( xform, temp );

	return types::vector2( temp.x, temp.y );
}
//-----------------------------------------------------------------------------

types::vector2 Sprite::TransformWithAllParents( const types::vector2& mouse_pos )
{
	types::vector2 result( 0, 0 );
	std::vector< DisplayObjectContainer* > parents;
	getParentTree( parents );

	types::xform xform;
	// the i > 0 is on purpouse, so that we don't transform with ourselves
	for( int i = (int)parents.size() - 1; i > 0; --i )
	{
		cassert( parents[ i ] );
		const Sprite* parent = dynamic_cast< const Sprite* >( parents[ i ] );
		if( parent )
			xform = ceng::math::Mul( xform, parent->GetXForm() );
	}

	return ceng::math::MulTWithScale( xform, mouse_pos );
}
//-----------------------------------------------------------------------------

types::vector2 Sprite::MultiplyByParentXForm( const types::vector2& p ) const
{
	types::vector2 result = ceng::math::Mul( GetXForm(), p );
	
	Sprite* parent = dynamic_cast< Sprite* >( getParent() );

	if( parent == NULL ) 
		return result;
	else 
		return parent->MultiplyByParentXForm( result );
}
//-----------------------------------------------------------------------------

std::vector< Sprite* > Sprite::FindSpritesAtPoint( const types::vector2& p )
{
	// bool ignore_hidden = true;
	//if( ignore_hidden && GetVisibility() == false ) return std::vector< Sprite* >();

	std::vector< Sprite* > result;

	Transform t;
	FindSpritesAtPointImpl( p, t, result );

	return result;
}
//-----------------------------------------------------------------------------

void Sprite::FindSpritesAtPointImpl( const types::vector2& pos, Transform& transform, std::vector< Sprite* >& results )
{
	// NOTE( Petri ): 2018.12.07 - I added the ignore_hidden, I'm terrified that it'll break something somewhere
	bool ignore_hidden = true;

	const types::xform& matrix = transform.GetXForm();
	types::rect dest_rect( 0, 0, mSize.x, mSize.y );
	if( mRect ) 
	{
		dest_rect.w = mRect->w; dest_rect.h = mRect->h; 
	}

	if( dest_rect.w != 0 && dest_rect.h != 0 )
	{
		types::vector2 polygon[ 3 ];
		polygon[ 0 ].Set( -mCenterOffset.x,					-mCenterOffset.y );
		polygon[ 1 ].Set( -mCenterOffset.x,					dest_rect.h - mCenterOffset.y );
		polygon[ 2 ].Set( dest_rect.w - mCenterOffset.x,	-mCenterOffset.y );
		for( int i = 0; i < 3; ++i )
		{
			polygon[ i ] = ceng::math::MulWithScale( mXForm, polygon[ i ] );
			polygon[ i ] = ceng::math::MulWithScale( matrix, polygon[ i ] );
		}

		types::vector2 x_axis = polygon[1] - polygon[0];
		types::vector2 y_axis = polygon[2] - polygon[0];
		types::vector2 local_pos = pos - polygon[0];
		float u = ceng::math::Dot( local_pos, x_axis );
		float v = ceng::math::Dot( local_pos, y_axis );
		cassert( x_axis.LengthSquared() > 0 );
		cassert( y_axis.LengthSquared() > 0 );
		u /= x_axis.LengthSquared();
		v /= y_axis.LengthSquared();

		if( u >= 0 && u < 1.f && v >= 0 && v < 1.f && (ignore_hidden == false || GetVisibility() ) )
		{
			results.push_back( this );
		}
	}

	if( mChildren.empty() || (ignore_hidden && GetVisibility() == false ) )
		return;

	transform.PushXForm( mXForm, mColor );

	for ( auto& child : mChildren )
	{
		if( child->GetSpriteType() == this->GetSpriteType() )
		{			
			auto current = dynamic_cast< Sprite* >( child );
			cassert( current );
			if( current == NULL || current->IsSpriteDead() )
				continue;
		
			current->FindSpritesAtPointImpl( pos, transform, results );
		}
	}

	transform.PopXForm();
}
//-----------------------------------------------------------------------------

} // end of namespace as
