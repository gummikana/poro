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


#ifndef INC_COMMON_POSTFIXOPERANDS_H
#define INC_COMMON_POSTFIXOPERANDS_H

#include "ipostfixoperand.h"

#include <stack>

namespace ceng {

///////////////////////////////////////////////////////////////////////////////

//! Basic addition operand
template< class NumericType >
class PostfixOperandAddition : public IPostfixOperand< NumericType >
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
		
		if( stack.empty() ) 
		{
			first = 0;
		}
		else
		{
			first = stack.top();
			stack.pop();
		}
	
		result = first + second;

		stack.push( result );
	}
};

///////////////////////////////////////////////////////////////////////////////

//! Basic subtract operand
template< class NumericType >
class PostfixOperandSubtract : public IPostfixOperand< NumericType >
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
		
		if( stack.empty() ) 
		{
			first = 0;
		}
		else
		{
			first = stack.top();
			stack.pop();
		}
	
		result = first - second;

		stack.push( result );
	}
};

///////////////////////////////////////////////////////////////////////////////

//! basic multiply operand
template< class NumericType >
class PostfixOperandMultiply : public IPostfixOperand< NumericType >
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
	
		result = first * second;

		stack.push( result );
	}
};

///////////////////////////////////////////////////////////////////////////////

//! basic divide operand
template< class NumericType >
class PostfixOperandDivide : public IPostfixOperand< NumericType >
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
	
		// cassert( second != 0 ).log( "PostfixOperandDivide::Operate() Divide by zero" );
		if( second != 0 )
			result = first / second;

		stack.push( result );
	}
};

///////////////////////////////////////////////////////////////////////////////

} // end of namespace ceng

#endif

