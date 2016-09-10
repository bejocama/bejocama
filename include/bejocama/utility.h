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
#include <future>

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

	template<typename F>
	decltype(auto) make_async(F&& f)
	{
		return [f(std::move(std::forward<F>(f)))]() mutable {

			return std::async(std::launch::async, f);
		};
	}

	template<typename T>
	decltype(auto) make_value(T&& t)
	{
		return [t(std::move(std::forward<T>(t)))](){ return std::move(t); };
	}

	template<typename P, typename T, typename... A>
	decltype(auto) make_function(T&& t, A&&... a)
	{
		return [t(std::move(std::forward<T>(t)))](auto&&... aa) {

			auto p = static_cast<P>(&T::operator());
			
			return (t.*p)(std::forward<decltype(aa)>(aa)...);
		};
	}
}
