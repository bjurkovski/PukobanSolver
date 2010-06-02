#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cassert>

using namespace std;

#define MAX_SIZE 10

typedef enum eCode {
	EMPTY,
	PKB,
	PKB_OVER_TARGET,
	WALL,
	BOX,
	BOX_OVER_TARGET,
	TARGET,
	NCODES
} Code;

static const char input_codes[NCODES + 1] = ".*-#+!@";

#define PUKOBAN 1

class Board
{
public:
	void read();
	void print();
	static int meaning(char c);

private:
	int m, n;
	int board[MAX_SIZE][MAX_SIZE];
};


void Board::print()
{
	int i, j;
	for(i = 0; i < m; i++) {
		for(j = 0; j < n; j++)
			printf("%c", input_codes[board[i][j]]);
		printf("\n");
	}
	printf("\n");
}

void Board::read()
{
	string line;
	int i = 0, j;
	while(cin >> line) {
		n = line.size();
		//cout << line << endl;
		for(j = 0; j < (int) line.size(); j++) {
			board[i][j] = meaning(line[j]);
		}
		i++;
	}
	m = i;
}

int Board::meaning(char c)
{
	for(int i = 0; i < NCODES; i++) {
		if(input_codes[i] == c)
			return i;
	}
	return -1;
}


int main(int argc, char **argv)
{
	Board b;
	if(argc != 2) {
		printf("Usage: %s <file_name>\n", argv[0]);
		exit(1);
	}
	assert(freopen(argv[1], "r", stdin));
	b.read();
	b.print();
	return 0;
}

