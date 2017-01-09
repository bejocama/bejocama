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

#include "bejocama/factory/list.h"
#include "bejocama/provider/list.h"

namespace bejocama
{
	namespace provider
	{
		template<typename T>
		template<typename U>
		bejocama::base::list<T>* factory<bejocama::base::list<T>>
		::create_impl(U&& u, tag<char>)
		{
			using TT = typename bejocama::clear_type<T>::type;
			using UU = typename bejocama::clear_type<U>::type;

			return new list<TT,UU>(std::forward<U>(u));
		}
	}
}
