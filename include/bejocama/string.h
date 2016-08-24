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
#include <cstring>
#include <memory>
#include <string.h>

namespace bejocama
{
	struct string
	{
		using type = char[256];
		
		static constexpr auto capacity()
		{
			return sizeof(type) - 1;
		}

		string(const string& s) : _buffer(new type[1])
		{
			auto a = _buffer.get();

			auto b = s._buffer.get();
			
			::strncpy(*a, *b, capacity());
		}
		
		string() : _buffer(nullptr)
		{
		}

		string& set(type& str)
		{
			_buffer.reset(&str);

			return *this;
		}
		
		string(const char* str)
		{
			if (_buffer == nullptr) allocate();

			char* c = (char*)_buffer.get();
			
			strncpy(c, str, capacity());
		}
		
		std::size_t size()
		{
			if (_buffer == nullptr) return 0;

			char* c = (char*)_buffer.get();
			
			return strlen(c);
		}

		type& operator()() const
		{
			if (_buffer == nullptr) throw;

			return *_buffer.get();
		}

		
		type& operator()()
		{
			if (_buffer == nullptr) allocate();

			return *_buffer.get();
		}

		void allocate()
		{
			_buffer.reset(new type[1]);
		}
		
		std::unique_ptr<type> _buffer;
	};
}
