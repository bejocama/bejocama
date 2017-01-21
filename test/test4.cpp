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
#include "bejocama/interface/file.h"
#include "bejocama/interface/list.h"
#include "bejocama/interface/string.h"
#include "bejocama/io.h"
#include "client.h"

namespace bejocama
{

	template<typename T>
	void add_and_print_file(string fn, T&& t)
	{
		using t_plus = list<T>(list<T>::interface::*)(const T& t);

		t_plus m_plus = &list<T>::interface::add;

		auto xopen = curry<0>(fopen,returns(io(fn)));

		auto xmap = curry<1,1>(mmap<T>,
							   returns((long int)0),
							   returns((unsigned long int)0));
		composer(xopen,
				 fstat,
				 xmap,
				 make_type<file<T>,io>,
				 make_type<list<T>,file<T>>,
				 m_plus,
				 print<T>())(std::move(t));
	}
}

void test4()
{
	bejocama::add_and_print_file<client>(std::string("client.data"),
										 client{"tom", "orlando", .age=20,.height=178});
}
