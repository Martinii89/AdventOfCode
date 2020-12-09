// AdventOfCode_08.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <stack>

enum class OPCODE
{
    NOP,ACC,JMP
};

struct Instruction
{
    OPCODE op_code;
    int arg;
    Instruction(const std::string& line)
    {
        auto space = line.find(" ");
        auto op_code_s = line.substr(0, space);
        auto arg_s = line.substr(space + 1, line.size());
        arg = std::stoi(arg_s);
        if (op_code_s == "nop")
        {
            op_code = OPCODE::NOP;
        }
        else if (op_code_s == "acc")
        {
            op_code = OPCODE::ACC;
        }
        else if (op_code_s == "jmp")
        {
            op_code = OPCODE::JMP;
        }
        else {
            throw std::invalid_argument("Invalid argument");
        }
    }
};

struct Program
{

    int memory = 0;
    int position = 0;
    std::vector<Instruction> instructions;

    void Execute(Instruction& instruction)
    {
        switch (instruction.op_code)
        {
        case OPCODE::ACC:
            memory += instruction.arg;
            position++;
            break;
        case OPCODE::JMP:
            position += instruction.arg;
            break;
        case OPCODE::NOP:
            position++;
            break;
        default:
            break;
        }
    }

    void Run()
    {
        std::set<int> instruction_already_run;
        while (instruction_already_run.count(position) == 0)
        {
            instruction_already_run.insert(position);
            Execute(instructions[position]);
        }
    }

    void Run2(int skip_potential_fixes)
    {
        int fixes_skipped = 0;
        std::set<int> instruction_already_run;
        auto can_fix = [&](Instruction& i) {
            return i.op_code == OPCODE::JMP || i.op_code == OPCODE::NOP;
        };
        auto fix = [&](Instruction& i) {
            if (i.op_code == OPCODE::JMP) {
                i.op_code = OPCODE::NOP;
            }
            else {
                i.op_code = OPCODE::JMP;
            }
        };
        while (instruction_already_run.count(position) == 0 && position < instructions.size())
        {
            instruction_already_run.insert(position);
            Instruction& instruction = instructions[position];
            if (can_fix(instruction)) {
                if (fixes_skipped == skip_potential_fixes) {
                    fix(instruction);
                    
                }
                fixes_skipped++;
            }
            Execute(instruction);
        }
    }
};

int DeCorruptProgram(Program& p)
{
    p.memory = 0;
    p.position = 0;
    for (size_t i = 0; i < p.instructions.size(); i++)
    {
        auto p_copy = p;
        p_copy.Run2(i);
        if (p_copy.position == p_copy.instructions.size())
        {
            std::cout << "When skipping " << i << " fixes. The program ends at position " << p_copy.position << " with the value of " << p_copy.memory << std::endl;
        }
    }
    return 0;
}


int main()
{
    auto input = std::ifstream{ "input.txt" };
    std::string line;
    Program program;
    while (std::getline(input, line))
    {
        program.instructions.push_back(Instruction(line));
    }
    program.Run();
    std::cout << "The program ends with a value of " << program.memory << std::endl;

    DeCorruptProgram(program);
}

