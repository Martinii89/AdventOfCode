// AdventOfCode_23.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

struct Cup
{
	int value;
	bool in_the_ring = true;
	Cup* next;
	Cup* prev;

	void SetNext(Cup* new_next)
	{
		new_next->prev = this;
		next = new_next;
	}

	void RemoveFromChain()
	{
		// prev <-> this <-> next
		// prev     <->      next
		prev->next = next;
		next->prev = prev;
		next = nullptr;
		prev = nullptr;
	}

	void RemoveThisAndNextNextFromChain()
	{
		// prev <-> this <-> next1 <-> next2 <-> next3
		// prev <-> next3
		// this <-> next1 <-> next2
		prev->SetNext(next->next->next);

		prev = nullptr;
		next->next->next = nullptr;

		in_the_ring = false;
		next->in_the_ring = false;
		next->next->in_the_ring = false;
	}

	void SpliceIntoChain(Cup* start_of_splice_chain)
	{
		auto* old_next = next;
		auto* end_of_new_chain = start_of_splice_chain;
		start_of_splice_chain->in_the_ring = true;
		while (end_of_new_chain->next)
		{
			end_of_new_chain = end_of_new_chain->next;
			end_of_new_chain->in_the_ring = true;
		}
		SetNext(start_of_splice_chain);
		end_of_new_chain->SetNext(old_next);
	}
};

struct Cups2
{
	std::map<int, Cup*> the_cup_ring;
	std::vector<Cup> cups; // Where all the pointers points to. don't touch this after the initial setup.
	std::vector<Cup*> sorted_cups;
	Cup* current_cup = nullptr;

	int round = 1;

	void InitNextPrev()
	{
		for (int i = 0; i < cups.size() - 1; ++i)
		{
			cups[i].SetNext(&cups[i + 1]);
		}
		cups[cups.size() - 1].SetNext(&cups[0]);
		for (auto& cup : cups)
		{
			the_cup_ring[cup.value] = &cup;
		}
		current_cup = &cups[0];
		for (auto& cup : cups)
		{
			sorted_cups.push_back(&cup);
		}
		// for fast FindCupSearches
		std::sort(sorted_cups.begin(), sorted_cups.end(), [](Cup* a, Cup* b) { return a->value < b->value; });
	}

	void AddCup(int cup_value)
	{
		cups.push_back(Cup{cup_value, true, nullptr, nullptr});
	}

	Cup* FindCupInRing(int cup_value)
	{
		auto* cup = sorted_cups[cup_value - 1];
		if (cup->value != cup_value)
		{
			throw std::logic_error("Wrong cup!");
		}
		return cup->in_the_ring ? cup : nullptr;
	}

	void PrintChain()
	{
		std::cout << "Round " << round << "\t\t";
		auto* start = FindCupInRing(1);
		auto* current = start->next;
		std::cout << start->value << " ";
		while (current != start)
		{
			if (current->value == current_cup->value)
			{
				std::cout << "(" << current->value << ") ";
			}
			else
			{
				std::cout << current->value << " ";
			}

			current = current->next;
		}
		std::cout << std::endl;
	}

	Cup* FindDestinationCup(const int current_cup_value)
	{
		auto dest_value = current_cup_value - 1;
		if (dest_value < 1)
		{
			dest_value = (*sorted_cups.rbegin())->value;
		}
		auto* cup = FindCupInRing(dest_value);
		while (cup == nullptr)
		{
			dest_value--;
			if (dest_value < 1)
			{
				dest_value = cups.rbegin()->value;
			}
			cup = FindCupInRing(dest_value);
		}
		return cup;
	}

	void PlayRound()
	{
		auto* to_remove = current_cup->next;
		to_remove->RemoveThisAndNextNextFromChain();
		auto* dest = FindDestinationCup(current_cup->value);
		dest->SpliceIntoChain(to_remove);
		current_cup = current_cup->next;
		round++;
	}
};

int main()
{
	Cups2 cups2;

	std::vector<int> input{3, 9, 8, 2, 5, 4, 7, 1, 6}; // input
	std::vector<int> test{3, 8, 9, 1, 2, 5, 4, 6, 7}; // test
	for (auto i : input)
	{
		cups2.AddCup(i);
	}

	// Add a stupid amount of cups
	for (int i = 10; i <= 1000000; ++i)
	{
		cups2.AddCup(i);
	}
	cups2.InitNextPrev();
	std::cout << "Done initializing the caches\n";

	//// part 1 moves: 100
	//// part 2 moves: 10 000 000
	int rounds = 10000000;
	auto ten_perc = rounds / 10;

	for (int i = 0; i < rounds; ++i)
	{
		if (i == ten_perc)
		{
			ten_perc += rounds / 10;
			std::cout << "10% done\n";
		}
		cups2.PlayRound();
	}

	auto one = cups2.FindCupInRing(1);
	auto next = one->next;
	auto nextnext = one->next->next;

	std::cout << next->value << " * " << nextnext->value << "=" << (int64_t)next->value * nextnext->value << std::endl;

	std::cout << "Done playing\n";
}
