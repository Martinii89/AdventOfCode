// AdventOfCode_22.cpp : This file contains the 'main' function. Program execution begins and ends there.
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


struct Combat
{
	inline static int global_game_counter = 1;
	inline static int global_round_counter = 1;

	enum class Winner { P1, P2, UNDECIDED };

	int game_count = 1;
	int round_number = 1;
	Winner winner = Winner::UNDECIDED;

	std::deque<size_t> player_1_deck;
	std::deque<size_t> player_2_deck;

	static std::size_t deck_hash(const std::deque<size_t>& deck)
	{
		auto seed = deck.size();
		for (const auto& i : deck)
		{
			seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}

	std::map<size_t, size_t> state_hashes;

	void CheckWinner()
	{
		if (player_1_deck.empty())
		{
			winner = Winner::P2;
		}
		else if (player_2_deck.empty())
		{
			winner = Winner::P1;
		}
	}

	void PlayRound()
	{
		const auto p1_card = player_1_deck.front();
		player_1_deck.pop_front();
		const auto p2_card = player_2_deck.front();
		player_2_deck.pop_front();
		if (p1_card > p2_card)
		{
			player_1_deck.push_back(p1_card);
			player_1_deck.push_back(p2_card);
		}
		else
		{
			player_2_deck.push_back(p2_card);
			player_2_deck.push_back(p1_card);
		}
		round_number++;
		CheckWinner();
	}


	bool HasThisStateBeenBefore(const size_t p1_hash, const size_t p2_hash)
	{
		//const auto p1_hash = deck_hash(player_1_deck);
		if (const auto it = state_hashes.find(p1_hash); it != state_hashes.end())
		{
			const auto p2_hash = deck_hash(player_2_deck);
			return p2_hash == it->second; //.Equal(*this);
		}
		return false;
	}


	void PlayRecursiveRound()
	{
		// Check for infinite loops. give p1 the win if it is a loop
		const auto p1_hash = deck_hash(player_1_deck);
		const auto p2_hash = deck_hash(player_2_deck);
		if (HasThisStateBeenBefore(p1_hash, p2_hash))
		{
			winner = Winner::P1;
			return;
		}
		// store the state for later
		state_hashes.emplace(p1_hash, p2_hash);

		Winner round_winner = Winner::UNDECIDED;

		// Draw cards
		const auto p1_card = player_1_deck.front();
		player_1_deck.pop_front();
		const auto p2_card = player_2_deck.front();
		player_2_deck.pop_front();

		// Check for recursive game
		if (p1_card <= player_1_deck.size() && p2_card <= player_2_deck.size())
		{
			// play a recurse game
			Combat recurse_game;
			recurse_game.game_count = ++global_game_counter;
			recurse_game.player_1_deck = {player_1_deck.begin(), player_1_deck.begin() + p1_card};
			recurse_game.player_2_deck = {player_2_deck.begin(), player_2_deck.begin() + p2_card};

			// Play the recursive game until a winner is found
			while (recurse_game.winner == Winner::UNDECIDED)
			{
				//recurse_game.PrintRound();
				recurse_game.PlayRecursiveRound();
			}
			round_winner = recurse_game.winner;
		}
		else // Regular game
		{
			if (p1_card > p2_card)
			{
				round_winner = Winner::P1;
			}
			else
			{
				round_winner = Winner::P2;
			}
		}

		// Give the winner their cards
		if (round_winner == Winner::P1)
		{
			player_1_deck.push_back(p1_card);
			player_1_deck.push_back(p2_card);
		}
		else
		{
			player_2_deck.push_back(p2_card);
			player_2_deck.push_back(p1_card);
		}

		// Check for overall winner
		round_number++;
		global_round_counter++;
		CheckWinner();
	}

	void PrintRound()
	{
		std::cout << "Combat number: " << round_number << "( Game " << game_count << ")" << std::endl;
		std::cout << "Player 1 deck:";
		for (auto& n : player_1_deck)
		{
			std::cout << n << ",";
		}
		std::cout << std::endl;
		std::cout << "Player 2 deck:";
		for (auto& n : player_2_deck)
		{
			std::cout << n << ",";
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}
};


int main()
{
	auto input = std::ifstream{"input.txt"};
	Combat combat;
	std::string line;
	auto* deck = &combat.player_1_deck;
	while (std::getline(input, line))
	{
		if (line.find("Player") != std::string::npos)
		{
			continue;
		}
		if (line.empty())
		{
			deck = &combat.player_2_deck;
			continue;
		}
		deck->push_back(std::stoi(line));
	}

	while (combat.winner == Combat::Winner::UNDECIDED)
	{
		//combat.PrintRound();
		//combat.PlayRound();
		combat.PlayRecursiveRound();
	}

	auto* winner_deck = combat.player_1_deck.empty() ? &combat.player_2_deck : &combat.player_1_deck;
	size_t start_multiplier = winner_deck->size();
	size_t answer = 0;
	for (auto& i : *winner_deck)
	{
		answer += i * start_multiplier;
		start_multiplier--;
	}
	std::cout << "Winner found after: " << combat.round_number << " rounds in the main game" << std::endl;
	std::cout << "Total games (including recursive games): " << Combat::global_game_counter << std::endl;
	std::cout << "Total rounds (including recursive games): " << Combat::global_round_counter << std::endl;
	std::cout << "Deck value at end: " << answer << std::endl;

	std::cout << "Done\n";
}
