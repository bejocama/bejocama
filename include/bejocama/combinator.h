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
#include <future>
#include "bejocama/list.h"

namespace bejocama
{
	template<typename A, typename B>
	struct combinator
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [f(std::move(std::forward<F>(f)))](auto&& a) mutable {
				
				return f(std::move(std::forward<decltype(a)>(a)));
			};
		}
	};

	template<typename A>
	struct combinator<maybe<A>,maybe<A>>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [f(std::move(std::forward<F>(f)))](auto&& a) mutable {

				using type = decltype(f(std::move(*std::forward<decltype(a)>(a))));
					
				if (!a) return type();

				return f(std::move(*std::forward<decltype(a)>(a)));
			};
		}
	};
		
	template<typename A>
	struct combinator<A,maybe<A>>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [f(std::move(std::forward<F>(f)))](auto&& a) mutable {

				using type = decltype(f(std::move(*std::forward<decltype(a)>(a))));
					
				if (!a) return type();

				return f(std::move(*std::forward<decltype(a)>(a)));
			};
		}
	};
	
	template<typename A>
	struct combinator<A,list<A>>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [f(std::move(std::forward<F>(f)))](auto&& a) mutable {

				auto it = a->begin();

				while(it) f(*it++);

				return true;
			};
		}
	};

	//
	// future
	//
	
	template<typename A>
	struct combinator<A,std::future<list<A>>>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [&f](auto&& a) mutable {

				auto l = [f(std::move(std::forward<F>(f))),
						  a(std::move(std::forward<decltype(a)>(a)))]() mutable {

					auto x = a.get();
					
					auto it = x->begin();

					while(it) { f(*it); ++it; }

					return true;
				};
			
				return std::async(std::launch::async,std::move(l));
			};
		}
	};

	template<typename A>
	struct combinator<A,std::future<maybe<A>>>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [&f](auto&& a) mutable {

				auto l = [f(std::move(std::forward<F>(f))),
						  a(std::move(std::forward<decltype(a)>(a)))]() mutable {

					return f(std::move(*(a.get())));
				};
				
				return std::async(std::launch::async,std::move(l));
			};
		}
	};

	template<typename A>
	struct combinator<std::future<A>,std::future<A>>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [&f](auto&& a) mutable {
				
				auto l = [f(std::move(std::forward<F>(f))),
						  a(std::move(std::forward<decltype(a)>(a)))]() mutable {

					return f(std::move(*(a.get())));
				};
			
				return std::async(std::launch::async,std::move(l));
			};
		}
	};
}
