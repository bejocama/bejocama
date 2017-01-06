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
#include "bejocama/file.h"
#include "bejocama/iterator.h"
#include "client.h"

namespace bejocama
{
	template<typename T>
	void print_file_test1(string fn)
	{
		using otype = maybe<file<T>>(make_file<T>::*)(io&&);
		
		auto mkf = make_function<otype>(make_file<T>());

		auto method = static_cast<list<T>(file<T>::*)()>
			(&file<T>::make_list);

		composer(fopen,
				 fstat,
				 mmap<T>,
				 mkf,
				 method,
				 print<T>())(io(fn),0,0);
	}
}

void test1()
{
	bejocama::print_file_test1<client>(std::string("client.data"));
}
