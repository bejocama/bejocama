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
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include "bejocama/maybe.h"
#include "bejocama/interface/string.h"

namespace bejocama
{
	template<typename T>
	struct print
	{
		void operator()(const T& t)
		{	
			std::cout << t << std::endl;
		}
	};

	struct mapping
	{
		char* start = nullptr;
		off_t poff = 0;
		off_t off = 0;
		std::size_t len = 0;
	};
	
	struct io
	{
		io() {}
		
		io(const string& fn) : _fn(fn->clone()) {}

		io(const io&  i) : _fn(i._fn->clone()),
						   _fd(i._fd),
						   _stat(i._stat),
						   _map(i._map),
						   _mode(i._mode)
		{
		}

		io operator=(const io& i)
		{
			return io(i);
		}
		
		string _fn;
		int _fd = -1;
		struct stat _stat;
		mapping _map;
		int _mode = O_RDWR | O_CREAT;
	};

	maybe<io> fopen(io&& i)
	{
		i._fd = open(i._fn->c_str(), i._mode , (mode_t)0600);
		
		if (i._fd == -1) {

			perror("fopen");
			
			return maybe<io>();
		}
		
		return maybe<io>(i);
	}

	maybe<io> fstat(io&& i)
	{
		
		if (::fstat(i._fd, &i._stat) == -1) {

			perror("fopen");
			
			return maybe<io>();
		}

		return maybe<io>(i);
	}

	template<typename T>
	maybe<io> mmap(io&& i, long int s, unsigned long int n)
	{
		long int o = s * sizeof(T);

		if (o < 0) {

			o = i._stat.st_size + o;
		}

		i._map.len = n * sizeof(T);

		if (i._map.len == 0) {

			i._map.len = i._stat.st_size;
		}
		
		if (o + i._map.len > i._stat.st_size) {

			return maybe<io>();
		}
		
		i._map.off = o;
		
		off_t pa_off = i._map.off & ~(sysconf(_SC_PAGE_SIZE) - 1);

		i._map.poff = i._map.off - pa_off;

	    auto plen = i._map.poff + i._map.len;

		i._map.start = (char*)::mmap(0, plen, PROT_READ | PROT_WRITE, MAP_SHARED, i._fd, pa_off);

		if (i._map.start == nullptr) {

			perror("mmap");
			
			return maybe<io>();
		}

		return maybe<io>(i);
	}

	template<typename T>
	maybe<io> ftruncate(io&& i, long int n = 0)
	{
		long int l = i._stat.st_size + n * sizeof(T);

		if (l < 0) {

			return maybe<io>();
		}
		
		if (::ftruncate(i._fd, l) == -1) {

			perror("ftruncate");
			
			return maybe<io>();
		}

		return maybe<io>(i);
	}

	template<typename T>
	maybe<io> fcopy(io&& i, const T& t)
	{
		memcpy(i._map.start + i._map.poff,&t,sizeof(T));
		
		return maybe<io>(i);
	}

	maybe<io> munmap(io&& i)
	{
		if (i._fd == -1) return maybe<io>();

		if (i._map.start != nullptr) {
		
			::munmap(i._map.start,i._map.poff + i._map.len);
		}

		return maybe<io>(i);
	}
	
	maybe<io> fclose(io&& i)
	{
		auto m = munmap(std::move(i));

		if (m) ::close(i._fd);
		
		return std::move(m);
	}
}
