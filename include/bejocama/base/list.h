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

namespace bejocama
{
	template<typename> struct iterator;
	template<typename> struct list;
	
	namespace base
	{
		template<typename T>
		struct list
		{
			virtual ~list() {}
			virtual std::size_t size() const = 0;
			virtual bejocama::iterator<T> begin() = 0;
			virtual bejocama::iterator<T> end() = 0;
			virtual bejocama::list<T> add(T&& t) = 0;
		};
	}
}
