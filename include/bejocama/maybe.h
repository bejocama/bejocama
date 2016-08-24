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
#include <stdexcept>
#include "bejocama/list.h"

namespace bejocama
{
	template<typename T>
	struct maybe
	{
		using value_type = T;

		maybe(const maybe&) = delete;
		
		maybe() : _l(std::list<T>{})
		{
		}

		maybe(maybe&& m) : _l(std::move(m._l))
		{
		}
		
		template<typename U>
		maybe(U&& u) : _l()
		{
			std::list<T> l;

			l.push_back(std::move(u));

			_l = std::move(l);
		}

		operator bool() const
		{
			return _l->size() == 1;
		}
		
		T& operator*()
		{
			if (!*this) {
			
				throw std::runtime_error("MAYBE: access to nothing requested");
			}

			return *(_l->begin());
		}

		list<T> _l;
	};
}
