#include <iostream>
#include <list>
#include <set>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

using namespace std;

#define MAX_SIZE 25
#define SLEEP_TIME 1//3
//#define DEBUG

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
int heu[MAX_SIZE][MAX_SIZE];

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
static const char moveStrings[NMOVES][30] = {"Going right", "Going up", "Going left", "Going down",
											  "Pulling right", "Pulling up", "Pulling left", "Pulling down"};

bool isPull(Move m) {
	return (m >= PULL_UP);
}

bool isBox(Code c) {
	return (c == BOX || c == BOX_OVER_TARGET);
}

class State
{
public:
	State() : f(0), g(0), h(0), trace(), m(0), n(0), pukoX(0), pukoY(0) { };
	void read();
	void print() const;
	static Code meaning(char c);
	const State& operator=(const State& b);
	bool operator<(const State& b) const;
	bool isGoal();
	bool canMove(Move move);
	void apply(Move move, State& child);

	friend class lessF;

	int f, g, h;
	list<Move> trace;
private:
	int m, n;
	int pukoX, pukoY;
	Code board[MAX_SIZE][MAX_SIZE];
};

class lessF
{
public:
	bool operator()(const State& l, const State& r) {
		return l.f > r.f;
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
	printf("f: %5i\tg: %5i\th: %5i", f, g, h);
	printf("\n");
	printf("Path: ");
	for(list<Move>::const_iterator it=trace.begin(); it!=trace.end(); it++)
		printf("%s ", moveStrings[*it]);
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

	// heurística
	memset(heu, 100, sizeof(heu));
	for(int i = 1; i <= m; i++) {
		for(int j = 1; j <= n; j++) {
			if(board[i][j] == TARGET || board[i][j] == BOX_OVER_TARGET || board[i][j] == PKB_OVER_TARGET) {
				for(int k = 1; k <= m; k++) {
					for(int l = 1; l <= n; l++) {
						int dist = abs(k - i) + abs(l - j);
						if(heu[k][l] > dist) {
							heu[k][l] = dist;
						}
					}
				}
			}
		}
	}

#ifdef DEBUG
	for(int i = 1; i <= m; i++) {
		for(int j = 1; j <= n; j++) {
			printf("%i ", heu[i][j]);
		}
		printf("\n");
	}
	printf("\n");
#endif
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
	h = b.h;
	pukoX = b.pukoX;
	pukoY = b.pukoY;
	trace = b.trace;
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
	return false;
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
		if(isBox(board[nextX][nextY])) {
			int nextNextX = nextX + moves[move][X];
			int nextNextY = nextY + moves[move][Y];

			if(board[nextNextX][nextNextY] == WALL || isBox(board[nextNextX][nextNextY]))
				return false;
		}
	}
	else {
		int boxX = pukoX - moves[move][X];
		int boxY = pukoY - moves[move][Y];

		if(!isBox(board[boxX][boxY]))
			return false;

		if(isBox(board[nextX][nextY]))
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
		if(isBox(board[nextX][nextY])) {
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
	child.h = 0;
	child.g = g + 1;

	for(int i = 1; i <= m; i++) {
		for(int j = 1; j <= n; j++) {
			if(isBox(child.board[i][j])) {
				child.h += heu[i][j];
			}
		}
	}

	child.f = child.g + child.h;
	child.trace.push_back(move);
}

list<Move> a_star(const State& start)
{
	int numStatesVisited=0;
	double avgBranchingFactor=0.0;
	int maxBranchingFactor=0;
	int minBranchingFactor=9999;
	int possibleMoves=0;
	priority_queue<State, vector<State>, lessF> open;
	set<State> states;
	open.push(start);
	states.insert(start);
	while(!open.empty()) {
		State best = open.top();
		open.pop();
		if(states.find(best)->g < best.g) {
			continue;
		}
		numStatesVisited++;
		#ifndef DEBUG
		//if(numStatesVisited%10000 == 0) printf("."); //best.print();
		#else
		printf("[%d](queue size: %d) Best in the queue:\n", numStatesVisited, open.size()+1);
		best.print();
		sleep(SLEEP_TIME);
		#endif
		possibleMoves=0;
		for(int m=0; m<NMOVES; m++) {
			if(best.canMove((Move)m)) {
				possibleMoves++;
				#ifdef DEBUG
				printf("canMove! - %s\n", moveStrings[m]);
				#endif
				State child;
				best.apply(m, child);
				if (child.isGoal()) {
					#ifdef DEBUG
					printf("isGoal()\n");
					child.print();
					#endif
					assert(numStatesVisited > 0);
					;
					printf("%i #total branching\n%lf #average branching\n%d #minimum branching\n%d #maximum branching\n%d #num visited\n%d #solution size\n",
							(int) avgBranchingFactor, avgBranchingFactor / numStatesVisited, minBranchingFactor, maxBranchingFactor, numStatesVisited, child.trace.size());
					exit(0);
					return child.trace; //return solution;
				}
				set<State>::iterator cached = states.find(child);
				if(cached == states.end()) {
					states.insert(child);
					open.push(child);
				} else if(cached->g > child.g) {
					states.erase(cached);
					states.insert(child);
					open.push(child);
				}

				#ifdef DEBUG
				printf("Inserting:\n");
				if(cached == states.end()) {
					printf("  new\n");
				} else {
					printf("  cached->g: %i, child.g: %i\n", cached->g, child.g);
				}
				child.print();
				#endif

#ifdef DEBUG
				printf("States:\n");
				for(set<State>::iterator i = states.begin(); i != states.end(); ++i) {
					i->print();
				}
#endif
			}
		}
		avgBranchingFactor += possibleMoves;
		minBranchingFactor = min(minBranchingFactor, possibleMoves);
		maxBranchingFactor = max(maxBranchingFactor, possibleMoves);
	}
	return list<Move>(0, 0);
}

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);

	State b;
	if(argc != 2) {
		printf("Usage: %s <file_name>\n", argv[0]);
		exit(1);
	}
	assert(freopen(argv[1], "r", stdin));
	b.read();
	#ifndef DEBUG
	//b.print();
	#endif
	list<Move> solution = a_star(b);
#ifdef DEBUG
	printf("Solution:\n");
	for(list<Move>::iterator it=solution.begin(); it!=solution.end(); it++)
		printf("%s ", moveStrings[*it]);
	printf("\n");
#endif
	return 0;
}
