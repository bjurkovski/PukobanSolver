#ifndef __IA_BOARD_H__
#define __IA_BOARD_H__

//#include "common.h"
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define rep(i, n) repi(i, n, 1)
#define repi(i, n, x) for(int i = 0; i < n; i += x)
#define repbe(i, b, n) repbc(i, b, n, <=)
#define repb(i, b, n) repbc(i, b, n, <)
#define repbc(i, b, n, c) for(int i = b; i c n; ++i)
#define reps(i, n) for(unsigned int i = 0; i < n.size(); ++i)

#define INDEX(i, j) (((i) << 3) + (j))
//#include "MinimaxEnemy.h"

#include <bitset>
#include <map>
#include <vector>
#include <set>

using namespace std;

#include "constraints.h"

#define DIAG0(i, j) ((i) + (j))
#define DIAG1(i, j) ((i) - (j) + 7)

#define MoveOt(x, y)  (((x) << 6) + (y))

#define INDEX_X(i) ((i) >> 3)
#define INDEX_Y(i) ((i) & 7)

#define INDEX_FROM(i) ((i) >> 6)
#define INDEX_TO(i) ((i) & 63)

extern char horiz[8], vert[8], diag[2][15];
extern std::bitset<64> adj, both;
extern bool g_player;
extern char dx[4], dy[4];
typedef double HeuType;

typedef struct S_Heuristics {
    HeuType centro[2], cmx[2], cmy[2], concentration[2],
	centralisation[2], masscenter[2], quads[2], connectedness[2],
	minx[2], miny[2], maxx[2], maxy[2], uniformity[2], mobility[2], total,
	cmxt[2], cmyt[2], walls[2];
	unsigned int count[2];
} Heuristics;
    

class Board
{
public:
	//representação das peças de um dos jogadores
  typedef std::bitset<64> Imp; 
	//representação das peças de um dos jogadores para fins de comparação
	typedef unsigned long long ImpComp;
	//representação do board para comparação
	typedef std::pair<ImpComp, ImpComp> Comp;
	//conjunto de boards
	typedef std::set<Comp, bool (*)(const Comp&, const Comp&)> Set;
	//representação dos movimentos (origem, destino) em coordenadas absolutas [0,63]
  typedef std::pair<char, char> Move; 
  typedef std::vector<Move> MoveList; //Lista de movimentos
	// representação otimizada (origem << 6 + destino) em coordenadas absolutas [0, 63]
	typedef unsigned int MoveOt;
	typedef MoveOt MoveListOt[8 * 12]; //Lista de movimentos otimizada
  
  Board();
  ~Board() {}
  void initial_position();
  
  inline void set(bool player, char i, char j, bool val = true) {
    //t[player].set(INDEX(i, j), val);
    t[player][INDEX(i, j)] = val;
  }
  
  inline bool get(bool player, char i, char j) const {
    return t[player]._Unchecked_test(INDEX(i, j)); //profiling
    //return t[player][INDEX(i, j)];
  }
  
  /* efetua o movimento move pelo jogador player */
	inline void move(bool player, Move move) {
		t[player][move.first] = t[!player][move.second] = 0;
		t[player][move.second] = 1;
		lastMove = 0;
	}

	/* efetua o movimento num tabuleiro novo */
	inline Board move_result(bool player, MoveOt move) const {
		Board res = *this;
		//cout << res.t[0] << endl << t[0] << endl;
		res.t[player][INDEX_FROM(move)] = res.t[!player][INDEX_TO(move)] = 0;
		res.t[player][INDEX_TO(move)] = 1;
		res.lastMove = 0;
		return res;
	}
		
	/* efetua o movimento num tabuleiro novo */
	inline Board move_result(bool player, Move move) {
		Board res = *this;
		res.t[player][move.first] = res.t[!player][move.second] = 0;
		res.t[player][move.second] = 1;
		res.lastMove = 0;
		return res;
	}
		
	/* gera uma lista com os movimentos partindo de fromX, fromY */
	void moves(bool player, int fromX, int fromY, MoveList &to);

	/* gera uma lista com todos os movimentos possíveis do jogador player */
	/* copy paste da moves, necessária por uma questão de eficiência */
	int all_moves(bool player, MoveListOt &to, int * mobility) const {
		rep(i, 8)
			horiz[i] = vert[i] = 0;
		rep(i, 15)
			diag[0][i] = diag[1][i] = 0;
		
		both = t[0] | t[1];
		
		int mob = 0;
		char pos = both._Find_first();
		do {
			int i = INDEX_X(pos), j = INDEX_Y(pos);
			horiz[i]++;
			vert[j]++;
			diag[0][DIAG0(i, j)]++;
			diag[1][DIAG1(i, j)]++;
		} while((pos = both._Find_next(pos)) != 64);
				
		int k, x, y, res = 0;
		bool valid_move;
		
		pos = t[player]._Find_first();
		do {
			int i = INDEX_X(pos), j = INDEX_Y(pos);
			rep(d, 4) {
				k = count(d, i, j);
				for(int m = -1; m <= 1; m += 2) {
					valid_move = true;
					int tmp1 = dx[d] * m;
					int tmp2 = dy[d] * m;
					
					x = i + k * tmp1;
					y = j + k * tmp2;
					if(x > 7 || x < 0 ||
						 y > 7 || y < 0) { // fora do tabuleiro
						continue;
					}
					
					x = i;
					y = j;
					repb(z, 1, k) {
						x += tmp1;
						y += tmp2;
						if(get(!player, x, y)) { // barrado por inimigo
							valid_move = false;
							break;
						}
					}
					
					if(valid_move) {
						x += tmp1;
						y += tmp2;
						if(!get(player, x, y)) { // não é sobre um amigo
						    if(get(!player, x, y)) {
								if( x < 1 || x > 6 ) {
								    if( y < 1 || y > 6 )
								        mob += 2;
								    else
								        mob += 4;
								} else {
									if( y < 1 || y < 6 )
									    mob += 4;
									else
									    mob += 8;
								}
							} else {
								if( x < 1 || x > 6 ) {
								    if( y < 1 || y > 6 )
								        mob += 1;
								    else
								        mob += 2;
								} else {
									if( y < 1 || y < 6 )
									    mob += 2;
									else
									    mob += 4;
								}
							}
							to[res++] = MoveOt(pos, INDEX(x, y));
						}
					}
				}
			}
		} while((pos = t[player]._Find_next(pos)) != 64);
		*mobility = mob;
		return res;
	}

	/* retorna a quantidade de peças vivas */
	inline char count() const {
		return t[0].count() + t[1].count();
	}
  
  /* retorna a quantidade de casas que a peça na posição (i, j) pode andar na direção d */
  inline char count(int d, char i, char j) const {
    switch(d) {
      case 1: return vert[(int) j];
      case 0: return horiz[(int) i];
      case 2: return diag[1][DIAG1(i, j)];
      case 3: return diag[0][DIAG0(i, j)];
    }
    return 0;
  }

	/* retorna uma cópia do tabuleiro */
	Board *copy() {
		Board *res = new Board();
		res->t[0] = t[0];
		res->t[1] = t[1];
		res->heu = heu;
		res->lastMove = lastMove;
		return res;
	}

	/* testa se o jogo terminou */
	bool is_end() const {
		return t[0].count() < 2 || t[1].count() < 2 || win(0) || win(1);
	}

	/* função para determinar se as peças do jogador g_player estão adjacentes,
	   partindo da peça na posição (i, j) */
	unsigned char adjacent(char i, char j) const {
		static char adjdx[] = {-1, -1, -1,  0, 0,  1, 1, 1},
				   adjdy[] = {-1,  0,  1, -1, 1, -1, 0, 1};
		char x, y, z; 
		unsigned char res = 1;
		adj._Unchecked_set(INDEX(i, j));
		rep(d, 8) {
			x = i + adjdx[d];
			y = j + adjdy[d];
			if(x > 7 || x < 0 || y > 7 || y < 0) continue;
			z = INDEX(x, y);
			if(adj._Unchecked_test(z) || !t[g_player]._Unchecked_test(z)) continue;
			res += adjacent(x, y);
		}
		return res;
	}

	/* testa se o jogador player venceu o jogo */
	inline bool win(bool player) const {
		adj.reset();
		g_player = player;
		char pos = t[player]._Find_first();
		return pos == 64 || adjacent(INDEX_X(pos), INDEX_Y(pos)) == t[player].count();
	}

	/* converte um bitset para facilitar comparação entre tabuleiros */
	inline Board::ImpComp to_imp_comp(const Imp &t) {
		Board::ImpComp res = 0;
		char pos = t._Find_first();
		do {
			res |= ((Board::ImpComp) 1) << pos;
		} while((pos = t._Find_next(pos)) != 64);
		return res;
	}

	/* converte um board para comparação */
	inline Board::Comp to_comp() {
		return Board::Comp(to_imp_comp(t[0]), to_imp_comp(t[1]));
	}
	friend class MinimaxEnemy;

  Heuristics heu;
  MoveOt lastMove;

private:
  Imp t[2]; //t[i] representa o tabuleiro somente com as peças do jogador i
	bitset<MAX_SIZE * MAX_SIZE> b;
};

/* compara boards */
bool board_compare(const Board::Comp& l, const Board::Comp& r);

#endif
