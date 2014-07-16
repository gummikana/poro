/********************************************************************
 ** File:     memcpy.h
 **
 ** Copyright (C) 2005 Daniel Vik
 ** 
 ** This software is provided 'as-is', without any express or implied
 ** warranty. In no event will the authors be held liable for any
 ** damages arising from the use of this software.
 ** Permission is granted to anyone to use this software for any
 ** purpose, including commercial applications, and to alter it and
 ** redistribute it freely, subject to the following restrictions:
 ** 
 ** 1. The origin of this software must not be misrepresented; you
 **    must not claim that you wrote the original software. If you
 **    use this software in a product, an acknowledgment in the
 **    use this software in a product, an acknowledgment in the
 **    product documentation would be appreciated but is not
 **    required.
 ** 
 ** 2. Altered source versions must be plainly marked as such, and
 **    must not be misrepresented as being the original software.
 ** 
 ** 3. This notice may not be removed or altered from any source
 **    distribution.
 ** 
 ** 
 ** Description: Implementation of the standard library function memcpy.
 **             This implementation of memcpy() is ANSI-C89 compatible.
 **
 *******************************************************************/

/* NOTE: MODIFIED THIS FILE TO INCLUDE X_aligned_memcpy_sse2 function,
also renamed the memcpy function to fast_memcpy so that it won't 
collide with actual memcpy. Petri Purho - March 2012
*/

/********************************************************************
 ** Includes for size_t definition
 *******************************************************************/


/********************************************************************
 **
 ** void *memcpy(void *dest, const void *src, size_t count)
 **
 ** Args:     dest    - pointer to destination buffer
 **           src     - pointer to source buffer
 **           count   - number of bytes to copy
 **
 ** Return:   A pointer to destination buffer
 **
 ** Purpose:  Copies count bytes from src to dest. No overlap check
 **           is performed.
 **
 *******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void *fast_memcpy(void *dest, const void *src, size_t count);

void X_aligned_memcpy_sse2(void* dest, const void* src, const unsigned long size_t);
	
#ifdef __cplusplus
}
#endif
