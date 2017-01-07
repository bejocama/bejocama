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

namespace bejocama
{
	template<typename> struct maybe;
	template<typename> struct list;

	template<typename T>
	struct make_tuple_index_sequence
	{
		static constexpr std::size_t N = std::tuple_size<T>::value;

		using type = std::make_index_sequence<N>;
	};
	
	template<typename O, typename F, typename P, size_t... I>
	decltype(auto) apply(O&& o, F&& f, P&& p, std::index_sequence<I...>)
	{
		return (o.*f)(std::move(std::get<I>(std::forward<P>(p)))...);
	}

	template<typename F, typename B, typename P, typename A, size_t... IB, size_t... IA>
	decltype(auto) apply(F&& f, B&& b, P&& p, A&& a,
						 std::index_sequence<IB...>, std::index_sequence<IA...>)
	{
		return f(std::move(std::get<IB>(std::forward<B>(b)))...,
				 std::move(std::forward<P>(p)),
				 std::move(std::get<IA>(std::forward<A>(a)))...);
	}
	
	template<template<typename> class C, typename T>
	static decltype(auto) make_type(T&& t)
	{
		return C<T>(t);
	}

	template<typename I, typename O, typename R>
	struct morphism
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [ff=std::forward<F>(f)](auto&& o, auto&&... a) mutable {

				return (o.*ff)(std::forward<decltype(a)>(a)...);
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
	struct morphism<T,T,R>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [ff=std::forward<F>(f)](auto&& o, auto&&... a) mutable {

				return (o.*ff)(std::forward<decltype(a)>(a)...);
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
	struct morphism<list<T>,T,R>
	{
		template<typename F, typename... B, typename P, typename... A>
		decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
		{
			return [ff=std::forward<F>(f)](B&&... b, P&& p, A&&... a) mutable {

				list<R> l;

				auto it = p->begin();

				while(it) {

					l->append(ff(std::forward<decltype(b)>(b)...,
								 *it++,
								 std::forward<decltype(a)>(a)...));
				}
				
				return l;
			};
		}
	};

	template<typename T>
	struct morphism<list<T>,T,void>
	{
		template<typename F, typename... B, typename P, typename... A>
		decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
		{
			return [ff=std::forward<F>(f)](B&&... b, P&& p, A&&... a) mutable {

				auto it = p->begin();

				while(it) {

					ff(std::forward<decltype(b)>(b)...,
					   std::move(*it++),
					   std::forward<decltype(a)>(a)...);
				}

				return maybe<bool>();
			};
		}
	};
	
	template<typename T, typename R>
	struct morphism<maybe<T>,T,R>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [ff=std::forward<F>(f)](auto&& o, auto&&... a) mutable {

				using type = decltype((*o.*ff)(std::forward<decltype(a)>(a)...));
				
				if (!o) return type();
				
				return (*o.*ff)(std::forward<decltype(a)>(a)...);
			};
		}

		template<typename F, typename... B, typename P, typename... A>
		decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
		{
			return [ff=std::forward<F>(f)](B&&... b, P&& p, A&&... a) mutable {

				if (!p) return R();
				
				return ff(std::move(std::forward<decltype(b)>(b))...,
						  std::move(*std::forward<decltype(p)>(p)),
						  std::move(std::forward<decltype(a)>(a))...);
			};
		}
	};

	template<typename T, typename R>
	struct morphism<maybe<T*>,T,R>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [ff=std::forward<F>(f)](auto&& o, auto&&... a) mutable {

				using type = decltype((*o.*ff)(std::forward<decltype(a)>(a)...));
				
				if (!o) return type();
				
				return (*o.*ff)(std::forward<decltype(a)>(a)...);
			};
		}

		template<typename F, typename... B, typename P, typename... A>
		decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
		{
			return [ff=std::forward<F>(f)](B&&... b, P&& p, A&&... a) mutable {

				if (!p) return R();
				
				return ff(std::move(std::forward<decltype(b)>(b))...,
						  std::move(*std::forward<decltype(p)>(p)),
						  std::move(std::forward<decltype(a)>(a))...);
			};
		}
	};
	
	template<typename T, typename R>
	struct morphism<std::future<T>,T,R>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [&f](auto&& o, auto&&... a) mutable {

				auto l = [f,oo=std::move(o),
						  p=std::make_tuple(std::forward<decltype(a)>(a)...)]() mutable {
				
					auto ooo = std::move(oo.get());

					using seq = typename make_tuple_index_sequence<decltype(p)>::type;

					return apply(ooo,f,p,seq{});
				};

				return std::async(std::launch::async,std::move(l));
			};
		}

		template<typename F, typename... B, typename P, typename... A>
		decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
		{
			return [&f](B&&... b, P&& p, A&&... a) mutable {

				auto l = [f,
						  pb=std::make_tuple(std::forward<decltype(b)>(b)...),
						  pp=std::move(p),
						  pa=std::make_tuple(std::forward<decltype(a)>(a)...)
						  ]() mutable {
				
					auto ppp = std::move(pp.get());

					using seqb = typename make_tuple_index_sequence<decltype(pb)>::type;
					using seqa = typename make_tuple_index_sequence<decltype(pa)>::type;
					
					return apply(f,pb,ppp,pa,seqb{},seqa{});
				};

				return std::async(std::launch::async,std::move(l));
			};
		}
	};

	template<typename T, typename R>
	struct morphism<std::future<maybe<T>>,T,R>
	{
		template<typename F>
		decltype(auto) operator()(F&& f)
		{
			return [&f](auto&& o, auto&&... a) mutable {

				auto l = [f,oo=std::move(o),
						  p=std::make_tuple(std::forward<decltype(a)>(a)...)]() mutable {

					auto m = std::move(oo.get());

					using seq = typename make_tuple_index_sequence<decltype(p)>::type;
					
					return apply(*m,f,p,seq{});
				};

				return std::async(std::launch::async,std::move(l));
			};
		}

		template<typename F, typename... B, typename P, typename... A>
		decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
		{
			return [&f](B&&... b, P&& p, A&&... a) mutable {

				auto l = [f,
						  pb=std::make_tuple(std::forward<decltype(b)>(b)...),
						  pp=std::move(p),
						  pa=std::make_tuple(std::forward<decltype(a)>(a)...)
						  ]() mutable {
				
					auto ppp = std::move(pp.get());

					using seqb = typename make_tuple_index_sequence<decltype(pb)>::type;
					using seqa = typename make_tuple_index_sequence<decltype(pa)>::type;
					
					return apply(f,pb,*ppp,pa,seqb{},seqa{});
				};

				return std::async(std::launch::async,std::move(l));
			};
		}		
	};

	template<typename T, typename R>
	struct morphism<std::future<list<T>>,T,R>
	{
		template<typename F, typename... B, typename P, typename... A>
		decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
		{
			return [&f](B&&... b, P&& p, A&&... a) mutable {

				auto lambda = [f,
							   pb=std::make_tuple(std::forward<decltype(b)>(b)...),
							   pp=std::move(p),
							   pa=std::make_tuple(std::forward<decltype(a)>(a)...)
							   ]() mutable {

					using seqb = typename make_tuple_index_sequence<decltype(pb)>::type;
					using seqa = typename make_tuple_index_sequence<decltype(pa)>::type;
					
					auto ppp = std::move(pp.get());

					list<R> l;

					auto it = ppp->begin();

					while(it) {

						l->append(apply(f,pb,*it++,pa,seqb{},seqa{}));
					}

					return l;
				};

				return std::async(std::launch::async,std::move(lambda));
			};
		}		
	};

	template<typename T>
	struct morphism<std::future<list<T>>,T,void>
	{
		template<typename F, typename... B, typename P, typename... A>
		decltype(auto) operator()(F&& f, typelist<B...>, typelist<P>, typelist<A...>)
		{
			return [&f](B&&... b, P&& p, A&&... a) mutable {

				auto lambda = [f,
							   pb=std::make_tuple(std::forward<decltype(b)>(b)...),
							   pp=std::move(p),
							   pa=std::make_tuple(std::forward<decltype(a)>(a)...)
							   ]() mutable {

					using seqb = typename make_tuple_index_sequence<decltype(pb)>::type;
					using seqa = typename make_tuple_index_sequence<decltype(pa)>::type;
					
					auto ppp = std::move(pp.get());

					auto it = ppp->begin();

					while(it) {

						apply(f,pb,*it++,pa,seqb{},seqa{});
					}

					return maybe<bool>();
				};

				return std::async(std::launch::async,std::move(lambda));
			};
		}
	};

	template<typename F>
	decltype(auto) make_morphism(F&& f)
	{
		return [&f](auto&& o, auto&&... a) mutable {

			using IT = typename clear_type<decltype(o)>::type;
			using OT = typename function_traits<typename clear_type<F>::type>::ctype;
			using RT = typename function_traits<typename clear_type<F>::type>::rtype;

			return morphism<IT,OT,RT>()(std::forward<F>(f))
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
			
			return morphism<IT,OT,RT>()(std::forward<F>(f),
										  BEFORE{},
										  typelist<IT>{},
										  AFTER{})(std::forward<decltype(a)>(a)...);
		};
	}
}	
