#include "../gtween.h"
#include "../../../utils/debug.h"

#ifdef CENG_TESTER_ENABLED

#include <memory>


namespace test
{

///////////////////////////////////////////////////////////////////////////////
namespace {

struct TestingThings
{
	TestingThings() : value( 0 ) { }


	void SetValue( float v ) { value = v; }
	float GetValue()  { return value; }

protected:
	float value;
};

struct GTweenListenerTest : public GTweenListener
{
	GTweenListenerTest() : 
		on_step( 0 ),
		on_complete( 0 ),
		on_start( 0 ) 
	{
	}

	virtual void GTween_OnStep( GTween* tweener ) { on_step++; }
	virtual void GTween_OnComplete( GTween* tweener ) { on_complete++; }
	virtual void GTween_OnStart( GTween* tweener ) { on_start++; }

	int on_step;
	int on_complete;
	int on_start;
};

struct FunctionTesting
{

	FunctionTesting() :
		func_int( 0 ),
		func_float( 0 ),
		func_string( 0 ),
		func_void( 0 ),
		func_copy( 0 ),
		func_ptr( 0 ) 
	{
	}

	int 	FunctionInt() 			{ func_int++; return 1; }
	float 	FunctionFloat()			{ func_float++; return 2.f; }

	std::string	FunctionString() 	{ func_string++; return "3"; }

	void		FunctionVoid()		{ func_void++; return; }
	

	int func_int;
	int func_float;
	int func_string;
	int func_void;
	int func_copy;
	int func_ptr;
	
};

} // end of anonymous namespace
///////////////////////////////////////////////////////////////////////////////

int GTween_Test()
{
	// the shit I want to happen
	{
		int variable_to_tween = 0;
		TestingThings t;
		std::auto_ptr< GTween > tween( new GTween );
		
		tween->AddVariable( variable_to_tween, 10, "v" );
		tween->AddGetterSetter( 
			ceng::CFunctionPtr<>( &t, &TestingThings::GetValue ),
			ceng::CFunctionPtr<>( &t, &TestingThings::SetValue ), 
			1.f, "test");
		//tween->SetFunction( Quadratic::easeInOut );
		tween->SetDuration( 1.f );

		test_assert( variable_to_tween == 0 );
		test_assert( t.GetValue() == 0 );
		UpdateGTweens( 0.5f );

		test_assert( variable_to_tween == 5 );
		test_assert( t.GetValue() == 0.5f );
		UpdateGTweens( 0.1f );

		test_assert( variable_to_tween == 6 );
		test_assert( t.GetValue() == 0.6f );
	
		tween.reset( NULL );
		UpdateGTweens( 0.1f );
		UpdateGTweens( 0.1f );
		UpdateGTweens( 0.1f );
		UpdateGTweens( 0.1f );
	
		test_assert( variable_to_tween == 6 );
		test_assert( t.GetValue() == 0.6f );
	}

	// test the autokill feature
	{
		int variable_to_tween = 0;
		TestingThings t;
		GTween* tween( new GTween );
		
		tween->AddVariable( variable_to_tween, 10, "v" );
		tween->AddGetterSetter( 
			ceng::CFunctionPtr<>( &t, &TestingThings::GetValue ),
			ceng::CFunctionPtr<>( &t, &TestingThings::SetValue ), 
			1.f, "test");
		//tween->SetFunction( Quadratic::easeInOut );
		tween->SetDuration( 1.f );
		tween->SetAutoKill( true );

		test_assert( ceng::CAutoList< GTween >::GetList().size() == 1 );
		UpdateGTweens( 2.f );
		test_assert( ceng::CAutoList< GTween >::GetList().size() == 0 );
	}

	// compare pointers
	{
		int variable_to_tween = 0;
		int random_variable = 1;
		TestingThings t;
		TestingThings another_random;
		std::auto_ptr< GTween > tween( new GTween );
		
		tween->AddVariable( variable_to_tween, 10, "v" );
		tween->AddGetterSetter( 
			ceng::CFunctionPtr<>( &t, &TestingThings::GetValue ),
			ceng::CFunctionPtr<>( &t, &TestingThings::SetValue ), 
			1.f, "test");

		tween->SetDuration( 1.f );

		test_assert( tween->ClearPointer( &another_random ) == false );
		test_assert( tween->ClearPointer( &t ) == true );

		test_assert( variable_to_tween == 0 );
		test_assert( t.GetValue() == 0 );
		UpdateGTweens( 0.5f );

		test_assert( variable_to_tween == 5 );
		test_assert( t.GetValue() == 0 );

		test_assert( tween->ClearPointer( &random_variable ) == false );
		test_assert( tween->ClearPointer( &variable_to_tween ) == true );

		test_assert( variable_to_tween == 5 );
		test_assert( t.GetValue() == 0 );

		UpdateGTweens( 1.0f );

		test_assert( variable_to_tween == 5 );
		test_assert( t.GetValue() == 0 );


		tween.reset( NULL );

	}

	// listeners
	{
		int variable_to_tween = 0;
		GTweenListenerTest listener;
		std::auto_ptr< GTween > tween( new GTween );
		tween->AddListener( &listener );
		tween->AddVariable( variable_to_tween, 10, "v" );

		tween->SetDuration( 1.f );
		tween->SetAutoKill( false );
		tween->SetDelay( 0.5f );
		tween->SetFunction( ceng::easing::Elastic::easeIn );

		test_assert( listener.on_complete == 0 );
		test_assert( listener.on_step == 0 );
		test_assert( listener.on_start == 0 );

		UpdateGTweens( 0.1f );
		
		test_assert( listener.on_complete == 0 );
		test_assert( listener.on_step == 0 );
		test_assert( listener.on_start == 0 );

		UpdateGTweens( 0.4f );
		
		test_assert( listener.on_complete == 0 );
		test_assert( listener.on_step == 0 );
		test_assert( listener.on_start == 1 );
		
		UpdateGTweens( 0.1f );

		test_assert( listener.on_complete == 0 );
		test_assert( listener.on_step == 1 );
		test_assert( listener.on_start == 1 );

		UpdateGTweens( 0.1f );

		test_assert( listener.on_complete == 0 );
		test_assert( listener.on_step == 2 );
		test_assert( listener.on_start == 1 );

		UpdateGTweens( 0.1f );

		test_assert( listener.on_complete == 0 );
		test_assert( listener.on_step == 3 );
		test_assert( listener.on_start == 1 );

		UpdateGTweens( 0.8f );

		test_assert( listener.on_complete == 1 );
		test_assert( listener.on_step == 4 );
		test_assert( listener.on_start == 1 );

	}
	return 0;
}

//-----------------------------------------------------------------------------

TEST_REGISTER( GTween_Test );

//-----------------------------------------------------------------------------

} // end of namespace test

#endif