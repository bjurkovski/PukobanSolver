#include <iostream>
#include <list>
#include <set>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

using namespace std;

#define MAX_SIZE 10

#define X 0
#define Y 1

enum {
	ERROR_CODE=-1,
	EMPTY = 0,
	TARGET = 1,
	BOX,
	BOX_OVER_TARGET,
	PKB,
	PKB_OVER_TARGET,
	WALL,
	NCODES
};

typedef int Code;
static const char input_codes[NCODES + 1] = ".@+!*-#";

enum {
	UP,
	LEFT,
	DOWN,
	RIGHT,
	PULL_UP,
	PULL_LEFT,
	PULL_DOWN,
	PULL_RIGHT,
	NMOVES
};

typedef int Move;
static const int moves[NMOVES][2] = {{0,1}, {-1,0}, {0,-1}, {1,0}, {0,1}, {-1,0}, {0,-1}, {1,0}};

bool isPull(Move m) {
	return (m >= PULL_UP);
}

class State
{
public:
	State() : f(0), g(0), m(0), n(0), pukoX(0), pukoY(0) { };
	void read();
	void print() const;
	static Code meaning(char c);
	const State& operator=(const State& b);
	bool operator<(const State& b) const;
	bool isGoal();
	bool canMove(Move move);
	void apply(Move move, State& child);

	friend class lessF;

	int f, g;

private:
	int m, n;
	int pukoX, pukoY;
	Code board[MAX_SIZE][MAX_SIZE];
};

class lessF
{
public:
	bool operator()(const State& l, const State& r) {
		return l.f < r.f;
	}
};

void State::print() const
{
	int i, j;
	for(i = 0; i <= m + 1; i++) {
		for(j = 0; j <= n + 1; j++)
			printf("%c", i == pukoX && j == pukoY ? board[i][j] == EMPTY ? input_codes[PKB] : input_codes[PKB_OVER_TARGET]  : input_codes[board[i][j]]);
		printf("\n");
	}
	printf("f: %5i\tg: %5i", f, g);
	printf("\n");
}

void State::read()
{
	string line;
	int i = 1, j;
	while(cin >> line) {
		n = line.size();
		//cout << line << endl;
		for(j = 1; j <= n; j++) {
			board[i][j] = meaning(line[j - 1]);
			if(board[i][j] == PKB || board[i][j] == PKB_OVER_TARGET) {
				if(board[i][j] == PKB) board[i][j] = EMPTY;
				else board[i][j] = TARGET;
				pukoX = i;
				pukoY = j;
			}
		}
		i++;
	}
	m = i - 1;

	assert(m + 1 < MAX_SIZE && n + 1 < MAX_SIZE);

	for(i = 0; i <= m + 1; i++)
		board[i][0] = board[i][n + 1] = WALL;

	for(j = 0; j <= n + 1; j++)
		board[0][j] = board[m + 1][j] = WALL;
}

Code State::meaning(char c)
{
	for(int i = 0; i < NCODES; i++) {
		if(input_codes[i] == c)
			return (Code)i;
	}
	printf("'%c'\n", c);
	assert(false);
	/* ERROR_CODE = -1 dá pau no print na hora de acessar a string */
	return ERROR_CODE;
}

const State& State::operator=(const State& b)
{
	m = b.m;
	n = b.n;
	f = b.f;
	g = b.g;
	memcpy(board, b.board, sizeof(board));
	return *this;
}

bool State::operator<(const State& b) const
{
	if(pukoX < b.pukoX) return true;
	if(pukoX > b.pukoX) return false;
	if(pukoY < b.pukoY) return true;
	if(pukoY > b.pukoY) return false;
	for(int i = 1; i <= m; i++)
		for(int j = 1; j <= n; j++)
			if(board[i][j] < b.board[i][j]) return true;
			else if(board[i][j] > b.board[i][j]) return false;
	//assert(false); não deveria chegar aqui, mas chega
	return true;
}

bool State::isGoal()
{
	for(int i=1; i<=m; i++)
		for(int j=1; j<=n; j++)
			if(board[i][j] == BOX)
				return false;
	return true;
}

bool State::canMove(Move move)
{
	int nextX = pukoX + moves[move][X];
	int nextY = pukoY + moves[move][Y];

	if(board[nextX][nextY] == WALL)
		return false;
		
	if(!isPull(move)) {
		// fazer isBox()
		if(board[nextX][nextY] == BOX || board[nextX][nextY] == BOX_OVER_TARGET) {
			int nextNextX = nextX + moves[move][X];
			int nextNextY = nextY + moves[move][Y];
			
			if(board[nextNextX][nextNextY] == WALL || board[nextNextX][nextNextY] == BOX || board[nextNextX][nextNextY] == BOX_OVER_TARGET)
				return false;
		}
	}
	else {
		int boxX = pukoX - moves[move][X];
		int boxY = pukoY - moves[move][Y];
		
		if(board[boxX][boxY] != BOX && board[boxX][boxY] != BOX_OVER_TARGET)
			return false;
			
		if(board[nextX][nextY] == BOX || board[nextX][nextY] == BOX_OVER_TARGET)
			return false;
	}
	
	return true;
}

void State::apply(Move move, State& child)
{
	child = *this;
	int nextX = pukoX + moves[move][X];
	int nextY = pukoY + moves[move][Y];

	if(!isPull(move)) {
		// fazer isBox()
		if(board[nextX][nextY] == BOX || board[nextX][nextY] == BOX_OVER_TARGET) {
			int nextNextX = nextX + moves[move][X];
			int nextNextY = nextY + moves[move][Y];
			child.board[nextX][nextY] -= BOX;
			child.board[nextNextX][nextNextY] += BOX;
		}
	}
	else {
		int boxX = pukoX - moves[move][X];
		int boxY = pukoY - moves[move][Y];

		child.board[boxX][boxY] -= BOX;
		child.board[pukoX][pukoY] += BOX;
	}

	child.pukoX = nextX;
	child.pukoY = nextY;
	child.f = f + 1;
}

list<Move> a_star(const State& start)
{
	priority_queue<State, vector<State>, lessF> open;
	set<State> states;
	open.push(start);
	while(!open.empty()) {
		State best = open.top();
		open.pop();
		best.print();
		for(int m=0; m<NMOVES; m++) {
			printf("m: %i\n", m);
			if(best.canMove((Move)m)) {
				printf("canMove()\n");
				State child;
				best.apply(m, child);
				if (child.isGoal()) {
					printf("isGoal()\n");
					child.print();
					return list<Move>(0,0); //return solution;
				}
				set<State>::iterator cached = states.find(child);
				if(cached == states.end() || cached->g > child.g) {
					printf("Inserting\n");
					states.insert(child);
					open.push(child);
				}
			}
		}
	}
	return list<Move>(0, 0);
}

int main(int argc, char **argv)
{
	State b;
	if(argc != 2) {
		printf("Usage: %s <file_name>\n", argv[0]);
		exit(1);
	}
	assert(freopen(argv[1], "r", stdin));
	b.read();
	b.print();
	a_star(b);
	return 0;
}

