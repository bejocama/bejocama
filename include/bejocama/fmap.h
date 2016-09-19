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
#include "list.h"

namespace bejocama
{

	namespace hidden
	{
		template<typename T, typename R>
		struct fmap;

		template<typename A, typename R>
		struct fmap<list<A>,R>
		{
			template<typename F, typename I>
			decltype(auto) operator()(F&& f, I&& i) {

				auto l = f(std::move(*i++));
				
				while(i) {

					l->append(f(std::move(*i++)));
				}
				
				return std::move(l);
			}
		};

		template<typename A>
		struct fmap<list<A>,void>
		{
			template<typename F, typename I>
			decltype(auto) operator()(F&& f, I&& i) {

				while(i) f(std::move(*i++));

				return list<bool>();
			}
		};
	}
	
	template<typename T>
	struct fmap;

	template<typename A>
	struct fmap<list<A>>
	{
		template<typename F>
		decltype(auto) operator()(F&& f) {

			return [f(std::move(std::forward<F>(f)))] (auto&& a) mutable {

				auto it = std::forward<decltype(a)>(a)->begin();

				using type = decltype(f(std::move(*it)));
				
				return hidden::fmap<list<A>,type>()(f,it);
			};
		}
	};
}
