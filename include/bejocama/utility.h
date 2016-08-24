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
#include <memory>

namespace bejocama
{

	enum class enabler {};

	template<typename... T>
	struct tag
	{
	};

	template<typename T, typename... TT>
	struct safe_unique_ptr : std::unique_ptr<T,TT...>
	{
		using std::unique_ptr<T,TT...>::unique_ptr;

		T* operator->() const
		{
			if (!this->get()) {

				throw std::runtime_error("Nullpointer error");
			}

			return this->get();
		}
	};

	template<typename T>
	struct clear_type
	{
		using type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
	};

	//wrapper for perfect forwading into catures
	template <typename _Tp,
			  bool _Deep_copy = std::is_rvalue_reference<_Tp>::value ||
			  (std::is_lvalue_reference<_Tp>::value &&
			   std::is_const<std::remove_reference_t<_Tp>>::value)>
		struct wrapper
		{
			wrapper(_Tp&& x) : value(std::forward<_Tp>(x)) {}
	
			_Tp& get() const { return value; }

			_Tp& get() { return value; }
	
			_Tp value;
		};

	template <typename _Tp>
	struct wrapper<_Tp, true>
	{
		wrapper(_Tp&& x) : value(std::forward<_Tp>(x)) {}

		template <typename _Up = _Tp>
		std::enable_if_t<std::is_rvalue_reference<_Up>::value, std::decay_t<_Tp>>
			get() const
		{
			return value;
		}
	
		template <typename _Up = _Tp>
		std::enable_if_t<!std::is_rvalue_reference<_Up>::value, _Tp>
		get() const
		{
			return value;
		}
		std::decay_t<_Tp> value;
	};	
}
