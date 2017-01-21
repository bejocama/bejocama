#pragma once

#include <utility>
#include "bejocama/typelist.h"
#include "bejocama/maybe.h"

namespace bejocama
{
	namespace provider
	{
		template<template<typename> class T>
		struct factory
		{
			template<typename D, typename U>
			static T<D>* create_impl
			(typelist<T<D>>, const U& u)
			{
				return u->clone();
			}
			
			template<typename D, typename... U>
			static T<D>* create_impl
			(bejocama::clear_typelist<U...>, U&&... u);

			template<typename D, typename U>
			static T<D>* create_impl
			(typelist<T<D>>, U&& u)
			{
				return u.release();
			}

			template<typename D, typename... U>
			static T<D>* create(U&&... u)
			{
				return create_impl<D>
					(bejocama::clear_typelist<U...>{},
					 std::forward<U>(u)...);
			}
		};
	}
	
	template<template<typename> class T, typename D>
	struct type : maybe<T<D>*>
	{
		using interface = T<D>;
		
		template<typename... U>
		type(U&&... u)
			: maybe<T<D>*>
			(provider::factory<T>::template create<D>(std::forward<U>(u)...))
		{
		}

		type(type&& t) : maybe<T<D>*>(t.release())
		{
		}

		type(const type&) = delete;

		type() {}

		type operator++() { return this->get()->operator++(); }
		type operator++(int) { return this->get()->operator++(0); }
		type operator--() { return this->get()->operator--(); }
		type operator--(int) { return this->get()->operator--(0); }		
	};

	template<typename T, typename... A>
	T make_type(A&&... a)
	{
		return T(std::forward<A>(a)...);
	}
}
