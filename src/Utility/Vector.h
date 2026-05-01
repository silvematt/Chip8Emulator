#pragma once

struct Vector2Int
{
	int x;
	int y;

	Vector2Int() : x(0), y(0)
	{
	}

	Vector2Int(int px, int py) : x(px), y(py)
	{
	}
};