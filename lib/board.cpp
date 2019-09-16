#include <bits/stdc++.h>
#define X first
#define Y second
#define pb push_back
#define params 23

using namespace std;
typedef pair<short, short> pii;

short forw; // for black, forw = -1 (one step forward decreases the row index) and for white, forw = 1 (one step forward increases the row index)
unsigned int _row, _col;
double inf = __DBL_MAX__;

float w[] = {250, 250, 250, 250, 350, 350, 350, 350,
                           150, 200, 150, 200, 100, 150, 200,
                           50, -75, -50, -25,
                           150, -150, 
                           100, -200
                           };


// Class for representing the game board
class Board {
private:
    vector<vector<short>> board, posi; // Board
    unsigned int ssc, esc, stc, etc; // Counts (self soldiers, enemy soldiers, self towns, enemy towns)
    vector<vector<pii>> sold;
    double scr;

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
        short k = 0;
        for(short i=1; i<_col; i+=2)
            for(short j=0; j<3; j++) {
                board[j][i] = _set*1;
                sold[!is_black][k] = {j, i};
                posi[j][i] = k++;
                esc++;
            }
        // BLACK
        // Setting black town-halls
        for(short i=1; i<_col; i+=2) {
            board[_row-1][i] = -2*_set;
            stc++;
        }
        // Setting black soldiers
        k = 0;
        for(short i=0; i<_col; i+=2)
            for(short j=0; j<3; j++) {
                board[_row - 1 - j][i] = -1*_set;
                sold[is_black][k] = {_row - 1 - j, i};
                posi[_row - 1 - j][i] = k++;
                ssc++;
            }
    }

public:

    string prev_step; // Step that lead to this game...
    float param[params];

    /*
     * Constructor
     * @param _col : columns on board
     * @param _row : rows on board
     * @param is_black : whether we are black/white
     */
    Board(bool is_black = true) {
        // Creating the vector
        this->board = vector<vector<short>>(_row, vector<short>(_col, 0));
        this->posi = vector<vector<short>>(_row, vector<short>(_col, -1));
        this->sold = vector<vector<pii>>(2, vector<pii>(_col*3/2));
        // Placing the pieces
        this->place(is_black);
        this->scr = __DBL_MAX__;
    }

    /*
     * Getting the board vector
     */
    vector<vector<short>>& pos() {
        return this->board;
    }

    /*
     * Getting the respecitive counts
     * @param _key : (1, ssc) (-1, esc) (2, stc) (-2, etc)
     */
    unsigned int& count(char _key) {
        switch(_key) {
            case 1: return ssc;
            case -1: return esc;
            case 2: return stc;
            case -2: return etc;
        }
        throw new runtime_error("Invalid key. (Must be -2, -1, 1, 2) [Board.count]");
    }

    vector<vector<pii>>& soldiers() {
        return this->sold;
    }

    vector<vector<short>>& get_posi() {
        return this->posi;
    }

    vector<vector<short>> get_board() const {
        return board;
    }

    /*
     * Get new board object when player at curr is moved to next
     * @param curr : current position of a player
     * @param next : new position of the player
     */
    Board move_player(pii curr, pii next, bool _we) {
        Board res = *this;
        res.set_score(__DBL_MAX__);
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
        short ind = posi[curr.X][curr.Y];
        assert(ind != -1);
        assert(res.soldiers()[_we][ind] == curr);
        res.soldiers()[_we][ind] = next;
        if(posi[next.X][next.Y] != -1) {
            res.soldiers()[!_we][posi[next.X][next.Y]] = {-1, -1};
        }
        res.pos()[next.X][next.Y] = res.pos()[curr.X][curr.Y];
        res.pos()[curr.X][curr.Y] = 0;
        res.get_posi()[curr.X][curr.Y] = -1;
        res.get_posi()[next.X][next.Y] = ind;
        res.prev_step = "S "+to_string(curr.Y)+" "+to_string(curr.X)+" M "+to_string(next.Y)+" "+to_string(next.X);
        return res;
    }

    /*
     * Get new board object when cannon destroys an item
     * @param ps : position of the destroyed entity
     */
    Board remove_player(pii crr, pii ps, bool _we) {
        //cout<<flush<<"hi";
        Board res = *this;
        res.set_score(__DBL_MAX__);
        if(_we) {
            assert(res.pos()[ps.X][ps.Y] <= 0);
            if(res.pos()[ps.X][ps.Y] == -1) res.esc--;
            if(res.pos()[ps.X][ps.Y] == -2) res.etc--;
        }
        else {
            assert(res.pos()[ps.X][ps.Y] >= 0);
            if(res.pos()[ps.X][ps.Y] == 1) res.ssc--;
            if(res.pos()[ps.X][ps.Y] == 2) res.stc--;
        }
        short ind = posi[ps.X][ps.Y];
        if(ind != -1) {
            res.get_posi()[ps.X][ps.Y] = -1;
            res.soldiers()[!_we][ind] = {-1, -1};
        }
        res.pos()[ps.X][ps.Y] = 0;
        res.prev_step = "S "+to_string(crr.Y)+" "+to_string(crr.X)+" B "+to_string(ps.Y)+" "+to_string(ps.X);
        return res;
    }

    /*
     * Get the next state given a "step" string
     */
    Board next_state(string _stp) {
        assert(_stp.length() == 11);
        if(_stp[6] == 'M') {
            return move_player({(int)(_stp[4]-'0'),(int)(_stp[2]-'0')}, {(int)(_stp[10]-'0'),(int)(_stp[8]-'0')}, false);
        }
        else if(_stp[6] == 'B') {
            return remove_player({(int)(_stp[4]-'0'),(int)(_stp[2]-'0')}, {(int)(_stp[10]-'0'),(int)(_stp[8]-'0')}, false);
        }
        else {
            cerr << flush << _stp.length() << " " << _stp << " " << "Error in [next_state]\n";
            throw new runtime_error("Error");
        }
    }

    /*
     * returns true if soldier present at (x, y) is adjacent to enemy soldier
     */
    bool is_adjacent_to_enemy(int x, int y, bool _we) {
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
    void get_positions_for_soldier_retreat(vector<Board>& ans, int x, int y, bool _we) {
        if(!is_adjacent_to_enemy(x, y, _we)) return;
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        short new_x = x - 2 * l_forw;
        if(new_x >= 0 && new_x < _row) {
            // * Multiplying with -_mark, to get the correct sign
            if(-_mark*board[new_x][y] <= 0) ans.pb(move_player({x, y}, {new_x, y}, _we));
            if(y-2 >= 0 && -_mark*board[new_x][y-2] <= 0) ans.pb(move_player({x, y}, {new_x, y-2}, _we));
            if(y+2 < _col && -_mark*board[new_x][y+2] <= 0) ans.pb(move_player({x, y}, {new_x, y+2}, _we));
        }
    }

    /*
     * returns the board positions when soldier present at (x, y) moves forward or sideways to an empty
     * space or captures an enemy piece
     */
    void get_positions_for_soldier_forward(vector<Board>& ans, int x, int y, bool _we) {
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        if(y-1 >= 0 && -_mark*board[x][y-1] < 0) ans.pb(move_player({x, y}, {x, y-1}, _we));
        if(y+1 < _col && -_mark*board[x][y+1] < 0) ans.pb(move_player({x, y}, {x, y+1}, _we));
        short new_x = x + l_forw;
        if(new_x >= 0 && new_x < _row) {
            // * Multiplying with -_mark, to get the correct sign
            if(-_mark*board[new_x][y] <= 0) ans.pb(move_player({x, y}, {new_x, y}, _we));
            if(y-1 >= 0 && -_mark*board[new_x][y-1] <= 0) ans.pb(move_player({x, y}, {new_x, y-1}, _we));
            if(y+1 < _col && -_mark*board[new_x][y+1] <= 0) ans.pb(move_player({x, y}, {new_x, y+1}, _we));
        }
    }

    /*
     * returns the soldier moves of a soldier present at (x, y)
     */
    void get_moves_for_soldiers(vector<Board>& ans, int x, int y, bool _we) {
        //assert(abs(board[x][y]) == 1);
        get_positions_for_soldier_forward(ans, x, y, _we);
        get_positions_for_soldier_retreat(ans, x, y, _we);
    }

    /*
     * returns all soldier moves of present state of board
     */
    void get_all_soldier_moves(vector<Board>& ans, bool _we) {
        for(auto& i :sold[_we])
            if(i.X != -1 || i.Y != -1)
                get_moves_for_soldiers(ans, i.X, i.Y, _we);
    }

    /*
     * Find the cannons on the present board (keeps only the middle element and orientation)
     */
    vector<pair<pii, char>> get_all_cannon_positions(bool _we) {
        vector<pair<pii, char>> cannons;
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short i, j;
        for(short ind=0; ind<sold[_we].size(); ind++)
        {
            i = sold[_we][ind].X; j = sold[_we][ind].Y;
            if(i == -1 && j == -1) continue;
            // Searching for vertical cannons (Case : V)
            if(i != 0 && i != _row-1)
                if(board[i-1][j] == -_mark && board[i+1][j] == -_mark)
                    cannons.pb({{i, j}, 'V'});
            // Searching for horizontal cannons (Case : H)
            if(j != 0 && j != _col-1)
                if(board[i][j-1] == -_mark && board[i][j+1] == -_mark)
                    cannons.pb({{i, j}, 'H'});
            // Searching for diagonal cannons (Case : L or R)
            if(!(j == 0 || i == 0 || j == _col-1 || i == _row-1)) {
                if(board[i+1][j-1] == -_mark && board[i-1][j+1] == -_mark)
                    cannons.pb({{i, j}, 'L'});
                if(board[i-1][j-1] == -_mark && board[i+1][j+1] == -_mark)
                    cannons.pb({{i, j}, 'R'});
            }
        }
        return cannons;
    }

    /*
     * All possible movements for the cannons
     */
    void get_all_cannon_steps(vector<Board>& ans, vector<pair<pii, char>>& cannons, bool _we) {
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        short xo, yo, xn, yn;
        for(auto& _c : cannons) {
                xo = _c.X.X;
                yo = _c.X.Y;
            switch(_c.Y) {
                case 'V' : {
                    yn = yo;
                    // Forward Move
                    xn = xo + 2*l_forw;
                    if(!(xn < 0 || xn >= _row))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo-l_forw, yo}, {xn, yn}, _we));
                    // Backward Move
                    xn = xo - 2*l_forw;
                    if(!(xn < 0 || xn >= _row))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo+l_forw, yo}, {xn, yn}, _we));
                    break;
                }
                case 'H' : {
                    xn = xo;
                    // Forward Move
                    yn = yo + 2*l_forw;
                    if(!(yn < 0 || yn >= _col))
                        if(board[xn][yn] == 0)
                            ans.pb(move_player({xo, yo-l_forw}, {xn, yn}, _we));
                    // Backward move
                    yn = yo - 2*l_forw;
                    if(!(yn < 0 || yn >= _col))
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
    }

    /*
     * All possible attacks by cannons
     */
    void get_all_cannon_attacks(vector<Board>& ans, vector<pair<pii, char>>& cannons, bool _we) {
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        short xo, yo;
        for(auto& _c : cannons) {
            xo = _c.X.X;
            yo = _c.X.Y;
            switch(_c.Y) {
                case 'V' : {
                    if(!(xo+3*l_forw < 0 || xo+3*l_forw >= _row))
                        if(-_mark*board[xo+3*l_forw][yo] < 0 && board[xo+2*l_forw][yo] == 0)
                            ans.pb(remove_player({xo, yo}, {xo+3*l_forw, yo}, _we));
                    if(!(xo-3*l_forw < 0 || xo-3*l_forw >= _row))
                        if(-_mark*board[xo-3*l_forw][yo] < 0 && board[xo-2*l_forw][yo] == 0)
                            ans.pb(remove_player({xo, yo}, {xo-3*l_forw, yo}, _we));
                    if(!(xo+4*l_forw < 0 || xo+4*l_forw >= _row))
                        if(-_mark*board[xo+4*l_forw][yo] < 0 && board[xo+2*l_forw][yo] == 0)
                            ans.pb(remove_player({xo, yo}, {xo+4*l_forw, yo}, _we));
                    if(!(xo-4*l_forw < 0 || xo-4*l_forw >= _row))
                        if(-_mark*board[xo-4*l_forw][yo] < 0 && board[xo-2*l_forw][yo] == 0)
                            ans.pb(remove_player({xo, yo}, {xo-4*l_forw, yo}, _we));
                    break;
                }
                case 'H' : {
                    if(!(yo+3*l_forw < 0 || yo+3*l_forw >= _col))
                        if(-_mark*board[xo][yo+3*l_forw] < 0 && board[xo][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo, yo+3*l_forw}, _we));
                    if(!(yo-3*l_forw < 0 || yo-3*l_forw >= _col))
                        if(-_mark*board[xo][yo-3*l_forw] < 0 && board[xo][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo, yo-3*l_forw}, _we));
                    if(!(yo+4*l_forw < 0 || yo+4*l_forw >= _row))
                        if(-_mark*board[xo][yo+4*l_forw] < 0 && board[xo][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo, yo+4*l_forw}, _we));
                    if(!(yo-4*l_forw < 0 || yo-4*l_forw >= _row))
                        if(-_mark*board[xo][yo-4*l_forw] < 0 && board[xo][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo, yo-4*l_forw}, _we));
                    break;
                }
                case 'L' : {
                    if(!(yo+3*l_forw < 0 || yo+3*l_forw >= _col || xo-3*l_forw < 0 || xo-3*l_forw >= _row))
                        if(-_mark*board[xo-3*l_forw][yo+3*l_forw] < 0  && board[xo-2*l_forw][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo-3*l_forw, yo+3*l_forw}, _we));
                    if(!(yo-3*l_forw < 0 || yo-3*l_forw >= _col || xo+3*l_forw < 0 || xo+3*l_forw >= _row))
                        if(-_mark*board[xo+3*l_forw][yo-3*l_forw] < 0  && board[xo+2*l_forw][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo+3*l_forw, yo-3*l_forw}, _we));
                    if(!(yo+4*l_forw < 0 || yo+4*l_forw >= _col || xo-4*l_forw < 0 || xo-4*l_forw >= _row))
                        if(-_mark*board[xo-4*l_forw][yo+4*l_forw] < 0  && board[xo-2*l_forw][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo-l_forw*4, yo+4*l_forw}, _we));
                    if(!(yo-4*l_forw < 0 || yo-4*l_forw >= _col || xo+4*l_forw < 0 || xo+4*l_forw >= _row))
                        if(-_mark*board[xo+4*l_forw][yo-4*l_forw] < 0  && board[xo+2*l_forw][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo+4*l_forw, yo-4*l_forw}, _we));
                    break;
                }
                case 'R' : {
                    if(!(yo+3*l_forw < 0 || yo+3*l_forw >= _col || xo+3*l_forw < 0 || xo+3*l_forw >= _row))
                        if(-_mark*board[xo+3*l_forw][yo+3*l_forw] < 0 && board[xo+2*l_forw][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo+3*l_forw, yo+3*l_forw}, _we));
                    if(!(yo-3*l_forw < 0 || yo-3*l_forw >= _col || xo-3*l_forw < 0 || xo-3*l_forw >= _row))
                        if(-_mark*board[xo-3*l_forw][yo-3*l_forw] < 0 && board[xo-2*l_forw][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo-3*l_forw, yo-3*l_forw}, _we));
                    if(!(yo+4*l_forw < 0 || yo+4*l_forw >= _col || xo+4*l_forw < 0 || xo+4*l_forw >= _row))
                        if(-_mark*board[xo+4*l_forw][yo+4*l_forw] < 0 && board[xo+2*l_forw][yo+2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo+4*l_forw, yo+4*l_forw}, _we));
                    if(!(yo-4*l_forw < 0 || yo-4*l_forw >= _col || xo-4*l_forw < 0 || xo-4*l_forw >= _row))
                        if(-_mark*board[xo-4*l_forw][yo-4*l_forw] < 0 && board[xo-2*l_forw][yo-2*l_forw] == 0)
                            ans.pb(remove_player({xo, yo}, {xo-4*l_forw, yo-4*l_forw}, _we));
                    break;
                }
            }
        }
    }

    /*
     * Getting all the cannon moves
     */
    void get_all_cannon_moves(vector<Board> &ans, bool _we) {
        // Getting all cannons
        vector<pair<pii, char>> cannons = get_all_cannon_positions(_we);
        // Getting all cannons attacks
        get_all_cannon_attacks(ans, cannons, _we);
        // Getting all cannon steps
        get_all_cannon_steps(ans, cannons, _we);
    }


    /*
     * Function for finding the count of cannons present (for both us and enemy)
     */
    double cannon_scr(bool _we) {
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        double sc=0;
        short i, j;
        for(short ind=0; ind<sold[_we].size(); ind++) {
            i = sold[_we][ind].X; j = sold[_we][ind].Y;
            if(i == -1 && j == -1) continue;
            // Searching for vertical cannons (Case : V)
            if(i != 0 && i != _row-1)
                if(board[i-1][j] == -_mark && board[i+1][j] == -_mark)
                    sc+=1.5;
            // Searching for horizontal cannons (Case : H)
            if(j != 0 && j != _col-1)
                if(board[i][j-1] == -_mark && board[i][j+1] == -_mark)
                    sc-=0.5;
            // Searching for diagonal cannons (Case : L or R)
            if(!(j == 0 || i == 0 || j == _col-1 || i == _row-1)) {
                if(board[i+1][j-1] == -_mark && board[i-1][j+1] == -_mark)
                    sc+=1;
                if(board[i-1][j-1] == -_mark && board[i+1][j+1] == -_mark)
                    sc+=1;
            }
        }
        return sc;
    }

    /*
     * Finding the no. of soldiers that are unsafe (for both enemy and us)
     */
    short unsafe_sold(bool cmp, bool _we) {
        short _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        short l_forw = -_mark*forw; // Local forward
        short i, j;
        vector<vector<int>> _p = vector<vector<int>>(_row, vector<int>(_col, 0));
        for(short ind=0; ind<sold[_we].size(); ind++) {
            i = sold[_we][ind].X; j = sold[_we][ind].Y;
            if(i == -1 && j == -1) continue;
            if(j>0)
                _p[i][j-1] = 1;
            if(j<_col-1)
                _p[i][j+1] = 1;
            if(i+l_forw >= 0 && i+l_forw < _row) {
                if(j > 0)
                    _p[i+l_forw][j-1] = 1;
                if(j < _col-1)
                    _p[i+l_forw][j+1] = 1;
                _p[i+l_forw][j] = 1;
            }
        }
        short sm = 0;
        _we = (cmp)?(_we):(!_we);
        // _mark = (_we)?(-1):(1); // Mark of the enemy soldier
        for(short ind=0; ind<sold[_we].size(); ind++) {
            i = sold[_we][ind].X; j = sold[_we][ind].Y;
            if(i == -1 && j == -1) continue;
            if(_p[i][j] == 1) sm++;
        }
        _p.clear();
        return sm;
    }

    void set_score(double val) {
        scr = val;
    }

    vector<Board> get_all_moves(bool _we) {
        vector<Board> ans;
        get_all_cannon_moves(ans, _we);
        get_all_soldier_moves(ans, _we);
        if(_we)
            sort(ans.begin(), ans.end(), [](Board& b1, Board& b2) { return b1.score() > b2.score(); });
        else
            sort(ans.begin(), ans.end(), [](Board& b1, Board& b2) { return b1.score() < b2.score(); });
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

        /*
    * Scoring function for the board
    * @param _b : input board
    */
    double score() {
        if(scr != __DBL_MAX__) return scr;
        scr = ssc;
        if(etc == 2) {
            if(stc == 4) scr += 50000;
            else scr += 30000;
            return scr;
        }
        if(stc == 2) {
            if(etc == 4) scr -= 50000;
            else scr -= 30000;
            return scr;
        }

        if(esc == 0) {
            if(stc == 4)
                scr += ((etc == 3) ? 50000 : 30000);
            else
                scr += ((etc == 3) ? 30000 : 10000);
            return scr;
        }
        if(ssc == 0) {
            if(etc == 4)
                scr -= ((stc == 3) ? 50000 : 30000);
            else
                scr -= ((stc == 3) ? 30000 : 10000);
            return scr;
        }

        scr = 0;
        if(etc == 3) scr += 10000;
        if(stc == 3) scr -= 10000;    // difference will depend on other weights
        // Setting parameters
        // Ist 11 parameters (Cannons)
        if(forw == -1) {
            // V1
            param[0] = (board[2][0]==1)&&(board[3][0]==1)&&(board[4][0]==1);
            param[1] = (board[2][2]==1)&&(board[3][2]==1)&&(board[4][2]==1);
            param[2] = (board[2][4]==1)&&(board[3][4]==1)&&(board[4][4]==1);
            param[3] = (board[2][6]==1)&&(board[3][6]==1)&&(board[4][6]==1);
            // V2
            param[4] = (board[5][0]==1)&&(board[3][0]==1)&&(board[4][0]==1);
            param[5] = (board[5][2]==1)&&(board[3][2]==1)&&(board[4][2]==1);
            param[6] = (board[5][4]==1)&&(board[3][4]==1)&&(board[4][4]==1);
            param[7] = (board[5][6]==1)&&(board[3][6]==1)&&(board[4][6]==1);
            // D T1
            param[8] = (board[2][2]==1)&&(board[3][3]==1)&&(board[4][4]==1);
            param[9] = (board[5][5]==1)&&(board[3][3]==1)&&(board[4][4]==1);
            // D T2
            param[10] = (board[2][4]==1)&&(board[3][5]==1)&&(board[4][6]==1);
            param[11] = (board[5][7]==1)&&(board[3][3]==1)&&(board[4][4]==1);
            // D T3
            param[12] = (board[2][2]==1)&&(board[3][1]==1)&&(board[4][0]==1);
            // D T4
            param[13] = (board[2][4]==1)&&(board[3][3]==1)&&(board[4][2]==1);
            param[14] = (board[5][1]==1)&&(board[3][3]==1)&&(board[4][2]==1);
        }
        else {
                        // V1
            param[0] = (board[5][7-0]==1)&&(board[4][7-0]==1)&&(board[3][7-0]==1);
            param[1] = (board[5][7-2]==1)&&(board[4][7-2]==1)&&(board[3][7-2]==1);
            param[2] = (board[5][7-4]==1)&&(board[4][7-4]==1)&&(board[3][7-4]==1);
            param[3] = (board[5][7-6]==1)&&(board[4][7-6]==1)&&(board[3][7-6]==1);
            // V2
            param[4] = (board[2][7-0]==1)&&(board[3][7-0]==1)&&(board[4][7-0]==1);
            param[5] = (board[2][7-2]==1)&&(board[3][7-2]==1)&&(board[4][7-2]==1);
            param[6] = (board[2][7-4]==1)&&(board[3][7-4]==1)&&(board[4][7-4]==1);
            param[7] = (board[2][7-6]==1)&&(board[3][7-6]==1)&&(board[4][7-6]==1);
            // D T1
            param[8] = (board[2][2]==1)&&(board[3][3]==1)&&(board[4][4]==1);
            param[9] = (board[5][5]==1)&&(board[3][3]==1)&&(board[4][4]==1);
            // D T2
            param[10] = (board[7-2][7-4]==1)&&(board[7-3][7-5]==1)&&(board[7-4][7-6]==1);
            param[11] = (board[7-5][7-7]==1)&&(board[7-3][7-3]==1)&&(board[7-4][7-4]==1);
            // D T3
            param[12] = (board[7-2][7-2]==1)&&(board[7-3][7-1]==1)&&(board[7-4][7-0]==1);
            // D T4
            param[13] = (board[7-2][7-4]==1)&&(board[7-3][7-3]==1)&&(board[7-4][7-2]==1);
            param[14] = (board[7-5][7-1]==1)&&(board[7-3][7-3]==1)&&(board[7-4][7-2]==1);
        }
        // Soldier neighbour relations
        param[15] = unsafe_sold(1, 1);
        param[16] = unsafe_sold(1, 0);
        param[17] = unsafe_sold(0, 1);
        param[18] = unsafe_sold(0, 0);
        // Cannon score
        param[19] = cannon_scr(1);
        param[20] = cannon_scr(0);
        // Enemy score
        param[21] = ssc;
        param[22] = esc;

        for(int i=0; i<params; i++)
            scr += param[i]*w[i];
            
        return scr;
    }
};

int mod = 7297;

struct hash_board {
    size_t operator()(const Board& b) const {
        vector<vector<short>> board = b.get_board();
        size_t ans = 0; int p = 1;
        for(short i=0;i<_row;i++) {
            for(short j=0;j<_col;j++) {
                ans = (ans + (board[i][j] + 2) * p) % mod;
                p = (p * 43) % mod;
            }
        }
        return ans;
    } 
};

struct board_equal{
    bool operator()(const Board &b1, const Board &b2) const {
        return b1.get_board() == b2.get_board();
    }
};

unordered_map<Board, vector<Board>, hash_board, board_equal> um;
unordered_map<Board, short, hash_board, board_equal> ud;

void sort_boards(vector<Board>& ans, bool _we)
{
    if(_we)
        sort(ans.begin(), ans.end(), [](Board& b1, Board& b2) { return b1.score() > b2.score(); });
    else
        sort(ans.begin(), ans.end(), [](Board& b1, Board& b2) { return b1.score() < b2.score(); });
}

double min_value(Board, double, double, short, short);

double max_value(Board _b, double alpha, double beta, short curr_depth, short max_depth) {
    if(_b.count(2) == 2 || _b.count(-2) == 2 || _b.count(1) == 0 || _b.count(-1) == 0)
        return _b.score();
    
    if(um.find(_b) == um.end()) {
        um[_b] = _b.get_all_moves(1);
        ud[_b] = curr_depth;
    }

    vector<Board> &curr = um[_b];

    if(curr.empty()) {
        if(_b.count(2) == _b.count(-2)) return -10000;
        if(_b.count(2) > _b.count(-2)) return 10000;
        return -30000;
    }

    if(curr_depth == max_depth-1 || ud[_b] != curr_depth)
        return curr[0].score();

    double v = -inf, tmp; short k = curr.size();
    for(int i=0;i<k;i++) {
        tmp = min_value(curr[i], alpha, beta, curr_depth+1, max_depth);
        curr[i].set_score(tmp);
        if(tmp > v) v = tmp;
        if(v >= beta) {
            sort_boards(curr, 1);
            return curr[0].score();
        }
        if(v > alpha) alpha = v;
    }
    sort_boards(curr, 1);
    return v;
}

double min_value(Board _b, double alpha, double beta, short curr_depth, short max_depth) {
    if(_b.count(2) == 2 || _b.count(-2) == 2 || _b.count(1) == 0 || _b.count(-1) == 0)
        return _b.score();

    if(um.find(_b) == um.end()) {
        um[_b] = _b.get_all_moves(0);
        ud[_b] = curr_depth;
    }

    vector<Board> &curr = um[_b];

    if(curr.empty()) {
        if(_b.count(2) == _b.count(-2)) return 10000;
        if(_b.count(2) > _b.count(-2)) return 30000;
        return -10000;
    }
    
    if(curr_depth == max_depth-1 || ud[_b] != curr_depth)
        return curr[0].score();

    double v = inf, tmp; short k = curr.size();
    for(int i=0;i<k;i++) {
        tmp = max_value(curr[i], alpha, beta, curr_depth+1, max_depth);
        curr[i].set_score(tmp);
        if(tmp < v) v = tmp;
        if(v <= alpha) {
            sort_boards(curr, 0);
            return curr[0].score();
        }
        if(v < beta) beta = v;
    }
    sort_boards(curr, 0);
    return v;
}

Board alpha_beta_search(Board _b, short depth)
{
    double alpha = -inf, beta = inf, v, tmp;
    vector<Board> neighbours = _b.get_all_moves(1);
    short k = neighbours.size();
    if(k == 0) return _b;
    
    um[_b] = neighbours; ud[_b] = 0;
    vector<Board> &curr = um[_b];

    for(short d=2; d<=depth;d++) {
        alpha = v = -inf; beta = inf;
        for(int i=0;i<k;i++) {
            tmp = min_value(curr[i], alpha, beta, 1, d);
            curr[i].set_score(tmp);
            if(tmp > v) v = tmp;
            if(v > alpha) alpha = v;
        }
        sort_boards(curr, 1);
    }

    if(v > _b.score()) {
        for(short i=0; i<params; i++) {
            if(_b.param[i] == 0) continue;
            if(_b.param[i] * w[i] > 0) w[i]*=1.05;
            else w[i]*=0.95;
        }
    }

    else if(v < _b.score()) {
        for(short i=0; i<params; i++) {
            if(_b.param[i] == 0) continue;
            if(_b.param[i] * w[i] > 0) w[i]*=0.95;
            else w[i]*=1.05;
        }
    }

    Board best = curr[0]; 
    // best.print_board();
    // cout<<best.score()<<"\n"; 
    // cout<<"Number of nodes generated: "<<um.size()<<"\n";
    um.clear();
    return best;
}

int main(int argc, char const *argv[]) {

    srand(time(NULL));

    /* code */
    int is_black;
    cin >> is_black;

    // if(is_black) cout << "Computer is black...\n";
    // else cout << "Computer is white...\n";

    is_black = (is_black == 1);

    cin >> _row >> _col;
    forw = (is_black == true) ? -1 : 1;

    double t; cin >> t;

    Board c = Board(is_black), tmp;
    c.prev_step = "Start";

    bool step = is_black;
    string user_step; char s;

    ofstream myfile;
    myfile.open ("weights.txt");
    for(short i=0;i<params;i++) myfile<<w[i]<<" ";
        myfile<<endl; 

    while(1) {
        if(step) {
            tmp = alpha_beta_search(c, 5);
            if(tmp.pos() == c.pos()) break;
            c = tmp;
            for(short i=0;i<params;i++) myfile<<w[i]<<" ";
            myfile<<endl; 
            cout << c.prev_step << endl;
        }
        else {
            user_step = "";
            for(int i=0; i<5; i++) {
                cin >> s;
                user_step += s;
                user_step += " ";
            }
            cin >> s;
            user_step += s;
            c = c.next_state(user_step);
        }
        step = !step;
    }
    myfile.close();
    return 0;
}
