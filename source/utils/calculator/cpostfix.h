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
// CPostfix
// ========
//
// A postfix calculator. 
//
// (3 + 4) * (5 / 2) equals as postfix 3 4 + 5 2 / *
//
// separator is space.
//
// You can add your own operands if you like. The default operands are derived
// from the IPostfixOperand class and used like the others.
//
// Created 10.10.2005 by Pete
//
//=============================================================================
//
// 24.08.2006 Pete
//		Fixed small things to make this more msvc2005 compatible.
//
//.............................................................................
#ifndef INC_CPOSTFIX_H
#define INC_CPOSTFIX_H

#include <sstream>
#include <string>
#include <stack>
#include <map>
#include <vector>

#include "calculator_libs.h"


#include "ipostfixoperand.h"
#include "common_postfixoperands.h"

namespace ceng {


//! A normal postfix calculator.
template< class NumericType, class OperatorType = std::string >
class CPostfix
{
public:


	//=========================================================================

	CPostfix() :
		myStack(),
		myOperands(),

		opAdd(),
		opSub(),
		opMul(),
		opDiv(),

		mySeparator( " " )
	{ 
		RegisterOperand( "+", &opAdd );
		RegisterOperand( "-", &opSub );
		RegisterOperand( "*", &opMul );
		RegisterOperand( "/", &opDiv );
	}

	~CPostfix() { }
	
	//=========================================================================

	//! Registers an operand, does not release them, you shoud take care of 
	//! that.
	void RegisterOperand( const OperatorType& name, IPostfixOperand< NumericType >* ope )
	{
		cassert( myOperands.find( name ) == myOperands.end() );
		myOperands.insert( std::pair< OperatorType, IPostfixOperand< NumericType >* >( name, ope ) );
	}

	//=========================================================================

	//! returns true if we have a operand by the given name
	/*!	
		Does not take into account white space.
	*/
	bool IsOperand( const std::string& str )
	{
		return ( myOperands.find( str ) != myOperands.end() );
	}

	//! returns true if the given string is number
	/*!
		Does not take into account white space.
	*/
	bool IsNumber( const std::string& str )
	{
		return ( str.find_first_not_of("0123456789.+-") == str.npos );
	}

	//=========================================================================

	//! "Parses" a single line / section.
	/*!
		does not take into account white space
	*/
	void Parse( const std::string& str )
	{
		if( IsOperand( str ) )
		{
			CallOperand( str );
		}
		else if ( IsNumber( str ) )
		{
			PushStack( str );
		}
	}

	//! "Parses" a bunch of operators a postfix line so to speak.
	/*!
		does take into account white space... which is nice
	*/
	NumericType operator() ( const std::string& str )
	{
		std::vector< std::string > str_vector = Split( mySeparator, str );
		for( unsigned int i = 0; i < str_vector.size(); i++ )
		{
			Parse( RemoveWhiteSpace( str_vector[ i ] ) );
        }
		
		return GetResult();
	}

	//=========================================================================
private:
	
	//! Returns the result of the operations. Pops the last element of the 
	//! stack, so that the CPostfix is ready to be used again.
	/*!
		asserts that the stack has only one element left and then returns it.
		in the case that the stack is empty will return a zero.
	*/
	NumericType GetResult()
	{
		cassert( myStack.size() == 1 );
		
		if( myStack.empty() == false )
		{
			NumericType result = myStack.top();
			myStack.pop();
			return result;
		}
		
		return NumericType();
	}

	//=========================================================================

	//! Pushes a stl string into the stack. Converts it to NumericType
	void PushStack( const std::string& str ) 
	{
		NumericType num;
		std::stringstream ss( str );
		ss >> num;
		myStack.push( num );
	}

	//! executes the desired operation
	/*!
		Does not take into account white space
	*/
	void CallOperand( const std::string& str )
	{
		cassert( myOperands.find( str ) != myOperands.end() );

		if( myOperands.find( str ) != myOperands.end() )
		{
			myOperands[ str ]->Operate( myStack );
		}
	}
	
	//=========================================================================

	std::stack< NumericType > myStack;	
	std::map< OperatorType, IPostfixOperand< NumericType >* > myOperands;

	PostfixOperandAddition< NumericType >	opAdd;
	PostfixOperandSubtract< NumericType >	opSub;
	PostfixOperandMultiply< NumericType >	opMul;
	PostfixOperandDivide< NumericType >		opDiv;

	std::string								mySeparator;
};

} // end of namespace ceng

#endif

