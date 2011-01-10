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

