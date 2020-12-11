// AdventOfCode_11.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

enum class SeatState
{
    EMPTY, TAKEN, FLOOR, VOID
};

struct SeatingChart
{

    std::vector<SeatState> next_chart;
    std::vector<SeatState> chart;
    int rows = 0;
    int cols = -1;

    int state_changes = 0;

    SeatState CharToState(char c)
    {
        if (c == '#')
        {
            return SeatState::TAKEN;
        }
        else if (c == '.')
        {
            return SeatState::FLOOR;
        }
        else if (c == 'L')
        {
            return SeatState::EMPTY;
        }
        throw std::exception("invalid seat state");
    }

    SeatState GetSeatState(int row, int col)
    {
        if (row < 0 || row >= rows || col < 0 || col >= cols)
        {
            //throw std::exception("Invalid index!");
            return SeatState::VOID;
        }
        auto index = row * cols + col;
        if (index >= chart.size())
        {
            throw std::exception("Invalid index!");
        }
        return chart[index];
    }

    int CountNeighbouringSeatTaken(int row, int col)
    {
        auto count = 0;
        count += GetSeatState(row - 1, col - 1) == SeatState::TAKEN;
        count += GetSeatState(row - 1, col) == SeatState::TAKEN;
        count += GetSeatState(row - 1, col + 1) == SeatState::TAKEN;

        count += GetSeatState(row, col -1 ) == SeatState::TAKEN;
        count += GetSeatState(row, col + 1 ) == SeatState::TAKEN;

        count += GetSeatState(row + 1, col - 1) == SeatState::TAKEN;
        count += GetSeatState(row + 1, col) == SeatState::TAKEN;
        count += GetSeatState(row + 1, col + 1) == SeatState::TAKEN;
        return count;
    }

    bool IsTakenSeatVisible(int row, int col, int d_row, int d_col)
    {
        auto seat = GetSeatState(row + d_row, col + d_col);
        switch (seat)
        {
        case SeatState::EMPTY:
        case SeatState::FLOOR:
            return IsTakenSeatVisible(row + d_row, col + d_col, d_row, d_col);
            break;
        case SeatState::TAKEN:
            return true;
            break;
        case SeatState::VOID:
        default:
            return false;
            break;
        }
    }

    int CountVisibleSeatsTaken(int row, int col)
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

    SeatState GetNextState(int row, int col)
    {
        auto current = GetSeatState(row, col);
        switch (current)    
        {
        case SeatState::EMPTY:
            if (CountNeighbouringSeatTaken(row, col) == 0)
            {
                state_changes++;
                return SeatState::TAKEN;
            }
            else {
                return SeatState::EMPTY;
            }
            break;
        case SeatState::TAKEN:
            if (CountNeighbouringSeatTaken(row, col) >= 4)
            {
                state_changes++;
                return SeatState::EMPTY;
            }
            else {
                return SeatState::TAKEN;
            }
            break;
        case SeatState::FLOOR:
            return SeatState::FLOOR;
            break;
        case SeatState::VOID:
        default:
            return SeatState::VOID;
            break;

        }

    }

    void AddRow(const std::string& row)
    {
        if (row.empty()) {
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
    //auto input = std::ifstream{ "test.txt" };
    auto input = std::ifstream{ "input.txt" };
    std::string line;
    SeatingChart chart;
    while (std::getline(input, line))
    {
        chart.AddRow(line);
    }
    std::cout << "done reading the chart" << std::endl;

    do
    {
        chart.state_changes = 0;
        chart.next_chart = chart.chart;
        for (size_t col = 0; col < chart.cols; col++)
        {
            for (size_t row = 0; row < chart.rows; row++)
            {
                // This increases .state_changes if the seat state changed
                auto state = chart.GetNextState(row, col);
                chart.next_chart[row * chart.cols + col] = state;
            }
        }
        chart.chart = chart.next_chart;
        std::cout << "changes this iteration" << chart.state_changes << std::endl;
    } while (chart.state_changes != 0);

    auto seats_taken = std::count_if(chart.chart.begin(), chart.chart.end(), [](SeatState s) {return s == SeatState::TAKEN; });
    std::cout << "Seats taken at the end: " << seats_taken << std::endl;

}
