#include "Animations.h"

namespace as {


Animations* LoadAllAnimations()
{
	using namespace impl;
	Animations* result = new Animations;

	const int number_of_parts = 50;
	const int frameCount = 1;
	const int loopAt = 0;
	const bool visible = false;
	double x = 1;
	double y = 1;
	double scaleX = 1;
	double scaleY = 1;
	double rotation = 0;
	double alpha = 0;


	result->animations.resize( 1 );
	result->animations[ 0 ] = Animation( "name", frameCount, loopAt );
	result->animations[ 0 ].parts.resize( number_of_parts );
	
	result->animations[ 0 ].parts[ 0 ] = Part( "part_name" );
	result->animations[ 0 ].parts[ 0 ].frames.resize( frameCount );

	result->animations[ 0 ].parts[ 0 ].frames[ 0 ] = Frame( visible, x, y, scaleX, scaleY, rotation, alpha );


	return result;
}

}