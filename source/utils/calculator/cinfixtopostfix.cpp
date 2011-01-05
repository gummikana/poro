#include "cinfixtopostfix.h"
#include "calculator_libs.h"

#include <stack>
#include <iostream>


namespace ceng {

///////////////////////////////////////////////////////////////////////////////

namespace {
	const int type_none = 0;
	const int type_operator = 1;
	// const int type_parenthesis = 2;
	const int type_opening_parenthesis = 3;
	const int type_closing_parenthesis = 4;
	const int type_operand = 5;

	int last_type = type_none;

	bool CouldAlsoBePartOfOperand( const std::string& operator_ )
	{
		return( operator_ == "+" || operator_ == "-" );
	}
}

///////////////////////////////////////////////////////////////////////////////

CInfixToPostfix::CInfixToPostfix() :
	myOperators(),
	myOpeningParenthesis( "(" ),
	myClosingParenthesis( ")" ),
	mySeparator( " " )
{
	AddOperator( "+", 0 );
	AddOperator( "-", 0 );
	AddOperator( "/", 1 );
	AddOperator( "*", 1 );
}

///////////////////////////////////////////////////////////////////////////////

std::string CInfixToPostfix::ConvertToPostfix( std::string infix )
{
	std::stack< std::string > stack;
	std::string result;
	std::string input;
	int			input_type;
	last_type = type_none;

	input = GetNextInput( RemoveWhiteSpace( infix ), input_type );
	infix = infix.substr( infix.find( input ) + input.size() );
	
	while( input.empty() == false )
	{

		switch( input_type )
		{
		case type_operand:
			result += input + mySeparator;
			break;

		case type_opening_parenthesis:
			stack.push( input );
			break;

		case type_operator:
			{
				while( !(	stack.empty() || 
							stack.top() == myOpeningParenthesis || 
							GetPriority( input ) > GetPriority( stack.top() ) ) )
				{
					result += stack.top() + mySeparator;
					stack.pop();
				}
				stack.push( input );
			}
			break;

		case type_closing_parenthesis:
			{
				while( stack.empty() == false && stack.top() != myOpeningParenthesis )
				{
					result += stack.top() + mySeparator;
					stack.pop();
				}
				
				if( stack.empty() == false ) 
					stack.pop();
			}
			break;

		default:
			break;
		}

		if( infix.empty() )
		{
			input = "";
		}
		else
		{
			input = GetNextInput( infix, input_type );
			if( infix.find( input ) + input.size() >= infix.size() )
			{
				infix = "";
			}
			else
			{
				infix = infix.substr( infix.find( input ) + input.size() );
			}
		}

	}

	while( stack.empty() == false )
	{
		result += stack.top() + mySeparator;
		stack.pop();
	}

	if( result.size() >= 1 )
	{
		result = result.substr( 0, result.size() - 1 );
	}

	return result.substr( );	
}

///////////////////////////////////////////////////////////////////////////////

void CInfixToPostfix::AddOperator( const std::string& oper, int priority )
{
	myOperators.insert( std::pair< std::string, int >( oper, priority ) );
}

///////////////////////////////////////////////////////////////////////////////

std::string CInfixToPostfix::GetNextInput( const std::string& input, int& input_type )
{
	if( input.empty() ) return "";

	std::string::size_type min_pos = std::string::npos;
	std::string result;
	std::map< std::string, int >::iterator i;
	input_type = type_none;
	//bool pitty_operand = false; // commented to get rid of warning, i assume this is not important?

	for( i = myOperators.begin(); i != myOperators.end(); ++i )
	{
		if( input.find( i->first ) < min_pos )
		{
			result = i->first;
			min_pos = input.find( i->first );
			input_type = type_operator;
		}
	}

	// opening and closing parenthesis
	if( input.find( myOpeningParenthesis ) < min_pos )
	{
		result = myOpeningParenthesis;
		min_pos = input.find( myOpeningParenthesis );
		input_type = type_opening_parenthesis;
	}

	if( input.find( myClosingParenthesis ) < min_pos )
	{
		result = myClosingParenthesis;
		min_pos = input.find( myClosingParenthesis );
		input_type = type_closing_parenthesis;
	}

	if( input.find_first_not_of( " \t" ) < min_pos )
	{
		result = RemoveWhiteSpace( input.substr( 0, min_pos ) );
		input_type = type_operand;
	}

	
	// pitty operator
	if( input_type == type_operator && ( last_type == type_operator || last_type == type_none || last_type == type_opening_parenthesis ) )
	{
		if( CouldAlsoBePartOfOperand( result ) )
		{
			//std::cout << "Crappy shit " << last_type << std::endl;
			//std::cout << input << std::endl;
			
			
			if( input.find( result ) + result.size() < input.size() )
			{
				int bar = type_none;
				std::string foo = GetNextInput( input.substr( input.find( result ) + result.size() ), bar );
				
				cassert( bar != type_operator ); // this is just illegal shit, we cant parse stuff like +-+++-+
				if( bar == type_operand ) 
				{
					result = result + foo;
					input_type = type_operand;
				}
			} 

		}
		
	}
	

	last_type = input_type;

	return result;
}

///////////////////////////////////////////////////////////////////////////////

int CInfixToPostfix::GetPriority( const std::string& str )
{
	cassert( myOperators.find( str ) != myOperators.end() );

	if( myOperators.find( str ) == myOperators.end() )
		return 0;

	return myOperators[ str ];
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace ceng

