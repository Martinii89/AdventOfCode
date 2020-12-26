// AdventOfCode_24.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stack>
#include <queue>
#include <memory>
#include <string_view>
#include <numeric>
#include <regex>
#include <list>

enum class HexDirection { E, SE, SW, W, NW, NE, ERROR };

HexDirection GetNextDirection(std::istringstream& stream)
{
	unsigned char next_char;
	stream >> next_char;
	switch (next_char)
	{
	case 'e':
		return HexDirection::E;
	case 'w':
		return HexDirection::W;
	case 'n':
		stream >> next_char;
		if (next_char == 'w')
			return HexDirection::NW;
		if (next_char == 'e')
			return HexDirection::NE;
		throw std::invalid_argument("invalid direction");
	case 's':
		stream >> next_char;
		if (next_char == 'w')
			return HexDirection::SW;
		if (next_char == 'e')
			return HexDirection::SE;
		throw std::invalid_argument("invalid direction");
	default: ;
		return HexDirection::ERROR;
	}
}

HexDirection GetNextDirection(std::string_view& view)
{
	if (view.rfind("nw", 0) == 0)
	{
		view = view.substr(2, view.size());
		return HexDirection::NW;
	}
	if (view.rfind("ne", 0) == 0)
	{
		view = view.substr(2, view.size());
		return HexDirection::NE;
	}

	if (view.rfind("sw", 0) == 0)
	{
		view = view.substr(2, view.size());
		return HexDirection::SW;
	}
	if (view.rfind("se", 0) == 0)
	{
		view = view.substr(2, view.size());
		return HexDirection::SE;
	}

	if (view.rfind('e', 0) == 0)
	{
		view = view.substr(1, view.size());
		return HexDirection::E;
	}
	if (view.rfind('w', 0) == 0)
	{
		view = view.substr(1, view.size());
		return HexDirection::W;
	}

	return  HexDirection::ERROR;
}


struct HexCoordinate
{
	int q = 0;
	int r = 0;


	friend bool operator<(const HexCoordinate& lhs, const HexCoordinate& rhs)
	{
		return std::tie(lhs.q, lhs.r) < std::tie(rhs.q, rhs.r);
	}

	friend bool operator<=(const HexCoordinate& lhs, const HexCoordinate& rhs)
	{
		return !(rhs < lhs);
	}

	friend bool operator>(const HexCoordinate& lhs, const HexCoordinate& rhs)
	{
		return rhs < lhs;
	}

	friend bool operator>=(const HexCoordinate& lhs, const HexCoordinate& rhs)
	{
		return !(lhs < rhs);
	}

	friend HexCoordinate operator+(const HexCoordinate& lhs, const HexCoordinate& rhs)
	{
		return {lhs.q + rhs.q, lhs.r + rhs.r};
	}

	HexCoordinate& operator+=(const HexCoordinate& rhs)
	{
		q += rhs.q;
		r += rhs.r;
		return *this;
	}


	static HexCoordinate DirectionOffset(const HexDirection& dir)
	{
		switch (dir)
		{
		case HexDirection::E:
			return {1, 0};
		case HexDirection::W:
			return {-1, 0};
		case HexDirection::SE:
			return {0, 1};
		case HexDirection::SW:
			return {-1, 1};
		case HexDirection::NW:
			return {0, -1};
		case HexDirection::NE:
			return {1, -1};
		}

		throw std::invalid_argument("invalid enum!");
	}

	void AddDirection(const HexDirection dir)
	{
		const auto offset = DirectionOffset(dir);
		q += offset.q;
		r += offset.r;
	}
};

struct HexMap
{
	std::map<HexCoordinate, bool> the_map;

	static inline std::vector<HexCoordinate> neighbour_offsets = {{1, 0},{-1, 0},{0, 1},{-1, 1},{0, -1},{1, -1}
	};

	void FillMissingNeighbours()
	{
		std::set<HexCoordinate> to_check;
		for(auto& [coord, _]: the_map)
		{
			for(auto& offset: neighbour_offsets)
			{
				auto offset_coord = coord + offset;
				to_check.insert(offset_coord);
			}
		}
		for(const auto& coord: to_check)
		{
			if (auto it = the_map.find(coord) == the_map.end())
			{
				//std::cout << "Filling: " << coord.q << "," << coord.r << std::endl;
				the_map.emplace(coord, false);
			}
		}

	}

	std::vector<std::pair<HexCoordinate&, bool>> GetNeighbours(const HexCoordinate& tile)
	{
		std::vector<std::pair<HexCoordinate&, bool>> res;
		for(auto& offset: neighbour_offsets)
		{
			auto offset_coord = tile + offset;
			res.emplace_back(offset_coord, the_map[offset_coord]);
		}
		return res;
	}

	long long CountWhiteNeighbours(const HexCoordinate& tile)
	{
		auto neighbours = GetNeighbours(tile);
		return std::count_if(neighbours.begin(), neighbours.end(), [](auto pair) { return pair.second == false; });
	}

	long long CountBlackNeighbours(const HexCoordinate& tile)
	{
		auto neighbours = GetNeighbours(tile);
		return std::count_if(neighbours.begin(), neighbours.end(), [](auto pair) { return pair.second == true; });
	}

	long long CountBlackTiles()
	{
		return std::count_if(the_map.begin(), the_map.end(), [](auto kv) { return kv.second == true; });
	}
};


int main()
{
	auto input = std::ifstream{"input.txt"};
	std::string line;
	HexMap map;
	while (std::getline(input, line))
	{
		HexCoordinate start{0, 0};
		std::string_view line_stream {line};
		while (!line_stream.empty())
		{
			auto new_dir = GetNextDirection(line_stream);
			const auto offset = HexCoordinate::DirectionOffset(new_dir);
			start += offset;
		}
		std::cout << "Done with a line\n";
		map.the_map[start] = !map.the_map[start];
	}

	std::cout << "Done reading input\n";

	for(auto& [coord, value]: map.the_map)
	{
		std::cout << coord.q << "," << coord.r << ":" << value << std::endl;
	}

	auto black_tiles = map.CountBlackTiles();
	auto next_map = map;
	next_map.FillMissingNeighbours();

	for (int i = 1; i <= 100; ++i)
	{
		map = next_map;
		std::vector<HexCoordinate> to_white;
		std::vector<HexCoordinate> to_black;
		for (auto& [coord, tile_is_black] : map.the_map)
		{
			if (tile_is_black)
			{
				auto black_neighbours = next_map.CountBlackNeighbours(coord);
				if (black_neighbours == 0 || black_neighbours > 2)
				{
					to_white.push_back(coord);
				}
			}
			else
			{
				auto black_neighbours = next_map.CountBlackNeighbours(coord);
				if (black_neighbours == 2)
				{
					to_black.push_back((coord));
				}
			}
		}
		for (auto& c : to_white)
		{
			//std::cout << c.q << ", " << c.r << ": will turn white\n";
			next_map.the_map[c] = false;
		}
		std::cout << std::endl;
		for (auto& c : to_black)
		{
			//std::cout << c.q << ", " << c.r << ": will turn black\n";
			next_map.the_map[c] = true;
		}
		black_tiles = next_map.CountBlackTiles();

		std::cout << "Day " << i << " Black tiles cont  is now: " << black_tiles << std::endl;
	}
}
