// Main runner for game.

#include <iostream>
#include <fstream>
#include <time.h>

#include "board.hpp"
#include "moves.hpp"
#include "score.hpp"
#include "minimax.hpp"

using namespace std;

// Writing the learned weights
ofstream _wts;
// State of the game weights, dynamic:learner, static:tester
bool _learn;

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
    string input; char _c;
    // Setting the turn-step
    bool step = Board::type();
    double time_elapsed = 0; time_t _st;
    // The game loop
    while(true) {
        // Our turn
        if(step) {
            _st = time(NULL);
            // Call Mini-Max
            if(time_elapsed < 0.45*_TL)
                minimax(_game, 5, _learn, 5.5);
            else if(time_elapsed > 0.90*_TL)
                minimax(_game, 4, _learn, 1.0);
            else
                minimax(_game, 5, _learn, 1.5);

            // Print the game step
            cout << _game.step() << "\n";
            // Writing the game weights
            if(_learn) {
                _wts << "{ ";
                loop(i, 0, _total) _wts << flush << _weights[i] << ", ";
                _wts << "}" << endl;
            }
            time_elapsed += time(NULL) - _st;
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
    // Checking the state of the game.
    _learn = (argv[1][0] == '1');
    // Opening a file if learning
    if(_learn) _wts = ofstream("weights.txt");
    // Printing the starting weights
    // cerr << "Starting Weights.\n";
    // loop(i, 0, _total) {
    //     cerr << _weights[i] << " ";
    // }
    // cerr << "\n";
    // Read the game & set the board
    Board _game = start();
    // Play the game
    play(_game);
    // Return
    return 0;
}
