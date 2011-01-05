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

