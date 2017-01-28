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

			static void Set(N& n, N n_new)
			{
				n = n_new;
			}
		};

		template<unsigned N>
		struct string_column
		{
			using type = char[N];

			static void Set(char (&c)[N], const char* c_new)
			{
				strncpy(c,c_new,N);
			}
		};

		template<typename... T>
		struct container : std::tuple<typename T::type...>
		{
			using columns = typelist<T...>;
			
			template<typename U>
			decltype(auto) get()
			{
				return std::get<type_index<U,columns>::value>(*this);
			}
		};

		template<typename... T>
		struct row : container<T...>
		{
			template<typename U, typename V>
			void set(V&& v)
			{
				U::Set(this->template get<U>(),v);
			}
			
			template<typename D>
			bool stream(std::ostream& os, D&& d)
			{
				return (os << std::forward<D>(d));
			}

			template<typename D>
			bool stream(std::istream& is, D&& d)
			{
				return (is >> std::forward<D>(d));
			}
			
			template<typename S, std::size_t P = 0>
			bool io(S& s, typelist<bool> = {})
			{
				using type = typename std::conditional<(P+1<sizeof...(T)),bool,char>::type;
				
				return stream(s,std::get<P>(*this)) ? io<S,P+1>(s,typelist<type>{}) : false;
			}

			template<typename S, std::size_t P>
			bool io(S& s, typelist<char>)
			{
				return true;
			}
		};

		template<typename... T>
		struct table : bejocama::list<row<T...>>
		{
			using row = bejocama::db::row<T...>;
		};		
	}
}

template<typename R, typename S = typename std::decay<R>::type,
		 typename = typename std::enable_if
		 <bejocama::is_base_of<bejocama::db::row,S>::value,int>::type>
std::ostream& operator<<(std::ostream& os, R&& r)
{
	return r.io(os) ? os : os;
}

template<typename R, typename S = typename std::decay<R>::type,
		 typename = typename std::enable_if
		 <bejocama::is_base_of<bejocama::db::row,S>::value,int>::type>
std::istream& operator>>(std::istream& is, R&& r)
{
	return r.io(is) ? is : is;
}

template<typename R, typename D, typename S = typename std::decay<R>::type,
		 typename = typename std::enable_if
		 <bejocama::is_base_of<bejocama::db::row,S>::value,int>::type>
decltype(auto) operator>>(R&& r, D&& d)
{
	return std::forward<R>(r).set(std::forward<D>(d));
}
