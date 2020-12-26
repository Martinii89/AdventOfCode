// AdventOfCode_25.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>

//The handshake used by the card and the door involves an operation that transforms a subject number.
//To transform a subject number, start with the value 1. Then, a number of times called the loop size, perform the following steps:
//
//Set the value to itself multiplied by the subject number.
//Set the value to the remainder after dividing the value by 20201227.

uint64_t GetNextTransform(uint64_t subject_number, uint64_t val)
{
	val *= subject_number;
	val = val % 20201227;
	return val;
}

uint64_t Transform(uint64_t subject_number, size_t loop_size)
{
	uint64_t val = 1;
	for (auto i = 0; i < loop_size; ++i)
	{
		val = GetNextTransform(subject_number, val);
	}
	return val;
}

int GetLoopCount(uint64_t value)
{
	static std::map<uint64_t, int> loop_count_table;
	static auto last_value = GetNextTransform(7, 1);
	if (loop_count_table.empty())
	{
		loop_count_table[last_value] = 0;
	}

	if (auto it = loop_count_table.find(value); it != loop_count_table.end())
	{
		return it->second;
	}

	static int loop_count = 1;
	while (last_value != value)
	{
		loop_count++;
		last_value = GetNextTransform(7, last_value);
		//std::cout << last_value << std::endl;
		loop_count_table[last_value] = loop_count;
		
	}
	return  loop_count;
	
}



int main()
{
	std::cout << Transform(12320657, GetLoopCount(9659666)) << std::endl;

	//std::cout << GetLoopCount(5764801) << std::endl;
	//std::cout << GetLoopCount(17807724) << std::endl;
	//std::cout << GetLoopCount(12320657) << std::endl;
	//std::cout << GetLoopCount(9659666) << std::endl;
	std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
