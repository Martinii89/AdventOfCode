// AdventOfCode_03.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

enum class MapTile
{
    OPEN, TREE
};

struct Slope
{
	int dx = 0;
	int dy = 0;
};

struct Map
{
	std::vector<MapTile> map;
	int rows = 0;
	int cols = 0;

	int TestSlope(const Slope& slope)
	{
		auto dx = slope.dx;
		auto dy = slope.dy;
		auto x = 0;
		auto y = 0;
		auto tree_hits = 0;
		while (y < rows)
		{
			auto index = cols * y + x;
			if (map[index] == MapTile::TREE)
			{
				tree_hits++;
			}
			x = (x + dx) % cols;
			y += dy;
		}
		return tree_hits;
	}
};







int main()
{
	Map map;
	auto input = std::ifstream{ "input.txt" };
	std::string line;
	while (std::getline(input, line))
	{
		int cols = 0;
		for (auto c : line)
		{
			if (c == '.')
			{
				map.map.emplace_back(MapTile::OPEN);
				cols++;
			}
			else if (c == '#')
			{
				map.map.emplace_back(MapTile::TREE);
				cols++;
			}
			else {
				std::cout << "Unknown char? " << c << std::endl;
			}
		}
		map.cols = cols;
		map.rows++;
	}
	std::vector<Slope> slopes_to_test{
		{1,1}, {3,1}, {5,1}, {7,1}, {1,2}
	};
	std::vector<int> slope_results;
	unsigned long long tree_product = 1;
	for (auto& slope : slopes_to_test)
	{
		auto res = map.TestSlope(slope);
		tree_product *= res;
		slope_results.push_back(res);
		std::cout << "The slope " << slope.dx << "," << slope.dy << " hits " << res << " trees" << std::endl;
	}
	std::cout << "The total product is " << tree_product << std::endl;
}
