// Library for games actions and state transitions

#include <vector>
#include <map>
#include <algorithm>


#include "board.hpp"

#ifndef __MV__
#define __MV__

using namespace std;

#define _x first
#define _y second
#define pss pair<short, short>
#define loop(_var, _start, _end) for(int _var=_start; _var<_end; _var++)
#define loopx(_var, _start, _end, _step) for(int _var=_start; _var<_end; _var+=_step)


/*
 * Get new board object when player at curr is moved to next
 * @param curr : current position of a player
 * @param next : new position of the player
 */
Board move_player(Board& _current, pss _pc, pss _pn, bool _we) {

    // Creating a Board-Copy
    Board _next = Board(_current);
    // Resetting the score of the board
    _next.reset_score();

    // Removing the other player (if any) accordingly
    if(_we) {
        assert(_next.board()[_pn._x][_pn._y] <= 0);
        _next.decrease(_next.board()[_pn._x][_pn._y]);
    }
    else {
        assert(_next.board()[_pn._x][_pn._y] >= 0);
        _next.decrease(_next.board()[_pn._x][_pn._y]);
    }

    // Adjusting the position for the soldier
    short _index = find(_next.positions(_we).begin(), _next.positions(_we).end(), _pc) - _next.positions(_we).begin();
    // Asserting the presence of the node
    assert(_index < _next.positions(_we).size());
    // Updating soldier position
    _next.positions(_we)[_index] = _pn;

    // Now working with the new position
    _index = find(_next.positions(!_we).begin(), _next.positions(!_we).end(), _pn) - _next.positions(!_we).begin();
    // Removing the player if it was present
    if(_index != _next.positions(!_we).size())
        _next.positions(!_we).erase(_index + _next.positions(!_we).begin());
    
    // Setting values on the board
    _next.board()[_pn._x][_pn._y] = _next.board()[_pc._x][_pc._y];
    _next.board()[_pc._x][_pc._y] = 0;

    // Creating the string move
    _next.note_step("S "+to_string(_pc._y)+" "+to_string(_pc._x)+" M "+to_string(_pn._y)+" "+to_string(_pn._x));
    
    // Return the copy board
    return _next;
}

#endif