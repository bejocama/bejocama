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

#include "bejocama/functional.h"
#include "bejocama/interface/file.h"
#include "bejocama/io.h"
#include "bejocama/string.h"

namespace bejocama
{
	template<typename> struct maybe;
	
	template<typename T>
	struct make_file
	{
		maybe<file<T>> operator()(io&& i)
		{
			return maybe<file<T>>(i);
		}

		maybe<file<T>> operator()(const string& fn)
		{
			auto xopen = curry<0>(fopen,returns(io(fn)));

			auto xmap = curry<1,1>(mmap<T>,
								   returns(0),
								   returns(0));

			return composer(xopen,fstat,xmap);
		}
	};
			
	namespace internal
	{
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

		template<typename>
		struct factory;

		template<typename T>
		struct factory<bejocama::base::file<T>>
		{
			template<typename U>
			static bejocama::base::file<T>* create(U&& u)
			{
				using TT = typename bejocama::clear_type<T>::type;

				return new file<TT>(std::forward<U>(u));
			}
		};
	}
}
