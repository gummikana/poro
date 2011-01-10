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
