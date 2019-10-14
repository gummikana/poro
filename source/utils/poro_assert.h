#ifndef PORO_ASSERT_H
#define PORO_ASSERT_H

#include <stdio.h>
#include <ctime>

// assert that could be used during run time. Just outputs the error, instead of crashing things
#ifdef PORO_USE_RELEASE_ASSERTS
#		define cassert(cond) \
			do \
			{ \
				if (!(cond)) \
				{ \
					printf("Assert failed: (" #cond ") in " __FILE__ " at line "  __LINE__ "\n"); \
				} \
			} while(0)
#else

bool PORO_AssertBox( const char* filename, int line, const char* msg );

#ifdef WIZARD_DEBUG
		// prints the assert and crashes
// #		define cassert(cond) if( !(cond)) { printf("Assert failed: (%s) in %s at line %d\n", (#cond), __FILE__, __LINE__); std::time_t cassert_now = std::time(0); tm *ltm = std::localtime(&cassert_now); FILE* outf = fopen("log_asserts.txt", "a");fprintf(outf, "%d, %d, %d - %d:%d\n", ( 1900 + ltm->tm_year ), (1 + ltm->tm_mon), ltm->tm_mday, ltm->tm_hour, ltm->tm_min  );fprintf(outf, "Assert failed: (%s) in %s at line %d\n", (#cond), __FILE__, __LINE__);fclose(outf); int* _break_me_ = 0; (*_break_me_) = 7337; }

#		define cassert(cond) if( !(cond)) { printf("Assert failed: (%s) in %s at line %d\n", (#cond), __FILE__, __LINE__); std::time_t cassert_now = std::time(0); tm *ltm = std::localtime(&cassert_now); FILE* outf = fopen("log_asserts.txt", "a");fprintf(outf, "%d, %d, %d - %d:%d\n", ( 1900 + ltm->tm_year ), (1 + ltm->tm_mon), ltm->tm_mday, ltm->tm_hour, ltm->tm_min  );fprintf(outf, "Assert failed: (%s) in %s at line %d\n", (#cond), __FILE__, __LINE__);fclose(outf);  if( PORO_AssertBox( __FILE__, __LINE__, #cond ) ) { int* _break_me_ = 0; (*_break_me_) = 7337; } }

	#else

// writes to a file log_asserts.txt when and continues the execution
// #		define cassert(cond) if( !(cond)) { printf("Assert failed: (%s) in %s at line %d\n", (#cond), __FILE__, __LINE__); std::time_t cassert_now = std::time(0); tm *ltm = std::localtime(&cassert_now); FILE* outf = fopen("log_asserts.txt", "a");fprintf(outf, "%d, %d, %d - %d:%d\n", ( 1900 + ltm->tm_year ), (1 + ltm->tm_mon), ltm->tm_mday, ltm->tm_hour, ltm->tm_min  );fprintf(outf, "Assert failed: (%s) in %s at line %d\n", (#cond), __FILE__, __LINE__);fclose(outf); /*int* _break_me_ = 0; (*_break_me_) = 7337;*/ LogError << "Assert failed: (" << (#cond) << ") in " << __FILE__ << " at line " << __LINE__ << "\n"; }

#define cassert(cond) ((void)0)

	#endif


#endif

#endif