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

namespace bejocama
{
	template<typename> struct maybe;
	template<typename> struct iterator;
	template<typename> struct list;
	template<typename> struct file;

	namespace base
	{
		template<typename T>
		struct iterator
		{
			virtual T& operator*() = 0;

			virtual bejocama::iterator<T> operator++() = 0;
			virtual bejocama::iterator<T> operator++(int) = 0;
			virtual bejocama::iterator<T> operator--() = 0;
			virtual bejocama::iterator<T> operator--(int) = 0;
			virtual operator bool() const = 0;
			virtual iterator<T>* clone() = 0;
		};
		
		template<typename T>
		struct list
		{
			virtual ~list() {}
			virtual std::size_t size() const = 0;
			virtual bejocama::iterator<T> begin() = 0;
			virtual bejocama::iterator<T> end() = 0;
			virtual bejocama::iterator<T> at(std::size_t) = 0;
			virtual bejocama::list<T> add(T&& t) = 0;
		};

		template<typename T>
		struct file
		{
			using value_type = T;

			virtual ~file()
			{
			}

			virtual const std::size_t size() const = 0;

			virtual T* begin() const = 0;

			virtual T* end() const  = 0;

			virtual maybe<bejocama::file<T>> add(T&&) = 0;
		};
	}

	namespace internal
	{
		template<typename,typename> struct list;
		template<typename> struct file;
		
		template<typename>
		struct factory;

		template<typename T>
		struct factory<bejocama::base::list<T>>
		{
			template<typename U>
			static bejocama::base::list<T>* create_impl(U&& u, tag<bool>)
			{
				return u.release();
			}

			template<typename U>
			static bejocama::base::list<T>* create_impl(U&& u, tag<char>)
			{
				using TT = typename bejocama::clear_type<T>::type;
				using UU = typename bejocama::clear_type<U>::type;

				return new list<TT,UU>(std::forward<U>(u));
			}

			template<typename U>
			static bejocama::base::list<T>* create(U&& u)
			{
				using way = typename std::conditional
					<std::is_same<typename clear_type<U>::type,bejocama::list<T>>::value,bool,char>::type;

				return create_impl(std::forward<U>(u),tag<way>{});
			}			
		};

		template<typename T>
		struct factory<bejocama::base::file<T>>
		{
			template<typename U>
			static bejocama::base::file<T>* create(U&& u)
			{
				using TT = typename bejocama::clear_type<T>::type;

				return new file<TT>(std::forward<U>(u));
			}
		};
	}
}
