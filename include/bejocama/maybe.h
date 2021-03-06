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
#include <list>
#include <memory>

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
			m._l.clear();
		}
		
		template<typename U>
		maybe(U&& u) : _l()
		{
			std::list<T> l;

			l.push_back(std::move(u));

			_l = std::move(l);
		}

		~maybe()
		{
		}
		
		operator bool() const
		{
			return _l.size() == 1;
		}

		T* operator->()
		{
			return &(_l.front());
		}
		
		const T* operator->() const
		{
			return &(_l.front());
		}
		
		T& operator*()
		{
			if (!*this) {
			
				throw std::runtime_error("MAYBE: access to nothing requested");
			}

			return _l.front();
		}

		std::list<T> _l;
	};

	template<typename T>
	struct maybe<T*> : std::unique_ptr<T>
	{
		using value_type = T;

		maybe(const maybe&) = delete;
		
		maybe() : std::unique_ptr<T>()
		{
		}

		maybe(maybe&& m) : std::unique_ptr<T>(std::move(m))
		{
		}
		
		template<typename U>
		maybe(U&& u) : std::unique_ptr<T>(u)
		{
		}

		~maybe()
		{
		}
	};
}
