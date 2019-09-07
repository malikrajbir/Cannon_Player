#include <iostream>
#include <vector>
#include <iomanip>
#include <exception>
#include <assert.h>
#define X first
#define Y second
#define pb push_back

using namespace std;
typedef pair<short, short> pii;

short forw; // for black, forw = -1 (one step forward decreases the row index) and for white, forw = 1 (one step forward
            // increases the row index)

// Class for representing the game board
class Board {
private:
    vector<vector<short>> board; // Board
    unsigned int ssc, esc, stc, etc; // Counts (self soldies, enemy soldiers, self towns, enemy towns)
    unsigned int _row, _col; // Board dimensions

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
        for(short i=0; i<this->_col; i+=2) {
            board[0][i] = _set*2;
            etc++;
        }
        // Setting white soldiers
        for(short i=1; i<this->_col; i+=2)
            for(short j=0; j<3; j++) {
                board[j][i] = _set*1;
                esc++;
            }
        // BLACK
        // Setting black town-halls
        for(short i=1; i<this->_col; i+=2) {
            board[this->_row-1][i] = -2*_set;
            stc++;
        }
        // Setting black soldiers
        for(short i=0; i<this->_col; i+=2)
            for(short j=0; j<3; j++) {
                board[this->_row - 1 - j][i] = -1*_set;
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
    Board(short _col, short _row, bool is_black = true) {
        // Creating the vector
        this->board = vector<vector<short>>(_row, vector<short>(_col, 0));
        // Setting parameters
        this->_row = _row;
        this->_col = _col;
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
    Board move_player(pii curr, pii next) {
        Board res = *this;
        if(res.pos()[next.X][next.Y] == -1) res.esc--;
        if(res.pos()[next.X][next.Y] == -2) res.etc--;
        res.pos()[next.X][next.Y] = res.pos()[curr.X][curr.Y];
        res.pos()[curr.X][curr.Y] = 0;
        return res;
    }

    /*
     * returns true if soldier present at (x, y) is adjacent to enemy soldier
     */
    bool is_adjacent_to_enemy(int x, int y) {
        if(y-1 >= 0 && board[x][y-1] == -1) return true;
        if(y+1 < this->_col && board[x][y+1] == -1) return true;
        if(x+forw >= 0 && x+forw < this->_row) {
            if(board[x+forw][y] == -1) return true;
            if(y-1 >= 0 && board[x+forw][y-1] == -1) return true;
            if(y+1 < this->_col && board[x+forw][y+1] == -1) return true;
        }
        return false;
    }

    /*
     * returns the board positions when soldier present at (x, y) retreats if it is adjacent to enemy
     */
    vector<Board> get_positions_for_soldier_retreat(int x, int y) {
        vector<Board> ans;
        if(!is_adjacent_to_enemy(x, y)) return ans;
        short new_x = x - 2 * forw;
        if(new_x >= 0 && new_x < this->_row) {
            if(board[new_x][y] <= 0) ans.pb(move_player({x, y}, {new_x, y}));
            if(y-2 >= 0 && board[new_x][y-2] <= 0) ans.pb(move_player({x, y}, {new_x, y-2}));
            if(y+2 < this->_col && board[new_x][y+2] <= 0) ans.pb(move_player({x, y}, {new_x, y+2}));
        }
        return ans;
    }

    /*
     * returns the board positions when soldier present at (x, y) moves forward or sideways to an empty
     * space or captures an enemy piece
     */
    vector<Board> get_positions_for_soldier_forward(int x, int y) {
        vector<Board> ans;
        if(y-1 >= 0 && board[x][y-1] < 0) ans.pb(move_player({x, y}, {x, y-1}));
        if(y+1 < this->_col && board[x][y+1] < 0) ans.pb(move_player({x, y}, {x, y+1}));
        short new_x = x + forw;
        if(new_x >= 0 && new_x < this->_row) {
            if(board[new_x][y] <= 0) ans.pb(move_player({x, y}, {new_x, y}));
            if(y-1 >= 0 && board[new_x][y-1] <= 0) ans.pb(move_player({x, y}, {new_x, y-1}));
            if(y+1 < this->_col && board[new_x][y+1] <= 0) ans.pb(move_player({x, y}, {new_x, y+1}));
        }
        return ans;
    }

    /*
     * returns the soldier moves of a soldier present at (x, y)
     */
    vector<Board> get_moves_for_soldiers(int x, int y) {
        assert(board[x][y] == 1);
        vector<Board> ans = get_positions_for_soldier_forward(x, y);
        vector<Board> tmp = get_positions_for_soldier_retreat(x, y);
        ans.insert(ans.end(), tmp.begin(), tmp.end());
        return ans;
    }

    /*
     * returns all soldier moves of present state of board
     */
    vector<Board> get_all_soldier_moves() {
        vector<Board> ans, tmp;
        for(short i=0; i<this->_row; i++)
            for(short j=0; j<this->_col; j++)
                if(board[i][j] == 1) {
                    tmp = get_moves_for_soldiers(i, j);
                    ans.insert(ans.end(), tmp.begin(), tmp.end());
                }
        return ans;
    }

    /*
     * prints the present state of board
     */
    void print_board() {
        for(short i=0; i<this->_row; i++) {
            for(short j=0; j<this->_col; j++)
                cout << flush << setw(5) << board[i][j] << " ";
            cout << "\n";
        }
        cout << "\n";
    }
};

int main(int argc, char const *argv[]) {
    /* code */
    bool is_black = 1;
    forw = (is_black == true) ? -1 : 1;
    Board c = Board(8, 8, is_black);
    c.print_board();
    vector<Board> v = c.get_all_soldier_moves();
    // for(auto b : v) b.print_board();
    return 0;
}
