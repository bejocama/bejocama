#pragma once

#include "bejocama/interface/list.h"

namespace bejocama
{
	namespace db
	{
		template<typename N>
		struct numeric_column
		{
			using type = N;

			numeric_column() : n(0) {}

			template<typename T>
			numeric_column(T&& t) : n(std::forward<T>(t))
			{
			}

			template<typename T>
			void operator=(T&& c)
			{
				n = std::forward<T>(c).n;
			}
			
			N n;
		};

		template<unsigned N>
		struct string_column
		{
			using type = char[N];

			string_column()
			{
				memset(v,0,N);
			}
			
			template<typename T>
			string_column(T&& t)
			{
				strncpy(v,std::forward<T>(t),N);
			}

			template<typename T>
			void operator=(T&& t)
			{
				strncpy(v,std::forward<T>(t),N);
			}

			char v[N];
		};

		template<typename... T>
		struct container : std::tuple<T...>
		{
			template<typename U>
			decltype(auto) get()
			{
				return std::get<type_index<U,container>::value>(*this);
			}
		};

		template<typename... T>
		struct row : container<T...>
		{
			template<typename U, typename V>
			decltype(auto) set(V&& v)
			{
				this->template get<U>() = v;

				return *this;
			}			
		};

		template<typename... T>
		struct table : bejocama::list<row<T...>>
		{
			using types = typelist<T...>;
				
			using row = bejocama::db::row<T...>;
		};		
	}
}

template<typename R, typename D, typename S = typename std::decay<R>::type,
		 typename std::enable_if
		 <bejocama::is_base_of<bejocama::db::row,S>::value>::type* = nullptr>
decltype(auto) operator<<(R&& r, D&& d)
{
	return r.template set<typename std::decay<D>::type>(d);
}

template<typename T, typename R, typename S = typename std::decay<T>::type,
		 typename std::enable_if
		 <bejocama::is_base_of<bejocama::db::table,S>::value,int>::type* = nullptr>
decltype(auto) operator<<(T&& t, R&& r)
{
	return std::forward<T>(t)->add(std::forward<R>(r));
}
