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

//interfaces
#include "bejocama/composition.h"
#include "bejocama/interface/file.h"
#include "bejocama/interface/list.h"
#include "bejocama/interface/string.h"
#include "bejocama/io.h"
#include "bejocama/db/db.h"
#include "client.h"

//implementations
#include "bejocama/factory/string_impl.h"
#include "bejocama/factory/list_impl.h"
#include "bejocama/factory/file_impl.h"
#include "bejocama/factory/iterator_impl.h"

namespace application
{
	struct database
	{
		struct column_id : bejocama::db::numeric_column<unsigned>
		{
			using bejocama::db::numeric_column<unsigned>::numeric_column;
			
			static constexpr const char* name = "id";
		};

		struct column_name : bejocama::db::string_column<40>
		{
			using bejocama::db::string_column<40>::string_column;
			
			static constexpr const char* name = "name";
		};

		struct column_city : bejocama::db::string_column<40>
		{
			using bejocama::db::string_column<40>::string_column;
			
			static constexpr const char* name = "city";
		};

		struct person : bejocama::db::table<
			column_id,
			column_name,
			column_city
			>
		{
			static constexpr const char* name = "person";
		};
	};
}

namespace bejocama
{

	void dbtest()
	{
		application::database::person t;
		
		application::database::person::row r;

		r << application::database::column_id(1)
		  << application::database::column_name("matthias")
		  << application::database::column_city("zürich");

		t << r;
	}

	template<typename T>
	void add_and_print_file(string fn, T&& t)
	{		
		
		/*
		  In situations where an identifier of a method isn't
		  unique, the specific type of the method is needed.

		  Together with the method type, the address operator
		  provides the right address of the method.
		*/

		using t_plus = list<T>(list<T>::interface::*)(const T& t);

		t_plus m_plus = &list<T>::interface::add;

		
		/*
		  Test of the curry function.
		*/
		
		auto xmap = curry<1,1>(mmap<T>,
							   returns(0),
							   returns(0));
		
		/*
		  make_async provides a function that starts a thread.
		  The curry function helps to define the thread function.
		  Because the curry function is based on the compose
		  function, the value to be captured must be provided
		  as a function -- improvements are planned later.
		*/
		
		auto xopen = make_async(curry<0>(fopen,returns(io(fn))));

		/*
		  The xopen thread returns a future and the combinator moves 
          that future to a following thread, which executes another 
          composition.
		*/

		auto result_assoc = composer(xopen,
									 composer(fstat,
											  xmap,
											  make_type<file<T>,io>,
											  make_type<list<T>,file<T>>,
											  m_plus,
											  print<T>()))(T(t));

		/*
		  Starting with the thread function xopen, all following steps
		  are running in a separate thread. All result futures are moved
		  to the subsequent thread.
		*/

		auto result_every = composer(xopen,
									 fstat,
									 xmap,
									 make_type<file<T>,io>,
									 make_type<list<T>,file<T>>,
									 m_plus,
									 print<T>())(T(t));

		/*
		  The result of all compositions is a future.
		*/

		result_assoc.get();

		result_every.get();
		
		/*
		  The serial composition differs from parallel by a
		  functional transformation.
		 */

		auto result_serial = composer(curry<0>(fopen,returns(io(fn))),
									  fstat,
									  xmap,
									  make_type<file<T>,io&&>,
									  make_type<list<T>,file<T>>,
									  m_plus,
									  print<T>())(T(t));

		static_assert(std::is_same<decltype(result_assoc.get()),decltype(result_serial)>::value,
					  "ERROR: types must be equal");

	}

	void test()
	{
		add_and_print_file<client>(std::string("client.data"),
								   client{"tom", "orlando", .age=20,.height=178});

		auto l = std::list<client>{client{"tom", "orlando", .age=20,.height=178}};

		//curry<1>(add_and_print_file<client>,returns(list<client>(l)))(std::string("client.data"));
	}
}

/*
  testprogram
*/

int main(int argc, char** argv)
{
	bejocama::test();

	return 0;
}
