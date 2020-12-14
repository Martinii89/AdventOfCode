// AdventOfCode_14.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <bitset>
#include <sstream>
#include <map>
#include <set>
#include <fstream>
#include <string>

struct Mask
{
	uint64_t ones = 0;
	uint64_t passthrough = 0;

	uint64_t Zeros()
	{
		return ~ones & ~passthrough;
	}

	uint64_t Write(const uint64_t additional_value)
	{
		uint64_t value = 0;
		value |= ones;
		value &= ~Zeros();
		value |= additional_value & passthrough;
		return value;
	}
};


std::set<uint64_t> GetAddresseFromMask(Mask& mask, uint64_t start)
{
	std::set<uint64_t> res;
	uint64_t non_floating = 0;
	non_floating |= start & mask.Zeros(); // Set the bits where start and zeros is 1 to 1 
	non_floating |= mask.ones & ~mask.passthrough; // Set the bits where the mask was 1 to 1
	res.insert(non_floating);

	std::bitset<36> floating_bits(mask.passthrough);
	for (int i = 0; i < floating_bits.size(); ++i)
	{
		if (floating_bits[i])
		{
			for (auto a : res)
			{
				res.insert(a ^ 1ULL << i); // it's a bit sketchy to insert while iterating. but it works
			}
		}
	}
	return res;
}


Mask GetMaskFromString(const std::string& input_mask)
{
	if (input_mask.size() != 36)
	{
		throw std::invalid_argument("string is not the right length!");
	}
	Mask mask;
	for (size_t i = 0; i < input_mask.size(); i++)
	{
		const auto c = input_mask[i];
		const auto bit = input_mask.size() - 1 - i;
		if (c == 'X')
		{
			mask.passthrough |= 1ULL << bit;
		}
		if (c == '1')
		{
			mask.ones |= 1ULL << bit;
		}
	}

	return mask;
}

struct Program
{
	Mask mask;
	std::map<uint64_t, uint64_t> memory;
};

int main()
{
	auto input = std::ifstream{"input.txt"};
	std::string line;
	Program program;
	Program program2;
	while (std::getline(input, line))
	{
		if (const auto it = line.find("mask"); it != std::string::npos)
		{
			auto mask = line.substr(7, line.size());
			program.mask = GetMaskFromString(mask);
			program2.mask = program.mask;
		}
		else
		{
			auto equal_sign = line.find('=');
			auto mem_part = line.substr(0, equal_sign - 1);
			auto mem_address_start = mem_part.find('[') + 1;
			auto mem_address_end = mem_part.find(']');
			auto mem_address_part = mem_part.substr(mem_address_start, mem_address_end - mem_address_start);
			auto value_part = line.substr(equal_sign + 1, line.size());
			auto mem_address = std::stoll(mem_address_part);
			auto value = std::stoll(value_part);
			program.memory[mem_address] = program.mask.Write(value);
			auto addresses_to_write_to = GetAddresseFromMask(program2.mask, mem_address);
			for (auto a : addresses_to_write_to)
			{
				program2.memory[a] = value;
			}
		}
	}

	uint64_t sum = 0;
	for (auto kv : program.memory)
	{
		sum += kv.second;
	}
	std::cout << "Final sum is : " << sum << std::endl;

	uint64_t sum2 = 0;
	for (auto kv : program2.memory)
	{
		sum2 += kv.second;
	}
	std::cout << "Final sum2 is : " << sum2 << std::endl;
}
