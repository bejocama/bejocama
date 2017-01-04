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

#include <string>
#include "bejocama/interface/string.h"

namespace bejocama
{
	namespace internal
	{
		template<typename P>
		struct string;

		template<>
		struct string<std::string> : base::string
		{
			using type = std::string;
			
			string(std::string&& s) : base::string(), _p(new std::string(std::move(s)))
			{
			}

			string(const std::string& s) : base::string(), _p(new std::string(s))
			{
			}
			
			const char* c_str() const override
			{
				return _p ? _p->c_str() : "";
			}

			base::string* clone() const override
			{
				return _p ? new string(*_p) : nullptr;
			}
			
			maybe<std::string*> _p;
		};

		template<typename>
		struct factory;

		template<>
		struct factory<bejocama::base::string>
		{
			template<typename U>
			static bejocama::base::string* create_impl(const U& u, tag<bool>)
			{
				return u->clone();
			}
			
			template<typename U>
			static bejocama::base::string* create_impl(U&& u, tag<bool>)
			{
				return u.release();
			}

			template<typename U>
			static bejocama::base::string* create_impl(U&& u, tag<char>)
			{
				using UU = typename bejocama::clear_type<U>::type;

				return new string<UU>(std::forward<U>(u));
			}

			template<typename U>
			static bejocama::base::string* create(U&& u)
			{
				using way = typename std::conditional
					<std::is_same<typename clear_type<U>::type,
								  bejocama::string>::value,bool,char>::type;

				return create_impl(std::forward<U>(u),tag<way>{});
			}			
		};		
	}
}
