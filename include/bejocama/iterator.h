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
#include <memory>
#include "bejocama/provider/iterator.h"
#include "bejocama/maybe.h"

namespace bejocama
{
	template<typename T>
	struct iterator : maybe<base::iterator<T>*>
	{
		iterator(const iterator& it)
			: maybe<base::iterator<T>*>(it->clone())
		{
		}

		iterator(base::iterator<T>* i)
			: maybe<base::iterator<T>*>(i)
		{
		}

		operator bool()
		{
			return *(this->get());
		}
		
		T& operator*()
		{
			return **(this->get());
		}

		iterator<T> operator++()
		{
			return ++(*(this->get()));
		}

		iterator<T> operator++(int)
		{
			return (*(this->get()))++;
		}		

		iterator<T> operator--()
		{
			return --(*(this->get()));
		}

		iterator<T> operator--(int)
		{
			return (*(this->get()))--;
		}		
	};
}
