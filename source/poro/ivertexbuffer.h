/***************************************************************************
*
* Copyright (c) 2010 Petri Purho, Dennis Belfrage, Olli Harjola
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

#ifndef INC_IVERTEXBUFFER_H
#define INC_IVERTEXBUFFER_H

namespace poro {

	class IVertexBuffer
	{
	public:
		uint32 impl_data = 0;
		uint32 num_vertices = 0;
		uint32 format = 0;
		uint32 vertex_size = 0;
	};

} // end of namespace poro

#endif