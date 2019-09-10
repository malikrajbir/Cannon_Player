#include <iostream>
#include <vector>
#include <iomanip>
#include <exception>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#define X first
#define Y second
#define pb push_back

using namespace std;
typedef pair<short, short> pii;

short forw; // for black, forw = -1 (one step forward decreases the row index) and for white, forw = 1 (one step forward increases the row index)
uint _row, _col;

// Class for representing the game board
class Board {
private:
    vector<vector<short>> board; // Board
    unsigned int ssc, esc, stc, etc; // Counts (self soldiers, enemy soldiers, self towns, enemy towns)

    /*
     * Setting the original state of the board
     * @param is_black : whether we are black or white
     */
    void place(bool is_black = true) {
        // Setting for +/-
        short _set = 1;
        if(is_black)
            _set = -1;
        // WHITE
        // Setting white town-halls
        ssc = esc = stc = etc = 0;
        for(short i=0; i<_col; i+=2) {
            board[0][i] = _set*2;
            etc++;
        }
        // Setting white soldiers
        for(short i=1; i<_col; i+=2)
            for(short j=0; j<3; j++) {
                board[j][i] = _set*1;
                esc++;
            }
        // BLACK
        // Setting black town-halls
        for(short i=1; i<_col; i+=2) {
            board[_row-1][i] = -2*_set;
            stc++;
        }
        // Setting black soldiers
        for(short i=0; i<_col; i+=2)
            for(short j=0; j<3; j++) {
                board[_row - 1 - j][i] = -1*_set;
                ssc++;
            }
    }

public:

    /*
     * Constructor
     * @param _col : columns on board
     * @param _row : rows on board
     * @param is_black : whether we are black/white
     */
    Board(bool is_black = true) {
        // Creating the vector
        this->board = vector<vector<short>>(_row, vector<short>(_col, 0));
        // Placing the pieces
        this->place(is_black);
    }

    /*
     * Getting the board vector
     */
    vector<vector<short>>& pos() {
        return this->board;
    }

    /*
     * Getting the respecitive counts
     * @param _key : (1, ssc) (2, esc) (3, stc) (4, etc)
     */
    unsigned int& count(char _key) {
        switch(_key) {
            case 1: return ssc;
            case 2: return esc;
            case 3: return stc;
            case 4: return etc;
        }
        throw new runtime_error("Invalid key. (Must be 1, 2, 3, 4) [Board.count]");
    }

    /*
     * Get new board object when player at curr is moved to next
     * @param curr : current position of a player
     * @param next : new position of the player
     */
    Board move_player(pii curr, pii next, bool _we=true) {
        Board res = *this;
        if(_we) {
            assert(res.pos()[next.X][next.Y] <= 0);
            if(res.pos()[next.X][next.Y] == -1) res.esc--;
            if(res.pos()[next.X][next.Y] == -2) res.etc--;
        }
        else {
            assert(res.pos()[next.X][next.Y] >= 0);
            if(res.pos()[next.X][next.Y] == 1) res.ssc--;
            if(res.pos()[next.X][next.Y] == 2) res.stc--;
        }
        res.pos()[next.X][next.Y] = res.pos()[curr.X][curr.Y];
        res.pos()[curr.X][curr.Y] = 0;
        return res;
    }

    /*
     * Get new board object when cannon destroys an item
     * @param ps : position of the destroyed entity
     */
    Board remove_player(pii ps, bool _we=true) {
        Board res = *this;
        if(_we) {
            assert(res.pos()[ps.X][ps.Y] < 0);
            if(res.pos()[ps.X][ps.Y] == -1) res.esc--;
            if(res.pos()[ps.X][ps.Y] == -2) res.etc--;
        }
        else {
            assert(res.pos()[ps.X][ps.Y] > 0);
            if(res.pos()[ps.X][ps.Y] == 1) res.ssc--;
            if(res.pos()[ps.X][ps.Y] == 2) res.stc--;
        }
        res.pos()[ps.X][ps.Y] = 0;
        return res;
    }

    /*
     * returns true if soldier present at (x, y) is adjacent to enemy soldier
     */
    bool is_adjacent_to_enemy(int x, int y, bool _we=true) {
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        if(y-1 >= 0 && board[x][y-1] == _mark) return true;
        if(y+1 < _col && board[x][y+1] == _mark) return true;
        if(x+l_forw >= 0 && x+l_forw < _row) {
            if(board[x+l_forw][y] == _mark) return true;
            if(y-1 >= 0 && board[x+l_forw][y-1] == _mark) return true;
            if(y+1 < _col && board[x+l_forw][y+1] == _mark) return true;
        }
        return false;
    }

    /*
     * returns the board positions when soldier present at (x, y) retreats if it is adjacent to enemy
     */
    vector<Board> get_positions_for_soldier_retreat(int x, int y, bool _we=true) {
        vector<Board> ans;
        if(!is_adjacent_to_enemy(x, y, _we)) return ans;
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        short new_x = x - 2 * l_forw;
        if(new_x >= 0 && new_x < _row) {
            // * Multiplying with -_mark, to get the correct sign
            if(-_mark*board[new_x][y] <= 0) ans.pb(move_player({x, y}, {new_x, y}, _we));
            if(y-2 >= 0 && -_mark*board[new_x][y-2] <= 0) ans.pb(move_player({x, y}, {new_x, y-2}, _we));
            if(y+2 < _col && -_mark*board[new_x][y+2] <= 0) ans.pb(move_player({x, y}, {new_x, y+2}, _we));
        }
        return ans;
    }

    /*
     * returns the board positions when soldier present at (x, y) moves forward or sideways to an empty
     * space or captures an enemy piece
     */
    vector<Board> get_positions_for_soldier_forward(int x, int y, bool _we=true) {
        vector<Board> ans;
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        if(y-1 >= 0 && -_mark*board[x][y-1] < 0) ans.pb(move_player({x, y}, {x, y-1}));
        if(y+1 < _col && -_mark*board[x][y+1] < 0) ans.pb(move_player({x, y}, {x, y+1}));
        short new_x = x + l_forw;
        if(new_x >= 0 && new_x < _row) {
            // * Multiplying with -_mark, to get the correct sign
            if(-_mark*board[new_x][y] <= 0) ans.pb(move_player({x, y}, {new_x, y}, _we));
            if(y-1 >= 0 && -_mark*board[new_x][y-1] <= 0) ans.pb(move_player({x, y}, {new_x, y-1}, _we));
            if(y+1 < _col && -_mark*board[new_x][y+1] <= 0) ans.pb(move_player({x, y}, {new_x, y+1}, _we));
        }
        return ans;
    }

    /*
     * returns the soldier moves of a soldier present at (x, y)
     */
    vector<Board> get_moves_for_soldiers(int x, int y, bool _we=true) {
        assert(abs(board[x][y]) == 1);
        vector<Board> ans = get_positions_for_soldier_forward(x, y, _we);
        vector<Board> tmp = get_positions_for_soldier_retreat(x, y, _we);
        ans.insert(ans.end(), tmp.begin(), tmp.end());
        return ans;
    }

    /*
     * returns all soldier moves of present state of board
     */
    vector<Board> get_all_soldier_moves(bool _we=true) {
        vector<Board> ans, tmp;
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        for(short i=0; i<_row; i++)
            for(short j=0; j<_col; j++)
                if(board[i][j] == -_mark) {
                    tmp = get_moves_for_soldiers(i, j, _we);
                    ans.insert(ans.end(), tmp.begin(), tmp.end());
                }
        return ans;
    }

    /*
     * Find the cannons on the present board (keeps only the middle element and orientation)
     */
    vector<pair<pii, char>> get_all_cannon_positions(bool _we=true) {
        vector<pair<pii, char>> cannons;
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        // Loop
        for(short i=0; i<_row; i++)
            for(short j=0; j<_col; j++)
                if(board[i][j] == -_mark) {
                    // Searching for vertical cannons (Case : V)
                    if(i != 0 && i != _row-1)
                        if(board[i-1][j] == -_mark && board[i+1][j] == -_mark)
                            cannons.pb(pair<pii, char>({i, j}, 'V'));
                    // Searching for horizontal cannons (Case : H)
                    if(j != 0 && j != _col-1)
                        if(board[i][j-1] == -_mark && board[i][j+1] == -_mark)
                            cannons.pb(pair<pii, char>({i, j}, 'H'));
                    // Searching for diagonal cannons (Case : L or R)
                    if(!(j == 0 || i == 0 || j == _col-1 || i == _row-1)) {
                        if(board[i+1][j-1] == -_mark && board[i-1][j+1] == -_mark)
                            cannons.pb(pair<pii, char>({i, j}, 'L'));
                        if(board[i-1][j-1] == -_mark && board[i+1][j+1] == -_mark)
                            cannons.pb(pair<pii, char>({i, j}, 'R'));
                    }
                }
        return cannons;
    }

    /*
     * All possible movements for the cannons
     */
    vector<Board> get_all_cannon_steps(vector<pair<pii, char>>& cannons, bool _we=true) {
        vector<Board> ans;
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        short xo, yo, xn, yn;
        for(auto& _c : cannons) {
                xo = _c.first.X;
                yo = _c.first.Y;
            switch(_c.second) {
                case 'V' : {
                    yn = yo;
                    // Forward Move
                    xn = xo + 2*l_forw;
                    if(!(yn < 0 || yn >= _col || xn < 0 || xn >= _row))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo-l_forw, yo}, {xn, yn}, _we));
                    // Backward Move
                    xn = xo - 2*l_forw;
                    if(!(yn < 0 || yn >= _col || xn < 0 || xn >= _row))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo+l_forw, yo}, {xn, yn}, _we));
                    break;
                }
                case 'H' : {
                    xn = xo;
                    // Forward Move
                    yn = yo + 2*l_forw;
                    if(!(yn < 0 || yn >= _col || xn < 0 || xn >= _row))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo, yo-l_forw}, {xn, yn}, _we));
                    // Backward move
                    yn = yo - 2*l_forw;
                    if(!(yn < 0 || yn >= _col || xn < 0 || xn >= _row))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo, yo+l_forw}, {xn, yn}, _we));
                    break;
                }
                case 'L' : {
                    // Forward move
                    xn = xo + 2*l_forw;
                    yn = yo - 2*l_forw;
                    if(!(yn < 0 || yn >= _col || xn < 0 || xn >= _row))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo-l_forw, yo+l_forw}, {xn, yn}, _we));
                    // Backward move
                    xn = xo - 2*l_forw;
                    yn = yo + 2*l_forw;
                    if(!(yn < 0 || yn >= _col || xn < 0 || xn >= _row))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo+l_forw, yo-l_forw}, {xn, yn}, _we));
                    break;
                }
                case 'R' : {
                    // Forward move
                    xn = xo + 2*l_forw;
                    yn = yo + 2*l_forw;
                    if(!(yn < 0 || yn >= _col || xn < 0 || xn >= _row))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo-l_forw, yo-l_forw}, {xn, yn}, _we));
                    // Backward move
                    xn = xo - 2*l_forw;
                    yn = yo - 2*l_forw;
                    if(!(yn < 0 || yn >= _col || xn < 0 || xn >= _row))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo+l_forw, yo+l_forw}, {xn, yn}, _we));
                    break;
                }
            }
        }
        return ans;
    }

    /*
     * All possible attacks by cannons
     */
    vector<Board> get_all_cannon_attacks(vector<pair<pii, char>>& cannons, bool _we=true) {
        vector<Board> ans;
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        short xo, yo;
        for(auto& _c : cannons) {
            xo = _c.first.X;
            yo = _c.first.Y;
            switch(_c.second) {
                case 'H' : {
                    if(!(xo+3*l_forw < 0 || xo+3*l_forw >= _row))
                        if(-_mark*board[xo+3*l_forw][yo] < 0 && board[xo+2*l_forw][yo] == 0)
                            ans.pb(remove_player({xo+3*l_forw, yo}, _we));
                    if(!(xo-3*l_forw < 0 || xo-3*l_forw >= _row))
                        if(-_mark*board[xo-3*l_forw][yo] < 0 && board[xo-2*l_forw][yo] == 0)
                            ans.pb(remove_player({xo-3*l_forw, yo}, _we));
                    if(!(xo+4*l_forw < 0 || xo+4*l_forw >= _row))
                        if(-_mark*board[xo+4*l_forw][yo] < 0 && board[xo+2*l_forw][yo] == 0)
                            ans.pb(remove_player({xo+4*l_forw, yo}, _we));
                    if(!(xo-4*l_forw < 0 || xo-4*l_forw >= _row))
                        if(-_mark*board[xo-4*l_forw][yo] < 0 && board[xo-2*l_forw][yo] == 0)
                            ans.pb(remove_player({xo-4*l_forw, yo}, _we));
                    break;
                }
                case 'V' : {
                    if(!(yo+3*l_forw < 0 || yo+3*l_forw >= _col))
                        if(-_mark*board[xo][yo+3*l_forw] < 0 && board[xo][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo+3*l_forw}, _we));
                    if(!(yo-3*l_forw < 0 || yo-3*l_forw >= _col))
                        if(-_mark*board[xo][yo-3*l_forw] < 0 && board[xo][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo-3*l_forw}, _we));
                    if(!(yo+4*l_forw < 0 || yo+4*l_forw >= _row))
                        if(-_mark*board[xo][yo+4*l_forw] < 0 && board[xo][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo+4*l_forw}, _we));
                    if(!(yo-4*l_forw < 0 || yo-4*l_forw >= _row))
                        if(-_mark*board[xo][yo-4*l_forw] < 0 && board[xo][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo-4*l_forw}, _we));
                    break;
                }
                case 'L' : {
                    if(!(yo+3*l_forw < 0 || yo+3*l_forw >= _col || xo-3*l_forw < 0 || xo-3*l_forw >= _row))
                        if(-_mark*board[xo-3*l_forw][yo+3*l_forw] < 0  && board[xo-2*l_forw][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo-3*l_forw, yo+3*l_forw}, _we));
                    if(!(yo-3*l_forw < 0 || yo-3*l_forw >= _col || xo+3*l_forw < 0 || xo+3*l_forw >= _row))
                        if(-_mark*board[xo+3*l_forw][yo-3*l_forw] < 0  && board[xo+2*l_forw][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo+3*l_forw, yo-3*l_forw}, _we));
                    if(!(yo+4*l_forw < 0 || yo+4*l_forw >= _col || xo-4*l_forw < 0 || xo-4*l_forw >= _row))
                        if(-_mark*board[xo-4*l_forw][yo+4*l_forw] < 0  && board[xo-2*l_forw][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo-l_forw*4, yo+4*l_forw}, _we));
                    if(!(yo-4*l_forw < 0 || yo-4*l_forw >= _col || xo+4*l_forw < 0 || xo+4*l_forw >= _row))
                        if(-_mark*board[xo+4*l_forw][yo-4*l_forw] < 0  && board[xo+2*l_forw][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo+4*l_forw, yo-4*l_forw}, _we));
                    break;
                }
                case 'R' : {
                    if(!(yo+3*l_forw < 0 || yo+3*l_forw >= _col || xo+3*l_forw < 0 || xo+3*l_forw >= _row))
                        if(-_mark*board[xo+3*l_forw][yo+3*l_forw] < 0 && board[xo+2*l_forw][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo+3*l_forw, yo+3*l_forw}, _we));
                    if(!(yo-3*l_forw < 0 || yo-3*l_forw >= _col || xo-3*l_forw < 0 || xo-3*l_forw >= _row))
                        if(-_mark*board[xo-3*l_forw][yo-3*l_forw] < 0 && board[xo-2*l_forw][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo-3*l_forw, yo-3*l_forw}, _we));
                    if(!(yo+4*l_forw < 0 || yo+4*l_forw >= _col || xo+4*l_forw < 0 || xo+4*l_forw >= _row))
                        if(-_mark*board[xo+4*l_forw][yo+4*l_forw] < 0 && board[xo+2*l_forw][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo+l_forw*4, yo+4*l_forw}, _we));
                    if(!(yo-4*l_forw < 0 || yo-4*l_forw >= _col || xo-4*l_forw < 0 || xo-4*l_forw >= _row))
                        if(-_mark*board[xo-4*l_forw][yo-4*l_forw] < 0 && board[xo-2*l_forw][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo-4*l_forw, yo-4*l_forw}, _we));
                    break;
                }
            }
        }
        return ans;
    }

    /*
     * Getting all the cannon moves
     */
    vector<Board> get_all_cannon_moves(bool _we=true) {
        // Getting all cannons
        vector<pair<pii, char>> cannons = get_all_cannon_positions(_we);
        vector<Board> ans, temp;
        // Getting all cannons attacks
        ans = get_all_cannon_attacks(cannons, _we);
        // Getting all cannon steps
        temp = get_all_cannon_steps(cannons, _we);
        ans.insert(ans.end(), temp.begin(), temp.end());
        return ans;
    }

    /*
     * prints the present state of board
     */
    void print_board() {
        for(short i=0; i<_row; i++) {
            for(short j=0; j<_col; j++)
                cout << flush << setw(5) << board[i][j] << " ";
            cout << "\n";
        }
        cout << "\n";
    }
};


/*
 * Scoring function for the board
 * @param _b : input board
 */
short score(Board& _b) {
    return (_b.count(1)-_b.count(2))*5 + (_b.count(3)-_b.count(4))*20;
}

int main(int argc, char const *argv[]) {
    /* code */
    bool is_black = 1;
    _row = 8;
    _col = 8;
    forw = (is_black == true) ? -1 : 1;
    Board c = Board(is_black);
    vector<Board> v, temp;
    bool step = true;
    for(int i=0; i<20; i++) {
        c.print_board();
        cout << score(c) << "\n";
        v = c.get_all_cannon_moves(step);
        temp = c.get_all_soldier_moves(step);
        v.insert(v.end(), temp.begin(), temp.end());
        if(step) {
            sort(v.begin(), v.end(), [](Board& b1, Board& b2) { return score(b1) > score(b2); });
        }
        else {
            sort(v.begin(), v.end(), [](Board& b1, Board& b2) { return score(b1) < score(b2); }); 
        }
        c = v[0];
        step = !step;
    }
    return 0;
}
