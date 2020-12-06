// AdventOfCode_06.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>



std::tuple<int, int> CountGroupAnswers(std::ifstream& s)
{
	std::map<char, int> answers;
	std::string line;
	auto group_people = 0;
	while (std::getline(s, line) && !line.empty())
	{
		group_people++;
		std::istringstream is_line(line);
		for(auto c : line)
		{
			answers[c] += 1; 
		}
	}
	auto any_yes = answers.size();
	auto all_yes = std::count_if(answers.begin(), answers.end(), [v = group_people](auto kv) {return kv.second == v;});
	return {any_yes, all_yes};
}

int main()
{
	auto input = std::ifstream{ "input.txt" };
	auto all_any_yes_answers = 0;
	auto all_yes_answers = 0;
	while (!input.eof())
	{
		const auto [any_yes, all_yes] = CountGroupAnswers(input);
		all_any_yes_answers += any_yes;
		all_yes_answers += all_yes;
	}
	std::cout << "Sum of any_yes is: " << all_any_yes_answers << std::endl;
	std::cout << "Sum of all_yes is: " << all_yes_answers << std::endl;
}
