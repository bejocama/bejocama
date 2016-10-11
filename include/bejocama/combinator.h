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
#include "bejocama/fmap.h"
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

	template<template<typename> class T, typename A>
	struct combinator<A,T<A>>
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

	template<template<typename> class T, typename A>
	struct combinator<A,T<A*>>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return combinator<A,T<A>>()(std::forward<F>(f));
		}
	};
	
	template<template<typename> class T, typename A>
	struct combinator<T<A>,T<A>>
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
			return fmap<list<A>>()(f);
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

					return fmap<list<A>>()(f)(x);
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
