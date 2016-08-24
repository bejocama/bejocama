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
#include <type_traits>
#include "bejocama/typelist.h"
#include "bejocama/utility.h"

namespace bejocama
{
	template<typename F>
	struct function_traits
	{
		using rtype = typename function_traits<decltype(&clear_type<F>::type::operator())>::rtype;
		using atype = typename function_traits<decltype(&clear_type<F>::type::operator())>::atype;
	
		template<size_t P, size_t N>
		using args = typename tl_sub<atype,P,N>::type;
	};

	template<typename R, typename... A>
	struct function_traits<R(A...)>
	{
		using rtype = R;
		using atype = typelist<A...>;
	
		template<size_t P, size_t N>
		using args = typename tl_sub<atype,P,N>::type;
	};

	template<typename R, typename... A>
	struct function_traits<R(&)(A...)>
	{
		using rtype = R;
		using atype = typelist<A...>;
	
		template<size_t P, size_t N>
		using args = typename tl_sub<atype,P,N>::type;
	};

	template<typename R, typename... A>
	struct function_traits<R(*)(A...)>
	{
		using rtype = R;
		using atype = typelist<A...>;
	
		template<size_t P, size_t N>
		using args = typename tl_sub<atype,P,N>::type;
	};
	
	template<typename R, typename C, typename... A>
	struct function_traits<R(C::*)(A...)>
	{
		using rtype = R;
		using atype = typelist<A...>;
	
		template<size_t P, size_t N>
		using args = typename tl_sub<atype,P,N>::type;
	};

	template<typename R, typename C, typename... A>
	struct function_traits<R(C::*)(A...) const>
	{
		using rtype = R;
		using atype = typelist<A...>;
	
		template<size_t P, size_t N>
		using args = typename tl_sub<atype,P,N>::type;
	};
	
	template<typename F>
	struct return_type
	{
		using type = typename function_traits<F>::rtype;
	};

	template<typename G, typename F>
	struct is_member_of_return_type : std::false_type
	{
	};

	template<typename R, typename C, typename F>
	struct is_member_of_return_type<R(C::*)() const, F>
	{		
		using RT = typename function_traits<F>::rtype;

		using VT = typename RT::value_type;
		
		static constexpr bool value = std::is_same<C,VT>::value;
	};

	template<typename R, typename C, typename F>
	struct is_member_of_return_type<R(C::*)(), F>
	{
		using RT = typename function_traits<F>::rtype;

		using VT = typename RT::value_type;
		
		static constexpr bool value = std::is_same<C,VT>::value;
	};
}
