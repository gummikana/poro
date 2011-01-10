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
// CCalculator
// ===========
//
// A basic calculator class that can be used to generate numbers from a script 
// file. Support registering / adding real c variables to be used as const 
// variables.
//
// Created 11.10.2005 by Pete
//
//.............................................................................
//
// 14.08.2006 Pete
//		Added the advanced operations to the calculator.
//
//=============================================================================

#ifndef INC_CCALCULATOR_H
#define INC_CCALCULATOR_H

#include "advanced_postfixoperands.h"
#include "cinfixtopostfix.h"
#include "cpostfix.h"


#include <string>

namespace ceng {

//! Calculates simple arithmetic expressions, with scriptable variables
/*!
	This class uses (as composite) CInfixToPostfix class and CPostfix classes 
	to convert regular arithemetic expressions to desired form.

*/
template< class Num = float, class Op = std::string >
class CCalculator
{
public:
	
	CCalculator() :
		myPostfix(),
		myConverter(),

		mySinOp(),
		myCosOp(),
		myTanOp(),
		mySqrtOp(),
		myPowOp()
	{
		myPostfix.RegisterOperand( "sin",	&mySinOp );
		myPostfix.RegisterOperand( "cos",	&myCosOp );
		myPostfix.RegisterOperand( "tan",	&myTanOp );
		myPostfix.RegisterOperand( "sqrt",	&mySqrtOp );
		myPostfix.RegisterOperand( "^",		&myPowOp );

		myConverter.AddOperator( "sin", 2 );
		myConverter.AddOperator( "cos", 2 );
		myConverter.AddOperator( "tan", 2 );
		myConverter.AddOperator( "sqrt", 2 );
		myConverter.AddOperator( "^", 2 );

	}


	~CCalculator()
	{
	}


	Num operator()( const std::string& str )
	{
		if( str.empty() ) return Num();
		return myPostfix( myConverter.ConvertToPostfix( str ) );
	}


private:
	CPostfix< Num, Op >		myPostfix;
	CInfixToPostfix			myConverter;
	
	PostfixOperandSine< Num >		mySinOp;
	PostfixOperandCosine< Num >		myCosOp;
	PostfixOperandTangent< Num >	myTanOp;
	PostfixOperandSqrt< Num >		mySqrtOp;
	PostfixOperandExponent< Num >	myPowOp;
	

};

} // end of namespace ceng

#endif

