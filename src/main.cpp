#include <raylib.h>
#include <vector>
#include <iostream>

using namespace std;

struct Line
{
	Vector2 init;
	Vector2 end;
};

struct Polygon
{
	vector<Line> lines;
	Vector2 latestPoint = { numeric_limits<float>::max(), numeric_limits<float>::max() };
	bool closed = false;
};

struct Cursor
{
	float radius = 5.0f;
	Vector2 position = { 0.0f, 0.0f };
};

float Distance(Vector2 a, Vector2 b);

bool lineToLineIntersection(Vector2 poly1Init, Vector2 poly1End, Vector2 poly2Init, Vector2 poly2End, Vector2& point);
bool checkCollisionPointCircle(Vector2 circlePos, Vector2 pointPos, float radius);
bool checkCollisionLineCircle(Vector2 circlePos, Vector2 pointPos1, Vector2 pointPos2, float radius);
bool linePoint(Vector2 linePoint1, Vector2 linePoint2, Vector2 linePoint3);

Polygon* polygonSelected(Cursor cursor, vector<Polygon>& polygons);

int main()
{
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 600, "TP1 Algebra - Poligonos Irregulares");
	SetTargetFPS(60);

	Cursor cursor;

	Vector2 mouse = { 0,0 };
	Vector2 lastMousePos = { 0,0 };

	vector<Polygon> polygons;
	polygons.push_back(Polygon());

	Polygon* selectedPolygon = nullptr;
	bool canDraw = true;

	string text = "Presione R para limpiar la pantalla";
	const int TEXT_SIZE = 24;

	while (!WindowShouldClose())
	{
		cout << polygons.size() << endl;

		if (IsKeyPressed(KEY_R))
		{
			polygons.clear();
			polygons.push_back(Polygon());
		}

		lastMousePos = mouse;
		mouse = GetMousePosition();

		Vector2 deltaMouse = { mouse.x - lastMousePos.x, mouse.y - lastMousePos.y };

		cursor.position = mouse;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && canDraw)
		{
			Polygon& currentPoly = polygons.back();

			if (currentPoly.lines.empty())
			{
				Line currentline;

				currentline.init = mouse;
				currentline.end = mouse;
				currentPoly.lines.push_back(currentline);
			}
			else if (currentPoly.lines.size() == 1 && currentPoly.latestPoint.x == numeric_limits<float>::max() && currentPoly.latestPoint.y == numeric_limits<float>::max())
			{
				Line& currentline = currentPoly.lines.back();
				currentline.end = mouse;
				currentPoly.latestPoint = mouse;
			}
			else
			{
				if (currentPoly.lines.size() >= 2)
				{
					Vector2 firstVertex = currentPoly.lines[0].init;
					Line currentline;
					currentline.init = currentPoly.latestPoint;
					currentline.end = mouse;

					bool hasCollision = false;
					Vector2 point = { 0,0 };

					for (int i = 0; i < currentPoly.lines.size() - 1; i++)
					{
						if (lineToLineIntersection(currentPoly.lines[i].init, currentPoly.lines[i].end, currentline.init, currentline.end, point))
						{
							hasCollision = true;
						}
					}

					if (!hasCollision)
					{
						if (Distance(mouse, firstVertex) < 50.0f)
						{
							currentPoly.closed = true;
							currentline.end = firstVertex;
							currentPoly.latestPoint = currentline.end;
							currentPoly.lines.push_back(currentline);
							polygons.push_back(Polygon());
						}
						else
						{
							currentPoly.latestPoint = currentline.end;
							currentPoly.lines.push_back(currentline);
						}
					}
				}
				else
				{
					Line currentline;
					currentline.init = currentPoly.latestPoint;
					currentline.end = mouse;
					currentPoly.latestPoint = currentline.end;
					currentPoly.lines.push_back(currentline);
				}
			}
		}

		if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
		{
			if (selectedPolygon == nullptr)
			{
				selectedPolygon = polygonSelected(cursor, polygons);
				canDraw = false;
			}
			else
			{
				selectedPolygon = nullptr;
				canDraw = true;
			}
		}

		if (selectedPolygon != nullptr)
		{
			for (int i = 0; i < (*selectedPolygon).lines.size(); i++)
			{
				(*selectedPolygon).lines[i].init.x += deltaMouse.x;
				(*selectedPolygon).lines[i].init.y += deltaMouse.y;

				(*selectedPolygon).lines[i].end.x += deltaMouse.x;
				(*selectedPolygon).lines[i].end.y += deltaMouse.y;
			}
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (int p = 0; p < polygons.size(); p++)
		{
			Polygon& poly = polygons[p];

			if (poly.latestPoint.x != numeric_limits<float>::max() && poly.latestPoint.y != numeric_limits<float>::max())
			{
				for (int i = 0; i < poly.lines.size(); i++)
				{
					DrawLineV(poly.lines[i].init, poly.lines[i].end, WHITE);
				}
			}

			for (int v = 0; v < poly.lines.size(); v++)
			{
				DrawCircleV(poly.lines[v].init, 4, YELLOW);

				if (poly.latestPoint.x != numeric_limits<float>::max() && poly.latestPoint.y != numeric_limits<float>::max())
				{
					DrawCircleV(poly.lines[v].end, 4, YELLOW);
				}
			}
		}

		for (int i = 0; i < polygons.size(); i++)
		{
			for (int j = 0; j < polygons.size(); j++)
			{
				if (i == j)
				{
					continue;
				}

				for (int k = 0; k < polygons[i].lines.size(); k++)
				{
					for (int l = 0; l < polygons[j].lines.size(); l++)
					{
						Vector2 collisionPoint;

						if (lineToLineIntersection(polygons[i].lines[k].init, polygons[i].lines[k].end, polygons[j].lines[l].init, polygons[j].lines[l].end, collisionPoint))
						{
							DrawCircleV(collisionPoint, 4, RED);
						}
					}
				}
			}
		}

		DrawText(text.c_str(), (GetScreenWidth() / 2) - MeasureText(text.c_str(), TEXT_SIZE) / 2, GetScreenHeight() - 30, TEXT_SIZE, YELLOW);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}

float Distance(Vector2 a, Vector2 b)
{
	float dx = abs(a.x) - abs(b.x);
	float dy = abs(a.y) - abs(b.y);

	return sqrtf((dx * dx) + (dy * dy));
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
		if (x >= min(poly1Init.x, poly1End.x) && x <= max(poly1Init.x, poly1End.x) &&
			x >= min(poly2Init.x, poly2End.x) && x <= max(poly2Init.x, poly2End.x) &&
			y >= min(poly1Init.y, poly1End.y) && y <= max(poly1Init.y, poly1End.y) &&
			y >= min(poly2Init.y, poly2End.y) && y <= max(poly2Init.y, poly2End.y))
			return true;
	}
	return false;
}

Polygon* polygonSelected(Cursor cursor, vector<Polygon>& polygons)
{
	for (int i = 0; i < polygons.size(); i++)
	{
		for (int j = 0; j < polygons[i].lines.size(); j++)
		{
			if (checkCollisionLineCircle(cursor.position, polygons[i].lines[j].init, polygons[i].lines[j].end, cursor.radius))
			{
				if (polygons[i].closed)
				{
					return &polygons[i];
				}
			}
		}
	}

	return nullptr;
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

	Vector2 closests = { pointPos1.x + (dot * pointPos2.x - pointPos1.x), pointPos1.y + (dot * (pointPos2.y - pointPos1.y)) };

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
	float d2 = sqrt(((abs(linePoint3.x - linePoint2.x)) * (abs(linePoint3.x - linePoint2.x))) + ((abs(linePoint3.y - linePoint2.y)) * (abs(linePoint3.x - linePoint2.x))));

	float lineLen = sqrt(((abs(linePoint1.x - linePoint2.x)) * (abs(linePoint1.x - linePoint2.x))) + ((abs(linePoint1.y - linePoint2.y)) * (abs(linePoint1.y - linePoint2.y))));

	float buffer = 0.1;

	if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer)
	{
		return true;
	}
	return false;
}
