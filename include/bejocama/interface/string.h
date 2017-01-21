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

#pragma once

#include "bejocama/factory.h"

namespace bejocama
{
	namespace base
	{
		template<typename> struct string;
	}

	template<typename D>
	using basic_string = type<base::string, D>;

	using string = basic_string<char>;
	
	namespace base
	{
		template<typename T>
		struct string
		{
			virtual const T* c_str() const = 0;
			virtual bejocama::basic_string<T> clone() const = 0;
		};
	}
}
