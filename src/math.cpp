#include "math.h"

#include <cmath>
#include <iostream>

float Distance(Vector2 a, Vector2 b)
{
	return std::hypot(a.x - b.x, a.y - b.y);
}

bool lineToLineIntersection(Vector2 poly1Init, Vector2 poly1End, Vector2 poly2Init, Vector2 poly2End, Vector2& point)
{
	float a1 = poly1End.y - poly1Init.y;
	float b1 = poly1Init.x - poly1End.x;
	float c1 = a1 * poly1Init.x + b1 * poly1Init.y;
	float a2 = poly2End.y - poly2Init.y;
	float b2 = poly2Init.x - poly2End.x;
	float c2 = a2 * poly2Init.x + b2 * poly2Init.y;
	float determinante = a1 * b2 - a2 * b1;

	if (determinante != 0)
	{
		float x = (b2 * c1 - b1 * c2) / determinante;
		float y = (a1 * c2 - a2 * c1) / determinante;
		point.x = x;
		point.y = y;
		if (x >= std::min(poly1Init.x, poly1End.x) && x <= std::max(poly1Init.x, poly1End.x) &&
			x >= std::min(poly2Init.x, poly2End.x) && x <= std::max(poly2Init.x, poly2End.x) &&
			y >= std::min(poly1Init.y, poly1End.y) && y <= std::max(poly1Init.y, poly1End.y) &&
			y >= std::min(poly2Init.y, poly2End.y) && y <= std::max(poly2Init.y, poly2End.y))
			return true;
	}
	return false;
}

bool checkCollisionPointCircle(Vector2 circlePos, Vector2 pointPos, float radius)
{
	float distX = pointPos.x - circlePos.x;
	float distY = pointPos.y - circlePos.y;
	float distance = sqrt((distX * distX) + (distY * distY));

	if (distance <= radius)
	{
		return true;
	}
	return false;
}

bool checkCollisionLineCircle(Vector2 circlePos, Vector2 pointPos1, Vector2 pointPos2, float radius)
{
	bool inside1 = checkCollisionPointCircle(circlePos, pointPos1, radius);
	bool inside2 = checkCollisionPointCircle(circlePos, pointPos2, radius);

	if (inside1 || inside2)
	{
		return true;
	}

	float distX = pointPos1.x - pointPos2.x;
	float distY = pointPos1.y - pointPos2.y;
	float len = sqrt((distX * distX) + (distY * distY));

	float dot = (((circlePos.x - pointPos1.x) * (pointPos2.x - pointPos1.x)) + ((circlePos.y - pointPos1.y) * (pointPos2.y - pointPos1.y))) / pow(len, 2);

	Vector2 closests = { pointPos1.x + dot * (pointPos2.x - pointPos1.x), pointPos1.y + dot * (pointPos2.y - pointPos1.y) };

	bool onSegment = linePoint(pointPos1, pointPos2, closests);

	if (!onSegment)
	{
		return false;
	}

	distX = closests.x - circlePos.x;
	distY = closests.y - circlePos.y;

	float distance = sqrt((distX * distX) + (distY * distY));

	if (distance <= radius)
	{
		return true;
	}
	return false;
}

bool linePoint(Vector2 linePoint1, Vector2 linePoint2, Vector2 linePoint3)
{
	float d1 = sqrt(((abs(linePoint3.x - linePoint1.x)) * (abs(linePoint3.x - linePoint1.x))) + ((abs(linePoint3.y - linePoint1.y)) * (abs(linePoint3.y - linePoint1.y))));
	float d2 = sqrt(((abs(linePoint3.x - linePoint2.x)) * (abs(linePoint3.x - linePoint2.x))) + ((abs(linePoint3.y - linePoint2.y)) * (abs(linePoint3.y - linePoint2.y))));

	float lineLen = sqrt(((abs(linePoint1.x - linePoint2.x)) * (abs(linePoint1.x - linePoint2.x))) + ((abs(linePoint1.y - linePoint2.y)) * (abs(linePoint1.y - linePoint2.y))));

	float buffer = 0.1;

	if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer)
	{
		return true;
	}
	return false;
}
