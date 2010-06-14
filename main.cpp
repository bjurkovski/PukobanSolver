#include <iostream>
#include <list>
#include <set>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <ctime>
#include <climits>
#include <bitset>

using namespace std;

#include "constraints.h"
#include "match.h"

#define MAX_TARGET 10
#define SLEEP_TIME 1//3

#define X 0
#define Y 1

#define INDEX(i, j) ((i) * MAX_SIZE + (j))

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
int heu[MAX_SIZE][MAX_SIZE][MAX_TARGET];
int dist[MAX_SIZE][MAX_SIZE][MAX_SIZE][MAX_SIZE];

int ntarget = 0; // number of targets
int cost[MAX_SIZE][MAX_SIZE]; //cost matrix

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
static int movesBegin=0;
static int movesEnd=NMOVES;
static const int moves[NMOVES][2] = {{0,1}, {-1,0}, {0,-1}, {1,0}, {0,1}, {-1,0}, {0,-1}, {1,0}};
static const char moveStrings[NMOVES][30] = {"Going right", "Going up", "Going left", "Going down",
											  "Pulling right", "Pulling up", "Pulling left", "Pulling down"};

enum {
	MIN_DISTANCE,
	MATCH
};

static int heuristic = MATCH;

bool isPull(Move m) {
	return (m >= PULL_UP);
}

bool isBox(Code c) {
	return (c == BOX || c == BOX_OVER_TARGET);
}

class State
{
public:
	State() : f(0), g(0), h(0), trace(), m(0), n(0), pukoX(0), pukoY(0), box() { };
	void read();
	void print() const;
	static Code meaning(char c);
	const State& operator=(const State& b);
	bool operator<(const State& b) const;
	bool isGoal();
	bool canMove(Move move);
	void apply(Move move, State& child);
	void calculateNewF();

	friend class lessF;

	int f, g, h;
	list<Move> trace;
private:
	int m, n;
	int pukoX, pukoY;
	bitset<MAX_SIZE*MAX_SIZE> box;
};

Code board[MAX_SIZE][MAX_SIZE];

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
			printf("%c", i == pukoX && j == pukoY ? board[i][j] == EMPTY ? input_codes[PKB] : input_codes[PKB_OVER_TARGET]  : box[INDEX(i, j)] ? board[i][j] == TARGET ? input_codes[BOX_OVER_TARGET] : input_codes[BOX] : input_codes[board[i][j]]);
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
			} else if(board[i][j] == BOX || board[i][j] == BOX_OVER_TARGET) {
				if(board[i][j] == BOX) board[i][j] = EMPTY;
				else board[i][j] = TARGET;
				box[INDEX(i, j)] = 1;
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

	// floyd warshal
	memset(dist, 1, sizeof(dist));
	static const int dx[] = {-1, 0, 1,  0},
	                 dy[] = { 0, 1, 0, -1};
	for(int i = 1; i <= m; i++) {
		for(int j = 1; j <= n; j++) {
			dist[i][j][i][j] = 0;
			for(int d = 0; d < 4; d++) {
				int x = i + dx[d],
						y = j + dy[d];
				if(x > 0 && x <= m && y > 0 && y <= n && board[x][y] != WALL) {
					dist[i][j][x][y] = 1;
				}
			}
		}
	}
	for(int kx = 1; kx <= m; kx++) {
		for(int ky = 1; ky <= n; ky++) {
			for(int ix = 1; ix <= m; ix++) {
				for(int iy = 1; iy <= n; iy++) {
					for(int jx = 1; jx <= m; jx++) {
						for(int jy = 1; jy <= n; jy++) {
							dist[ix][iy][jx][jy] = min(dist[ix][iy][jx][jy], dist[ix][iy][kx][ky] + dist[kx][ky][jx][jy]);
						}
					}
				}
			}
		}
	}

#ifdef DEBUG
	for(int i = 1; i <= m; i++) {
		for(int j = 1; j <= n; j++) {
			printf("dist[%2i][%2i]:\n", i, j);
			for(int k = 1; k <= m; k++) {
				for(int l = 1; l <= n; l++) {
					printf("%2i ", dist[i][j][k][l] == 0x01010101 ? -1 : dist[i][j][k][l]);
				}
				printf("\n");
			}
			printf("\n");
		}
	}
#endif

	// heurística
	memset(heu, -1, sizeof(heu));
	for(int i = 1; i <= m; i++) {
		for(int j = 1; j <= n; j++) {
			if(board[i][j] == TARGET) {
				ntarget++;
				for(int x = 1; x <= m; x++) {
					for(int y = 1; y <= n; y++) {
						if(board[x][y] != WALL) {
							heu[x][y][ntarget] = dist[i][j][x][y];
						}
					}
				}
			}
		}
	}

	assert(ntarget < MAX_TARGET);

	for(int i = 1; i <= m; i++) {
		for(int j = 1; j <= n; j++) {
			heu[i][j][0] = heu[i][j][1];
			for(int t = 2; t <= ntarget; t++) {
				if(heu[i][j][t] < heu[i][j][0]) {
					heu[i][j][0] = heu[i][j][t];
				}
			}
		}
	}

#ifdef DEBUG
	for(int t = 0; t <= ntarget; t++) {
		printf("heu[%2i]:\n", t);
		for(int i = 1; i <= m; i++) {
			for(int j = 1; j <= n; j++) {
				printf("%2i ", heu[i][j][t]);
			}
			printf("\n");
		}
		printf("\n");
	}
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
	box = b.box;
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
			if(box[INDEX(i, j)] < b.box[INDEX(i, j)]) return true;
			else if (box[INDEX(i, j)] > b.box[INDEX(i, j)]) return false;
	return false;
}

bool State::isGoal()
{
	for(int i = 1; i <= m; i++) {
		for(int j = 1; j <= n; j++) {
			if(box[INDEX(i, j)] && board[i][j] != TARGET)
				return false;
		}
	}
	return true;
}

bool State::canMove(Move move)
{
	int nextX = pukoX + moves[move][X];
	int nextY = pukoY + moves[move][Y];

	if(board[nextX][nextY] == WALL)
		return false;

	if(!isPull(move)) {
		if(box[INDEX(nextX, nextY)]) {
			int nextNextX = nextX + moves[move][X];
			int nextNextY = nextY + moves[move][Y];

			if(board[nextNextX][nextNextY] == WALL || box[INDEX(nextNextX, nextNextY)])
				return false;
		}
	}
	else {
		int boxX = pukoX - moves[move][X];
		int boxY = pukoY - moves[move][Y];

		if(!box[INDEX(boxX, boxY)])
			return false;

		if(box[INDEX(nextX, nextY)])
			return false;
	}

	return true;
}

void State::apply(Move move, State& child)
{
	child = *this;
	int nextX = pukoX + moves[move][X];
	int nextY = pukoY + moves[move][Y];
	bool isBoxMove = false;

	if(!isPull(move)) {
		if(box[INDEX(nextX, nextY)]) {
			int nextNextX = nextX + moves[move][X];
			int nextNextY = nextY + moves[move][Y];

			child.box[INDEX(nextX, nextY)] = 0;
			child.box[INDEX(nextNextX, nextNextY)] = 1;

			isBoxMove = true;
		}
	}
	else {
		int boxX = pukoX - moves[move][X];
		int boxY = pukoY - moves[move][Y];

		child.box[INDEX(boxX, boxY)] = 0;
		child.box[INDEX(pukoX, pukoY)] = 1;

		isBoxMove = true;
	}

	child.pukoX = nextX;
	child.pukoY = nextY;

	// para minimizar número de movimentos de caixas
	//child.g = g + isBoxMove;
	// para minimizar número de movimentos do sukoban
	child.g = g + 1;

	child.trace.push_back(move);
}

void State::calculateNewF()
{
	h = 0;

	if(heuristic == MIN_DISTANCE) {
		int mindist = INT_MAX;
		for(int i = 1; i <= m; i++) {
			for(int j = 1; j <= n; j++) {
				if(box[INDEX(i, j)]) {
					h += heu[i][j][0];
					int tdist = dist[pukoX][pukoY][i][j];
					if(tdist < mindist) {
						mindist = tdist;
					}
				}
			}
		}
#ifdef DEBUG
		printf("mindist: %i\n", mindist);
#endif
		h += mindist;
	}
	else if(heuristic == MATCH) {
		int mindist = INT_MAX;
		int boxIndex = 0;
		for(int i = 1; i <= m; i++) {
			for(int j = 1; j <= n; j++) {
				if(box[INDEX(i, j)]) {
					for(int t = 1; t <= ntarget; t++) {
						cost[boxIndex][t - 1] = heu[i][j][t];
					}
					boxIndex++;
					int tdist = dist[pukoX][pukoY][i][j];
					if(tdist < mindist) {
						mindist = tdist;
					}
				}
			}
		}
		int tmatch = hungarian();
		h += tmatch;
		h += mindist;
#ifdef DEBUG
		printf("tmatch: %i\n", tmatch);
		printf("mindist: %i\n", mindist);
#endif
	}

	f = g + h;
}

list<Move> a_star(const State& start)
{
	int numStatesVisited=0;
	double avgBranchingFactor=0.0;
	int maxBranchingFactor=0;
	int minBranchingFactor=9999;
	int possibleMoves=0;
	clock_t begin = clock();

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
		for(int m=movesBegin; m<movesEnd; m++) {
			if(best.canMove((Move)m)) {
				possibleMoves++;
				#ifdef DEBUG
				printf("canMove! - %s\n", moveStrings[m]);
				#endif
				State child;
				best.apply(m, child);
				//child.calculateNewF();
				if (child.isGoal()) {
#ifdef DEBUG
					printf("isGoal()\n");
					child.print();
#endif
					assert(numStatesVisited > 0);
					double time = (double)(clock() - begin)/CLOCKS_PER_SEC;
					printf("%lfs #time elapsed\n%i #total branching\n%lf #average branching\n%d #minimum branching\n%d #maximum branching\n%d #num visited\n%d #solution size\n",
							time, (int) avgBranchingFactor, avgBranchingFactor / numStatesVisited, minBranchingFactor, maxBranchingFactor, numStatesVisited, child.trace.size());
#ifndef DEBUG
					// leva muito tempo desalocando a memória
					exit(0);
#endif
					return child.trace; //return solution;
				}
				set<State>::iterator cached = states.find(child);
				if(cached == states.end()) {
					child.calculateNewF();
					states.insert(child);
					open.push(child);
				} else if(cached->g > child.g) {
					states.erase(cached);
					child.calculateNewF();
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
	if(argc < 2 && argc > 4) {
		printf("Usage: %s <file_name> [push-only/pull-only] [match/min-distance]\n", argv[0]);
		exit(1);
	}
	assert(freopen(argv[1], "r", stdin));
	b.read();
#ifndef DEBUG
	b.print();
#endif
	if(argc>=3) {
		if(!strcmp(argv[2], "push-only"))
			movesEnd /= 2;
		else if(!strcmp(argv[2], "pull-only"))
			movesBegin += 4;
	}
	if(argc==4) {
		if(!strcmp(argv[3], "match"))
			heuristic = MATCH;
		else if(!strcmp(argv[3], "min-distance"))
			heuristic = MIN_DISTANCE;
	}
	list<Move> solution = a_star(b);
#ifdef DEBUG
	printf("Solution:\n");
	for(list<Move>::iterator it=solution.begin(); it!=solution.end(); it++)
		printf("%s\n", moveStrings[*it]);
	printf("\n");
#endif
	return 0;
}
