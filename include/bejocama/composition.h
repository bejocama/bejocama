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
#include "functor.h"

namespace bejocama
{
	namespace hidden
	{
		struct recursion
		{
			struct yes;
			struct no;
		};
		
		template<size_t P,
				 typename G,
				 typename F,
				 typename... REPLACE,
				 typename... BEFORE,
				 typename AT,
				 typename... AFTER>
		decltype(auto) compose_indexed(G&& g, F&& f,
									   typelist<REPLACE...>,
									   typelist<BEFORE...>,
									   typelist<AT>,
									   typelist<AFTER...>)
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
		decltype(auto) compose_object_indexed(G&& g, F&& f, typelist<METHOD...>, typelist<OBJECT...>)
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

				return compose_indexed<P>
					(std::forward<G>(g), std::forward<F>(f), REPLACE{}, BEFORE{}, AT{}, AFTER{});
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
	
				return compose_object_indexed<P>(std::forward<G>(g),
												 std::forward<F>(f),
												 METHOD{},OBJECT{});
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
					(compose<0>(std::forward<G>(g), std::forward<F>(f)),std::forward<H>(h)...);
			}
		};

		template<>
		struct composer<tag<bool>>
		{
			template<typename F, typename G>
			static constexpr decltype(auto) action(F&& f, G&& g)
			{
				return compose<0>(std::forward<G>(g), std::forward<F>(f));
			}
		};

		template<typename, std::size_t...>
		struct curry;

		template<std::size_t P, std::size_t... PP>
		struct curry<recursion::yes,P,PP...>
		{
			template<typename F, typename G, typename... GG>
			static constexpr decltype(auto) action(F&& f, G g, GG... gg)
			{
				using way = typename std::conditional<(sizeof...(GG) > 1),
					recursion::yes,
					recursion::no>::type;
				
				return curry<way,PP...>::action(compose<P>(std::forward<F>(f),
														   std::forward<G>(g)),
												std::forward<GG>(gg)...);
			}
		};
		
		template<std::size_t P>
		struct curry<recursion::no,P>
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

		return hidden::composer<tag<way>>::action(std::forward<F>(f)...);
	}

	template<std::size_t... P, typename F, typename... G>
	decltype(auto) curry(F&& f, G&&... g)
	{
		using way = typename std::conditional<(sizeof...(G) > 1),
			hidden::recursion::yes,
			hidden::recursion::no>::type;
		
		return hidden::curry<way,P...>::action(std::forward<F>(f),
											   std::forward<G>(g)...);
	}
}
