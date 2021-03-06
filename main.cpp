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
#include <algorithm>

using namespace std;

#include "constraints.h"
#include "match.h"

#define MAX_SIZE 25
#define SLEEP_TIME 1//3
#define MAX_BRANCH 50
//time limit in minutes
double TIME_LIMIT = 1.0;
//#define WITH_HASH

#define X 0
#define Y 1

#define INDEX(i, j) ((i) * MAX_SIZE + (j))
#define INDEX_X(p) ((p) / MAX_SIZE)
#define INDEX_Y(p) ((p) % MAX_SIZE)

typedef unsigned long long Hash;
#define HASH_SIZE (MAX_SIZE * MAX_SIZE / sizeof(Hash) + 1)
#define HASH_INDEX(i) ((i) / sizeof(Hash))
#define HASH_BIT(i) (((Hash) 1) << (i % sizeof(Hash)))

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
int cost[MAX_TARGET][MAX_TARGET]; //cost matrix

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


class Move
{
	public:
		int boxX, boxY, moveIndex;
		Move() : boxX(0), boxY(0), moveIndex(-1) { };
		Move(int cx, int cy, int move) : boxX(cx), boxY(cy), moveIndex(move) { };
		friend ostream& operator<<(ostream& out, const Move& m);
};

typedef Move MoveList[MAX_BRANCH];
typedef bitset<MAX_SIZE * MAX_SIZE> BitBoard;

static int movesBegin=0;
static int movesEnd=NMOVES;
static const int moves[NMOVES][2] = {{0,1}, {-1,0}, {0,-1}, {1,0}, {0,1}, {-1,0}, {0,-1}, {1,0}};
static const char moveStrings[NMOVES][30] = {"Going right", "Going up", "Going left", "Going down",
											  "Pulling right", "Pulling up", "Pulling left", "Pulling down"};

ostream& operator<<(ostream& out, const Move& m)
{
	out << moveStrings[m.moveIndex] << "(" << m.boxX << ", " << m.boxY << ")";
	return out;
}

static enum Heuristic {
	MIN_DISTANCE,
	MATCH
} heuristic = MATCH;

static enum Criteria {
	PUKO_MOVES,
	BOX_MOVES
} criteria = PUKO_MOVES;

bool component = false;
BitBoard visited;

bool isPull(Move m) {
	return (m.moveIndex >= PULL_UP);
}

class State
{
public:
	State() : f(0), g(0), h(0), puko_moves(0), box_moves(0), move(Move(0,0,-1)), pukoX(0), pukoY(0), box(), father(NULL) { };
	void read();
	void print() const;
	static Code meaning(char c);
	const State& operator=(const State& b);
	bool operator<(const State& b) const;
	bool isGoal();
	bool canMove(Move move);
	bool canMoveBox(Move move, int cx, int cy);
	void apply(Move move, State* child);
	void calculateNewF();
	int traceSize();
	list<Move> trace() const;
	void showSolution(list<Move> l);
	// coloca movimentos possíveis em to
	// retorna quantidade de movimentos possíveis
	int getPossibleMoves(MoveList &to); 
	int getPossibleMovesComponent(MoveList &to, int curRes, int cx, int cy);

	friend class lessF;

	int f, g, h;
	int puko_moves, box_moves;
	//list<Move> trace;
	Move move;
private:
	int pukoX, pukoY;
	BitBoard box;
	State *father;
#ifdef WITH_HASH
	Hash hash[HASH_SIZE];
#endif
};

Code board[MAX_SIZE][MAX_SIZE];
static int m, n;

class lessF
{
public:
	bool operator()(const State* l, const State* r) {
		return l->f > r->f;
	}
};

class lessBoard
{
public:
	bool operator()(const State* l, const State* r) {
		return *l < *r;
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
	printf("f: %5i\tg: %5i\th: %5i\tpm: %5i\tbm: %5i", f, g, h, puko_moves, box_moves);
	printf("\n");
#ifdef WITH_HASH
	printf("hash: ");
	for(i = 0; i < (int)HASH_SIZE; i++) {
		printf("%10lli ", hash[i]);
	}
	printf("\n");
#endif
	printf("Path: ");
	list<Move> tr = trace();
	for(list<Move>::const_iterator it=tr.begin(); it!=tr.end(); it++) {
		assert(0 <= it->moveIndex && it->moveIndex < NMOVES);
		//printf("%s ", moveStrings[it->moveIndex]);
		cout << *it << " ";
	}
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
	f = b.f;
	g = b.g;
	h = b.h;
	pukoX = b.pukoX;
	pukoY = b.pukoY;
	father = b.father;
	move = b.move;
	box = b.box;
	puko_moves = b.puko_moves;
	box_moves = b.box_moves;
#ifdef WITH_HASH
	memcpy(hash, b.hash, sizeof(hash));
#endif
	return *this;
}

bool State::operator<(const State& b) const
{
	if(pukoX < b.pukoX) return true;
	if(pukoX > b.pukoX) return false;
	if(pukoY < b.pukoY) return true;
	if(pukoY > b.pukoY) return false;

#ifdef WITH_HASH
	for(unsigned int i = 0; i < HASH_SIZE; i++) {
		if(hash[i] < b.hash[i]) return true;
		if(hash[i] > b.hash[i]) return false;
	}
#else
	unsigned int l = box._Find_first(),
			 				 r = b.box._Find_first();

	while(1) {
		if(l < r) return false;     // 1 == box[l] > b.box[l] == 0
		else if(r < l) return true; // 0 == box[r] < b.box[r] == 1
		if(l == box.size()) return false;
		l = box._Find_next(l);
		r = b.box._Find_next(r);
	}
#endif

	return false;
}

bool State::isGoal()
{
	unsigned int p = box._Find_first();

	do {
		if(board[INDEX_X(p)][INDEX_Y(p)] != TARGET)
			return false;
	} while((p = box._Find_next(p)) != box.size());

	return true;
}

bool State::canMove(Move move)
{
	int nextX = move.boxX/*pukoX*/ + moves[move.moveIndex][X];
	int nextY = move.boxY/*pukoY*/ + moves[move.moveIndex][Y];

	if(board[nextX][nextY] == WALL)
		return false;

	if(!isPull(move)) {
		if(box[INDEX(nextX, nextY)]) {
			int nextNextX = nextX + moves[move.moveIndex][X];
			int nextNextY = nextY + moves[move.moveIndex][Y];

			if(board[nextNextX][nextNextY] == WALL || box[INDEX(nextNextX, nextNextY)])
				return false;
		}
	} else {
		int boxX = move.boxX/*pukoX*/ - moves[move.moveIndex][X];
		int boxY = move.boxY/*pukoY*/ - moves[move.moveIndex][Y];

		if(!box[INDEX(boxX, boxY)] || box[INDEX(nextX, nextY)])
			return false;
	}

	return true;
}

bool State::canMoveBox(Move move, int cx, int cy)
{
	int nextX = move.boxX/*cx*/ + moves[move.moveIndex][X];
	int nextY = move.boxY/*cy*/ + moves[move.moveIndex][Y];

	if(board[nextX][nextY] == WALL)
		return false;

	if(!isPull(move)) {
		if(box[INDEX(nextX, nextY)]) {
			int nextNextX = nextX + moves[move.moveIndex][X];
			int nextNextY = nextY + moves[move.moveIndex][Y];

			if(board[nextNextX][nextNextY] == WALL || box[INDEX(nextNextX, nextNextY)])
				return false;
		} else {
			return false;
		}
	} else {
		int boxX = move.boxX/*cx*/ - moves[move.moveIndex][X];
		int boxY = move.boxY/*cy*/ - moves[move.moveIndex][Y];

		if(!box[INDEX(boxX, boxY)] || box[INDEX(nextX, nextY)])
			return false;
	}

	return true;
}

void State::apply(Move move, State* child)
{
	*child = *this;
	int nextX = move.boxX/*pukoX*/ + moves[move.moveIndex][X];
	int nextY = move.boxY/*pukoY*/ + moves[move.moveIndex][Y];
	bool isBoxMove = false;

	if(!isPull(move)) {
		if(box[INDEX(nextX, nextY)]) {
			int nextNextX = nextX + moves[move.moveIndex][X];
			int nextNextY = nextY + moves[move.moveIndex][Y];

			child->box[INDEX(nextX, nextY)] = 0;
			child->box[INDEX(nextNextX, nextNextY)] = 1;

			isBoxMove = true;
		}
	}
	else {
		int boxX = move.boxX/*pukoX*/ - moves[move.moveIndex][X];
		int boxY = move.boxY/*pukoY*/ - moves[move.moveIndex][Y];

		child->box[INDEX(boxX, boxY)] = 0;
		child->box[INDEX(move.boxX/*pukoX*/, move.boxY/*pukoY*/)] = 1;

		isBoxMove = true;
	}

	child->pukoX = nextX;
	child->pukoY = nextY;

	child->puko_moves++;
	child->box_moves += isBoxMove;

	if(criteria == PUKO_MOVES) {
		child->g = g + 1;
	} else if(criteria == BOX_MOVES) {
		child->g = g + isBoxMove;
	}

	child->father = this;
	child->move = move;
}

void State::calculateNewF()
{
	h = 0;

	if(heuristic == MIN_DISTANCE) {
		int mindist = INT_MAX;

		unsigned int p = box._Find_first();
		do {
			int i = INDEX_X(p), j = INDEX_Y(p);
			h += heu[i][j][0];
			int tdist = dist[pukoX][pukoY][i][j];
			if(tdist < mindist) {
				mindist = tdist;
			}
		} while((p = box._Find_next(p)) != box.size());

#ifdef DEBUG
		printf("mindist: %i\n", mindist);
#endif
		h += mindist;
	}
	else if(heuristic == MATCH) {
		int mindist = INT_MAX;
		int boxIndex = 0;

		unsigned int p = box._Find_first();
		do {
			int i = INDEX_X(p), j = INDEX_Y(p);

			for(int t = 1; t <= ntarget; t++) {
				cost[boxIndex][t - 1] = -heu[i][j][t];
			}
			boxIndex++;
			int tdist = dist[pukoX][pukoY][i][j];
			if(tdist < mindist) {
				mindist = tdist;
			}
		} while((p = box._Find_next(p)) != box.size());

		int tmatch = hungarian();
		h += tmatch;
		h += mindist;
#ifdef DEBUG
		printf("tmatch: %i\n", tmatch);
		printf("mindist: %i\n", mindist);
#endif
	}

	f = g + h;

#ifdef WITH_HASH
	unsigned int p = box._Find_first();
	memset(hash, 0, sizeof(hash));
	do {
		hash[HASH_INDEX(p)] |= HASH_BIT(p);
	} while((p = box._Find_next(p)) != box.size());
#endif
}

int State::traceSize()
{
	int res = 0;
	State *f = father;
	while(f) {
		res++;
		f = f->father;
	}
	return res;
}

list<Move> State::trace() const
{
	list<Move> res;
	const State *f = this;
	while(f->move.moveIndex != -1) {
		res.push_back(f->move);
		f = f->father;
	}
	reverse(res.begin(), res.end());
	return res;
}

void State::showSolution(list<Move> solution)
{
	State *cur;
	cur = this;
	for(list<Move>::iterator it=solution.begin(); it!=solution.end(); it++) {
		State *temp = new State();
		cur->print();
		cur->apply(*it, temp);
		temp->calculateNewF();
		cur = temp;
	}
	cur->print();
}

int State::getPossibleMoves(MoveList &to)
{
	if(!component) {
		int res = 0;
		for(int m = movesBegin; m < movesEnd; m++) {
			if(canMove(Move(pukoX,pukoY,m))) {
				to[res++] = Move(pukoX,pukoY,m);
			}
		}
		return res;
	} else {
		visited.reset();
		//TODO to virar global, para não ficar passando
		return getPossibleMovesComponent(to, 0, pukoX, pukoY);
	}
}

int State::getPossibleMovesComponent(MoveList &to, int curRes, int cx, int cy)
{
#ifdef DEBUG
	printf("%s(%i, %i, %i)\n", __FUNCTION__, curRes, cx, cy);
#endif

	visited[INDEX(cx, cy)] = 1;

	bool movedFromHere = false;
	for(int d = 0; d < 4; d++)
	{
		int nx = cx + moves[d][0];
		int ny = cy + moves[d][1];
				
		if(!visited[INDEX(nx, ny)] && board[nx][ny] != WALL) {
			if(box[INDEX(nx, ny)] && !movedFromHere) {
				movedFromHere = true;
#ifdef DEBUG
				printf("moving from (%i, %i)!\n", cx, cy);
#endif
				for(int m = movesBegin; m < movesEnd; m++) {
					if(canMoveBox(Move(cx,cy,m), cx, cy)) {
#ifdef DEBUG
						printf("  can move to %s (%i)\n", moveStrings[m], curRes);
#endif
						to[curRes++] = Move(cx,cy,m);
					}
				}
			} else if (!box[INDEX(nx, ny)]) {
				curRes = getPossibleMovesComponent(to, curRes, nx, ny);
			}
		}
	}

	return curRes; //0;
}

double toSeconds(double minutes)
{
	return minutes*60;
}

list<Move> a_star(State* start)
{
	int numStatesVisited=0;
	double avgBranchingFactor=0.0;
	int maxBranchingFactor=0;
	int minBranchingFactor=9999;
	int numPossibleMoves=0;
	clock_t begin = clock();

	priority_queue<State*, vector<State*>, lessF> open;
	set<State*, lessBoard> states;
	open.push(start);
	states.insert(start);
	MoveList possibleMoves;

	while(!open.empty()) {
		if((double)(clock()-begin)/CLOCKS_PER_SEC >= toSeconds(TIME_LIMIT)) { printf("time limit (%.2lf minutes)\n", TIME_LIMIT); exit(0); }
		State* best = open.top();
		open.pop();
		if((*(states.find(best)))->g < best->g) {
			continue;
		}
		numStatesVisited++;
#ifndef DEBUG
		//if(numStatesVisited%100000 == 0) printf("."); //best.print();
#else
		printf("[%d](queue size: %d) Best in the queue:\n", numStatesVisited, open.size()+1);
		best->print();
		sleep(SLEEP_TIME);
#endif
		numPossibleMoves = best->getPossibleMoves(possibleMoves);
		//printf("num moves: %d\n", numPossibleMoves);
		for(int m = 0; m < numPossibleMoves; m++) {

			#ifdef DEBUG
			printf("canMove!");
			cout << possibleMoves[m];
			#endif
			State* child = new State();
			best->apply(possibleMoves[m], child);

			if (child->isGoal()) {
#ifdef DEBUG
				printf("isGoal()\n");
				child->print();
#endif
				assert(numStatesVisited > 0);
				double time = (double)(clock() - begin)/CLOCKS_PER_SEC;
				//printf("%lfs #time elapsed\n%i #total branching\n%lf #average branching\n%d #minimum branching\n%d #maximum branching\n%d #num visited\n%d #puko moves\n%d #box_moves\n", time, (int) avgBranchingFactor, avgBranchingFactor / numStatesVisited, minBranchingFactor, maxBranchingFactor, numStatesVisited, child->puko_moves, child->box_moves);
				printf("%lf,%i,%lf,%d,%d,%d,%d,%d\n", time, (int) avgBranchingFactor, avgBranchingFactor / numStatesVisited, minBranchingFactor, maxBranchingFactor, numStatesVisited, child->puko_moves, child->box_moves);
				return child->trace(); //return solution;
			}
			set<State*>::iterator cached = states.find(child);
			if(cached == states.end()) {
				child->calculateNewF();
				states.insert(child);
				open.push(child);
			} else if((*cached)->g > child->g) {
				states.erase(cached);
				child->calculateNewF();
				states.insert(child);
				open.push(child);
			}

#ifdef DEBUG
			printf("Inserting:\n");
			if(cached == states.end()) {
				printf("  new\n");
			} else {
				printf("  cached->g: %i, child->g: %i\n", (*cached)->g, child->g);
			}
			child->print();
#endif

#ifdef DEBUG
			printf("States:\n");
			for(set<State*>::iterator i = states.begin(); i != states.end(); ++i) {
				(*i)->print();
			}
#endif
		}
		avgBranchingFactor += numPossibleMoves;
		minBranchingFactor = min(minBranchingFactor, numPossibleMoves);
		maxBranchingFactor = max(maxBranchingFactor, numPossibleMoves);
	}
	printf("search ended with no solution!\n");
	return list<Move>(0, Move());
}

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);

	State b;
	if(argc < 3 || argc > 7 || (argc >= 3 && atof(argv[2]) <= 0.001)) {
		printf("Usage: %s <file_name> <max_time> [push-pull/push-only] [match/min-distance] [puko-moves/box-moves] [component]\n", argv[0]);
		printf("Default: push-pull match puko-moves\n");
		exit(1);
	}
	assert(freopen(argv[1], "r", stdin));
	TIME_LIMIT = atof(argv[2]);
	b.read();
#ifndef DEBUG
	//b.print();
#endif
	
	for(int i = 3; i < argc; i++) {
		if(!strcmp(argv[i], "push-pull")) {
			movesEnd = NMOVES;
		} else if(!strcmp(argv[i], "push-only")) {
			movesEnd = NMOVES / 2;
		} else if(!strcmp(argv[i], "match")) {
			heuristic = MATCH;
		} else if(!strcmp(argv[i], "min-distance")) {
			heuristic = MIN_DISTANCE;
		} else if(!strcmp(argv[i], "puko-moves")) {
			criteria = PUKO_MOVES;
		} else if(!strcmp(argv[i], "box-moves")) {
			criteria = BOX_MOVES;
		} else if(!strcmp(argv[i], "component")) {
			component = true;
		} else {
			printf("Option %s doesn't exist!\n", argv[i]);
			exit(1);
		}
	}
			
	list<Move> solution = a_star(&b);
#ifdef DEBUG 
	printf("Solution:\n");
	int i = 0;
	for(list<Move>::iterator it=solution.begin(); it!=solution.end(); it++)
		printf("%i: %s\n", i++, moveStrings[it->moveIndex]);
	printf("\n");
	b.showSolution(solution);
#endif
	return 0;
}

