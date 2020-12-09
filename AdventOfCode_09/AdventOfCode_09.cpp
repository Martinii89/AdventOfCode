// AdventOfCode_09.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <set>
#include <deque>
#include <vector>
#include <algorithm>

struct Preamble
{
    int length = 0;
    std::deque<int> preamble;
    std::set<int> number_set;

    Preamble(std::ifstream& stream, int preamble_length): length(preamble_length)
    {
        for (size_t i = 0; i < preamble_length; i++)
        {
            int n;
            stream >> n;
            AddNumber(n);
        }
    }

    void AddNumber(int number)
    {
        if (preamble.size() >= length)
        {
            auto oldest = preamble.front();
            preamble.pop_front();
            number_set.erase(oldest);
        }
        preamble.push_back(number);
        number_set.insert(number);
    }

    bool ValidNewNumber(int number)
    {
        // the set is sorted
        auto front = number_set.begin();
        auto back = number_set.rbegin();
        while (*front != *back) // set only contains unique values. if the values are equal, the iterators are pointing to the same spot.
        {
            auto s = *front + *back;
            if (s == number)
            {
                return true;
            }
            else if (s > number)
            {
                back++;
            }
            else if (s < number) {
                front++;
            }
        }

        return false;
    }
};


int main()
{
    auto input = std::ifstream{ "input.txt" };
    int new_number;
    Preamble preamble{ input, 25 };
    std::vector<long long> all_the_numbers{preamble.preamble.begin(), preamble.preamble.end()};
    bool invalid_found = false;
    long long invalid = -1;
    while (input >> new_number)
    {
        all_the_numbers.push_back(new_number);
        if (!invalid_found)
        {
            if (preamble.ValidNewNumber(new_number)) {
                preamble.AddNumber(new_number);
            }
            else {
                std::cout << "The first invalid number was: " << new_number << std::endl;
                invalid = new_number;
                invalid_found = true;
            }
        }
    }

    auto front = all_the_numbers.begin();
    auto back = front+1;
    auto s = *front + *back;
    auto increment_back = [&]() {
        back++;
        s += *back;
    };

    auto increment_front = [&]() {
        s -= *front;
        front++;
        if (back <= front)
        {
            increment_back();
        }
    };

    while (back != all_the_numbers.end())
    {
        if (s == invalid) {
            break;
        }
        if (s > invalid)
        {
            increment_front();

        }
        else if (s < invalid)
        {
            increment_back();
        }
    }

    auto min_n = std::min_element(front, back);
    auto max_n = std::max_element(front, back);
    auto sum = *max_n + *min_n;
    std::cout << "Solution: " << sum << std::endl;


}
