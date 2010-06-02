#include <iostream>
#include <list>
#include <set>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <cassert>

using namespace std;

#define MAX_SIZE 10
#define X 0
#define Y 1

enum {
	ERROR_CODE=-1,
	EMPTY,
	PKB,
	PKB_OVER_TARGET,
	WALL,
	BOX,
	BOX_OVER_TARGET,
	TARGET,
	NCODES
};

typedef int Code;
static const char input_codes[NCODES + 1] = ".*-#+!@";

#define PUKOBAN 1

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
static const int moves[NMOVES/2][2] = {{0,1}, {-1,0}, {0,-1}, {1,0}};

bool isPull(Move m) {
	return (m >= PULL_UP);
}

class State
{
public:
	State() : m(0), n(0), pukoX(0), pukoY(0) { };
	void read();
	void print();
	static Code meaning(char c);
	State& operator=(const State& b);
	bool isGoal();
	bool canMove(Move move);

private:
	int m, n;
	int pukoX, pukoY;
	Code board[MAX_SIZE][MAX_SIZE];
	bool insideBoard(int posX, int posY);
};

void State::print()
{
	int i, j;
	for(i = 0; i < m; i++) {
		for(j = 0; j < n; j++)
			printf("%c", input_codes[board[i][j]]);
		printf("\n");
	}
	printf("\n");
}

void State::read()
{
	string line;
	int i = 0, j;
	while(cin >> line) {
		n = line.size();
		//cout << line << endl;
		for(j = 0; j < (int) line.size(); j++) {
			board[i][j] = meaning(line[j]);
			if(board[i][j] == PKB || board[i][j] == PKB_OVER_TARGET) {
				pukoX = i;
				pukoY = j;
			}
		}
		i++;
	}
	m = i;
}

Code State::meaning(char c)
{
	for(int i = 0; i < NCODES; i++) {
		if(input_codes[i] == c)
			return (Code)i;
	}
	return ERROR_CODE;
}

State& State::operator=(const State& b)
{
	m = b.m;
	n = b.n;
	for(int i=0; i<m; i++)
		for(int j=0; j<n; j++)
			board[i][j] = b.board[i][j];
	return *this;
}

bool State::isGoal()
{
	for(int i=0; i<m; i++)
		for(int j=0; j<n; j++)
			if(board[i][j] == BOX)
				return false;
	return true;
}

bool State::canMove(Move move)
{
	int nextX = pukoX + moves[move][X];
	int nextY = pukoY + moves[move][Y];

	if(!insideBoard(nextX, nextY))
		return false;
		
	if(board[nextX][nextY] == WALL)
		return false;
		
	if(!isPull(move)) {
		if(board[nextX][nextY] == BOX || board[nextX][nextY] == BOX_OVER_TARGET) {
			int nextNextX = nextX + moves[move][X];
			int nextNextY = nextY + moves[move][Y];
			
			if(!insideBoard(nextNextX, nextNextY))
				return false;
				
			if(board[nextNextX][nextNextY] == WALL || board[nextNextX][nextNextY] == BOX || board[nextNextX][nextNextY] == BOX_OVER_TARGET)
				return false;
		}
	}
	else {
		int boxX = pukoX - moves[move][X];
		int boxY = pukoY - moves[move][Y];
		
		if(!insideBoard(boxX, boxY))
			return false;
		
		if(board[boxX][boxY] != BOX && board[boxX][boxY] != BOX_OVER_TARGET)
			return false;
			
		if(board[nextX][nextY] == BOX || board[nextX][nextY] == BOX_OVER_TARGET)
			return false;
	}
	
	return true;
}

bool State::insideBoard(int posX, int posY) {
	if(posX < 0 || posY < 0 || posX >= m || posY >= n)
		return false;
	return true;
}

#if 0
//*
list<Move> a_star(const State& start)
{
	priority_queue<State> open;
	set<State> states;
	open.push(start);
	while(!open.empty()) {
		State best = open.top();
		open.pop();
		for(int m=0; m<NMOVES; m++) {
			if(best.canMove((Move)m)) {
				State child;// = best.apply(m);
				if (child.isGoal())
					return list<Move>(0,0); //return solution;
				/*State*/set<State>::iterator cached = states.find(child);
				if(1){//if (cached == states.end() || cached.g > child.g) {
					states.insert(child);
					open.push(child);
				}
			}
		}
	}
}
//*/
#endif

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
	return 0;
}

