///////////////////////////////////////////////////////////////////////////////
//
// CInfixToPostfix
// ===============
// 
// A class that converts an infix to postfix statemets
//
// Created 10.10.2005 by Pete
//
//.............................................................................
//
// 24.08.2006 Pete
//		Fixed small thing so this now compiles under msvc2005.
//
//=============================================================================
#ifndef INC_CINFIXTOPOSTFIX_H
#define INC_CINFIXTOPOSTFIX_H

#include <string>
#include <map>

namespace ceng {

class CInfixToPostfix
{
public:
	CInfixToPostfix();


	std::string ConvertToPostfix( std::string infix );
	
	//! Adds a operator to be used. Higher priority higher the operator
	void		AddOperator( const std::string& oper, int priority );

private:

	std::string GetNextInput( const std::string& input, int& input_type );
	int			GetPriority( const std::string& str );

	std::map< std::string, int >	myOperators;
	std::string						myOpeningParenthesis;
	std::string						myClosingParenthesis;
	std::string						mySeparator;

};


} // end of namespace ceng

#endif

