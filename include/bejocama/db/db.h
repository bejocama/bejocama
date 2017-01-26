#pragma once

namespace bejocama
{
	namespace db
	{
		template<typename U, typename S, typename T = S>
		struct tuple_pos;

		template<typename U, typename... S, typename... T>
		struct tuple_pos<U, typelist<S...>, typelist<T...>>
		{
			static constexpr unsigned value =
				std::is_same<U,typename typelist<T...>::first>::value ? sizeof...(S) - sizeof...(T)
				: tuple_pos<U, typelist<S...>, typename typelist<T...>::tail>::value;
		};

		template<typename U, typename... S>
		struct tuple_pos<U, typelist<S...>, typelist<>>
		{
			static constexpr unsigned value = 9999;
		};

		template<typename... T>
		struct make_tuple;

		template<typename... T>
		struct make_tuple<typelist<T...>>
		{
			using type = std::tuple<T...>;
		};

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
		
		template<typename... T>
		struct table
		{
			static constexpr unsigned columns = sizeof...(T);
			
			using column_types = typelist<T...>;

			struct row : make_tuple<typelist<typename T::type...>>::type
			{
				template<typename U>
				decltype(auto) get()
				{
					return std::get<tuple_pos<U,typename table::column_types>::value>(*this);
				}

				template<typename S, std::size_t P = 0>
				bool io(S& s, typelist<bool> = {})
				{
					using type = typename std::conditional<(P+1<table::columns),bool,char>::type;
				
					return stream(s,std::get<P>(*this)) ? io<S,P+1>(s,typelist<type>{}) : false;
				}

				template<typename S, std::size_t P>
				bool io(S& s, typelist<char>)
				{
					return true;
				}
			};
		};
	}
}
