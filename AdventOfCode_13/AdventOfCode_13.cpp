// AdventOfCode_13.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void Part1(const int my_time, const std::vector<long long>& busses)
{
	auto min_wait = my_time;
	auto min_wait_bus_id = -1;
	for (auto bus : busses)
	{
		if (bus != -1)
		{
			const int factor = my_time / bus;
			const auto next_bus = bus * (factor + 1);
			const auto wait = next_bus - my_time;
			if (wait < min_wait)
			{
				min_wait = wait;
				min_wait_bus_id = bus;
			}
		}
	}

	std::cout << "Min wait is " << min_wait << " with bus " << min_wait_bus_id << " the product of this is " << min_wait
		* min_wait_bus_id << std::endl;
}

int main()
{
	auto input = std::ifstream{"test.txt"};
	int my_time;
	input >> my_time;
	std::vector<long long> busses;
	std::string line;
	while (std::getline(input, line, ','))
	{
		if (line.empty()) continue;
		if (line == "x")
		{
			busses.push_back(-1);
		}
		else
		{
			busses.push_back(std::stoi(line));
		}
	}
	//Part1(my_time, busses);


	auto t = busses[0];
	auto dt = t;
	
	for (auto i = 1; i < busses.size();)
	{
		const auto val = busses[i];
		if (val == -1)
		{
			i++;
		}
		else if ((t + i) % val == 0)
		{
			std::cout << t << " was valid for " << val << std::endl;
			// The next valid timestamp for val. is gonna be t+val
			// The next valid timestamp for all previous ones. Is gonna be t + (val_0 * val_1 * ... * val_i)
			dt *= val;
			i++;
		}
		else
		{
			t += dt;
		}
	}
	std::cout << "final timestamp is: " << t;
}