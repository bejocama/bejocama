#pragma once

#include "bejocama/interface/list.h"

namespace bejocama
{
	namespace db
	{
		template<typename... A>
		struct row : std::tuple<A...>
		{
			using type = std::tuple<A...>;
	
			using type::tuple;
			
			template<typename T>
			decltype(auto) get() {

				return std::get<type_index<T,std::tuple<A...>>::value>(*this);
			}

			template<typename T, typename V>
			decltype(auto) set(V&& v)
			{
				this->template get<T>() = std::forward<V>(v);

				return *this;
			}			

			template<std::size_t N = 0>
			std::ostream& stream_handler(std::ostream& os, bool = true)
			{
				os << std::get<N>(*this);

				return stream_handler<N+1>
					(os,typename std::conditional<(N < sizeof...(A) - 1),bool,int>::type{0});
			}

			template<std::size_t N>
			std::ostream& stream_handler(std::ostream& os, int = 0)
			{
				return os;
			}
		};

		template<typename... A>
		struct row<std::tuple<A...>> : row<A...>
		{
			using row<A...>::row;
		};

		template<typename N>
		struct numeric_column : row<N>
		{
			using type = N;

			numeric_column() : row<N>(0) {}

			template<typename T>
			numeric_column(T&& t) : row<N>(std::forward<T>(t)) {}

			template<typename V>
			void set(V&& v)
			{
				std::get<0>(*this) = std::get<0>(std::forward<V>(v));
			}
		};

		template<unsigned N>
		struct string_column : row<char[N]>
		{
			using type = char[N];

			string_column() : row<char[N]>()
			{
				memset(std::get<0>(*this),0,N);
			}
			
			template<typename T>
			string_column(T&& t) : row<char[N]>()
			{
				strncpy(std::get<0>(*this),std::forward<T>(t),N);
			}

			template<typename V>
			void set(V&& v)
			{
				strncpy(std::get<0>(*this),std::get<0>(std::forward<V>(v)),N);
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
		 <bejocama::is_base_of<bejocama::db::row,S,S>::value>::type* = nullptr>
decltype(auto) operator<<(R&& r, D&& d)
{
	std::forward<R>(r).template
		get<typename std::decay<D>::type>().template set(std::forward<D>(d));

	return std::forward<R>(r);
}

template<typename T, typename R, typename S = typename std::decay<T>::type,
		 typename std::enable_if
		 <bejocama::is_base_of<bejocama::db::table,S,typename S::row>::value>::type* = nullptr>
decltype(auto) operator<<(T&& t, R&& r)
{
	return std::forward<T>(t)->add(std::forward<R>(r));
}
