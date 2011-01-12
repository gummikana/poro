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
//	This is a Signal Slot library created for the game engine Ceng. Its a 
//	independent from the game engine so you can use it where ever you want.
//
//	For that don't know anything about the Signal/Slot mechanism, please
//  visit the Trolltechs magnificent tutorial on the subject.
//	http://doc.trolltech.com/3.3/signalsandslots.html
//
//	I was inspired by the Trolltechs Qt library and its use of the Signal/Slot
//	mechanism, but I didn't like the idea of using a yeat another compiler to
//	make the mechanism work. So I wrote a library for the easy use of 
//	Signal/Slot mechanism.
//
//  Heres a small example how-to use this library. This is actually ripped from
//	Trolltechs tutorial only converted to use my Signal/Slot library.
/*
	class Foo 
	{
		CSS_Object
	public:
		Foo() { }
		int value() const { return val; }

		// a public slot
		void setValue( int v )
		{
			if ( v != val ) {
				val = v;
				valueChanged( v );
			}
		}

		signal valueChanged;

	private:
		int val;
	};

	{
		Foo a, b;
		a.valueChanged += CSlot( &b, Foo::setValue );
		b.setValue( 11 ); // a == undefined  b == 11
		a.setValue( 79 ); // a == 79         b == 79
		b.value();        // returns 79    
	}
*/
///////////////////////////////////////////////////////////////////////////////
#ifndef INC_CENGSIGNALSLOT_H
#define INC_CENGSIGNALSLOT_H

#include "csignal.h"
#include "cslot.h"
#include "cmyincomingsignals.h"

namespace ceng {

typedef CSignal signal;

} // end of namespace ceng

#define CSS_Object \
	public: \
	::ceng::CMyIncomingSignals cssIncomingSignals; \
	private: \

#endif