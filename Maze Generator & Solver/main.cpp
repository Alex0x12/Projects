#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <stack>
#include <vector>
#include <string>
#include <curses.h>
#include <algorithm>

using namespace std;
using namespace sf;


const int w_width = 500;
const int w_height = 500;

const int m_width = 50;
const int m_height = 50;

const float margin = max(w_width, w_height) / (2 * max(m_width, m_height));
const float CellRelationX = ((w_width - margin * 2) / m_width);
const float CellRelationY = (w_height - margin * 2) / m_height;

RenderWindow window(VideoMode({w_width, w_height}), "Maze");

enum CellState { Closed, Open };

struct Cell {
	int x=int(), y = int();
	bool Visited = false;
	bool pVisited = false;
	CellState Top = Closed;
	CellState Bottom = Closed;
	CellState Left = Closed;
	CellState Right = Closed;

	static Cell* Entrance;
	static Cell* Exit;

	bool operator==(const Cell& other) {
		if (this->x == other.x && this->y == other.y) 
			return 1;
		else return 0;
	}
};

Cell* Cell::Entrance = nullptr;
Cell* Cell::Exit = nullptr;


Cell** CreateMaze() {

	Cell** Labyrinth = new Cell*[m_width];
	for (int i = 0; i < m_width; i++)
	{
		Labyrinth[i] = new Cell[m_height];
	}

	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			Labyrinth[i][j].x = i;
			Labyrinth[i][j].y = j;
		}
	}

	Cell* CurrentCell = &Labyrinth[rand() % m_width][rand() % m_height];
	CurrentCell->Visited = true;

	// Ñòåê äëÿ õðàíåíèÿ ïóòè
	// Âåêòîð äëÿ õðàíåíèÿ âîçìîæíûõ íàïðàâëåíèé
	stack <Cell> path;
	vector<Cell*> direction;

	path.push(*CurrentCell); // Äîáàâëÿåì â ïóòü ïåðâóþ êëåòêó

	Cell _cell;
	while (!path.empty()) {

		_cell = path.top(); // Ïåðåìåííàÿ, ÷åðåç êîòîðóþ ìû îáðàùàåìñÿ ê âåðøèíå ñòåêà - ïî ñóòè, òî÷êå ïðîäîëæåíèÿ ïóòè

		// Ïðîâåðÿåì âîçìîæíîñòü äâèæåíèÿ ïî ÷åòûðåì íàïðàâëåíèÿì

		if (_cell.x > 0 && !Labyrinth[_cell.x - 1][_cell.y].Visited)
			direction.push_back(&Labyrinth[_cell.x - 1][_cell.y]);

		if (_cell.x < m_width - 1 && !Labyrinth[_cell.x + 1][_cell.y].Visited)
			direction.push_back(&Labyrinth[_cell.x + 1][_cell.y]);

		if (_cell.y > 0 && !Labyrinth[_cell.x][_cell.y - 1].Visited)
			direction.push_back(&Labyrinth[_cell.x][_cell.y - 1]);

		if (_cell.y < m_height - 1 && !Labyrinth[_cell.x][_cell.y + 1].Visited)
			direction.push_back(&Labyrinth[_cell.x][_cell.y + 1]);

		//Åñëè åñòü äîñòóïíûå íàïðàâëåíèÿ, ñëó÷àéíî âûáèðàåì ëþáîå è äîáàâëÿåì åãî â ñòåê ïóòè.
		// Åñëè äîñòóïíûõ íàïðàâëåíèé â äàííîé êëåòêå íåò - óäàëÿåì âåðøèíó ñòåêà, òî åñòü îòêàòûâàåìñÿ äî ïðåäûäóùåãî øàãà ïóòè.

		if (!direction.empty()) {
			CurrentCell = direction[rand() % direction.size()];
			CurrentCell->Visited = true;

			path.push(*CurrentCell);
			direction.clear();

			//Äàëåå îáîçíà÷àåì ïðîõîäû ìåæäó êëåòêàìè
			if (_cell.x != CurrentCell->x) {
				if (_cell.x - CurrentCell->x > 0) {
					Labyrinth[_cell.x][_cell.y].Left = Open;
					Labyrinth[CurrentCell->x][CurrentCell->y].Right = Open;
				}
				else {
					Labyrinth[_cell.x][_cell.y].Right = Open;
					Labyrinth[CurrentCell->x][CurrentCell->y].Left = Open;
				}
			}

			if (_cell.y != CurrentCell->y) {
				if (_cell.y - CurrentCell->y > 0) {
					Labyrinth[_cell.x][_cell.y].Top = Open;
					Labyrinth[CurrentCell->x][CurrentCell->y].Bottom = Open;
				}
				else {
					Labyrinth[_cell.x][_cell.y].Bottom = Open;
					Labyrinth[CurrentCell->x][CurrentCell->y].Top = Open;
				}
			}

			

		}
		else {
			path.pop();
		}
	}

	int index = rand() % m_height;
	Labyrinth[0][index].Left = Open;
	Cell::Entrance = &Labyrinth[0][index];

	index = rand() % m_height;
	Labyrinth[m_width - 1][index].Right = Open;
	Cell::Exit = &Labyrinth[m_width - 1][index];
	direction.clear();

	return Labyrinth;
}

vector<Cell>FindPath(Cell** Labyrinth) {

	vector<Cell> Path;
	stack <Cell> CurrentPath;
	vector<Cell*> direction;
	Cell _cell;

	Cell* CurrentCell = Cell::Entrance;
	CurrentCell->pVisited = true;
	CurrentPath.push(*CurrentCell);

	while(true) {

		_cell = CurrentPath.top();

		if (_cell.x > 0 && Labyrinth[_cell.x][_cell.y].Left == Open && !Labyrinth[_cell.x - 1][_cell.y].pVisited) direction.push_back(&Labyrinth[_cell.x - 1][_cell.y]);
		if (_cell.x < m_width - 1 && Labyrinth[_cell.x][_cell.y].Right == Open && !Labyrinth[_cell.x + 1][_cell.y].pVisited) direction.push_back(&Labyrinth[_cell.x + 1][_cell.y]);
		if (_cell.y > 0 && Labyrinth[_cell.x][_cell.y].Top == Open && !Labyrinth[_cell.x][_cell.y - 1].pVisited) direction.push_back(&Labyrinth[_cell.x][_cell.y - 1]);
		if (_cell.y < m_height - 1 && Labyrinth[_cell.x][_cell.y].Bottom == Open && !Labyrinth[_cell.x][_cell.y + 1].pVisited) direction.push_back(&Labyrinth[_cell.x][_cell.y + 1]);

		if (!direction.empty()) {

			CurrentCell = direction[rand() % direction.size()];
			CurrentCell->pVisited = true;
			CurrentPath.push(*CurrentCell);
			direction.clear();
		}
		else {
			if (!CurrentPath.empty()) CurrentPath.pop();
		}
		if (*CurrentCell == *Cell::Exit) break;
	}

	while (!CurrentPath.empty()) {
		Path.push_back(CurrentPath.top());
		CurrentPath.pop();
	}
	reverse(Path.begin(), Path.end());

	return Path;
}

void Free(Cell** M) {
	for (int i = 0; i < m_width; i++)
	{
		delete[] M[i];
	}
	delete[] M;
}

void Render_Lines(Cell** Labyrinth) {
	for (int i = 0; i < m_width; i++) {
		for (int j = 0; j < m_height; j++) {
			if (Labyrinth[i][j].Bottom == Closed)
			{
				RectangleShape line(Vector2f(CellRelationX, 1));
				line.setFillColor(Color::White);
				line.setPosition({float(Labyrinth[i][j].x) * CellRelationX + margin, float(Labyrinth[i][j].y) * CellRelationY + margin + CellRelationY});
				window.draw(line);
			}
			if (Labyrinth[i][j].Top == Closed)
			{
				RectangleShape line(Vector2f(CellRelationX, 1));
				line.setFillColor(Color::White);
				line.setPosition({float(Labyrinth[i][j].x) * CellRelationX + margin, float(Labyrinth[i][j].y) * CellRelationY + margin});
				window.draw(line);
			}
			if (Labyrinth[i][j].Left == Closed)
			{
				RectangleShape line(Vector2f(CellRelationY, 1));
				line.setFillColor(Color::White);
				line.setPosition({float(Labyrinth[i][j].x) * CellRelationX + margin, float(Labyrinth[i][j].y) * CellRelationY + margin});
				line.setRotation(degrees(90));
				window.draw(line);
			}
			if (Labyrinth[i][j].Right == Closed)
			{
				RectangleShape line(Vector2f(CellRelationY, 1));
				line.setFillColor(Color::White);
				line.setPosition({float(Labyrinth[i][j].x) * CellRelationX + margin + CellRelationX, float(Labyrinth[i][j].y) * CellRelationY + margin});
				line.setRotation(degrees(90));
				window.draw(line);
			}

			if (Labyrinth[i][j] == *Cell::Entrance) {
				RectangleShape line(Vector2f(CellRelationY, 1));
				line.setFillColor(Color::Red);
				line.setPosition({float(Labyrinth[i][j].x) * CellRelationX + margin, float(Labyrinth[i][j].y) * CellRelationY + margin});
				line.setRotation(degrees(90));
				window.draw(line);
			}

			if (Labyrinth[i][j] == *Cell::Exit) {
				RectangleShape line(Vector2f(CellRelationY, 1));
				line.setFillColor(Color::Cyan);
				line.setPosition({float(Labyrinth[i][j].x) * CellRelationX + margin + CellRelationX, float(Labyrinth[i][j].y) * CellRelationY + margin});
				line.setRotation(degrees(90));
				window.draw(line);
			}
		}
	}

}

void Render_Path(vector<Cell>& Path, vector<Cell>::iterator& _it) {
	RectangleShape Dot;

	Dot.setSize(Vector2f(CellRelationX - margin, CellRelationY - margin));
	Dot.setPosition({Path.begin()->x * CellRelationX + 1.5f * margin, Path.begin()->y * CellRelationY + 1.5f * margin});
	if(_it==Path.begin()) Dot.setFillColor(Color::Yellow); else Dot.setFillColor(Color::Green);
	window.draw(Dot);

	for (auto it = Path.begin(); it != _it; it++)
	{
		Dot.setSize(Vector2f(CellRelationX - margin, CellRelationY - margin));
		Dot.setPosition({(it + 1)->x * CellRelationX + 1.5f * margin, (it + 1)->y * CellRelationY + 1.5f * margin});
		if (it == _it - 1) Dot.setFillColor(Color::Yellow); else Dot.setFillColor(Color::Green);
		window.draw(Dot);
	}


}


int main()
{
	srand(time(0));
	
	Cell** Labyrinth = CreateMaze();
	vector<Cell> Path = FindPath(Labyrinth);

	bool Rendered = false;

	while (window.isOpen())
	{
		while (const optional event = window.pollEvent())
		{
			if (event->is<Event::Closed>())
				window.close();
		}
		
		if(!Rendered){

			for (auto it = Path.begin(); it!=Path.end();it++)
			{
				window.clear();
				sleep(milliseconds(1000 / max(m_width, m_height)));
				Render_Path(Path, it);
				Render_Lines(Labyrinth);
				window.display();
				/*
				if (it == Path.end() - 1) {
					Rendered = true;
					wstring str = "The path is found! It has " + to_wstring(Path.size()) + " steps";
					MessageBox(0, str.c_str(), L"Message", MB_OK);
					
				}*/
			}
		}
	}

	Free(Labyrinth);
	return 0;
}
