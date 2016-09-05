
#  This file is part of BEJOCAMA software
#  Copyright 2016 Matthias Wolff <matthias.wolff@bejocama.de>
#
#  This work is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation;
#  either version 3 of the License, or any later version.
#
#  This work is distributed in the hope that it will be useful,
#  but without any warranty; without even the implied warranty
#  of merchantability or fitness for a particular purpose. See
#  the GNU Lesser General Public License for more details. You
#  should have received a copy of the GNU Lesser General Public
#  License along with this library.
#

CPP := main.cpp

TEST_CPP := $(notdir $(wildcard test/*.cpp))

TEST_OBJ := $(patsubst %.cpp, %.o, $(TEST_CPP))

INCLUDE := -Iinclude -Itest

VPATH := test

.PHONY: program test

program: $(TEST_OBJ)
	g++ -g -std=c++14 -o $@ $(CPP) $(INCLUDE) -lpthread

test:
	./program

clean:
	rm -f program *.data

#compile tests -- objects not needed
%.o: %.cpp
	g++ -std=c++14 -c $< $(INCLUDE) -o /dev/null
