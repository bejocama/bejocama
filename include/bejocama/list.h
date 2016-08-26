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
#include <memory>
#include <list>
#include "bejocama/utility.h"
#include "bejocama/base.h"

namespace bejocama
{
	template<typename T>
	struct list : safe_unique_ptr<base::list<T>>
	{
		list(const list&) = delete;

		list() : safe_unique_ptr<base::list<T>>()
		{
		}

		list(list&& l) : safe_unique_ptr<base::list<T>>(l.release())
		{
		}
		
		template<typename U>
			list(U&& u)
			: safe_unique_ptr<base::list<T>>
			(internal::factory<base::list<T>>::create(std::forward<U>(u)))
		{
		}

		template<typename U>
		list<T>& operator=(U&& u)
		{
			(*this).reset(internal::factory<base::list<T>>::create(std::forward<U>(u)));

			return *this;
		}
		
		list<T>& operator*()
		{
			return *this;
		}

		list<T> operator+(T&& t)
		{
			return (*this)->append(std::move(t));
		}
	};
}
