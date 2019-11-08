// Main runner for game.

#include <iostream>
#include <fstream>
#include <time.h>

#include "board.hpp"
#include "moves.hpp"
#include "score.hpp"
#include "minimax.hpp"

using namespace std;

ofstream _wts;

/*
 * Starting the game, setting up variables and main board.
 */
Board start() {

    // Local Variables
    short _blk; short _r, _c;

    // Reading player color
    cin >> _blk, _blk = (_blk == 1);

    // Reading row and column
    cin >> _r >> _c;

    // Initialising the board class
    Board::_init(_r, _c);

    // Minimum Townhalls, for game to finish
    min_townhalls = Board::shape(0) / 2 - 2;

    // Return the board
    return Board(_blk);
}


/*
 * Playing the game with the given board.
 * @param _game (Board&) : The main playing board.
 */
void play(Board& _game) {
    // Reading the time limit
    double _TL;
    cin >> _TL;
    // Local variable (containers)
    string input; time_t _t = time(NULL); char _c;
    // Setting the turn-step
    bool step = Board::type();
    // The game loop
    while(true) {
        // Our turn
        if(step) {
            // Call Mini-Max
            minimax(_game, 5);
            // Print the game step
            cout << _game.step() << "\n";
            // Writing the game weights
            _wts << "{ ";
            loop(i, 0, _total) _wts << flush << _weights[i] << ", ";
            _wts << "}" << endl;
        }
        // Other player's turn
        else {
            // Taking in the other players move
            input = "";
            loop(i,0,5) {
                cin >> _c;
                input += _c;
                input += " ";
            }
            cin >> _c;
            input += _c;
            // Processing the move
            _game = next_state(_game, input);
        }
        // Reversing the step
        step = !step;
    }
}


// MAIN
int main(int argc, char const *argv[]) {
    // Opening writeup file
    _wts = ofstream(argv[1]);
    // Printing the starting weights
    cerr << "Constant Starting Weights.\n";
    loop(i, 0, _total) cerr << _weights[i] << " ";
    cerr << "\n";
    // Read the game & set the board
    Board _game = start();
    // Play the game
    play(_game);
    // Return
    return 0;
}
