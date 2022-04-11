#pragma once

#include "ofMain.h"

struct Coord
{
	int x;
	int y;

	bool operator==(const Coord &other) const
	{
		return (x == other.x
			&& y == other.y);
	}
}; 

namespace std {

	template <>
	struct hash<Coord>
	{
		std::size_t operator()(const Coord& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			size_t h1 = std::hash<int>()(k.x);
			size_t h2 = std::hash<int>()(k.y);
			return (h1 ^ (h2 << 1));
		}
	};

}
