// AdventOfCode_12.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>

enum class Direction: char
{
	FORWARD = 'F',
	LEFT = 'L',
	RIGHT ='R',
	NORTH = 'N',
	SOUTH = 'S',
	WEST = 'W',
	EAST = 'E'
};

int ToAngle(const Direction dir)
{
	switch (dir)
	{
	case Direction::EAST: return 0;
	case Direction::NORTH: return 90;
	case Direction::WEST: return 180;
	case Direction::SOUTH: return 270;
	default: return 0;
	}
}

Direction FromAngle(int angle)
{
	angle = angle % 360;
	if (angle == 0) return Direction::EAST;
	if (angle == 90 || angle == -270) return Direction::NORTH;
	if (angle == 180 || angle == -180) return Direction::WEST;
	if (angle == 270 || angle == -90) return Direction::SOUTH;
	throw std::exception("invalid angle");
}


struct Ship
{
	Direction direction = Direction::EAST;
	int x = 0, y = 0;

	void Move(Direction dir, int val)
	{
		int current_angle, new_angle;

		switch (dir)
		{
		case Direction::FORWARD:
			Move(direction, val);
			break;
		case Direction::LEFT:
			current_angle = ToAngle(direction);
			new_angle = (current_angle + val);
			direction = FromAngle(new_angle);
			break;
		case Direction::RIGHT:
			current_angle = ToAngle(direction);
			new_angle = (current_angle - val);
			direction = FromAngle(new_angle);
			break;
		case Direction::NORTH:
			y += val;
			break;
		case Direction::SOUTH:
			y -= val;
			break;
		case Direction::WEST:
			x -= val;
			break;
		case Direction::EAST:
			x += val;
			break;
		}
	}
};

double degToRad(double degree)
{
	double pi = 3.14159265359;
	return (degree * (pi / 180));
}

struct WaypointShip
{
	int x = 0, y = 0;
	int waypoint_x = 10;
	int waypoint_y = 1;

	void RotateWaypoint(int angle)
	{
		int x0 = waypoint_x;
		int y0 = waypoint_y;
		waypoint_x = std::round(x0 * std::cos(degToRad(angle)) - y0 * std::sin(degToRad(angle)));
		waypoint_y = std::round(x0 * std::sin(degToRad(angle)) + y0 * std::cos(degToRad(angle)));
	}

	void Move(Direction dir, int val)
	{
		switch (dir)
		{
		case Direction::FORWARD:
			x += waypoint_x * val;
			y += waypoint_y * val;
			break;
		case Direction::LEFT:
			RotateWaypoint(val);
			break;
		case Direction::RIGHT:
			RotateWaypoint(-val);
			break;
		case Direction::NORTH:
			waypoint_y += val;
			break;
		case Direction::SOUTH:
			waypoint_y -= val;
			break;
		case Direction::WEST:
			waypoint_x -= val;
			break;
		case Direction::EAST:
			waypoint_x += val;
			break;
		}
	}
};

int main()
{
	auto input = std::ifstream{"input.txt"};
	std::string line;
	Ship ship;
	WaypointShip ship2;
	while (std::getline(input, line))
	{
		if (line.empty()) continue;
		const auto instruction = static_cast<Direction>(line[0]);
		const auto insturction_value = std::stoi(line.substr(1, line.size()));
		ship.Move(instruction, insturction_value);
		ship2.Move(instruction, insturction_value);
	}

	const auto manhatten_distance = std::abs(ship.x) + std::abs(ship.y);
	std::cout << "Manhatten distance is " << manhatten_distance << std::endl;

	const auto manhatten_distance2 = std::abs(ship2.x) + std::abs(ship2.y);
	std::cout << "Manhatten distance2 is " << manhatten_distance2 << std::endl;
}
