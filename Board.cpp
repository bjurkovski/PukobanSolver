#include "Board.h"
//#include "common.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>

#include <vector>
#include <string>
#include <bitset>
#include <iostream>
#include <map>
#include <set>

//direções de movimento: vertical, horizontal e duas diagonais
char dx[] = {0, 1, 1, 1}, dy[] = {1, 0, 1, -1};
char horiz[8], vert[8], diag[2][15];
Board::Imp both, adj;
bool g_player;

Board::Board()
{
}

#define SQU(x) ((x) * (x))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

void Board::initial_position()
{
	t[0].reset();
	t[1].reset();

  rep(i, 2)
    repbe(j, 1, 6) {
      set(0, i * 7, j);
      set(1, j, i * 7);
    }
	
	lastMove = 0;
	heu.centro[0] = heu.centro[1] = 15.16;
	heu.cmx[0] = heu.cmx[1] = heu.cmy[0] = heu.cmy[1] = 3.5;
	heu.concentration[0] = heu.concentration[1] = 22.0;
	heu.centralisation[0] = heu.centralisation[1] = -21.66;
	heu.masscenter[0] = heu.masscenter[1] = 24.5;
	heu.quads[0] = heu.quads[1] = 0.0;
	heu.connectedness[0] = heu.connectedness[1] = 1.67;
	heu.uniformity[0] = heu.uniformity[1] = 48;
	/*heu.mobility and heu.total*/

}

void Board::moves(bool player, int fromX, int fromY, MoveList &to)
{
	rep(i, 8)
		horiz[i] = vert[i] = 0;
	rep(i, 15)
		diag[0][i] = diag[1][i] = 0;
	
	both = t[0] | t[1];
	
	rep(i, 8)
		rep(j, 8)
			if(both.test(INDEX(i, j))) {
				horiz[i]++;
				vert[j]++;
				diag[0][DIAG0(i, j)]++;
				diag[1][DIAG1(i, j)]++;
			}
			
	int k, x, y;
	bool valid_move;
	
	int i = fromX,
			j = fromY;
	if(get(player, i, j)) {
		rep(d, 4) {
			k = count(d, i, j);
			for(int m = -1; m <= 1; m += 2) {
				valid_move = true;
				
				x = i + k * dx[d] * m;
				y = j + k * dy[d] * m;
				if(x > 7 || x < 0 ||
					 y > 7 || y < 0) { // fora do tabuleiro
				  continue;
				}
				
				repb(z, 1, k) {
					x = i + z * dx[d] * m;
					y = j + z * dy[d] * m;
					if(get(!player, x, y)) { // barrado por inimigo
						valid_move = false;
						break;
					}
				}
				
				if(valid_move) {
					x = i + k * dx[d] * m;
					y = j + k * dy[d] * m;
					if(!get(player, x, y)) { // não é sobre um amigo
						to.push_back(Move(INDEX(i, j), INDEX(x, y)));
					}
				}
			}
		}
	}
}

bool board_compare(const Board::Comp& l, const Board::Comp& r) {
	return l.first < r.first || (l.first == r.first && l.second < r.second);
}

