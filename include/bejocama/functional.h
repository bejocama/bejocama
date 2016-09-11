#pragma once

#include "traits.h"

namespace bejocama
{
	namespace hidden
	{
		template<typename P, typename L>
		struct make_function;

		template<typename P, typename... A>
		struct make_function<P,typelist<A...>>
		{
			template<typename T>
				static decltype(auto) make(T&& t)
			{
				return [t(std::move(t))](A&&... a) mutable {

					P p = static_cast<P>(&clear_type<T>::type::operator());
			
					return (t.*p)(std::forward<A>(a)...);
				};
			}
		};
	}
	
	template<typename P, typename T>
	decltype(auto) make_function(T&& t)
	{
		return hidden::make_function<P,typename function_traits<P>::atype>::make(t);
	}
}
