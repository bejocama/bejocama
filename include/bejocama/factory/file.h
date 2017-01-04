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

#include "bejocama/provider/file.h"

namespace bejocama
{
	namespace provider
	{
		template<typename>
		struct factory;

		template<typename T>
		struct factory<bejocama::base::file<T>>
		{
			template<typename U>
			static bejocama::base::file<T>* create(U&& u)
			{
				using TT = typename bejocama::clear_type<T>::type;

				return new file<TT>(std::forward<U>(u));
			}
		};
	}
}
