// AdventOfCode_15.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

struct Number
{
	int last_round = 0;
	int second_to_last_round = 0;
	int count = 0;

	void ItHappenedAgain(const int round)
	{
		count++;
		second_to_last_round = last_round;
		last_round = round;
	}
};


struct Game
{
	std::unordered_map<unsigned int, Number> round_cache;
	int round_counter = 0;
	int previous_number = -1;

	int FindAgeOfNumber(const int number)
	{
		if (const auto it = round_cache.find(number); it != round_cache.end())
		{
			if (it->second.count == 1)
			{
				// only happened once
				return 0;
			}
			const auto diff_between_last_and_second_to_last = it->second.last_round - it->second.second_to_last_round;
			return diff_between_last_and_second_to_last;
		}

		// This number hasn't happened yet. This should not happen!
		throw std::runtime_error("requested the age of a number that isn't in the cache!");
	}

	void CalcNextRound()
	{
		const auto prev = previous_number;
		const auto previous_age = FindAgeOfNumber(prev);
		round_cache[previous_age].ItHappenedAgain(round_counter);
		round_counter++;
		previous_number = previous_age;
	}
};

int main()
{
	Game game;
	std::vector<int> start = {9, 6, 0, 10, 18, 2, 1};
	//std::vector<int> start = { 3,1,2 };
	for (size_t i = 0; i < start.size(); i++)
	{
		auto& n = game.round_cache[start[i]];
		n.ItHappenedAgain(i);
		game.round_counter++;
		game.previous_number = start[i];
	}
	while (game.round_counter < 2020)
	{
		game.CalcNextRound();
	}
	std::cout << "Last round number is: " << game.previous_number << std::endl;

	while (game.round_counter < 30000000)
	{
		game.CalcNextRound();
	}
	std::cout << "Last round number is: " << game.previous_number << std::endl;
}
