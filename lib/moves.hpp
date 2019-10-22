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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * PIECE MOVE / REMOVE
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */


/*
 * Get new board object when player at curr is moved to next
 * @param _current (Board&) : The current board
 * @param _pc : Current position of a player
 * @param _pn : New position of the player
 * @param _we : Whether we are playing the move or other
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


/*
 * Get new board object when a cannon destroys a position
 * @param _current (Board&) : The current board
 * @param _pc : one of the positions of cannon (usually middle)
 * @param _ps : The position that is attacked
 * @param _we : Whether we are playing the move or other
 */
Board remove_player(Board& _current, pss _pc, pss _ps, bool _we) {

    // Creating a Board-Copy
    Board _next = Board(_current);
    // Resetting the score of the board
    _next.reset_score();

    // Removing the other player (if any) accordingly
    if(_we) {
        assert(_next.board()[_ps._x][_ps._y] <= 0);
        _next.decrease(_next.board()[_ps._x][_ps._y]);
    }
    else {
        assert(_next.board()[_ps._x][_ps._y] >= 0);
        _next.decrease(_next.board()[_ps._x][_ps._y]);
    }

    // Searching for the position in soldier positions
    short _index = find(_next.positions(!_we).begin(), _next.positions(!_we).end(), _ps) - _next.positions(!_we).begin();
    // Removing the player if it was present
    if(_index != _next.positions(!_we).size())
        _next.positions(!_we).erase(_index + _next.positions(!_we).begin());
    
    // Setting the values on the board
    _next.board()[_ps._x][_ps._y] = 0;

    // Creating the string move
    _next.note_step("S "+to_string(_pc._y)+" "+to_string(_pc._x)+" B "+to_string(_ps._y)+" "+to_string(_ps._x));

    // Return the board
    return _next;
}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * SOLDIER PROXIMITY ANALYSIS
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */


/*
 * Checking if a soldier is in the proximity of an enemy soldier.
 * @param _pos (vector<vector<short>>&) : Board positions.
 * @param x, y (short) : Position-pair of the soldier.
 * @param _we : Whether we are playing the move or other
 */
bool _adjacent_to_enemy(vector<vector<short>>& _pos, short x, short y, bool _we) {
    
    // Setting the enemy soldier
    short _enemy = (_we)?(-1):(1);

    // Setting the movement
    short _move = (_we ^ Board::type())?(1):(-1);

    // Checking for adjacent (H) positions
    if(y-1 >=0 && _pos[x][y-1] == _enemy) return true;
    if(y+1 < Board::shape(0) && _pos[x][y+1] == _enemy) return true;

    // Checking for soldiers in the front.
    if(x+_move >= 0 && x+_move < Board::shape(1)) {
        if(_pos[x+_move][y] == _enemy) return true;
        if(y-1 >= 0 && _pos[x+_move][y-1] == _enemy) return true;
        if(y+1 < Board::shape(0) && _pos[x+_move][y+1] == _enemy) return true;
    }

    // Return False
    return false;
}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * SOLDIER MOVES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */


/*
 * Getting positions for retreat of soldier.
 * @param (vector<Board>&) : The main container, referenced.
 * @param _b (Board&) : The board to work with.
 * @param x, y (short) : Position-pair of the soldier.
 * @param _we : Whether we are playing the move or other
 */
void _retreat_positions(vector<Board>& _states, Board& _b, short x, short y, bool _we) {

    // Checking if the player is in threat, return if not
    if(!_adjacent_to_enemy(_b.board(), x, y, _we)) return;

    // Setting the enemy soldier (also represents the sign)
    short _enemy = (_we)?(-1):(1);

    // Setting the movement
    short _move = (_we ^ Board::type())?(1):(-1);

    // Setting retreat-x
    short _xn = x - 2 * _move;

    // Checking if _xn in range, and proceeding
    if(_xn >= 0 && _xn < Board::shape(1)) {

        // The immediate back
        if(_enemy*_b.board()[_xn][y] >= 0) 
            _states.push_back(move_player(_b,{x, y}, {_xn, y}, _we));
        
        // Left back (perspective)
        if(y-2 >= 0 && _enemy*_b.board()[_xn][y-2] >= 0) 
            _states.push_back(move_player(_b, {x, y}, {_xn, y-2}, _we));
        
        // Right back (perspective)
        if(y+2 < Board::shape(0) && _enemy*_b.board()[_xn][y+2] >= 0) 
            _states.push_back(move_player(_b, {x, y}, {_xn, y+2}, _we));
    }
}


/*
 * Getting positions for forward move of the soldier.
  * @param (vector<Board>&) : The main container, referenced.
 * @param _b (Board&) : The board to work with.
 * @param x, y (short) : Position-pair of the soldier.
 * @param _we : Whether we are playing the move or other
 */
void _forward_positions(vector<Board>& _states, Board& _b, short x, short y, bool _we) {

    // Setting the enemy soldier (also represents the sign)
    short _enemy = (_we)?(-1):(1);

    // Setting the movement
    short _move = (_we ^ Board::type())?(1):(-1);

    // Checking on soldier adjacent positions
        // LEFT
    if(y-1 >= 0 && _enemy*_b.board()[x][y-1] > 0) 
        _states.push_back(move_player(_b, {x, y}, {x, y-1}, _we));
        // RIGHT
    if(y+1 < Board::shape(0) && _enemy*_b.board()[x][y+1] > 0) 
        _states.push_back(move_player(_b, {x, y}, {x, y+1}, _we));

    // Setting forward X
    short _xf = x + _move;

    // Checking if _xn in range, and proceeding
    if(_xf >= 0 && _xf < Board::shape(1)) {

        // The immediate back
        if(_enemy*_b.board()[_xf][y] >= 0) 
            _states.push_back(move_player(_b,{x, y}, {_xf, y}, _we));
        
        // Left back (perspective)
        if(y-1 >= 0 && _enemy*_b.board()[_xf][y-1] >= 0) 
            _states.push_back(move_player(_b, {x, y}, {_xf, y-1}, _we));
        
        // Right back (perspective)
        if(y+1 < Board::shape(0) && _enemy*_b.board()[_xf][y+1] >= 0) 
            _states.push_back(move_player(_b, {x, y}, {_xf, y+1}, _we));
    }

}


/*
 * Getting moves for all the soldiers.
 * @param _states (vector<Board>&) : The main container, referenced.
 * @param _b (Board&) : Board to work with. (present board)
 * @param _we : Whether we are playing the move or other
 */
void _soldier_moves(vector<Board>& _states, Board& _b, bool _we) {
    for(auto& _p : _b.positions(_we)) {
        _forward_positions(_states, _b, _p._x, _p._y, _we);
        _retreat_positions(_states, _b, _p._x, _p._y, _we);
    }
}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * CANNON POSITIONS
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * CANNON MOVES/ATTACKS
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */


#endif