// AdventOfCode_05.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

struct Seat
{
	int row;
	int col;
	int seat_id;

	explicit Seat(const std::string& bsp)
	{
		int row_high = 127;
		int row_low = 0;
		int col_high = 7;
		int col_low = 0;
		for (auto c : bsp)
		{
			if (c == 'F')
			{
				row_high = (row_low + row_high) / 2;
			}
			else if (c == 'B')
			{
				row_low = std::ceil((row_low + row_high) / 2.f);
			}
			else if (c == 'L')
			{
				col_high = (col_low + col_high) / 2;
			}
			else if (c == 'R')
			{
				col_low = std::ceil((col_low + col_high) / 2.f);
			}
		}
		if (row_high == row_low)
		{
			row = row_high;
		}else
		{
			std::cout << "rows not equal?";
		}
		if (col_low == col_high)
		{
			col = col_low;
		}else
		{
			std::cout << "cols not equal?";
		}
		seat_id = GetSeatId();
	}

	int GetSeatId()
	{
		return row * 8 + col;
	}
};

int main()
{
	auto input = std::ifstream{"input.txt"};
	std::string line;
	int max_id = 0;
	std::set<int> seats_taken;
	
	while(std::getline(input, line))
	{
		Seat s{line};
		seats_taken.insert(s.seat_id);
		max_id = std::max(max_id, s.seat_id);
	}
	for(auto s: seats_taken)
	{
		if ((seats_taken.count(s+1) == 0))
		{
			std::cout << "the seat with ID " << s+1 << " is free\n";
			if (seats_taken.count(s+2))
			{
				std::cout << "Your seat has id: " << s+1 << std::endl;
			}
		}
	}
	std::cout << "Max ID: " << max_id << std::endl;
	
}
