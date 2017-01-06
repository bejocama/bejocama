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
#include "bejocama/functional.h"
#include "bejocama/iterator.h"
#include "bejocama/file.h"
#include "client.h"

namespace bejocama
{

	template<typename T>
	void add_and_print_file(string fn, T&& t)
	{
		using t_list = list<T>(file<T>::*)();
		using t_plus = list<T>(list<T>::*)(T&& t);

		t_list m_list = &file<T>::make_list;
		t_plus m_plus = &list<T>::operator+;

		auto xopen = curry<0>(fopen,returns(io(fn)));

		auto xmap = curry<1,1>(mmap<T>,
							   returns((long int)0),
							   returns((unsigned long int)0));

		using otype = maybe<file<T>>(make_file<T>::*)(io&&);
		
		auto mkf = make_function<otype>(make_file<T>());


		composer(xopen,
				 fstat,
				 xmap,
				 mkf,
				 m_list,
				 m_plus,
				 print<T>())(std::move(t));
	}
}

void test4()
{
	bejocama::add_and_print_file<client>(std::string("client.data"),
										 client{"tom", "orlando", .age=20,.height=178});
}
