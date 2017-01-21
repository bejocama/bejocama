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
#include "bejocama/interface/file.h"
#include "bejocama/interface/list.h"
#include "client.h"

void test3()
{
	auto ll = bejocama::list<client>()->add(client{"tom", "orlando", .age=20,.height=178});
}
