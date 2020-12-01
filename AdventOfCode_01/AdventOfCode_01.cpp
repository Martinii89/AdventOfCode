// AdventOfCode_01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <tuple>
#include <set>


std::tuple<bool, int> find_sum_pair(const int n, const int goal, std::set<int>& options)
{
	auto target_diff = goal - n;
	if (options.count(target_diff) > 0)
	{
		return {true, target_diff};
	}
	return {false, -1};
}

void SolvePart2(const int target, std::set<int> input_numbers)
{
	for(auto i: input_numbers)
	{
		const auto this_target = target - i;
		for (auto j: input_numbers)
		{
			if (i == j) continue; // cba to write a proper nested loop that doesn't repeat.
			const auto [found, answer] = find_sum_pair(j, this_target, input_numbers);
			if (found)
			{
				std::cout << "Solution 2:" << i * j * answer << std::endl;
				return;
			}
		}
	}
}

void SolvePart1(const int target, std::set<int> input_numbers)
{
	for(auto i: input_numbers)
	{
		const auto [found, answer] = find_sum_pair(i, target, input_numbers);
		if (found)
		{
			std::cout << "Solution 1: " << answer * i << std::endl;
			return;
		}
	}
}

std::set<int> ReadInputUnique(const int max_value)
{
	std::set<int> input_numbers {};
	auto input = std::ifstream{"input.txt"};
	int n;
	while (input >> n)
	{
		if (n <= max_value)
		{
			input_numbers.insert(n);
		}
	}
	return input_numbers;
}

int main()
{
	const auto target = 2020;
	const auto input_numbers = ReadInputUnique(target);
	SolvePart1(target, input_numbers);
	SolvePart2(target, input_numbers);
}
