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
#include "typelist.h"
#include "traits.h"
#include <future>
#include <utility>
#include <tuple>
#include "bejocama/interface/list.h"
#include "bejocama/maybe.h"
#include <iostream>

namespace bejocama
{
	template<typename T>
	struct make_tuple_index_sequence
	{
		static constexpr std::size_t N = std::tuple_size<T>::value;

		using type = std::make_index_sequence<N>;
	};
	
	template<typename O, typename F, typename P, size_t... I>
	decltype(auto) apply(O&& o, F&& f, P&& p, std::index_sequence<I...>)
	{
		return (o.*f)(std::get<I>(std::forward<P>(p))...);
	}

	template<typename F, typename B, typename P, typename A, size_t... IB, size_t... IA>
	decltype(auto) apply(F&& f, B&& b, P&& p, A&& a,
						 std::index_sequence<IB...>, std::index_sequence<IA...>)
	{
		return f(std::get<IB>(std::forward<B>(b))...,
				 std::forward<P>(p),
				 std::get<IA>(std::forward<A>(a))...);
	}
	
	namespace functor
	{
		template<typename T>
		struct fmap
		{
			template<typename F, typename P, typename... A>
			decltype(auto) operator()(F&& f, typelist<P>, typelist<A...>);

			template<typename F, typename... B, typename P, typename... A>
			decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>);
		};
		
		template<typename I, typename O, typename R>
		struct fmap<typelist<I,O,R>>
		{
			template<typename F, typename P, typename... A>
			decltype(auto) operator()(F&& f, typelist<P>, typelist<A...>)
			{
				return [&f](P&& p, A&&... a) mutable {

					if (!p) return R();

					return (p.get()->*f)(std::forward<A>(a)...);
				};
			}

			template<typename F, typename... B, typename P, typename... A>
			decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
			{
				return [ff=std::forward<F>(f)](B&&... b, P&& p, A&&... a) mutable {

					return ff(std::forward<decltype(b)>(b)...,
							  std::forward<decltype(p)>(p),
							  std::forward<decltype(a)>(a)...);
				};
			}
		};
	
		template<typename T, typename R>
		struct fmap<typelist<T,T,R>>
		{
			template<typename F, typename... B, typename P, typename... A>
			decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
			{
				return [ff=std::forward<F>(f)](B&&... b, P&& p, A&&... a) mutable {

					return ff(std::forward<decltype(b)>(b)...,
							  std::forward<decltype(p)>(p),
							  std::forward<decltype(a)>(a)...);
				};
			}
		};

		template<typename T>
		struct fmap<typelist<list<T>,T,void>>
		{
			template<typename F, typename... B, typename P, typename... A>
			decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
			{
				return [ff=std::forward<F>(f)](B&&... b, P&& p, A&&... a) mutable {

					auto it = p->begin();

					while(!it->at_end()) {

						ff(std::forward<decltype(b)>(b)...,
						   it++->get(),
						   std::forward<decltype(a)>(a)...);
					}
					
					return maybe<bool>();
				};
			}
		};

		template<typename T, typename R>
		struct fmap<typelist<maybe<T>,T,R>>
		{
			template<typename F, typename... B, typename P, typename... A>
			decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
			{
				return [&f](B&&... b, P&& p, A&&... a) mutable {

					if (!p) return R();

					return fmap<typelist<T,T,R>>()(std::forward<F>(f),
												   typelist<B...>{},
												   typelist<T>{},
												   typelist<A...>{})
						(std::forward<B>(b)...,
						 std::move(*std::forward<P>(p)),
						 std::forward<A>(a)...);
				};
			}
		};

		template<typename T, typename R>
		struct fmap<typelist<std::future<T>,T,R>>
		{
			template<typename F, typename... B, typename P, typename... A>
			decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
			{
				return [&f](B&&... b, P&& p, A&&... a) mutable {

					auto l = [&f, &b..., pp=std::move(p), &a...]() mutable {

						return fmap<typelist<T,T,R>>()
						(std::forward<F>(f),
						 typelist<B...>{},
						 typelist<T>{},
						 typelist<A...>{})
						(std::forward<B>(b)..., std::move(pp.get()), std::forward<A>(a)...);
					};

					return std::async(std::launch::async,std::move(l));
				};
			}
		};

		template<typename T, typename R>
		struct fmap<typelist<std::future<maybe<T>>,T,R>>
		{
			template<typename F, typename... B, typename P, typename... A>
			decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
			{
				return [&f](B&&... b, P&& p, A&&... a) mutable {

					auto l = [f, &b..., pp=std::move(p), &a...]() mutable {
				
						auto ppp = std::move(pp.get());

						return fmap<typelist<maybe<T>,T,R>>()
						(f,
						 typelist<B...>{},
						 typelist<decltype(ppp)>{},
						 typelist<A...>{})(std::forward<B>(b)...,
										   std::move(ppp),
										   std::forward<A>(a)...);
					};

					return std::async(std::launch::async,std::move(l));
				};
			}
		};

		template<typename T, typename R>
		struct fmap<typelist<std::future<list<T>>,typename list<T>::interface,R>>
		{
			template<typename F, typename O, typename... A>
			decltype(auto) operator()(F&& f, typelist<O>, typelist<A...>)
			{
				return [&f](O&& o, A&&... a) mutable {

					auto l = [f,oo=std::move(o), &a...]() mutable {

						return fmap<typelist<list<T>,
						typename list<T>::interface,
						R>>()
						(f, typelist<list<T>>{}, typelist<A...>{})
						(std::move(oo.get()), std::forward<A>(a)...);
					};

					return std::async(std::launch::async,std::move(l));
				};
			}
		};

		template<typename T>
		struct fmap<typelist<std::future<list<T>>,T,void>>
		{
			template<typename F, typename... B, typename P, typename... A>
			decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
			{
				return [&f](B&&... b, P&& p, A&&... a) mutable {

					auto lambda = [f, &b..., pp=std::move(p), &a...]() mutable {

						auto ppp = std::move(pp.get());

						return fmap<typelist<list<T>,T,void>>()
						(f,
						 typelist<B...>{},
						 typelist<decltype(ppp)>{},
						 typelist<A...>{})(std::forward<B>(b)...,
										   std::move(ppp),
										   std::forward<A>(a)...);
					};

					return std::async(std::launch::async,std::move(lambda));
				};
			}
		};
	}
	
	template<typename F>
	decltype(auto) make_morphism(F&& f)
	{
		return [&f](auto&& o, auto&&... a) mutable {

			using IT = typename clear_type<decltype(o)>::type;
			using OT = typename function_traits<typename clear_type<F>::type>::ctype;
			using RT = typename function_traits<typename clear_type<F>::type>::rtype;
			using MT = typename function_traits<typename clear_type<F>::type>::atype;

			return functor::fmap<typelist<IT,OT,RT>>()
				(std::forward<F>(f),typelist<IT>{}, MT{})
				(std::forward<decltype(o)>(o), std::forward<decltype(a)>(a)...);
		};
	}

	template<std::size_t P, typename F>
	decltype(auto) make_morphism(F&& f)
	{
		return [&f](auto&&... a) mutable {

			constexpr size_t N = sizeof...(a);
			
			using itypes = typelist<typename clear_type<decltype(a)>::type...>;			
			using IT = typename clear_type<typename tl_get<itypes,P>::type>::type;
			using otypes = typename function_traits<typename clear_type<F>::type>::atype;
			using OT = typename clear_type<typename tl_get<otypes,P>::type>::type;
			using BEFORE = typename tl_sub<otypes,0,P>::type;
			using AFTER = typename tl_sub<otypes,P+1,N-P-1>::type;
			using RT = typename function_traits<typename clear_type<F>::type>::rtype;

			return functor::fmap<typelist<IT,OT,RT>>()
				(std::forward<F>(f),
				 BEFORE{},
				 typelist<IT>{},
				 AFTER{})(std::forward<decltype(a)>(a)...);
		};
	}
}	
