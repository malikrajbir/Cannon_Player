// Library for handelling the board class

#include <iostream>
#include <vector>
#include <map>
#include <iomanip>

#ifndef __BOARD__
#define __BOARD__

using namespace std;

#define _x first
#define _y second
#define pss pair<short, short>
#define loop(_var, _start, _end) for(int _var=_start; _var<_end; _var++)
#define loopx(_var, _start, _end, _step) for(int _var=_start; _var<_end; _var+=_step)

#define _red "\033[91m"
#define _green "\033[92m"
#define _yellow "\033[93m"
#define _cend "\033[0m"

/*
 * Board Class
 * Class for representing the state of the game-board.
 */
class Board {

// STATIC MEMBERS

// Board row and column size.
static short _row, _col;

// Player parity. (T -> Black) (F -> White)
static bool _black;

// Private
private:
    
    // Board vector
    vector<vector<short>> _board;

    // Soldier positions for both the players (1 -> US) (0 -> OTHER)
    vector<pss> _positions[2];

    // Counts (self soldiers, enemy soldiers, self towns, enemy towns)
    unsigned char ssc, esc, stc, etc;

    // Score of the board
    double _score;

    // FLAG: Keeping the check on score.
    bool score_set;

    // String for stacking the previous move
    string _step;


    /*
     * Initial (starting) placement of the pieces on the board.
     * 
     * NOTE: _move (player parity) must be set (in static)
     */
    void __place() {
        // Setting +/- (based on parity)
        short _set = 1;
        if(_black) _set = -1;
        // Initialising entities
        ssc = esc = stc = etc = 0;
        // WHITE
        // Setting white town-halls
        loopx(i, 0, _col, 2) {
            _board[0][i] = _set*2;
            etc++;
        }
        // Setting white soldiers
        short k = 0;
        loopx(i, 1, _col, 2)
            loop(j, 0, 3) {
                _board[j][i] = _set*1;
                _positions[!_black][k++] = {j,i};
                esc++;
            }
        // BLACK
        // Setting black town-halls
        loopx(i, 1, _col, 2) {
            _board[_row-1][i] = -2*_set;
            stc++;
        }
        // Setting black soldiers
        k = 0;
        loopx(i, 0, _col, 2)
            loop(j, 0, 3) {
                _board[_row - 1 - j][i] = -1*_set;
                _positions[_black][k++] = {_row-1-j,i};
                ssc++;
            }
        // Complete
        return;
    }

// Public
public:


// ------------------------------------------------------------
// CLASS FUNCTIONS
// ------------------------------------------------------------

    /*
     * Board constructor.
     * @param _blk : whether we are black/white player
     * 
     * NOTE: The class static, Board._row & Board._col, must be set before calling the constructor.
     */
    Board(bool _blk = true) {
        // Creating the board vector
        _board = vector<vector<short>>(_row, vector<short>(_col, 0));
        // Setting the class parity
        _black = _blk;
        // Initialsing the _position vectors
        _positions[0] = vector<pss>((_col/2)*3);
        _positions[1] = vector<pss>((_col/2)*3);
        // Placing the pieces
        __place();
        // Setting the score to be -INF
        _score = -__DBL_MAX__;
        score_set = false;
    }


    /*
     * Board deconstructor.
     * Clears the space occupied the vector.
     */
    ~Board() {
        // Clearing the vector
        _board.clear();
        // Clearing positions
        _positions[0].clear();
        _positions[1].clear();
    }


    /*
     * Intialising the class static variables.
     * Callable only once.
     * 
     * NOTE: This is the first function that must be called before using the class.
     */
    static void _init(short _r, short _c) {
        _row = _r;
        _col = _c;
    }


    /*
     * Getting the board dimensions.
     * @param __row (bool) : Whether _row needed or _col
     * @return (short) (True -> Row) (False -> Column)
     * 
     * NOTE: This function must be called after setting type (creating the first board).
     */
    static short shape(bool __row) {
        if(__row)
            return _row;
        else
            return _col;
    }


    /*
     * Getting the main player type. (Black or White?)
     * @return (Black -> True) (White -> False)
     * 
     * NOTE: This function must be called after setting type (creating the first board).
     */
    static bool type() {
        return _black;
    }


// ------------------------------------------------------------
// SETTERS/RESETTING
// ------------------------------------------------------------


    /*
     * Setting the scoree of a board, to a particular value.
     * @param _scr : Score to be set.
     */
    void set_score(float _scr) {
        // Set score.
        _score = _scr;
        // Set flag.
        score_set = true;
    }

    
    /*
     * Reset the score to -INF.
     */
    void reset_score() {
        // Reset Score
        _score = -__DBL_MAX__;
        // Unset flag
        score_set = false;
    }


    /*
     * Decreasing the count of an entity (if valid).
     * @param _key : (1, ssc) (-1, esc) (2, stc) (-2, etc)
     */
    void decrease(int _key) {
        switch(_key) {
            case 1:  { ssc--; return; }
            case -1: { esc--; return; }
            case 2:  { stc--; return; }
            case -2: { etc--; return; }
            default: return;
        }
    }


    /*
     * Setting the "String-Step", that created this board.
     * @param _str: The stepping string.
     */
    void note_step(string _str) {
        _step = _str;
    }


// ------------------------------------------------------------
// GETTERS
// ------------------------------------------------------------


    /*
     * Getting the board-vector.
     * @return _board (vector<vector<short>>&) : The board set in the game
     */
    vector<vector<short>>& board() {
        return _board;
    }


    /*
     * Getting the respecitive counts of entites.
     * @param _key : (1, ssc) (-1, esc) (2, stc) (-2, etc)
     */
    unsigned char& count(char _key) {
        switch(_key) {
            case 1: return ssc;
            case -1: return esc;
            case 2: return stc;
            case -2: return etc;
        }
        throw new runtime_error("Invalid key. (Must be -2, -1, 1, 2) [Board.count]");
    }


    /*
     * Get all the soldier positions.
     * @param _us : Whether our or other players informtion is required.
     * @return (vector<pair<short, short>>&) 
     */
    vector<pss>& positions(bool _us) {
        return _positions[_us];
    }


    // /*
    //  * Getting the score of the board.
    //  */
    // float score() {
    //     return _score;
    // }

    /*
     * Getting the step that created the move.
     */
    string step() {
        return _step;
    }

    /*
     * Getting the score of the board.
     * Defined in "score.hpp".
     */
    double score();


// ------------------------------------------------------------
// MISC.
// ------------------------------------------------------------


    /*
     * Printing the complete board.
     */
    void _print(ostream& _out = cout, bool _clr = true) {
        // Printing dimensions
        _out << "Rows: " << _row << " | Columns: " << _col << "\n";
        // Printing the board
        loop(i, 0, _row) {
            loop(j, 0, _col)
                if(_clr) {
                    if(_board[i][j] == 0) _out << _yellow;
                    else if (_board[i][j] < 0) _out << _red;
                    else _out << _green;
                    if(_board[i][j])
                        _out << setw(3) << _board[i][j] << " " << _cend;
                    else
                        _out << "  • " << _cend;
                }
                else {
                    _out << setw(3) << _board[i][j] << " ";
                }
            _out << "\n";
        }
    }

};

// Adding definitions of static members
short Board::_row, Board::_col;
bool Board::_black;


#endif