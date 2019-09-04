
#include <iostream>
#include <vector>
#include <iomanip>
#include <exception>

using namespace std;

// Class for representing the game board
class Board {
private:
    vector<vector<short>> board; // Board
    uint _row, _col; // Board dimensions
    uint wsc, bsc, wtc, btc; // Counts (white soldies, black soldiers, white towns, black towns)

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
        wtc = 0;
        for(short i=0; i<this->_col; i+=2) {
            board[0][i] = _set*2;
            wtc++;
        }
        // Setting white soldiers
        for(short i=1; i<this->_col; i+=2)
            for(short j=0; j<3; j++) {
                board[j][i] = _set*1;
                wsc++;
            }
        // BLACK
        // Setting black town-halls
        for(short i=1; i<this->_col; i+=2) {
            board[this->_row-1][i] = -2*_set;
            btc++;
        }
        // Setting black soldiers
        for(short i=0; i<this->_col; i+=2)
            for(short j=0; j<3; j++) {
                board[this->_row - 1 - j][i] = -1*_set;
                bsc++;
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
     * @param _key: (1, bsc) (2, wsc) (3, btc) (4, wtc)
     */
    uint& count(char _key) {
        switch(_key) {
            case 1: return bsc;
            case 2: return wsc;
            case 3: return btc;
            case 4: return wtc;
        }
        throw new runtime_error("Invalid key. (Must be 1, 2, 3, 4) [Board.count]");
    }

};


int main(int argc, char const *argv[]) {
    /* code */
    Board c = Board(8, 8);
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++)
            cout << flush << setw(5) << c.pos()[i][j] << " ";
        cout << "\n";
    }
    return 0;
}
