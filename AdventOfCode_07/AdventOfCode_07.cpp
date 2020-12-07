// AdventOfCode_07.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <string_view>
#include <queue>

struct Bag
{
	std::string bag_name;
	std::map<std::string, int> content;

	Bag(const std::string& line)
	{
		const auto i = line.find(" bags contain ");
		bag_name = line.substr(0, i);
		std::string_view view = line;
		view.remove_prefix(i + 14);
		view.remove_suffix(1); //remove the .

		auto j = view.find("bag");

		while (j != std::string::npos)
		{
			if (view == "no other bags") break;
			auto content_part = view.substr(0, j - 1);
			const auto number_index = content_part.find(" ");
			std::string number_s{content_part.data(), number_index};
			const auto number = std::stoi(number_s);
			content_part.remove_prefix(number_index + 1);
			std::string bag_content_name{content_part.data(), content_part.size()};


			view.remove_prefix(j + 3);
			if (number > 1)
			{
				view.remove_prefix(1); // bags instead of bag
			}
			if (const auto comma = view.find(","); comma != std::string::npos)
			{
				view.remove_prefix(comma + 2);
			}
			j = view.find("bag");
			content[bag_content_name] = number;
		}
	}
};

int CountBagsInBag(const Bag& bag, std::map<std::string, Bag>& bag_graph)
{
	int count = 0;
	for (const auto& [name, sub_count] : bag.content)
	{
		if (auto bag_find = bag_graph.find(name); bag_find != bag_graph.end())
		{
			count += sub_count + sub_count * CountBagsInBag(bag_find->second, bag_graph);
		}
	}
	return count;
}

bool BagContainsBag(const Bag& bag, const std::string search, std::map<std::string, Bag>& bag_graph)
{
	for (const auto& [name, sub_count] : bag.content)
	{
		if (name == search)
		{
			return true;
		}
		if (auto bag_find = bag_graph.find(name); bag_find != bag_graph.end())
		{
			if (BagContainsBag(bag_find->second, search, bag_graph))
			{
				return true;
			}
		}
	}
	return false;
}

int main()
{
	std::map<std::string, Bag> bag_graph;
	auto input = std::ifstream{"input.txt"};
	std::string line;
	while (std::getline(input, line))
	{
		Bag bag{line};
		bag_graph.emplace(bag.bag_name, bag);
	}

	std::string target_bag = "shiny gold";
	int target_count = 0;

	for (auto& [name, bag] : bag_graph)
	{
		if (BagContainsBag(bag, target_bag, bag_graph))
		{
			target_count++;
		}
	}
	auto shiny_bagbag_count = 0;
	if (auto my_bag = bag_graph.find(target_bag); my_bag != bag_graph.end())
	{
		shiny_bagbag_count += CountBagsInBag(my_bag->second, bag_graph);
	}
	std::cout << "Outer bags that can contain the shiny gold is: " << target_count << std::endl;
	std::cout << "Shiny gold contains this many bags: " << shiny_bagbag_count << std::endl;
}
