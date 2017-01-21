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
		template<typename> struct file;
	}
	
	template<typename T>
	using file = type<base::file, T>;
	
	namespace base
	{
		template<typename T>
		struct file
		{
			using value_type = T;

			virtual ~file()
			{
			}

			virtual const std::size_t size() const = 0;

			virtual T* begin() const = 0;

			virtual T* end() const  = 0;

			virtual T* rbegin() const = 0;

			virtual T* rend() const  = 0;
			
			virtual bejocama::file<T> add(const T&) = 0;
		};
	}
}
