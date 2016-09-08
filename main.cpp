/*

  This file is part of BEJOCAMA software
  Copyright 2016 Matthias Wolff <matthias.wolff@bejocama.de>

  This work is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation;
  either version 3 of the License, or any later version.

  This work is distributed in the hope that it will be useful,
  but without any warranty; without even the implied warranty
  of merchantability or fitness for a particular purpose. See
  the GNU Lesser General Public License for more details. You
  should have received a copy of the GNU Lesser General Public
  License along with this library.

*/

#include "bejocama/composition.h"
#include "bejocama/combinator.h"
#include "bejocama/internal.h"
#include "bejocama/file.h"
#include "client.h"

namespace bejocama
{

	template<typename T>
	void add_and_print_file(const char* fn, T&& t)
	{
		/*
		  In situations where an identifier of a method isn't
		  unique, the specific type of the method is needed.
		*/
		
		using t_list = list<T>(file<T>::*)();
		using t_plus = list<T>(list<T>::*)(T&& t);

		/*
		  Together with the method type, the address operator
		  provides the right address of the method.
		*/
		
		t_list m_list = &file<T>::make_list;
		t_plus m_plus = &list<T>::operator+;

		/*
		  make_async provides a function that starts a thread.
		  The curry function helps to define the thread function.
		  Because the curry function is based on the compose
		  function, the value to be captured must be provided
		  as a function -- improvements are planned later.
		*/
		
		auto xopen = make_async(curry<0>(fopen,identity(io(fn))));

		/*
		  Test of the curry function.
		*/
		
		auto xmap = curry<1,1>(mmap<T>,
							   identity(0),
							   identity(0));

		/*
		  The xopen thread returns a future and the combinator moves 
          that future to a following thread, which executes another 
          composition.
		*/
		
		auto result_assoc = composer(xopen,
									 composer(fstat,
											  xmap,
											  make_file<T>(),
											  m_list,
											  m_plus,
											  print<T>()))(std::move(t));

		/*
		  Starting with the thread function xopen, all following steps
		  are running in a separate thread. All result futures are moved
		  to the subsequent thread.
		*/
		
		auto result_every = composer(xopen,
									 fstat,
									 xmap,
									 make_file<T>(),
									 m_list,
									 m_plus,
									 print<T>())(std::move(t));


		/*
		  The result of all compositions is a future.
		*/
		
		result_assoc.get();
		result_every.get();

		/*
		  The serial composition differs from parallel by a
		  functional transformation.
		 */
		
		auto result_serial = composer(curry<0>(fopen,identity(io(fn))),
									  fstat,
									  xmap,
									  make_file<T>(),
									  m_list,
									  m_plus,
									  print<T>())(std::move(t));

		static_assert(std::is_same<decltype(result_assoc.get()),decltype(result_serial)>::value,
					  "ERROR: types must be equal");
	}
}

/*
  testprogram
*/

int main(int argc, char** argv)
{
	bejocama::add_and_print_file<client>("client.data",
										 client{"tom", "orlando", .age=20,.height=178});

	return 0;
}
