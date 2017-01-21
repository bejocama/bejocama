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
	namespace provider
	{
		template<typename P, typename D>
		struct string;
		
		template<typename D>
		struct string<std::basic_string<D>,D> : base::string<D>
		{
			using type = std::basic_string<D>;
			
			string(std::basic_string<D>&& s) : _p(std::move(s)), base::string<D>()
			{
			}

			string(const std::basic_string<D>& s) : _p(s), base::string<D>()
			{
			}
			
			const D* c_str() const override
			{
				return _p.c_str();
			}

			bejocama::basic_string<D> clone() const override
			{
				return bejocama::basic_string<D>(_p);
			}
			
			std::basic_string<D> _p;
		};
	}
}
