#ifndef INC_CTESTER_NUMERIC_H
#define INC_CTESTER_NUMERIC_H

namespace poro {
namespace tester {
namespace numeric {

bool floatCheck( float A, float B, float maxAbsoluteError = 0.0001, double maxRelativeError = 0.0001 );
bool doubleCheck( double A, double B, double maxAbsoluteError = 0.0000001, double maxRelativeError = 0.00000001 );

//! A helper class for doing the float conversations, POD class
class CFloatingPointCheck
{
public:
	CFloatingPointCheck();
	CFloatingPointCheck( double value );
	CFloatingPointCheck( float value );
	CFloatingPointCheck( int value );

	enum Type
	{
		t_double = 0,
		t_float,
		t_int
	};

	const Type type;

	double d_value;
	float  f_value;
	int	   i_value;
		
};


//! another helper class for doing float conversations, POD class
class CComparePair
{
public:
	CComparePair();
	CComparePair( const CFloatingPointCheck& left, const CFloatingPointCheck& right, bool is_true = true  );

	CFloatingPointCheck left;
	CFloatingPointCheck right;
	bool isTrue;
};

CFloatingPointCheck operator + ( const CFloatingPointCheck& L, const CFloatingPointCheck& R );
CComparePair operator == ( const CFloatingPointCheck& L, const CFloatingPointCheck& R );


bool AssertFloat( const CComparePair& values, double accuracy = 0.0001, double relative = 0.0001 );

} // end of namespace numeric
} // end of namespace tester
} // end of namespace poro

#define test_float( x ) test_assert( ::poro::tester::numeric::AssertFloat( ::poro::tester::numeric::CFloatingPointCheck( 0 ) + x ) )


#endif
