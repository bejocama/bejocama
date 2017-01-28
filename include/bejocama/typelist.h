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
#include <cstddef>
#include <utility>
#include "utility.h"

namespace bejocama
{
	
	template<typename... A>
	struct typelist;

	template<typename A, typename... AA>
	struct typelist<A,AA...>
	{
		static const std::size_t size = 1 + sizeof...(AA);

		using first = A;
		using tail = typelist<AA...>;
		using index = decltype(std::make_index_sequence<size>());

		static constexpr auto get_index()
		{
			return std::make_index_sequence<size>();
		}
	};

	template<>
	struct typelist<>
	{
		static const size_t size = 0;

		using first = void;
		using tail = typelist<>;
	};

	template<typename A, typename B>
	struct tl_concat;
	
	template<typename... A, typename... B>
	struct tl_concat<typelist<A...>, typelist<B...>>
	{
		using type = typelist<A..., B...>;
	};
	
	template<typename L, typename A>
	struct tl_append;

	template<typename A, typename... E>
	struct tl_append<typelist<E...>,A>
	{
		using type = typelist<E...,A>;
	};

	template<typename A>
	struct tl_append<typelist<>, A>
	{
		using type = typelist<A>;
	};

	template<>
	struct tl_append<typelist<>, void>
	{
		using type = typelist<>;
	};

	template<typename L, size_t P, size_t S = 0, typename E = enabler>
	struct tl_get;

	template<typename L, size_t P, size_t S>
	struct tl_get<L,P,S,typename std::enable_if<(S < P),enabler>::type>
	{
		using type = typename tl_get<typename L::tail,P,S+1>::type;
	};

	template<typename L, size_t P, size_t S>
	struct tl_get<L,P,S,typename std::enable_if<(S == P),enabler>::type>
	{
		using type = typename L::first;
	};

	template<typename L, size_t P, size_t S, size_t CP = 0, size_t CS = 0,
			 typename R = typelist<>, typename E = enabler>
	struct tl_sub
	{
		using type = typelist<>;
	};

	template<typename L, size_t P, size_t S, size_t CP, size_t CS, typename R>
	struct tl_sub<L,P,S,CP,CS,R,typename std::enable_if<(CP < P) && (L::size > 0),enabler>::type>
	{
		using type = typename tl_sub<typename L::tail,P,S,CP+1,CS,R>::type;
	};

	template<typename L, size_t P, size_t S, size_t CP, size_t CS, typename R>
	struct tl_sub<L,P,S,CP,CS,R,typename std::enable_if<((CS < S) && (CP == P) && (L::size > 0)),
		enabler>::type>
	{
		using type = typename tl_sub<
			typename L::tail,P,S,P,CS+1,
			typename tl_append<R,
							   typename L::first>::type>::type;
	};

	template<typename L, size_t P, size_t S, size_t CP, size_t CS, typename R>
	struct tl_sub<L,P,S,CP,CS,R,typename std::enable_if<((CS == S) && (CP == P)),enabler>::type>
	{
		using type = R;
	};

	template<typename L, typename T>
	struct typelist_contains
	{
		static constexpr bool value = std::is_same<typename L::first, T>::value ? true :
			typelist_contains<typename L::tail,T>::value;
	};

	template<typename T>
	struct typelist_contains<typelist<>,T>
	{
		static constexpr bool value = false;
	};

	template<typename... T>
	using clear_typelist = typelist<typename clear_type<T>::type...>;

	template<std::size_t P, typename... T>
	struct tl_extract
	{
		using type = typelist<typename tl_get<T,P>::type...>;
	};

	template<typename U, typename S, typename T = S>
	struct tl_index;

	template<typename U, typename... S, typename... T>
	struct tl_index<U, typelist<S...>, typelist<T...>>
	{
		static constexpr unsigned value =
			std::is_same<U,typename typelist<T...>::first>::value ? sizeof...(S) - sizeof...(T)
			: tl_index<U, typelist<S...>, typename typelist<T...>::tail>::value;
	};

	template<typename U, typename... S>
	struct tl_index<U, typelist<S...>, typelist<>>
	{
		static constexpr unsigned value = 9999;
	};

	template<typename... T>
	struct tl_make_tuple;

	template<typename... T>
	struct tl_make_tuple<typelist<T...>>
	{
		using type = std::tuple<T...>;
	};

	template<typename S, typename T>
	struct type_index;

	template<typename S, template<typename...> class C, typename... T>
	struct type_index<S,C<T...>>
	{
		static constexpr unsigned value = tl_index<S,typelist<T...>>::value;
	};
}
