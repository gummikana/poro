#ifndef INC_IPOSTFIXOPERAND_H
#define INC_IPOSTFIXOPERAND_H

#include <stack>

namespace ceng {

//! An interface class for the PostfixOperands
template< class NumericType >
class IPostfixOperand
{
public:
	virtual ~IPostfixOperand() { }

	virtual void Operate( std::stack< NumericType >& stack ) = 0;

};


} // end of namespace ceng

#endif

