// AdventOfCode_18.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stack>
#include <memory>

enum class TokenType { OPEN_PARANTHESES, CLOSE_PARANTHESES, NUMBER, PLUSS, MINUS, MULT, DIV, WHITESPACE, END_OF_FILE };

struct Token
{
	TokenType token;
	std::string value;
};

Token GetNextToken(std::istringstream& in)
{
	char next;
	in.get(next);
	if (in.tellg() == -1)
	{
		return {TokenType::END_OF_FILE, " "};
	}
	switch (next)
	{
	case '(': return {TokenType::OPEN_PARANTHESES, "("};
	case ')': return {TokenType::CLOSE_PARANTHESES, ")"};
	case '*': return {TokenType::MULT, "*"};
	case '/': return {TokenType::DIV, "/"};
	case '+': return {TokenType::PLUSS, "+"};
	case '-': return {TokenType::MINUS, "-"};
	case ' ': return {TokenType::WHITESPACE, " "};
	default:
		std::stringstream number;
		number << next;
		auto pre_peek_pos = in.tellg();
		auto peek = GetNextToken(in);
		while (peek.token == TokenType::NUMBER)
		{
			number << peek.value;
			pre_peek_pos = in.tellg();
			peek = GetNextToken(in);
		}
		if (peek.token != TokenType::NUMBER)
		{
			in.seekg(pre_peek_pos);
			//std::for_each(peek.value.begin(), peek.value.end(), [&](auto c){in.putback(c);});
		}
		// number
		return {TokenType::NUMBER, number.str()};
	}
}

bool IsOperatorToken(const Token& token)
{
	return token.token == TokenType::PLUSS ||
		token.token == TokenType::MINUS ||
		token.token == TokenType::MULT ||
		token.token == TokenType::DIV;
}


class AstNode
{
public:
	std::shared_ptr<AstNode> left = nullptr;
	std::shared_ptr<AstNode> right = nullptr;

	AstNode(std::shared_ptr<AstNode> _left, std::shared_ptr<AstNode> _right): left(std::move(_left)),
	                                                                          right(std::move(_right))
	{
	}

	AstNode() = default;

	virtual int64_t eval() = 0;
};

class ValueNode : public AstNode
{
public:
	int64_t value;

	ValueNode(int _value): value(_value)
	{
	};

	// Inherited via AstNode
	virtual int64_t eval() override
	{
		return value;
	}
};

class AddNode : public AstNode
{
public:
	AddNode(std::shared_ptr<AstNode> _left, std::shared_ptr<AstNode> _right): AstNode(_left, _right)
	{
	}

	virtual int64_t eval() override
	{
		if (!left || !right)
		{
			throw std::runtime_error("Add node is missing arguments");
		}
		return left->eval() + right->eval();
	}
};

class SubNode : public AstNode
{
public:
	SubNode(std::shared_ptr<AstNode> _left, std::shared_ptr<AstNode> _right): AstNode(_left, _right)
	{
	}

	virtual int64_t eval() override
	{
		if (!left || !right)
		{
			throw std::runtime_error("Add node is missing arguments");
		}
		return left->eval() - right->eval();
	}
};

class DivNode : public AstNode
{
public:
	DivNode(std::shared_ptr<AstNode> _left, std::shared_ptr<AstNode> _right): AstNode(_left, _right)
	{
	}

	virtual int64_t eval() override
	{
		if (!left || !right)
		{
			throw std::runtime_error("Add node is missing arguments");
		}
		return left->eval() / right->eval();
	}
};

class MultNode : public AstNode
{
public:
	MultNode(std::shared_ptr<AstNode> _left, std::shared_ptr<AstNode> _right): AstNode(_left, _right)
	{
	}

	virtual int64_t eval() override
	{
		if (!left || !right)
		{
			throw std::runtime_error("Add node is missing arguments");
		}
		return left->eval() * right->eval();
	}
};

std::shared_ptr<AstNode> CreateSubExpresssion(std::stack<Token>& stack);

std::shared_ptr<AstNode> CreateOpExpression(std::stack<Token>& stack, std::shared_ptr<AstNode> right)
{
	auto operation = stack.top();
	stack.pop();
	if (operation.token == TokenType::OPEN_PARANTHESES)
	{
		return right;
	}

	auto left = CreateSubExpresssion(stack);
	std::shared_ptr<AstNode> op_node;
	if (operation.token == TokenType::PLUSS)
	{
		op_node = std::make_shared<AddNode>(right, left);
	}
	else if (operation.token == TokenType::MINUS)
	{
		op_node = std::make_shared<SubNode>(left, right);
	}
	else if (operation.token == TokenType::DIV)
	{
		op_node = std::make_shared<DivNode>(left, right);
	}
	else if (operation.token == TokenType::MULT)
	{
		op_node = std::make_shared<MultNode>(left, right);
	}
	else
	{
		throw std::runtime_error("Failed to generate Op expression");
	}
	return op_node;
}

std::shared_ptr<AstNode> CreateSubExpresssion(std::stack<Token>& stack)
{
	auto next = stack.top();
	stack.pop();
	std::shared_ptr<AstNode> right;
	switch (next.token)
	{
	case TokenType::CLOSE_PARANTHESES: // parsing backwards actually
		{
			auto sub = CreateSubExpresssion(stack);
			if (stack.empty())
			{
				return sub;
			}
			auto op_node = CreateOpExpression(stack, sub);
			if (!op_node)
			{
				return sub;
			}
			return op_node;
		}
	case TokenType::OPEN_PARANTHESES:
		{
			return nullptr;
		}


	case TokenType::NUMBER:
		{
			right = std::make_shared<ValueNode>(std::stoi(next.value));
			if (stack.empty())
			{
				return right;
			}
			auto opNode = CreateOpExpression(stack, right);
			return opNode;
		}

	default:
		throw std::runtime_error("Invalid program. Expression must start with ( or a number");
		break;
	}
}

std::shared_ptr<AstNode> GetNextExpression(std::stack<Token>& stack)
{
	auto next = stack.top();
	stack.pop();

	if (next.token == TokenType::NUMBER)
	{
		return std::make_shared<ValueNode>(std::stoi(next.value));
	}
	if (next.token == TokenType::CLOSE_PARANTHESES)
	{
		while (stack.top().token != TokenType::OPEN_PARANTHESES)
		{
			return GetNextExpression(stack);
		}
		stack.pop();
	}
}

std::tuple<std::reverse_iterator<std::vector<Token>::iterator>, std::reverse_iterator<std::vector<Token>::iterator>>
FindOperatorArgument(const std::vector<Token>& tokens,
                     const std::reverse_iterator<std::vector<Token>::iterator>& op_iterator)
{
	auto left_start = op_iterator + 1;
	auto right_start = op_iterator - 1;

	if (left_start->token == TokenType::CLOSE_PARANTHESES)
	{
		int level = -1;
		while (level != 0 && left_start != tokens.rend())
		{
			++left_start;
			if (left_start->token == TokenType::CLOSE_PARANTHESES) level--;
			else if (left_start->token == TokenType::OPEN_PARANTHESES) level++;
		}
	}
	else if (left_start->token == TokenType::NUMBER)
	{
		// were all good
	}
	else
	{
		//uhoh 
		throw std::runtime_error("Failed to find the operator argument");
	}

	if (right_start->token == TokenType::OPEN_PARANTHESES)
	{
		int level = -1;
		while (level != 0 && right_start != tokens.rend())
		{
			--right_start;
			if (right_start->token == TokenType::CLOSE_PARANTHESES) level++;
			else if (right_start->token == TokenType::OPEN_PARANTHESES) level--;
		}
	}
	else if (right_start->token == TokenType::NUMBER)
	{
		// were all good
	}
	else
	{
		//uhoh 
		throw std::runtime_error("Failed to find the operator argument");
	}

	return {left_start, right_start};
}


void AddParenthesis(std::stack<Token>& input, TokenType op)
{
	std::vector<Token> tokens;
	while (!input.empty())
	{
		auto next = input.top();
		input.pop();
		tokens.push_back(next);
	}

	std::reverse(tokens.begin(), tokens.end());

	std::cout << "Before:";
	for(auto& t: tokens)
	{
		std::cout << t.value;
	}
	std::cout << std::endl;

	auto search_pointer = tokens.rbegin();
	// Inserting while iterating causes all kinds of issues. Cache and insert after
	std::map<size_t, Token> tokens_to_insert;

	while (search_pointer != tokens.rend())
	{
		auto op_search = std::find_if(search_pointer, tokens.rend(),
		                              [op](const Token& token) { return token.token == op; });
		// It's a compound expression. find the start of it
		if (op_search == tokens.rend()) break;
		auto [left_arg, right_arg] = FindOperatorArgument(tokens, op_search);
		const auto left_pos = tokens.size() - 1 - std::distance(tokens.rbegin(), left_arg);
		const auto right_pos = tokens.size() - 1 - std::distance(tokens.rbegin(), right_arg);
		tokens_to_insert[right_pos + 1] = {TokenType::CLOSE_PARANTHESES, ")"};
		tokens_to_insert[left_pos] = {TokenType::OPEN_PARANTHESES, "("};
		search_pointer = op_search + 1;
	}
	for (auto iter = tokens_to_insert.rbegin(); iter != tokens_to_insert.rend(); ++iter)
	{
		tokens.insert(tokens.begin() + iter->first, iter->second);
	}

	std::cout << "After:";
	for(auto& t: tokens)
	{
		std::cout << t.value;
		input.push(t);
	}
	std::cout << std::endl;
}


int main()
{
	//std::string test_input = "356 + 434 * 2";
	auto input = std::ifstream{"input.txt"};
	std::string line;
	uint64_t sum = 0;
	while (std::getline(input, line))
	{
		std::stack<Token> tokens;
		std::istringstream in_stream{line};
		auto token = GetNextToken(in_stream);
		do
		{
			if (token.token != TokenType::WHITESPACE && token.token != TokenType::END_OF_FILE)
			{
				tokens.push(token);
			}
			token = GetNextToken(in_stream);
		}
		while (token.token != TokenType::END_OF_FILE);

		AddParenthesis(tokens, TokenType::PLUSS);
		auto program = CreateSubExpresssion(tokens);
		auto program_value = program->eval();
		std::cout << program_value << " Is the answer to: " << line << std::endl;
		sum += program_value;
	}

	std::cout << "Final sum is: " << sum << std::endl;


	std::cout << "Done\n";
}
