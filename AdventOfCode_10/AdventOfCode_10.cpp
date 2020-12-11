// AdventOfCode_10.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <stack>
#include <algorithm>
#include <map>

void CountPossibleArrangements(std::vector<int>& sorted_adapters)
{
	std::map<int, unsigned long long> adapter_graph;
	adapter_graph[0] = 1;
	for (auto jolt : sorted_adapters)
	{
		if (jolt == 0) continue;
		adapter_graph[jolt] = 0;
		for (size_t i = 3; i > 0; i--)
		{
			if (auto it = adapter_graph.find(jolt - i); it != adapter_graph.end())
			{
				adapter_graph[jolt] += it->second;
			}
		}
		std::cout << "number of possible paths to :" << jolt << " is " << adapter_graph[jolt] << std::endl;
	}
}

int main()
{
	auto input = std::ifstream{ "input.txt" };
	std::vector<int> adapters;
	adapters.push_back(0); // outlet
	int jolt;
	while (input >> jolt)
	{
		adapters.push_back(jolt);
	}
	std::sort(adapters.begin(), adapters.end());
	auto biggest = adapters.back();
	adapters.push_back(biggest + 3); // device
	std::vector<int> adapter_diff;
	for (size_t i = 0; i < adapters.size() -1; i++)
	{
		adapter_diff.push_back(adapters[i + 1] - adapters[i]);
	}
	auto ones = std::count_if(adapter_diff.begin(), adapter_diff.end(), [](auto a) {return a == 1; });
	auto threes = std::count_if(adapter_diff.begin(), adapter_diff.end(), [](auto a) {return a == 3; });
	auto product = ones * threes;
	std::cout << "The product of one and tree difference jolts is :" << product;
	CountPossibleArrangements(adapters);
}
