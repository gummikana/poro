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

