// AdventOfCode_19.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include <memory>
#include <string_view>


struct GrammarSubRule
{
	enum class GrammarType { STRING, RULEREFERENCE };

	GrammarType rule_type;
	std::string rule_value;
	int rule_number = -1;

	explicit GrammarSubRule(const std::string& in)
	{
		if (in.rfind('"', 0) == 0)
		{
			rule_type = GrammarType::STRING;
			rule_value = in.substr(1, in.size() - 2);
		}
		else
		{
			rule_type = GrammarType::RULEREFERENCE;
			rule_number = std::stoi(in);
		}
	}
};

struct GrammarRule
{
	std::vector<GrammarSubRule> rule_sequence;

	explicit GrammarRule(const std::string& in)
	{
		std::istringstream in_stream{in};
		std::string token;
		while (std::getline(in_stream, token, ' '))
		{
			if (token.empty()) continue;
			rule_sequence.emplace_back(GrammarSubRule{token});
		}
	}
};

struct Grammar
{
	std::map<int, std::vector<GrammarRule>> rules;

	const std::vector<GrammarRule>& GetRule(size_t i) const
	{
		if (const auto it = rules.find(i); it != rules.end())
		{
			return it->second;
		}

		throw std::invalid_argument("invalid rule number");
	}
};


class RecursiveDescentParser
{
	Grammar grammar_;
	size_t look_ahead_ = 0;
	size_t input_length = 0;
	std::string_view input_;
	std::string_view look_ahead_input_;
	int eights_length_ = 0;
	// The pick of 8 puts some restrictions on 11
	// 42 42 31 31 31 is impossible for example
	// 42 42 31 31 is also impossible
	// 42 42 42 31 31 is possible
	//42 42 42 42 31 31 is also possible
	// there must be at least one more 42 than 31
	
public:
	explicit RecursiveDescentParser(Grammar grammar): grammar_(std::move(grammar))
	{
	}


	bool Parse(const std::string_view& message)
	{
		look_ahead_ = 0;
		input_ = message;
		input_length = input_.length();
		look_ahead_input_ = message;
		eights_length_ = 0;
		Match(0);
		return look_ahead_ == input_.length();
	}


	bool Match(const std::string_view& match_string)
	{
		if (input_.substr(look_ahead_).rfind(match_string, 0) == 0)
		{
			look_ahead_ += match_string.length();
			look_ahead_input_ = input_.substr(look_ahead_);
			return true;
		}
		return false;
	}

	bool Match(const GrammarSubRule& sub_rule)
	{
		const auto backtrack = look_ahead_;
		if (sub_rule.rule_type == GrammarSubRule::GrammarType::RULEREFERENCE)
		{
			if (Match(sub_rule.rule_number))
			{
				return true;
			}
		}
		else
		{
			if (Match(sub_rule.rule_value))
			{
				return true;
			}
		}
		look_ahead_ = backtrack;
		return false;
	}

	bool Match(const GrammarRule& rule)
	{
		const auto backtrack = look_ahead_;
		const auto all_match = std::all_of(rule.rule_sequence.begin(), rule.rule_sequence.end(),
		                                   [&](const GrammarSubRule& sub_rule) { return Match(sub_rule); });
		if (all_match)
		{
			return true;
		}
		look_ahead_ = backtrack;
		return false;
	}

	bool FirstOf(const std::vector<GrammarRule>& rules, size_t rule_num)
	{
		const auto backtrack = look_ahead_;
		for (const auto& rule : rules)
		{
			if (rule_num == 11)
			{
				if (rule.rule_sequence.size() >= eights_length_)
				{
					continue;
				}
			}
			//std::cout << "Checkig sequence: ";
			//for (const auto& s : rule.rule_sequence)
			//{
			//	if (s.rule_type == GrammarSubRule::GrammarType::RULEREFERENCE)
			//	{
			//		std::cout << s.rule_number << ",";
			//	}
			//	else
			//	{
			//		std::cout << s.rule_value << ",";
			//	}
			//}
			//std::cout << std::endl;
			// This will terminate at the first rule that partly matches..
			if (Match(rule))
			{
				if (rule_num == 8)
				{
					eights_length_ = rule.rule_sequence.size();
				}
				return true;
			}
			look_ahead_ = backtrack;
		}
		return false;
	}


	bool Match(size_t rule_number)
	{
		const auto backtrack = look_ahead_;
		const auto& production_rules = grammar_.GetRule(rule_number);
		//std::cout << "Checking rule : " << rule_number << std::endl;
		const auto match_res = FirstOf(production_rules, rule_number);
		if (match_res)
		{
			return true;
		}
		look_ahead_ = backtrack;
		return false;
	}
};


int main()
{
	auto input = std::ifstream{"input.txt"};
	std::string line;
	Grammar grammar;
	while (std::getline(input, line))
	{
		if (line.empty()) break;
		auto rule_num = std::stoi(line.substr(0, line.find(':')));
		std::istringstream in_stream{line.substr(line.find(':') + 1, line.size())};
		std::string sub_rule;
		while (std::getline(in_stream, sub_rule, '|'))
		{
			grammar.rules[rule_num].emplace_back(GrammarRule{sub_rule});
		}
	}
	//0: 8 11
	//8: 42 | 42 8
	//11: 42 31 | 42 11 31
	//0: (42 | 42 8) (42 31 | 42 11 31)
	//0: (two or more 42) (1 or more 31)
	//std::string eights_s = "42 42 42 42 42 42 | 42 42 42 42 42 | 42 42 42 42 | 42 42 42 | 42 42 ";
	//std::string eleven_s = "31 31 31 31 31 31 | 42 42 42 42 42 31 31 31 31 31 | 42 42 42 42 31 31 31 31 | 42 42 42 31 31 31 | 42 42 31 31 | 42 31";
	//std::istringstream eights {eights_s};
	//std::istringstream elevens {eleven_s};
	//std::string token;
	grammar.rules[8].clear();
	for (int i = 20; i >= 2; --i)
	{
		std::stringstream s;
		for (int j = 0; j < i; ++j)
		{
			s << "42 ";
		}
		grammar.rules[8].emplace_back(GrammarRule{s.str()});
	}
	grammar.rules[11].clear();
	for (int i = 20; i >= 1; --i)
	{
		std::stringstream s;
		for (int j = 0; j < i; ++j)
		{
			s  << " 31 ";
		}
		grammar.rules[11].emplace_back(GrammarRule{s.str()});
	}

	RecursiveDescentParser parser{grammar};
	std::cout << "Done parsing rules\n";
	std::vector<std::string> messages;
	int match_count = 0;
	while (std::getline(input, line))
	{
		messages.push_back(line);

		auto res = parser.Parse(line);
		if (res)
		{
			std::cout << line << ": Matched rule 0\n";
			match_count++;
		}
	}

	// 244 too low
	// 355 too high
	// 348 too high
	// 296 (no info)
	// 343 (no info)
	// 339 next guess
	std::cout << "Found " << match_count << " matches in the list";
}
