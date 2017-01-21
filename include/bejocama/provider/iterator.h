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

#include "bejocama/interface/iterator.h"

namespace bejocama
{
	namespace provider
	{
		template<typename> struct iterator;
		
		template<template<typename...> class C, typename T, typename... TT>
		struct iterator<C<T,TT...>> : bejocama::base::iterator<T>
		{
			using type = iterator<C<T,TT...>>;
			
			iterator(C<T,TT...>& c, typename C<T,TT...>::iterator i) :
				_c(c), _i(i)
			{
			}

			iterator(const iterator& it) : _c(it._c), _i(it._i)
			{
			}

			bejocama::base::iterator<T>* clone() override
			{
				return new type(*this);
			}

			bejocama::iterator<T> operator++() override
			{
				*this && ++_i;

				return bejocama::iterator<T>(_c,_i);
			}

			bejocama::iterator<T> operator++(int) override
			{
				auto ii = _i;

				*this && ++_i;

				return bejocama::iterator<T>(_c,ii);
			}
			
			bejocama::iterator<T> operator--() override
			{
				(_i != _c.begin()) && --_i || (_i = _c.end());

				return bejocama::iterator<T>(_c,_i);
			}

			bejocama::iterator<T> operator--(int) override
			{
				auto ii = _i;

				*this && --_i;
				
				return bejocama::iterator<T>(_c,ii);
			}
			
			T& get() override
			{
				return *_i;
			}

			bool at_end() const override
			{
				return (_i == _c.end()) && (_i == _c.rend());
			}
			
			C<T,TT...>& _c;
			typename C<T,TT...>::iterator _i;
		};
		
		template<typename T>
		struct iterator<bejocama::file<T>> : bejocama::base::iterator<T>
		{
			using type = iterator<bejocama::file<T>>;
			
			iterator(bejocama::file<T>& f, T* i) : _f(f), _i(i)
			{
			}

			iterator(const iterator& it) : _f(it._f), _i(it._i)
			{
			}

			bejocama::base::iterator<T>* clone() override
			{
				return new type(*this);
			}
			
			bejocama::iterator<T> operator++() override
			{				
				at_end() || ++_i;

				return bejocama::iterator<T>(_f,_i);
			}

			bejocama::iterator<T> operator++(int) override
			{
				auto ii = _i;

				at_end() || ++_i;

				return bejocama::iterator<T>(_f,ii);
			}

			bejocama::iterator<T> operator--() override
			{
				_i != _f->begin() && --_i || (_i = _f->end());

				return bejocama::iterator<T>(_f,_i);
			}

			bejocama::iterator<T> operator--(int) override
			{
				auto ii = _i;

				at_end() || --_i;

				return bejocama::iterator<T>(_f,ii);
			}
			
			T& get() override
			{
				return *_i;
			}

			bool at_end() const override
			{
				return (_i == _f->end()) || (_i == _f->rend());
			}
			
			bejocama::file<T>& _f;
			T* _i;
		};		
	}
}
