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
#include <utility>
#include "bejocama/functional.h"
#include "bejocama/base.h"
#include "bejocama/iterator.h"
#include "bejocama/io.h"
#include "bejocama/file.h"

namespace bejocama
{
	namespace internal
	{
		template<typename T> struct iterator;
		
		template<template<typename...> class C, typename T, typename... TT>
		struct iterator<C<T,TT...>> : bejocama::base::iterator<T>
		{
			using type = iterator<C<T,TT...>>;
			
			iterator(const typename C<T,TT...>::iterator& i,
					 const typename C<T,TT...>::iterator& b,
					 const typename C<T,TT...>::iterator& e) : _i(i), _b(b), _e(e)
			{
			}

			iterator(const iterator& it) : _i(it._i), _b(it._b), _e(it._e)
			{
			}

			bejocama::base::iterator<T>* clone() override
			{
				return new type(*this);
			}

			bejocama::iterator<T> operator++() override
			{
				*this && ++_i;

				return new type(*this);
			}

			bejocama::iterator<T> operator++(int) override
			{
				auto it = new type(*this);

				*this && ++_i;

				return it;
			}
			
			bejocama::iterator<T> operator--() override
			{
				(_i != _b) && --_i || (_i = _e);

				return new type(*this);
			}

			bejocama::iterator<T> operator--(int) override
			{
				auto it = new type(*this);

				*this && --_i;
				
				return it;
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
		
		template<typename T>
		struct iterator<bejocama::file<T>> : bejocama::base::iterator<T>
		{
			using type = iterator<bejocama::file<T>>;
			
			iterator(T* i, T* b, T* e) : _i(i), _b(b), _e(e)
			{
			}

			iterator(const iterator& it) : _i(it._i), _b(it._b), _e(it._e)
			{
			}

			bejocama::base::iterator<T>* clone() override
			{
				return new type(*this);
			}
			
			bejocama::iterator<T> operator++() override
			{				
				*this && ++_i;

				return new type(*this);
			}

			bejocama::iterator<T> operator++(int) override
			{
				auto it = new type(*this);

				*this && ++_i;

				return it;
			}

			bejocama::iterator<T> operator--() override
			{
				_i != _b && --_i || (_i = _e);

				return new type(*this);
			}

			bejocama::iterator<T> operator--(int) override
			{
				auto it = new type(*this);

				*this && --_i;

				return it;
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
		struct file : bejocama::base::file<T>
		{
			using value_type = T;

			file(bejocama::io&& i) : _io(new bejocama::io(std::move(i))) {}

			~file()
			{
				curry<0>(bejocama::fclose,returns(std::move(_io)))();
			}

			const char* get_mode() const;

			const std::size_t size() const override
			{
				return _io->_stat.st_size;
			}

			T* begin() const override
			{
				return reinterpret_cast<T*>(_io->_map.start + _io->_map.poff);
			}

			T* end() const override
			{
				return reinterpret_cast<T*>(_io->_map.start + _io->_map.poff + _io->_map.len);
			}

			maybe<bejocama::file<T>> add(T&& t) override
			{
				using otype = maybe<bejocama::file<T>>(make_file<T>::*)(io&&);
				
				auto mkf = make_function<otype>(make_file<T>());

				return composer
					(curry<0>(fclose,returns(std::move(*_io.release()))),
					 fopen,
					 fstat,
					 curry<1>(ftruncate<T>,returns(1)),
					 fstat,
					 curry<1,1>(mmap<T>,returns(-1),returns(1)),
					 fcopy<T>,
					 munmap,
					 curry<1,1>(mmap<T>,returns(0),returns(0)),
					 mkf)
					(std::forward<T>(t));
			}
			
			maybe<bejocama::io*> _io;
		};
	}
}
