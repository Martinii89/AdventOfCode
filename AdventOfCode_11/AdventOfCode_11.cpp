// AdventOfCode_11.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

enum class SeatState
{
	EMPTY,
	TAKEN,
	FLOOR,
	VOID
};

struct SeatingChart
{
	std::vector<SeatState> next_chart;
	std::vector<SeatState> chart;
	int rows = 0;
	int cols = -1;

	int state_changes = 0;

	static SeatState CharToState(const char c)
	{
		if (c == '#')
		{
			return SeatState::TAKEN;
		}
		if (c == '.')
		{
			return SeatState::FLOOR;
		}
		if (c == 'L')
		{
			return SeatState::EMPTY;
		}
		throw std::exception("invalid seat state");
	}

	SeatState GetSeatState(const int row, const int col)
	{
		if (row < 0 || row >= rows || col < 0 || col >= cols)
		{
			//throw std::exception("Invalid index!");
			return SeatState::VOID;
		}
		const auto index = row * cols + col;
		if (index >= chart.size())
		{
			throw std::exception("Invalid index!");
		}
		return chart[index];
	}

	int CountNeighbouringSeatTaken(const int row, const int col)
	{
		auto count = 0;
		count += GetSeatState(row - 1, col - 1) == SeatState::TAKEN;
		count += GetSeatState(row - 1, col) == SeatState::TAKEN;
		count += GetSeatState(row - 1, col + 1) == SeatState::TAKEN;

		count += GetSeatState(row, col - 1) == SeatState::TAKEN;
		count += GetSeatState(row, col + 1) == SeatState::TAKEN;

		count += GetSeatState(row + 1, col - 1) == SeatState::TAKEN;
		count += GetSeatState(row + 1, col) == SeatState::TAKEN;
		count += GetSeatState(row + 1, col + 1) == SeatState::TAKEN;
		return count;
	}

	bool IsTakenSeatVisible(int row, int col, const int d_row, const int d_col)
	{
		row += d_row;
		col += d_col;
		auto seat = GetSeatState(row, col);
		while (seat != SeatState::VOID)
		{
			if (seat == SeatState::TAKEN) return true;
			if (seat == SeatState::EMPTY) return false;
			row += d_row;
			col += d_col;
			seat = GetSeatState(row, col);
		}
		return false;
	}

	int CountVisibleSeatsTaken(const int row, const int col)
	{
		auto count = 0;
		count += IsTakenSeatVisible(row, col, -1, -1);
		count += IsTakenSeatVisible(row, col, -1, 0);
		count += IsTakenSeatVisible(row, col, -1, 1);

		count += IsTakenSeatVisible(row, col, 0, -1);
		count += IsTakenSeatVisible(row, col, 0, 1);

		count += IsTakenSeatVisible(row, col, 1, -1);
		count += IsTakenSeatVisible(row, col, 1, 0);
		count += IsTakenSeatVisible(row, col, 1, 1);
		return count;
	}

	SeatState GetNextState(const int row, const int col)
	{
		const auto current = GetSeatState(row, col);
		switch (current)
		{
		case SeatState::EMPTY:
			{
				if (CountNeighbouringSeatTaken(row, col) == 0)
				{
					state_changes++;
					return SeatState::TAKEN;
				}
				return SeatState::EMPTY;
			}
		case SeatState::TAKEN:
			{
				if (CountNeighbouringSeatTaken(row, col) >= 4)
				{
					state_changes++;
					return SeatState::EMPTY;
				}
				return SeatState::TAKEN;
			}
		case SeatState::FLOOR:
			return SeatState::FLOOR;
		case SeatState::VOID:
		default:
			return SeatState::VOID;
		}
	}

	SeatState GetNextState2(const int row, const int col)
	{
		const auto current = GetSeatState(row, col);
		switch (current)
		{
		case SeatState::EMPTY:
			{
				if (CountVisibleSeatsTaken(row, col) == 0)
				{
					state_changes++;
					return SeatState::TAKEN;
				}
				return SeatState::EMPTY;
			}
		case SeatState::TAKEN:
			{
				if (CountVisibleSeatsTaken(row, col) >= 5)
				{
					state_changes++;
					return SeatState::EMPTY;
				}
				return SeatState::TAKEN;
			}
		case SeatState::FLOOR:
			return SeatState::FLOOR;
		case SeatState::VOID:
		default:
			throw std::exception("invalid state");
			//return SeatState::VOID;
		}
	}

	void AddRow(const std::string& row)
	{
		if (row.empty())
		{
			return;
		}
		rows++;
		auto row_cols = 0;
		for (auto c : row)
		{
			row_cols++;
			chart.push_back(CharToState(c));
		}
		if (cols == -1)
		{
			cols = row_cols;
		}
		if (cols != row_cols)
		{
			throw std::exception("column count does not match previous rows!");
		}
	}
};


int main()
{
	//auto input = std::ifstream{"test.txt"};
	auto input = std::ifstream{"input.txt"};
	std::string line;
	SeatingChart chart;
	while (std::getline(input, line))
	{
		chart.AddRow(line);
	}
	std::cout << "done reading the chart" << std::endl;

	SeatingChart chart_part2 = chart;

	do
	{
		chart.state_changes = 0;
		chart.next_chart = chart.chart;
		for (size_t col = 0; col < chart.cols; col++)
		{
			for (size_t row = 0; row < chart.rows; row++)
			{
				// This increases .state_changes if the seat state changed
				const auto state = chart.GetNextState(row, col);
				chart.next_chart[row * chart.cols + col] = state;
			}
		}
		chart.chart = chart.next_chart;
		//std::cout << "changes this iteration" << chart.state_changes << std::endl;
	}
	while (chart.state_changes != 0);

	const auto seats_taken = std::count_if(chart.chart.begin(), chart.chart.end(),
	                                       [](const SeatState s) { return s == SeatState::TAKEN; });
	std::cout << "Seats taken at the end: " << seats_taken << std::endl;

	chart = chart_part2;
	do
	{
		chart.state_changes = 0;
		chart.next_chart = chart.chart;
		for (size_t col = 0; col < chart.cols; col++)
		{
			for (size_t row = 0; row < chart.rows; row++)
			{
				// This increases .state_changes if the seat state changed
				const auto state = chart.GetNextState2(row, col);
				chart.next_chart[row * chart.cols + col] = state;
			}
		}
		chart.chart = chart.next_chart;
		//std::cout << "changes this iteration" << chart.state_changes << std::endl;
	}
	while (chart.state_changes != 0);

	const auto seats_taken_part_2 = std::count_if(chart.chart.begin(), chart.chart.end(),
	                                              [](const SeatState s) { return s == SeatState::TAKEN; });
	std::cout << "Seats taken at the end part 2: " << seats_taken_part_2 << std::endl;
}
