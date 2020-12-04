// AdventOfCode04.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

struct Passport
{
	//std::string byr; //(Birth Year)
	//std::string iyr; //(Issue Year)
	//std::string eyr; //(Expiration Year)
	//std::string hgt; //(Height)
	//std::string hcl; //(Hair Color)
	//std::string ecl; //(Eye Color)
	//std::string pid; //(Passport ID)
	//std::string cid; //(Country ID)
	std::map<std::string, std::string> fields;

	bool ValidPart1()
	{
		if (fields.size() == 8)
		{
			return true;
		}
		if (fields.size() == 7 && fields.count("cid") == 0) {
			return true;
		}
		return false;
	}

	bool RangeValidation(std::string field_name, int min, int max, int length)
	{
		auto field = fields[field_name];
		if (field.size() != length) return false;
		auto field_value = std::stoi(field);
		if (field_value < min || field_value > max) return false;
		return true;
	}

	bool HairValidation()
	{
		auto field = fields["hcl"];
		if (field[0] != '#') return false;
		auto value = field.substr(1, field.size());
		for (auto c : value)
		{
			if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')) continue;
			return false;
		}
		return true;
	}

	bool EyeValidation()
	{
		// ecl(Eye Color) - exactly one of : amb blu brn gry grn hzl oth.
		auto field = fields["ecl"];
		static std::set<std::string> valid_colors{ "amb", "blu", "brn", "gry", "grn", "hzl", "oth" };
		return valid_colors.count(field) == 1;
	}

	bool HeightValidation()
	{
		auto field = fields["hgt"];
		if (auto f = field.find("cm"); f != std::string::npos)
		{
			auto value = std::stoi(field.substr(0, f));
			if (value < 150 || value > 193) return false;
			return true;
		}
		if (auto f = field.find("in"); f != std::string::npos)
		{
			auto value = std::stoi(field.substr(0, f));
			if (value < 59 || value > 76) return false;
			return true;
		}

		// if not cm or in. it's invalid
		return false;
	}

	bool ValidPart2() {
		if (!ValidPart1()) {
			return false;
		}
		//byr(Birth Year) - four digits; at least 1920 and at most 2002.
		if (!RangeValidation("byr", 1920, 2002, 4)) 
			return false;
		//iyr(Issue Year) - four digits; at least 2010 and at most 2020.
		if (!RangeValidation("iyr", 2010, 2020, 4)) 
			return false;
		//eyr(Expiration Year) - four digits; at least 2020 and at most 2030.
		if (!RangeValidation("eyr", 2020, 2030, 4)) 
			return false;
		//hgt(Height) - a number followed by either cm or in:
		//If cm, the number must be at least 150 and at most 193.
		//If in, the number must be at least 59 and at most 76.
		if (!HeightValidation()) 
			return false;
		//hcl(Hair Color) - a # followed by exactly six characters 0 - 9 or a - f.
		if (!HairValidation()) 
			return false;
		//ecl(Eye Color) - exactly one of : amb blu brn gry grn hzl oth.
		if (!EyeValidation()) 
			return false;
		//pid(Passport ID) - a nine - digit number, including leading zeroes.
		if (!RangeValidation("pid", 9, 9999999999, 9)) 
			return false;
		//cid(Country ID) - ignored, missing or not.
		
	}
};

Passport GetPassportFromStream(std::ifstream& s)
{
	Passport passport;
	std::string line;
	while (std::getline(s, line) && !line.empty())
	{
		std::istringstream is_line(line);
		std::string keyValPair;
		while (is_line >> keyValPair)
		{
			std::istringstream keyValStream(keyValPair);
			auto delim = keyValPair.find(':');
			auto key = keyValPair.substr(0, delim);
			auto val = keyValPair.substr(delim + 1, keyValPair.size());
			//std::cout << key << ":" << val << std::endl;
			passport.fields[key] = val;
		}
	}
	return passport;
}

int main()
{
	auto input = std::ifstream{ "input.txt" };
	int valid_part1 = 0;
	int valid_part2 = 0;
	while (!input.eof())
	{
		auto p = GetPassportFromStream(input);
		valid_part1 += p.ValidPart1();
		valid_part2 += p.ValidPart2();

		//for (auto& field : p.fields)
		//{
		//	std::cout << field.first << ":" << field.second << std::endl;
		//}
	}
	std::cout << "Valid part 1 " << valid_part1 << std::endl;
	std::cout << "Valid part 2 " << valid_part2 << std::endl;
}
