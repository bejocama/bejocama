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

#include "bejocama/composition.h"
#include "bejocama/internal.h"
#include "bejocama/file.h"

namespace bejocama
{

	template<typename T>
	void print_file(const char* fn)
	{
		composer(fopen,fstat,mmap<T>,make_file<T>(),&file<T>::make_list,print<T>())(fn,0,0);
	}
	

	template<typename T>
	void save(const char* fn, T&& t)
	{
		composer(fopen,fstat,ftruncate<T>,fstat,mmap<T>,fcopy<T>,fclose)
			(fn,1,-1,1,std::forward<T>(t));
	}
}

struct client
{
	char name[256];
	char city[256];
	int age;
	int height;
};

std::ostream& operator<<(std::ostream& os, const client& c)
{
	os << "name: " << c.name << std::endl;
}

int main(int argc, char** argv)
{
	bejocama::save<client>("client.data",client{"tom", "orlando", .age=20,.height=178});

	bejocama::print_file<client>("client.data");
	
	return 0;
}

