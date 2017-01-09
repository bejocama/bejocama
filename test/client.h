#pragma once

#include <ostream>

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

	return os;
}

