// AdventOfCode_02.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <regex>
#include <vector>

struct PasswordRequirements
{
	int min;
	int max;
	char letter;

	bool Validate(const std::string& password)
	{
		auto occurances = std::count(password.begin(), password.end(), letter);
		return occurances >= min && occurances <= max;
	}

	bool ValidatePart2(const std::string& password)
	{
		auto first_match = password[min - 1] == letter;
		auto second_match = password[max - 1] == letter;
		return (!first_match) != (!second_match); // XOR
		//return (first_match + second_match) == 1;
	}
};



int main()
{
	auto input = std::ifstream{ "input.txt" };
	std::string line;
	auto valid_password = 0;
	auto valid_password_part_two = 0;
	while (std::getline(input, line))
	{
		const std::regex regexp("(\\d+)-(\\d+)\\s(\\w+):\\s(\\w+)");
		std::smatch m;
		std::regex_search(line, m, regexp);
		if (m.size() == 5)
		{
			auto pw_req = PasswordRequirements{ std::stoi(m[1]), std::stoi(m[2]), m[3].str()[0] };
			if (pw_req.Validate(m[4]))
			{
				valid_password += 1;
			}
			if (pw_req.ValidatePart2(m[4]))
			{
				valid_password_part_two += 1;
			}
		}
	}
	std::cout << "valid passwords for part 1 is: " << valid_password << std::endl;
	std::cout << "valid passwords for part 2 is: " << valid_password_part_two << std::endl;
}
