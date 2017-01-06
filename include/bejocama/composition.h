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

	template<typename T>
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
	struct morphism<tag<list<T>,R>>
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
	struct morphism<tag<list<T>,void>>
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
	
	template<typename T>
	struct morphism<maybe<T>>
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
	};

	template<typename T, typename R>
	struct morphism<tag<maybe<T>,R>>
	{
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

	template<typename T>
	struct morphism<std::future<T>>
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
	};

	template<typename T, typename R>
	struct morphism<tag<std::future<T>,R>>
	{
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

	template<typename T>
	struct morphism<std::future<maybe<T>>>
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
	};

	template<typename T, typename R>
	struct morphism<tag<std::future<maybe<T>>,R>>
	{
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
	struct morphism<tag<std::future<list<T>>,R>>
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
	struct morphism<tag<std::future<list<T>>,void>>
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

			using C = typename clear_type<decltype(o)>::type;

			return morphism<C>()(std::forward<F>(f))
				(std::forward<decltype(o)>(o), std::forward<decltype(a)>(a)...);
		};
	}

	template<std::size_t P, typename F>
	decltype(auto) make_morphism(F&& f)
	{
		return [&f](auto&&... a) mutable {

			constexpr size_t N = sizeof...(a);
			
			using types = typelist<typename clear_type<decltype(a)>::type...>;			
			using BEFORE = typename tl_sub<types,0,P>::type;
			using AFTER = typename tl_sub<types,P+1,N-P-1>::type;
			using AT = typename clear_type<typename tl_get<types,P>::type>::type;

			using RT = typename function_traits<typename clear_type<F>::type>::rtype;
			
			return morphism<tag<AT,RT>>()(std::forward<F>(f),
										  BEFORE{},
										  typelist<AT>{},
										  AFTER{})(std::forward<decltype(a)>(a)...);
		};
	}
	
	template<typename A, typename B>
	struct combinator;
	
	template<size_t P,
			 typename G,
			 typename F,
			 typename... REPLACE,
			 typename... BEFORE,
			 typename AT,
			 typename... AFTER>
	decltype(auto) compose_typed(G&& g, F&& f, tag<typelist<REPLACE...>,
								 typelist<BEFORE...>, typelist<AT>, typelist<AFTER...>>)
	{
		return [gg=std::forward<G>(g),
				ff=std::forward<F>(f)]
			(BEFORE&&... before, REPLACE&&... replace, AFTER&&... after) mutable {

			return make_morphism<P>(gg)(std::forward<BEFORE>(before)...,
									   std::move(ff(std::forward<REPLACE>(replace)...)),
									   std::forward<AFTER>(after)...);
		};
	}

	template<size_t P,
			 typename G,
			 typename F,
			 typename... METHOD,
			 typename... OBJECT>
	decltype(auto) compose_object_typed(G&& g, F&& f, tag<typelist<METHOD...>,typelist<OBJECT...>>)
	{
		return [g(std::forward<G>(g)),f(std::forward<F>(f))](OBJECT&&... object,
															 METHOD&&... method) mutable {

			return make_morphism(g)(f(std::forward<OBJECT>(object)...),
									std::forward<METHOD>(method)...);
		};
	}
	
	template<typename T, std::size_t P>
	struct compose_at;
		
	template<size_t P>
	struct compose_at<tag<bool>,P>
	{
		template<typename G, typename F>
		decltype(auto) operator()(G&& g, F&& f)
		{
			constexpr auto N = function_traits<typename clear_type<G>::type>::atype::size;

			using BEFORE = typename function_traits<typename clear_type<G>::type>::template args<0,P>;
			using AFTER = typename function_traits<typename clear_type<G>::type>::template args<P+1,N-P-1>;
			using AT = typename function_traits<typename clear_type<G>::type>::template args<P,1>;
			using REPLACE = typename function_traits<typename clear_type<F>::type>::atype;

			return compose_typed<P>(std::forward<G>(g),
									std::forward<F>(f),
									tag<REPLACE,BEFORE,AT,AFTER>{});
		}
	};
	
	template<size_t P>
	struct compose_at<tag<char>,P>
	{
		template<typename G, typename F>
		decltype(auto) operator()(G&& g, F&& f)
		{
			using METHOD = typename function_traits<typename clear_type<G>::type>::atype;
			using OBJECT = typename function_traits<typename clear_type<F>::type>::atype;
	
			return compose_object_typed<P>(std::forward<G>(g),
										   std::forward<F>(f),
										   tag<METHOD,OBJECT>{});
		}
	};

	template<size_t P, typename G, typename F>
	decltype(auto) compose(G&& g, F&& f)
	{
		using way = typename std::conditional
			<is_member_of_return_type<typename clear_type<G>::type,
									  typename clear_type<F>::type>::value, char, bool>::type;

		return compose_at<tag<way>,P>()(std::forward<G>(g), std::forward<F>(f));
	}

	namespace utility
	{
		template<typename T>
		struct composer;

		template<>
		struct composer<tag<char>>
		{
			template<typename F, typename G, typename... H>
			static constexpr decltype(auto) action(F&& f, G&& g, H&&... h)
			{
				using way = typename std::conditional<(sizeof...(H) > 1), char, bool>::type;
				
				return composer<tag<way>>::action
					(bejocama::compose<0>(std::forward<G>(g), std::forward<F>(f)),std::forward<H>(h)...);
			}
		};

		template<>
		struct composer<tag<bool>>
		{
			template<typename F, typename G>
			static constexpr decltype(auto) action(F&& f, G&& g)
			{
				return bejocama::compose<0>(std::forward<G>(g), std::forward<F>(f));
			}
		};

		template<typename,std::size_t...>
		struct curry;

		template<std::size_t P, std::size_t... PP>
		struct curry<tag<char>,P,PP...>
		{
			template<typename F, typename G, typename... GG>
			static constexpr decltype(auto) action(F&& f, G g, GG... gg)
			{
				using way = typename std::conditional<(sizeof...(GG) > 1), char, bool>::type;
				
				return curry<tag<way>,PP...>::action(compose<P>(std::forward<F>(f),
																std::forward<G>(g)),
													 std::forward<GG>(gg)...);
			}
		};
		
		template<std::size_t P>
		struct curry<tag<bool>,P>
		{
			template<typename F, typename G>
			static constexpr decltype(auto) action(F&& f, G&& g)
			{
				return compose<P>(std::forward<F>(f),
								  std::forward<G>(g));
			}
		};
	}

	template<typename... F>
	decltype(auto) composer(F&&... f)
	{
		using way = typename std::conditional<(sizeof...(F) > 2), char, bool>::type;

		return utility::composer<tag<way>>::action(std::forward<F>(f)...);
	}

	template<std::size_t... P, typename F, typename... G>
	decltype(auto) curry(F&& f, G&&... g)
	{
		using way = typename std::conditional<(sizeof...(G) > 1), char, bool>::type;
		
		return utility::curry<tag<way>,P...>::action(std::forward<F>(f),
													 std::forward<G>(g)...);
	}
}
