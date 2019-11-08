// Library for performing mini-max operation on the game

#include <vector>

#include "board.hpp"
#include "moves.hpp"
#include "score.hpp"

using namespace std;

#ifndef _MMX_
#define _MMX_

#define _a first
#define _b second
#define pss pair<short, short>
#define pdd pair<double, double>
#define loop(_var, _start, _end) for(int _var=_start; _var<_end; _var++)
#define loopx(_var, _start, _end, _step) for(int _var=_start; _var<_end; _var+=_step)
#define INF __DBL_MAX__


// Function Declaration

double __min(Board& _b, short _depth, pdd _ab);
double __max(Board& _b, short _depth, pdd _ab);

short min_townhalls;

/*
 * MIN STEP
 * The minimum (adverserial) step of the algorithm.
 * 
 * @param _b (Board&) : The board to be worked up.
 * @param _depth (short) : Depth of the search left.
 * @param _ab (pair<short, short>) : Carriers for pruning (alpha-beta)
 */
double __min(Board& _b, short _depth, pdd _ab) {

    // Base case scoring
    if(_b.count(2) == min_townhalls || _b.count(-2) == min_townhalls || _b.count(1) == 0 || _b.count(-1) == 0)
        return _b.score();

    // Getting all possible moves
    vector<Board> moves = next_state(_b, 0);

    // If there is stale-mate
    if(moves.empty()) {
        if(_b.count(2) == _b.count(-2)) return 10000;
        if(_b.count(2) > _b.count(-2)) return 30000;
        return -10000;
    }

    // If we are at the final depth, just send the score (maximum-possible)
    // Recursive-base-case
    if(_depth == 1)
        return moves[0].score();

    // Alpha-Beta Pruning
    double _v = INF, _max;
    loop(i, 0, moves.size()) {
        // Getting the minimum-value in the next state
        _max = __max(moves[i], _depth-1, _ab);
        // Weaker maximum, update
        if(_max < _v) _v = _max;
        // Maximum less than possible, return (what?)
        if(_v <= _ab._a) break;
        // Maximum lower than upper bound, update
        if(_v < _ab._b) _ab._b = _v;
    }

    // Returning the max-value
    return _v; 
}


/*
 * MAX STEP
 * The maximum (optimistic) step of the algorithm.
 * 
 * @param _b (Board&) : The board to be worked up.
 * @param _depth (short) : Depth of the search left.
 * @param _ab (pair<short, short>) : Carriers for pruning (alpha-beta)
 */
double __max(Board& _b, short _depth, pdd _ab) {

    // Base case scoring
    if(_b.count(2) == min_townhalls || _b.count(-2) == min_townhalls || _b.count(1) == 0 || _b.count(-1) == 0)
        return _b.score();

    // Getting all possible moves
    vector<Board> moves = next_state(_b, 1);

    // If there is stale-mate
    if(moves.empty()) {
        if(_b.count(2) == _b.count(-2)) return -10000;
        if(_b.count(2) > _b.count(-2)) return 10000;
        return -30000;
    }

    // If we are at the final depth, just send the score (maximum-possible)
    // Recursive-base-case
    if(_depth == 1)
        return moves[0].score();

    // Alpha-Beta Pruning
    double _v = -INF, _min;
    loop(i, 0, moves.size()) {
        // Getting the minimum-value in the next state
        _min = __min(moves[i], _depth-1, _ab);
        // Better minimum, update
        if(_min > _v) _v = _min;
        // Minimum more than possible, return (what?)
        if(_v >= _ab._b) break;
        // Minimum more than lower bound, update
        if(_v > _ab._a) _ab._a = _v;
    }

    // Returning the max-value
    return _v;
}


/*
 * THE MINIMAX ALGORITHM
 * Find the best possible next step depending on the scores and ply-search.
 * 
 * @param _b (Board&) : The current board state. (It is updated to the latest state at the end of the algorithm)
 * @param _depth (short) : Depth of the complete search.
 * @param _learn (bool) : Whether the weights are to be trained
 */
void minimax(Board& _b, short _depth, bool _learn) {

    // Learning component (OLD SCORE & FEATURES)
    double _old = _b.score(), _new, *_oldf = _b.features();

    // Working with the base-case (current-board)
    // Setting _ab
    pdd _ab = pdd(-INF, INF);
    // Getting next_states
    vector<Board> moves = next_state(_b, 1);
    // Base-case
    if(moves.empty()) return;

    // Running MAX on present board
    double _v = -INF, _min;
    loop(i, 0, moves.size()) {
        // Getting the minimum-value in the next state
        _min = __min(moves[i], _depth-1, _ab);
        // Better minimum, update
        if(_min > _v) {
            _b = moves[i];
            _v = _min;
        }
        // Minimum more than lower bound, update
        if(_v > _ab._a) _ab._a = _v;
    }
    // Run complete. Return if not learning.
    if(!_learn) return;

    // Learning component (UPDATING WITH NEW SCORE)
    _new = _v;
    // If no change, return
    if(_new == _old || _new == -INF) return;
    // Updating weights
    loop(i, 0, _total) {
        if(_weights[i]*_oldf[i] > 0 && _new > _old)
            _weights[i] = _weights[i]*(1 + 2*(_oldf[i])*(_new-_old)/(_new+_old));
        if(_weights[i]*_oldf[i] < 0 && _new < _old)
            _weights[i] = _weights[i]*(1 - 2*(_oldf[i])*(_new-_old)/(_new+_old));
    }
}


#endif
