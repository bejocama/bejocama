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
#include <iostream>
#include "bejocama/string.h"
#include "bejocama/list.h"

namespace bejocama
{
	template<typename T>
	struct combinator;
	
	template<typename... A>
	struct combinator<tag<A...>>
	{
		template<typename F, typename T>
		decltype(auto) operator()(F&& f, T&& t)
		{
			using type = decltype(std::forward<F>(f)(std::move(*std::forward<T>(t))));
			
			if (!t) return type();
			
			return std::forward<F>(f)(std::move(*std::forward<T>(t)));
		}
	};

	template<typename A>
	struct combinator<tag<A,A>>
	{
		template<typename F, typename T>
		decltype(auto) operator()(F&& f, T&& t)
		{
			return std::forward<F>(f)(std::move(std::forward<T>(t)));
		}
	};
	
	template<typename D>
	struct combinator<tag<D,list<D>>>
	{
		template<typename F, typename T>
		decltype(auto) operator()(F&& f, T&& t)
		{
			auto it = t->begin();

			while(it) f(*it++);

			return true;
		}
	};
}
