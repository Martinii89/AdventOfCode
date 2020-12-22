// AdventOfCode_17.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <set>

struct Point
{
	int64_t x = 0, y = 0, z = 0;
};

struct HyperSpace
{
	// The space is infinite, but I can probably guess how large it can grow and account for that. if not a octtree copypasta is required..
	int64_t x_size;
	int64_t y_size;
	int64_t z_size;
	int64_t w_size;
	int64_t size_offset;

	//std::unordered_map<int64_t, bool> active_map; // default value is false
	std::vector<bool> active_map;

	HyperSpace(const size_t x, const size_t y, const size_t z, const size_t w): x_size(x), y_size(y), z_size(z), w_size(w), size_offset(x / 2)
	{
		active_map = std::vector<bool>(x*y*z*w, false);
	}

	// Offset every index so (0,0,0) becomes (size_offset, size_offset, size_offset)
	int64_t ToLinearIndex(int x, int y, int z, int w) const
	{
		//return (x_size * y_size * (z + size_offset)) + (x_size * (y + size_offset)) + (x + size_offset);
		return (x_size * y_size * z_size * (w + size_offset)) + (x_size * y_size * (z + size_offset)) + (x_size * (y +
			size_offset)) + (x + size_offset);
	}

	Point FromLinearIndex(int64_t indx) const
	{
		const auto x = indx % x_size;
		indx -= x;
		indx /= x_size;
		const auto y = indx % y_size;
		indx -= y;
		const auto z = indx / y_size;
		return {x - size_offset, y - size_offset, z - size_offset};
	}

	bool GetState(int x, int y, int z, int w) const
	{
		const auto ind = ToLinearIndex(x, y, z, w);
		return active_map[ind];
	}

	void SetState(const int x, const int y, const int z, const int w, const bool value)
	{
		active_map[ToLinearIndex(x, y, z, w)] = value;
	}

	int CountNeighbours(const int x, const int y, const int z, const int w) const
	{
		auto result = 0;
		for (auto i = -1; i < 2; i++)
		{
			for (auto j = -1; j < 2; j++)
			{
				for (auto k = -1; k < 2; k++)
				{
					for (auto l = -1; l < 2; ++l)
					{
						if (i == 0 && j == 0 && k == 0 && l == 0) continue;
						result += GetState(x + i, y + j, z + k, w + l);
					}
				}
			}
		}
		return result;
	}
};


void CountActiveCubes(HyperSpace& space)
{
	const auto active_count = std::count(space.active_map.begin(), space.active_map.end(), true);
	std::cout << "Active Cubes:" << active_count << " active cubes\n";
}

void PrintLayer(HyperSpace& space, const int layer, const int start, const int end)
{
	std::cout << "Z=" << layer << "\n";
	for (auto y = start; y < end; y++)
	{
		for (auto x = start; x < end; x++)
		{
			std::cout << (space.GetState(x, y, layer, 0) == true ? '#' : '.');
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

int main()
{
	HyperSpace space{21, 21, 21, 21};

	auto ind = space.ToLinearIndex(0,0,0,0);
	

	auto input = std::ifstream{"input.txt"};
	std::string line;
	auto line_count = 0;
	while (std::getline(input, line))
	{
		for (size_t i = 0; i < line.size(); i++)
		{
			auto c = line[i];
			if (c == '#')
			{
				space.SetState(i, line_count, 0, 0, true);
			}
			else if (c == '.')
			{
				space.SetState(i, line_count, 0, 0, false);
			}
		}
		line_count++;
	}
	CountActiveCubes(space);

	auto next = space;
	PrintLayer(space, 0, 0, 3);

	auto start_size = 8;
	auto iteratons_to_do = 6;
	for (auto iteration = 1; iteration <= iteratons_to_do; iteration++)
	{
		auto layer_size = start_size + iteration;
		for (auto w = -iteration; w <= iteration; ++w)
		{
			for (auto z = -iteration; z <= iteration; z++)
			{
				for (auto y = -iteration; y < layer_size; y++)
				{
					for (auto x = -iteration; x < layer_size; x++)
					{
						auto n = space.CountNeighbours(x, y, z, w);
						auto active = space.GetState(x, y, z, w);
						if (active)
						{
							if (n == 2 || n == 3)
							{
								next.SetState(x, y, z, w, true);
							}
							else
							{
								next.SetState(x, y, z, w, false);
							}
						}
						else
						{
							if (n == 3)
							{
								next.SetState(x, y, z, w, true);
							}
							else
							{
								next.SetState(x, y, z, w, false);
							}
						}
					}
				}
			}
		}

		CountActiveCubes(next);
		space = next;
	}

	std::cout << "Done\n";
}
