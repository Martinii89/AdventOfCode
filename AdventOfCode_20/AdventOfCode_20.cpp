// AdventOfCode_20.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

struct Edge
{
	enum class EdgeCompareResult { MATCH, REVERSEMATCH, NOMATCH };

	std::vector<bool> edge;

	EdgeCompareResult CompareEdge(const Edge& other)
	{
		if (std::equal(edge.begin(), edge.end(), other.edge.begin()))
		{
			return EdgeCompareResult::MATCH;
		}
		if (std::equal(edge.begin(), edge.end(), other.edge.rbegin()))
		{
			return EdgeCompareResult::REVERSEMATCH;
		}

		return EdgeCompareResult::NOMATCH;
	}
};

struct Tile
{
private:
	// Used by GetTile when flipping the tile
	bool x_flipped = false;
	bool y_flipped = false;
public:
	size_t tile_id = 0;
	std::vector<bool> data;
	size_t x_size = 0;
	size_t y_size = 0;
	int x_coord = -1;
	int y_coord = -1;


	explicit Tile(std::ifstream& stream)
	{
		std::string line;
		while (std::getline(stream, line))
		{
			if (line.empty()) break;
			if (auto it = line.find("Tile") != std::string::npos)
			{
				auto id_part = line.substr(line.find(' '));
				const auto id = std::stoi(id_part.substr(0, id_part.size() - 1));
				tile_id = id;
			}
			else
			{
				y_size++;
				x_size = line.size();
				for (auto c : line)
				{
					switch (c)
					{
					case '#':
						data.emplace_back(true);
						break;
					case '.':
						data.emplace_back(false);
						break;
					default:
						std::cout << "ERROR. UNKNOWN CHAR\n";
					}
				}
			}
		}
	}

	explicit Tile(std::map<size_t, Tile>& oriented_tiles_with_border)
	{
		std::set<size_t> ind_cache;
		const auto tiles_count = oriented_tiles_with_border.size();
		const auto tile_size = oriented_tiles_with_border.begin()->second.x_size - 2;
		y_size = x_size = tile_size * std::sqrt(tiles_count);
		data = std::vector<bool>(tile_size * tile_size * tiles_count);
		for (auto& [_, tile] : oriented_tiles_with_border)
		{
			const auto x_tile = tile.x_coord;
			const auto y_tile = tile.y_coord;
			const auto this_x_start = tile_size * x_tile;
			const auto this_y_start = tile_size * y_tile;
			for (auto x = 1; x < tile.x_size - 1; ++x)
			{
				for (auto y = 1; y < tile.y_size - 1; ++y)
				{
					const auto big_ind = GetLinearIndex(this_x_start + x - 1, this_y_start + y - 1);
					if (ind_cache.count(big_ind) != 0)
					{
						throw std::logic_error("Something wrong here!");
					}
					ind_cache.insert(big_ind);
					data[big_ind] = tile.GetTile(x, y);
				}
			}
		}
	}

	explicit Tile(std::map<size_t, Tile>& oriented_tiles_with_border, bool keep_border)
	{
		std::set<size_t> ind_cache;
		const auto tiles_count = oriented_tiles_with_border.size();
		const auto tile_size = oriented_tiles_with_border.begin()->second.x_size;
		y_size = x_size = tile_size * std::sqrt(tiles_count);
		data = std::vector<bool>(tile_size * tile_size * tiles_count);
		for (auto& [_, tile] : oriented_tiles_with_border)
		{
			const auto x_tile = tile.x_coord;
			const auto y_tile = tile.y_coord;
			const auto this_x_start = tile_size * x_tile;
			const auto this_y_start = tile_size * y_tile;
			for (auto x = 0; x < tile.x_size; ++x)
			{
				for (auto y = 0; y < tile.y_size; ++y)
				{
					const auto big_ind = GetLinearIndex(this_x_start + x, this_y_start + y);
					if (ind_cache.count(big_ind) != 0)
					{
						throw std::logic_error("Something wrong here!");
					}
					ind_cache.insert(big_ind);
					data[big_ind] = tile.GetTile(x, y);
				}
			}
		}
	}

	void FlipY()
	{
		std::vector<bool> new_data;
		y_flipped = true;
		for (int y = 0; y < y_size; ++y)
		{
			for (int x = 0; x < x_size; ++x)
			{
				new_data.push_back(GetTile(x, y));
			}
		}
		y_flipped = false;
		data = new_data;
	}

	void FlipX()
	{
		std::vector<bool> new_data;
		x_flipped = true;
		for (int y = 0; y < y_size; ++y)
		{
			for (int x = 0; x < x_size; ++x)
			{
				new_data.push_back(GetTile(x, y));
			}
		}
		x_flipped = false;
		data = new_data;
	}

	void RotateLeft()
	{
		std::vector<bool> new_data(x_size * y_size);
		for (int y = 0; y < y_size; ++y)
		{
			for (int x = 0; x < x_size; ++x)
			{
				new_data[GetLinearIndex(x, y)] = data[GetLinearIndex(x_size - y - 1, x)];
			}
		}
		data = new_data;
	}

	void PrintTile()
	{
		std::cout << "Tile: " << tile_id << std::endl;
		for (int y = 0; y < y_size; ++y)
		{
			for (int x = 0; x < x_size; ++x)
			{
				std::cout << (GetTile(x, y) == 0 ? '.' : '#');
			}
			std::cout << std::endl;
		}
	}

	size_t GetLinearIndex(size_t x, size_t y)
	{
		return x_size * y + x;
	}

	std::tuple<size_t, size_t> LinearIndexToXY(size_t ind)
	{
		auto x = ind % x_size;
		auto y = (ind - x) / x_size;
		return {x, y};
	}

	bool GetTile(int x, int y)
	{
		size_t x_adjusted = x;
		if (x_flipped)
		{
			x_adjusted = (x_size - 1) - x;
		}
		size_t y_adjusted = y;
		if (y_flipped)
		{
			y_adjusted = (y_size - 1) - y;
		}
		const auto ind = GetLinearIndex(x_adjusted, y_adjusted);
		const auto t = data[ind];
		return t;
	}

	Edge GetRightEdge()
	{
		Edge right;
		for (int a = 0; a < y_size; ++a)
		{
			right.edge.push_back(GetTile(x_size - 1, a));
		}
		return right;
	}

	Edge GetBottomEdge()
	{
		Edge bottom;
		for (int a = 0; a < x_size; ++a)
		{
			bottom.edge.push_back(GetTile(a, y_size - 1));
		}
		return bottom;
	}

	Edge GetLeftEdge()
	{
		Edge left;
		for (int a = 0; a < y_size; ++a)
		{
			left.edge.push_back(GetTile(0, a));
		}

		return left;
	}

	Edge GetTopEdge()
	{
		Edge top;
		for (int a = 0; a < x_size; ++a)
		{
			top.edge.push_back(GetTile(a, 0));
		}
		return top;
	}


	std::vector<Edge> GetEdges()
	{
		Edge left;
		Edge right;
		Edge top;
		Edge bottom;
		for (int a = 0; a < y_size; ++a)
		{
			left.edge.push_back(GetTile(0, a));
			right.edge.push_back(GetTile(x_size - 1, a));
		}
		for (int a = 0; a < x_size; ++a)
		{
			top.edge.push_back(GetTile(a, 0));
			bottom.edge.push_back(GetTile(a, y_size - 1));
		}

		return {left, right, top, bottom};
	}
};


// Assumption The other tile should orient itself to match up with either the right or bottom edge of ref
enum class OrientEdge { LEFT, RIGHT, TOP, BOTTOM };

OrientEdge GetEdgeToOrient(Tile& ref, Tile& other)
{
	auto other_edges = other.GetEdges();
	auto ref_right = ref.GetRightEdge();
	auto ref_bottom = ref.GetBottomEdge();
	for (auto& edge : other_edges)
	{
		if (ref_right.CompareEdge(edge) != Edge::EdgeCompareResult::NOMATCH)
		{
			return OrientEdge::RIGHT;
		}
		if (ref_bottom.CompareEdge(edge) != Edge::EdgeCompareResult::NOMATCH)
		{
			return OrientEdge::BOTTOM;
		}
	}

	throw std::logic_error("no matching edge?!");
}

void Orient(Tile& ref, Tile& other)
{
	const auto orient_mode = GetEdgeToOrient(ref, other);

	switch (orient_mode)
	{
	case OrientEdge::LEFT:
		{
			throw std::logic_error("should not need to orient against this edge");
		}
	case OrientEdge::RIGHT:
		{
			auto ref_edge = ref.GetRightEdge();
			auto match_res = ref_edge.CompareEdge(other.GetLeftEdge());
			while (match_res == Edge::EdgeCompareResult::NOMATCH)
			{
				other.RotateLeft();
				match_res = ref_edge.CompareEdge(other.GetLeftEdge());
			}
			if (match_res == Edge::EdgeCompareResult::REVERSEMATCH)
			{
				other.FlipY();
				match_res = ref_edge.CompareEdge(other.GetLeftEdge());
				if (match_res != Edge::EdgeCompareResult::MATCH)
				{
					throw std::logic_error("Wrong flip!");
				}
			}
			other.x_coord = ref.x_coord + 1;
			other.y_coord = ref.y_coord;
			break;
		}
	case OrientEdge::TOP:
		{
			throw std::logic_error("should not need to orient against this edge");
		}
	case OrientEdge::BOTTOM:
		{
			auto ref_edge = ref.GetBottomEdge();
			auto match_res = ref_edge.CompareEdge(other.GetTopEdge());
			while (match_res == Edge::EdgeCompareResult::NOMATCH)
			{
				other.RotateLeft();
				match_res = ref_edge.CompareEdge(other.GetTopEdge());
			}
			if (match_res == Edge::EdgeCompareResult::REVERSEMATCH)
			{
				other.FlipX();
				match_res = ref_edge.CompareEdge(other.GetTopEdge());
				if (match_res != Edge::EdgeCompareResult::MATCH)
				{
					throw std::logic_error("Wrong flip!");
				}
			}
			other.x_coord = ref.x_coord;
			other.y_coord = ref.y_coord + 1;
			break;
		}
	}
}


void OrientTopRightCorner(Tile& corner, Tile& edge1, Tile& edge2)
{
	auto get_edge_matches = [&](Tile& edge_tile, Tile& edge_tile2)
	{
		auto corner_edges = corner.GetEdges();
		auto edge_edges = edge_tile.GetEdges();
		auto edge2_edges = edge_tile2.GetEdges();
		std::vector<bool> edge_matches(4);
		for (int i = 0; i < corner_edges.size(); ++i)
		{
			for (auto& edge : edge_edges)
			{
				if (corner_edges[i].CompareEdge(edge) != Edge::EdgeCompareResult::NOMATCH)
				{
					edge_matches[i] = true;
					break;
				}
			}
			for (auto& edge : edge2_edges)
			{
				if (corner_edges[i].CompareEdge(edge) != Edge::EdgeCompareResult::NOMATCH)
				{
					if (edge_matches[i] == true)
					{
						throw std::logic_error("The two tiles should not match with the same edge");
					}
					edge_matches[i] = true;
					break;
				}
			}
		}
		return edge_matches;
	};

	// edge order is: left, right, top, bottom
	// the goal is to have a match for bottom and right
	auto edge_matches = get_edge_matches(edge1, edge2);
	while (edge_matches[1] == false || edge_matches[3] == false)
	{
		corner.RotateLeft();
		edge_matches = get_edge_matches(edge1, edge2);
	}
	corner.x_coord = 0;
	corner.y_coord = 0;
}


size_t GetOrientRefID(size_t tile_id, std::map<size_t, std::set<size_t>>& adj_map,
                      const std::set<size_t>& oriented_tiles)
{
	if (auto it = adj_map.find(tile_id); it != adj_map.end())
	{
		for (auto i : it->second)
		{
			if (oriented_tiles.count(i) > 0)
			{
				return i;
			}
		}
	}
	throw std::runtime_error("Failed to find a tile to use for orientation reference");
}

int SearchForMonsters(Tile& tile)
{
	std::vector<bool> monster_row_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};
	std::vector<bool> monster_row_2 = {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1};
	std::vector<bool> monster_row_3 = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0};
	const auto padding_between_each_row = tile.x_size - monster_row_1.size();
	std::vector<bool> padding(padding_between_each_row);
	std::vector<bool> monster_pattern;
	monster_pattern.insert(monster_pattern.end(), monster_row_1.begin(), monster_row_1.end());
	monster_pattern.insert(monster_pattern.end(), padding.begin(), padding.end());

	monster_pattern.insert(monster_pattern.end(), monster_row_2.begin(), monster_row_2.end());
	monster_pattern.insert(monster_pattern.end(), padding.begin(), padding.end());

	monster_pattern.insert(monster_pattern.end(), monster_row_3.begin(), monster_row_3.end());

	auto find_monster = [&](auto start)
	{
		return std::search(start, tile.data.end(), monster_pattern.begin(), monster_pattern.end(),
		                   [](auto data, auto pattern)
		                   {
			                   if (!pattern) return true;
			                   return pattern == data;
		                   });
	};

	int monster_found = 0;
	auto res = find_monster(tile.data.begin());
	while (res != tile.data.end())
	{
		monster_found++;
		auto ind = std::distance(tile.data.begin(), res);
		auto [x,y] = tile.LinearIndexToXY(ind);
		std::cout << "Found a monster at index: " << ind << "[" << x << "," << y << "]\n";
		res = find_monster(++res);
	}

	std::cout << "Found " << monster_found << " monsters\n";
	return monster_found;
}


int main()
{
	auto input = std::ifstream{"input.txt"};
	std::map<size_t, Tile> tiles;
	while (input)
	{
		Tile tile{input};
		tiles.emplace(tile.tile_id, tile);
	}

	std::map<size_t, std::set<size_t>> adj_map;
	std::set<size_t> visited;

	// Build the adjaceny map for the tiles
	for (auto& [id, tile] : tiles)
	{
		auto tile_edges = tile.GetEdges();
		visited.insert(tile.tile_id);
		for (auto& [sub_id, sub_tile] : tiles)
		{
			if (visited.count(sub_id) > 0) continue;
			auto sub_edges = sub_tile.GetEdges();
			for (auto& edge : tile_edges)
			{
				for (auto& sub_edge : sub_edges)
				{
					const auto comp = edge.CompareEdge(sub_edge);
					if (comp == Edge::EdgeCompareResult::MATCH || comp == Edge::EdgeCompareResult::REVERSEMATCH)
					{
						adj_map[tile.tile_id].insert(sub_tile.tile_id);
						adj_map[sub_tile.tile_id].insert(tile.tile_id);
						break;
					}
				}
			}
		}
	}
	

	// Place and orient a corner (the first corner can be picked at random)
	auto a_corner = std::find_if(adj_map.begin(), adj_map.end(), [](auto kv) { return kv.second.size() == 2; });
	auto& top_right = tiles.at(a_corner->first);
	auto& corner_adj = adj_map.at(a_corner->first);
	std::vector<size_t> corner_adj_vec {corner_adj.begin(), corner_adj.end()};
	if (corner_adj_vec.size() != 2)
	{
		throw std::logic_error("Should be only 2 adj tiles here");
	}
	OrientTopRightCorner(top_right, tiles.at(corner_adj_vec[0]), tiles.at(corner_adj_vec[1]));


	std::set<size_t> tiles_already_oriented;
	tiles_already_oriented.insert(top_right.tile_id);
	std::queue<size_t> orient_queue ({corner_adj_vec.begin(), corner_adj_vec.end()});

	while (!orient_queue.empty())
	{
		auto tile_to_orient_id = orient_queue.front();
		orient_queue.pop();
		if (tiles_already_oriented.count(tile_to_orient_id) != 0)
		{
			continue;
		}
		auto ref_id = GetOrientRefID(tile_to_orient_id, adj_map, tiles_already_oriented);
		auto& ref_tile = tiles.at(ref_id);
		auto& tile_to_orient = tiles.at(tile_to_orient_id);
		Orient(ref_tile, tile_to_orient);
		//std::cout << "Used tile at position [" << ref_tile.x_coord << "," << ref_tile.y_coord << "]. ";
		//std::cout << "To orient and place tile (" << tile_to_orient_id << ") at position [" << tile_to_orient.x_coord <<
		//	"," << tile_to_orient.y_coord << "]\n";
		tiles_already_oriented.insert(tile_to_orient_id);
		auto& this_adj_tiles = adj_map.at(tile_to_orient_id);
		for (auto id : this_adj_tiles)
		{
			if (tiles_already_oriented.count(id) == 0)
			{
				orient_queue.push(id);
			}
		}
	}

	//Tile big_tile_with_border{tiles, true};
	//big_tile_with_border.PrintTile();

	Tile big_tile{tiles};
	Tile big_tile2{tiles};
	big_tile2.FlipX();
	auto monster_count = 0;
	for (int i = 0; i < 4; ++i)
	{
		monster_count += SearchForMonsters(big_tile);
		monster_count += SearchForMonsters(big_tile2);
		big_tile.RotateLeft();
		big_tile2.RotateLeft();
		if (monster_count != 0)
		{
			break;
		}
	}
	auto monster_dots = monster_count * 15;
	auto true_dots = std::count_if(big_tile.data.begin(), big_tile.data.end(), [](auto a) { return a == true; });
	auto answer = true_dots - monster_dots;
	std::cout << "Answer: " << answer << std::endl;

	// 2016 too high


	std::cout << "DONE?!";
	return 0;
}
