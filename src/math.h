#pragma once
#include <raylib.h>

float Distance(Vector2 a, Vector2 b);

bool lineToLineIntersection(Vector2 poly1Init, Vector2 poly1End, Vector2 poly2Init, Vector2 poly2End, Vector2& point);
bool checkCollisionPointCircle(Vector2 circlePos, Vector2 pointPos, float radius);
bool checkCollisionLineCircle(Vector2 circlePos, Vector2 pointPos1, Vector2 pointPos2, float radius);
bool linePoint(Vector2 linePoint1, Vector2 linePoint2, Vector2 linePoint3);
