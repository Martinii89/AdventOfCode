// AdventOfCode_21.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include <queue>
#include <memory>
#include <string_view>
#include <numeric>
#include <regex>

struct Food
{
	std::set<std::string> ingredients;
	std::set<std::string> allergens;

	explicit Food(const std::string& line)
	{
		// mxmxvkd kfcds sqjhc nhms (contains dairy, fish)
		const std::regex word_pattern  {"(\\w+)"};
		std::smatch res;
		auto search_start( line.cbegin() );
		auto end_of_ingredients = false;
	    while ( std::regex_search( search_start, line.cend(), res, word_pattern ) )
	    {
	    	const auto& res_s = res[0];
	    	search_start = res.suffix().first;
	    	if (res_s == "contains")
	    	{
	    		end_of_ingredients = true;
	    		continue;
	    	}
	    	if (!end_of_ingredients)
	    	{
	    		ingredients.insert(res_s);
	    	}else
	    	{
	    		allergens.insert(res_s);
	    	}
	    }
	}
};


int main()
{
    auto input = std::ifstream{"input.txt"};
	std::vector<Food> all_foods;
	std::map<std::string, std::vector<Food*>> allergen_to_food;
	std::map<std::string, std::vector<Food*>> ingredient_to_food;
	std::set<std::string> all_ingredients;
	std::string line;
	while(std::getline(input, line))
	{
		all_foods.emplace_back(Food(line));
	}
	for(auto& food: all_foods)
	{
		for(const auto& allergen: food.allergens)
		{
			allergen_to_food[allergen].push_back(&food);
		}
		for(const auto& ingredient: food.ingredients)
		{
			all_ingredients.insert(ingredient);
			ingredient_to_food[ingredient].push_back(&food);
		}
	}

	std::set<std::string> not_safe;

	std::map<std::string, std::set<std::string>> allergen_reduction_map;
	
	
	for(auto& [allergen, foods]: allergen_to_food)
	{
		std::set<std::string> all_ingredients;
		std::set<std::string> ingredient_in_all_foods;
		for(auto* f: foods)
		{
			all_ingredients.insert(f->ingredients.begin(), f->ingredients.end());
			if (ingredient_in_all_foods.empty())
			{
				ingredient_in_all_foods.insert(f->ingredients.begin(), f->ingredients.end());
			}else
			{
				std::set<std::string> common_data;
				std::set_intersection(f->ingredients.begin(), f->ingredients.end(), 
					ingredient_in_all_foods.begin(), ingredient_in_all_foods.end(),
					std::inserter(common_data, common_data.begin()));
				ingredient_in_all_foods = common_data;
			}
		}

		std::set<std::string> not_this_allergen;
		std::set_difference(all_ingredients.begin(), all_ingredients.end(),
			ingredient_in_all_foods.begin(), ingredient_in_all_foods.end(),
			std::inserter(not_this_allergen, not_this_allergen.begin()));

		not_safe.insert(ingredient_in_all_foods.begin(), ingredient_in_all_foods.end());
		std::cout << "These ingredients is present in all the foods with : " << allergen << std::endl;
		allergen_reduction_map[allergen] = ingredient_in_all_foods;
		for(auto& ing: ingredient_in_all_foods)
		{
			std::cout << ing << std::endl;
		}
		//std::cout << "These ingredients is not present in all the foods with : " << allergen << std::endl;
		//for(auto& ing: not_this_allergen)
		//{
		//	std::cout << ing << std::endl;
		//}
		std::cout << std::endl;
		
	}

	std::cout << "These ingredients are not safe\n";
	for(auto& ing: not_safe)
	{
		std::cout << ing << std::endl;
	}
	std::cout << std::endl;

	std::set<std::string> probably_safe;
	std::set_difference(all_ingredients.begin(), all_ingredients.end(),
		not_safe.begin(), not_safe.end(),
		std::inserter(probably_safe, probably_safe.begin()));

	std::cout << "These ingredients are probably safe?\n";
	auto safe_count = 0;
	for(auto& ing: probably_safe)
	{
		safe_count += ingredient_to_food[ing].size();
		//std::cout << ing <<"\t\t(" << ingredient_to_food[ing].size() << " times)" << std::endl;
	}

	struct AllergenIngredientPair
	{
		std::string allergen;
		std::string ingredient;
	};
	std::vector<AllergenIngredientPair> solution;
    while (!allergen_reduction_map.empty())
    {
    	auto find_single = std::find_if(allergen_reduction_map.begin(), allergen_reduction_map.end(), [](auto kv){return kv.second.size() == 1;});
    	if (find_single == allergen_reduction_map.end())
    	{
    		throw std::logic_error("uhooh");
    	}
    	const auto allergen = find_single->first;
    	const auto ingredient = *find_single->second.begin();
    	solution.push_back({allergen, ingredient});
		allergen_reduction_map.erase(find_single);
    	for(auto& [allergen, potential_ingredients]: allergen_reduction_map)
    	{
    		potential_ingredients.erase(ingredient);
    	}
    	
    	
    }

	std::sort(solution.begin(), solution.end(), [](const AllergenIngredientPair& a, const AllergenIngredientPair& b){return a.allergen < b.allergen;});

	for(auto& s: solution)
	{
		std::cout << s.ingredient << ",";
	}
	std::cout << std::endl;

	std::cout << "Answer: " << safe_count << std::endl;

	

	std::cout << "Done\n";
}
