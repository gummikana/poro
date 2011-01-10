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


///////////////////////////////////////////////////////////////////////////////
//
// Advanced Postfix Operands
// =========================
//
// Some more advanced mathematical operations, like sin, cos, tan, sqrt 
// and pow.
//
//
// Created 14.08.2006 by Pete
//
//=============================================================================
//.............................................................................

#ifndef INC_ADVANCEDPOSTFIXOPERANDS_H
#define INC_ADVANCEDPOSTFIXOPERANDS_H

#include <math.h>
#include "ipostfixoperand.h"


namespace ceng { 

template< class NumericType >
class PostfixOperandSine : public IPostfixOperand< NumericType >
{
public:
	virtual void Operate( std::stack< NumericType >& stack )
	{
		NumericType first;
		NumericType result;
		
		if( stack.empty() ) return;
		first = stack.top();
		stack.pop();
		
		result = (NumericType)sin( (double)first );

		stack.push( result );
	}
};

template< class NumericType >
class PostfixOperandCosine : public IPostfixOperand< NumericType >
{
public:
	virtual void Operate( std::stack< NumericType >& stack )
	{
		NumericType first;
		NumericType result;
		
		if( stack.empty() ) return;
		first = stack.top();
		stack.pop();
		
		result = (NumericType)cos( (double)first );

		stack.push( result );
	}
};

template< class NumericType >
class PostfixOperandTangent : public IPostfixOperand< NumericType >
{
public:
	virtual void Operate( std::stack< NumericType >& stack )
	{
		NumericType first;
		NumericType result;
		
		if( stack.empty() ) return;
		first = stack.top();
		stack.pop();
		
		result = (NumericType)tan( (double)first );

		stack.push( result );
	}
};

template< class NumericType >
class PostfixOperandSqrt : public IPostfixOperand< NumericType >
{
public:
	virtual void Operate( std::stack< NumericType >& stack )
	{
		NumericType first;
		NumericType result;
		
		if( stack.empty() ) return;
		first = stack.top();
		stack.pop();
		
		result = (NumericType)sqrt( (double)first );

		stack.push( result );
	}
};

template< class NumericType >
class PostfixOperandExponent : public IPostfixOperand< NumericType >
{
public:
	virtual void Operate( std::stack< NumericType >& stack )
	{
		NumericType first;
		NumericType second;
		NumericType result;
		
		if( stack.empty() ) return;
		second = stack.top();
		stack.pop();
		
		if( stack.empty() ) return;
		first = stack.top();
		stack.pop();
		
		result = (NumericType)pow( (double)first, (double)second );

		stack.push( result );
	}
};

} // end of namespace ceng

#endif
