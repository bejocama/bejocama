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
#include "combinator.h"
#include "traits.h"

namespace bejocama
{
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
		return [&g,h=wrapper<F>(std::forward<F>(f))]
			(BEFORE&&... before, REPLACE&&... replace, AFTER&&... after) mutable {

			auto val = h.get()(std::forward<REPLACE>(replace)...);

			using VT = typename clear_type<decltype(val)>::type;
			using XT = typename clear_type<AT>::type;

			auto lambda = [&g,&before...,&after...](AT&& at) mutable {

				return std::forward<G>(g)(std::forward<BEFORE>(before)...,
										  std::forward<AT>(at),
										  std::forward<AFTER>(after)...);
			};

			return combinator<tag<XT,VT>>()(lambda,val);
		};
	}

	template<size_t P,
			 typename G,
			 typename F,
			 typename... METHOD,
			 typename... OBJECT>
	decltype(auto) compose_object_typed(G&& g, F&& f, tag<typelist<METHOD...>,typelist<OBJECT...>>)
	{
		return [&g,h=wrapper<F>(std::forward<F>(f))](OBJECT&&... object, METHOD&&... method) mutable {

			auto val = h.get()(std::forward<OBJECT>(object)...);

			using VT = typename clear_type<decltype(val)>::type;

			auto lambda = [&g, &method...](auto&& x) mutable {
			
				return (std::forward<decltype(x)>(x).*std::forward<G>(g))
				(std::forward<METHOD>(method)...);
			};
			
			return combinator<tag<VT>>()(lambda,val);
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

	template<typename... F>
	decltype(auto) composer(F&&... f);

	namespace utility
	{
		template<typename T>
		struct composer;

		template<>
		struct composer<tag<char>>
		{
			template<typename F, typename G, typename... H>
				decltype(auto) operator()(F&& f, G&& g, H&&... h)
			{
				return bejocama::composer
					(compose<0>(std::forward<G>(g), std::forward<F>(f)),std::forward<H>(h)...);
			}
		};

		template<>
		struct composer<tag<bool>>
		{
			template<typename F, typename G>
				decltype(auto) operator()(F&& f, G&& g)
			{
				return bejocama::compose<0>(std::forward<G>(g), std::forward<F>(f));
			}
		};
	}

	template<typename... F>
	decltype(auto) composer(F&&... f)
	{
		using way = typename std::conditional<(sizeof...(F) > 2), char, bool>::type;

		return utility::composer<tag<way>>()(std::forward<F>(f)...);
	}

	template<typename F>
	decltype(auto) curry(F&& f)
	{
		return [ff=wrapper<F>(std::forward<F>(f))](auto&& a) {

			return [&ff,&a](auto&&... b) { return ff.get()(std::forward<decltype(a)>(a),
														   std::forward<decltype(b)>(b)...); };
		};
	}
}
