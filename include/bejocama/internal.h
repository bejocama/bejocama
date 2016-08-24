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
#include <utility>
#include <cstring>
#include "bejocama/base.h"
#include "bejocama/iterator.h"

namespace bejocama
{
	struct io;
	template<typename T> struct file;
	template<typename T> struct maybe;
	maybe<io> fclose(io&&);
	
	namespace internal
	{
		template<typename T> struct iterator;
		
		template<template<typename...> class C, typename T, typename... TT>
		struct iterator<C<T,TT...>> : bejocama::base::iterator<T>
		{
			iterator(const typename C<T,TT...>::iterator& i,
					 const typename C<T,TT...>::iterator& b,
					 const typename C<T,TT...>::iterator& e) : _i(i), _b(b), _e(e)
			{
			}

			void operator++() override
			{
				if (*this) ++_i;
			}

			void operator++(int) override
			{
				if (*this) _i++;
			}

			void operator--() override
			{
				if (_i != _b) {

					--_i;
					
				} else {

					_i = _e;
				}
			}

			void operator--(int) override
			{
				if (_i != _b) {

					_i--;
					
				} else {

					_i = _e;
				}
			}

			T& operator*() override
			{
				return *_i;
			}

			operator bool() const
			{
				return (_i != _e);
			}
			
			typename C<T,TT...>::iterator _i;
			typename C<T,TT...>::iterator _b;
			typename C<T,TT...>::iterator _e;
		};
		
		template<typename T, typename P>
		struct list : bejocama::base::list<T>
		{
			template<typename... A>
			list(A&&... a) : _p(std::forward<A>(a)...),
							 bejocama::base::list<T>()
			{
			}

			list(P&& p)
				: _p(std::move(p)), bejocama::base::list<T>()
			{
			}

			std::size_t size() const override
			{
				return _p.size();
			}

			bejocama::iterator<T> begin() override
			{
				return new iterator<P>(_p.begin(), _p.begin(), _p.end());
			}

			bejocama::iterator<T> end() override
			{
				return new iterator<P>(_p.end(), _p.end(), _p.end());
			}

			bejocama::iterator<T> at(std::size_t pos) override
			{
				auto it = _p.begin();
				
				std::size_t p = 0;

				while ((it != _p.end()) && (p < pos)) {

					++p;
					++it;
				}

				return new iterator<P>(it,_p.begin(), _p.end());
			}

			P _p;
		};

		template<typename T>
		struct iterator<bejocama::file<T>> : bejocama::base::iterator<T>
		{
			iterator(T* i, T* b, T* e) : _i(i), _b(b), _e(e)
			{
			}

			void operator++() override
			{
				if (*this) ++_i;
			}

			void operator++(int) override
			{
				if (*this) _i++;
			}

			void operator--() override
			{
				if (_i != _b) {

					--_i;
					
				} else {

					_i = _e;
				}
			}

			void operator--(int) override
			{
				if (_i != _b) {

					_i--;
					
				} else {

					_i = _e;
				}
			}

			T& operator*() override
			{
				return *_i;
			}

			operator bool() const
			{
				return (_i != _e);
			}
			
			T* _i;
			T* _b;
			T* _e;
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

			bejocama::iterator<T> at(std::size_t pos) override
			{
				if (pos < size()) {
				
					return new iterator<bejocama::file<T>>(_p->begin() + pos, _p->begin(), _p->end());
				}

				return end();
			}

			bejocama::file<T> _p;
		};
		
		template<typename T, typename IO = bejocama::io>
		struct file : bejocama::base::file<T>
		{
			using value_type = T;

			file(const IO& i) : _io(i) {}

			~file()
			{
				bejocama::fclose(std::move(_io));
			}

			const char* get_mode() const;

			const std::size_t size() const override
			{
				return _io._stat.st_size;
			}

			T* begin() const override
			{
				return reinterpret_cast<T*>(_io._map.start + _io._map.poff);
			}

			T* end() const override
			{
				return reinterpret_cast<T*>(_io._map.start + _io._map.poff + _io._map.len);
			}

			IO _io;
		};
	}
}
