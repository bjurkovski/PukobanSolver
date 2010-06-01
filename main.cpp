#include <iostream>

void print();

class Board
{
public:
	void read();
	void print();

private:
	int m, n;
};

int main(int argc, char **argv)
{
	int m, n;
	if(argc != 2) {
		printf("Usage: %s <file_name>\n", argv[0]);
		exit(1);
	}
	freopen(stdin, argv[1]);
	int i = 0;
	while(cin >> line) {
		n = line.size();
		for(j = 0; j < line.size(); j++) {
			tab[i][j] = meaning[line[i]];
		}
	}
	int m = i;
	
	print();
	return 0;
}

void print()
{
	int i, j;
	for(i = 0; i < m; i++) {
		for(j = 0; j < n; j++)
