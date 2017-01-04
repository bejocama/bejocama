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

#include "bejocama/interface/list.h"

namespace bejocama
{
	template<typename> struct file;
	template<typename> struct maybe;
	
	namespace provider
	{
		template<typename> struct iterator;
		
		template<typename T, typename P>
		struct list : bejocama::base::list<T>
		{
			template<typename... A>
			list(A&&... a) : _p(new P(std::forward<A>(a)...)),
							 bejocama::base::list<T>()
			{
				static_assert(!std::is_same<bejocama::list<T>,P>::value,"bejocama::list may not be a provider");
			}

			std::size_t size() const override
			{
				return _p->size();
			}

			bejocama::iterator<T> begin() override
			{
				return new iterator<P>(_p->begin(), _p->begin(), _p->end());
			}

			bejocama::iterator<T> end() override
			{
				return new iterator<P>(_p->end(), _p->end(), _p->end());
			}

			bejocama::list<T> add(T&& t) override
			{
				_p->push_back(std::move(t));

				return std::move(*_p.release());
			}
			
			maybe<P*> _p;
		};

		template<typename T>
		struct list<T,bejocama::file<T>> : bejocama::base::list<T>
		{
			template<typename... A>
				list(A&&... a) : _p(std::forward<A>(a)...), bejocama::base::list<T>()
			{
			}

			list(bejocama::file<T>&& p) : _p(std::move(p)), bejocama::base::list<T>()
			{
			}

			std::size_t size() const override
			{
				return _p->size() / sizeof(T);
			}

			bejocama::iterator<T> begin() override
			{
				return new iterator<bejocama::file<T>>(_p->begin(), _p->begin(), _p->end());
			}

			bejocama::iterator<T> end() override
			{
				return new iterator<bejocama::file<T>>(_p->end(), _p->end(), _p->end());
			}

			bejocama::list<T> add(T&& t) override
			{
				auto f = _p->add(std::move(t));

				!f && make_throw_true(std::runtime_error("ERROR: cannot add to list"));

				return std::move(*f);
			}
			
			bejocama::file<T> _p;
		};
	}
}
