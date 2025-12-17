#include <raylib.h>
#include <vector>
#include <iostream>

#include "math.h"

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

Polygon* polygonSelected(Cursor cursor, vector<Polygon>& polygons);

void Draw(std::vector<Polygon>& polygons, std::string& text, const int TEXT_SIZE);
void drawPoly(std::vector<Polygon>& polygons);
void drawCollisionPoint(std::vector<Polygon>& polygons);

void Update(std::vector<Polygon>& polygons, Vector2& lastMousePos, Vector2& mouse, Cursor& cursor, bool& canDraw, Polygon*& selectedPolygon);
void movePoly(Polygon*& selectedPolygon, Cursor& cursor, std::vector<Polygon>& polygons, bool& canDraw, Vector2& deltaMouse);

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
	const int textSize = 24;

	while (!WindowShouldClose())
	{
		Update(polygons, lastMousePos, mouse, cursor, canDraw, selectedPolygon);
		Draw(polygons, text, textSize);
	}

	CloseWindow();
	return 0;
}

void Update(std::vector<Polygon>& polygons, Vector2& lastMousePos, Vector2& mouse, Cursor& cursor, bool& canDraw, Polygon*& selectedPolygon)
{
	BeginDrawing();
	ClearBackground(BLACK);

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

	movePoly(selectedPolygon, cursor, polygons, canDraw, deltaMouse);
}

void movePoly(Polygon*& selectedPolygon, Cursor& cursor, std::vector<Polygon>& polygons, bool& canDraw, Vector2& deltaMouse)
{
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
}

void Draw(std::vector<Polygon>& polygons, std::string& text, const int textSize)
{
	drawPoly(polygons);
	drawCollisionPoint(polygons);

	DrawText(text.c_str(), (GetScreenWidth() / 2) - MeasureText(text.c_str(), textSize) / 2, GetScreenHeight() - 30, textSize, YELLOW);

	EndDrawing();
}

void drawPoly(std::vector<Polygon>& polygons)
{
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
}

void drawCollisionPoint(std::vector<Polygon>& polygons)
{
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