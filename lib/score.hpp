// Library for basic scoring mechanism of the boards.

#include <iostream>
#include <vector>
#include <map>
#include <iomanip>

#include "board.hpp"
#include "moves.hpp"

#ifndef __SCORE__
#define __SCORE__

using namespace std;

#define _x first
#define _y second
#define pss pair<short, short>
#define loop(_var, _start, _end) for(int _var=_start; _var<_end; _var++)
#define loopx(_var, _start, _end, _step) for(int _var=_start; _var<_end; _var+=_step)

/* 
 * ******************************************************************************
 * FEATURES
 * ******************************************************************************
 */

/*
 * Features involving enemy-soldier/self-soldier interactions.
 * 
 * @param _b (Board&) : The board to perform with.
 * @param _features (double *) : The feature container.
 * 
 * Feature description... (with orders)
 * F5: No. of self soldiers in contact with each other
 * F6: No. of self soldiers in contact with enemy soldiers
 * F7: No. of enemy soldiers in contact with each other.
 * F8: No. of self soldiers in contact with enemy townhall
 * F9: No. of enemy soldiers in contact with self townhall
 * F10: No. of self soldiers in contact with self townhall
 * F11: No. of enemy soldiers in contact with enemy townhall
 */
void _soldier_interactions(Board* _b, short* _features) {

    // Containers
    short ss = 0, se = 0, ee = 0, sst = 0, set = 0, est = 0, eet = 0;
    
    // Setting moves
    short _move = (Board::type())?(-1):(1);

    // Starting with self soldiers
    for(auto& _p : _b->positions(1)) {
        // Checking on the right
        if(_p.second+1 < Board::shape(0)) {
            switch(_b->board()[_p.first][_p.second+1]) {
                case 1: ss++; break;
                case -1: se++; break;
                case 2: sst++; break;
                case -2: set++; break;
                default: break;
            }
        }
        // Checking on the left
        if(_p.second-1 >= 0) {
            switch(_b->board()[_p.first][_p.second-1]) {
                case -1: se++; break;
                case 2: sst++; break;
                case -2: set++; break;
                default: break;
            }
        }
        if(_p.first+_move < Board::shape(1) && _p.first+_move >= 0) {
            // Checking on the front
            switch(_b->board()[_p.first+_move][_p.second]) {
                case 1: ss++; break;
                case -1: se++; break;
                case -2: set++; break;
                default: break;
            }
            // Checking on the front-left
            if(_p.second+1 < Board::shape(0)) {
                switch(_b->board()[_p.first+_move][_p.second+1]) {
                    case 1: ss++; break;
                    case -1: se++; break;
                    case -2: set++; break;
                    default: break;
                }
            }
            // Checking on the front-right
            if(_p.second-1 >= 0) {
                switch(_b->board()[_p.first+_move][_p.second-1]) {
                    case 1: ss++; break;
                    case -1: se++; break;
                    case -2: set++; break;
                    default: break;
                }
            }
        }
        if(_p.first-_move < Board::shape(1) && _p.first-_move >= 0) {
            // Checking on the back
            if(_b->board()[_p.first-_move][_p.second] == 2)
                sst++;
            // Checking on the back-right
            if(_p.second+1 < Board::shape(0))
                if(_b->board()[_p.first-_move][_p.second+1] == 2)
                    sst++;
            // Checking on the back-right
            if(_p.second-1 >= 0)
                if(_b->board()[_p.first-_move][_p.second-1] == 2)
                    sst++;
        }
    }
    // Starting with enemy soldiers
    for(auto& _p : _b->positions(0)) {
        // Checking on the right
        if(_p.second+1 < Board::shape(0)) {
            switch(_b->board()[_p.first][_p.second+1]) {
                case -1: ee++; break;
                case 2: est++; break;
                case -2: eet++; break;
                default: break;
            }
        }
        // Checking on the left
        if(_p.second-1 >= 0) {
            switch(_b->board()[_p.first][_p.second-1]) {
                case 2: est++; break;
                case -2: eet++; break;
                default: break;
            }
        }
        if(_p.first+_move < Board::shape(1) && _p.first+_move >= 0) {
            // Checking on the back
            if(_b->board()[_p.first+_move][_p.second] == -2)
                eet++;
            // Checking on the back-right
            if(_p.second+1 < Board::shape(0))
                if(_b->board()[_p.first+_move][_p.second+1] == -2)
                    eet++;
            // Checking on the back-left
            if(_p.second-1 >= 0)
                if(_b->board()[_p.first+_move][_p.second-1] == -2)
                    eet++;
        }
        if(_p.first-_move < Board::shape(1) && _p.first-_move >= 0) {
            // Checking on the front
            switch(_b->board()[_p.first-_move][_p.second]) {
                case -1: ee++; break;
                case 2: est++; break;
                default: break;
            }
            // Checking on the front-left
            if(_p.second+1 < Board::shape(0)) {
                switch(_b->board()[_p.first-_move][_p.second+1]) {
                    case -1: ee++; break;
                    case 2: est++; break;
                    default: break;
                }
            }
            // Checking on the front-right
            if(_p.second-1 >= 0) {
                switch(_b->board()[_p.first-_move][_p.second-1]) {
                    case -1: ee++; break;
                    case 2: est++; break;
                    default: break;
                }
            }
        }
    }

    // Assigning the features, values
    _features[4] = ss;
    _features[5] = se;    // These values are counted twice, doesn't matter much
    _features[6] = ee;
    _features[7] = set;
    _features[8] = est;
    _features[9] = sst;
    _features[10] = eet;
}

/*
 * Features involving cannons and their interactions.
 * 
 * @param _b (Board&) : The board to perform with.
 * @param _features (double *) : The feature container.
 * 
 * Feature description... (with orders)
 * F12: No. of vertical self-cannons
 * F13: No. of horizontal self-cannons
 * F14: No. of diagonal self-cannons
 * F15: No. of vertical enemy-cannons
 * F16: No. of horizontal enemy-cannons
 * F17: No. of diagonal enemy-cannons
 * F18: No. of enemy-soldiers captured by self cannon-shots (self)
 * F19: No. of enemy-town captured by self cannon-shots (self)
 * F20: No. of self-soldiers captured by enemy cannon-shots (enemy)
 * F21: No. of self-town captured by enemy cannon-shots (enemy)
 */
void _cannon_interactions(Board* _b, short* _features) {

    // Containers
    short vsc=0, hsc=0, dsc=0, vec=0, hec=0, dec=0, ecs=0, etcs=0, sce=0, stce=0;

    // Containers
    vector<pss> _scann[4], _ecann[4];

    // Dimensions
    short _row = Board::shape(1), _col = Board::shape(0), xo, yo;

    // SELF
    // Getting all the cannon positions
    _cannon_positions(_b->board(), _b->positions(1), _scann, 1);
    // Setting parameters
    vsc = _scann[0].size();
    hsc = _scann[1].size();
    dsc = _scann[2].size() + _scann[3].size();
    // Getting the captured positions
    // Working with Vertical Cannons
    for(auto& _p : _scann[0]) {
        // Setting coordinates
        xo = _p._x, yo = _p._y;
        
        // Processing all attacks
        if(!(xo+3 < 0 || xo+3 >= _row) && _b->board()[xo+2][yo] == 0){
            if(_b->board()[xo+3][yo] == -1)
                ecs++;
            else if(_b->board()[xo+3][yo] == -2)
                etcs++;
        }
        if(!(xo-3 < 0 || xo-3 >= _row) && _b->board()[xo-2][yo] == 0){
            if(_b->board()[xo-3][yo] == -1)
                ecs++;
            else if(_b->board()[xo-3][yo] == -2)
                etcs++;
        }
        if(!(xo+4 < 0 || xo+4 >= _row) && _b->board()[xo+2][yo] == 0){
            if(_b->board()[xo+4][yo] == -1)
                ecs++;
            else if(_b->board()[xo+4][yo] == -2)
                etcs++;
        }
        if(!(xo-4 < 0 || xo-4 >= _row) && _b->board()[xo-2][yo] == 0){
            if(_b->board()[xo-4][yo] == -1)
                ecs++;
            else if(_b->board()[xo-4][yo] == -2)
                etcs++;
        }
    }
    
    // Working with Horizontal Cannons
    for(auto& _p : _scann[1]) {
        // Setting coordinates
        xo = _p._x, yo = _p._y;

        // Processing all attacks
        if(!(yo+3 < 0 || yo+3 >= _col) && _b->board()[xo][yo+2] == 0){
            if(_b->board()[xo][yo+3] == -1)
                ecs++;
            else if(_b->board()[xo][yo+3] == -2)
                etcs++;
        }
        if(!(yo-3 < 0 || yo-3 >= _col) && _b->board()[xo][yo-2] == 0){
            if(_b->board()[xo][yo-3] == -1)
                ecs++;
            else if(_b->board()[xo][yo-3] == -2)
                etcs++;
        }
        if(!(yo+4 < 0 || yo+4 >= _col) && _b->board()[xo][yo+2] == 0){
            if(_b->board()[xo][yo+4] == -1)
                ecs++;
            else if(_b->board()[xo][yo+4] == -2)
                etcs++;
        }
        if(!(yo-4 < 0 || yo-4 >= _col) && _b->board()[xo][yo-2] == 0){
            if(_b->board()[xo][yo-4] == -1)
                ecs++;
            else if(_b->board()[xo][yo-4] == -2)
                etcs++;
        }
    }

    // Working with Leftward Cannons
    for(auto& _p : _scann[2]) {
        // Setting coordinates
        xo = _p._x, yo = _p._y;

        // Processing all attacks
        if(!(yo+3 < 0 || yo+3 >= _col || xo-3 < 0 || xo-3 >= _row) && _b->board()[xo-2][yo+2] == 0){
            if(_b->board()[xo-3][yo+3] == -1)
                ecs++;
            else if(_b->board()[xo-3][yo+3] == -2)
                etcs++;
        }
        if(!(yo-3 < 0 || yo-3 >= _col || xo+3 < 0 || xo+3 >= _row) && _b->board()[xo+2][yo-2] == 0){
            if(_b->board()[xo+3][yo-3] == -1)
                ecs++;
            else if(_b->board()[xo+3][yo-3] == -2)
                etcs++;
        }
        if(!(yo+4 < 0 || yo+4 >= _col || xo-4 < 0 || xo-4 >= _row) && _b->board()[xo-2][yo+2] == 0){
            if(_b->board()[xo-4][yo+4] == -1)
                ecs++;
            else if(_b->board()[xo-4][yo+4] == -2)
                etcs++;
        }
        if(!(yo-4 < 0 || yo-4 >= _col || xo+4 < 0 || xo+4 >= _row) && _b->board()[xo+2][yo-2] == 0){
            if(_b->board()[xo+4][yo-4] == -1)
                ecs++;
            else if(_b->board()[xo+4][yo-4] == -2)
                etcs++;
        }
    }

    // Working with Rightward Cannons
    for(auto& _p : _scann[3]) {
        // Setting coordinates
        xo = _p._x, yo = _p._y;

        // Processing all attacks
        if(!(yo+3 < 0 || yo+3 >= _col || xo+3 < 0 || xo+3 >= _row) && _b->board()[xo+2][yo+2] == 0){
            if(_b->board()[xo+3][yo+3] == -1)
                ecs++;
            else if(_b->board()[xo+3][yo+3] == -2)
                etcs++;
        }
        if(!(yo-3 < 0 || yo-3 >= _col || xo-3 < 0 || xo-3 >= _row) && _b->board()[xo-2][yo-2] == 0){
            if(_b->board()[xo-3][yo-3] == -1)
                ecs++;
            else if(_b->board()[xo-3][yo-3] == -2)
                etcs++;
        }
        if(!(yo+4 < 0 || yo+4 >= _col || xo+4 < 0 || xo+4 >= _row) && _b->board()[xo+2][yo+2] == 0){
            if(_b->board()[xo+4][yo+4] == -1)
                ecs++;
            else if(_b->board()[xo+4][yo+4] == -2)
                etcs++;
        }
        if(!(yo-4 < 0 || yo-4 >= _col || xo-4 < 0 || xo-4 >= _row) && _b->board()[xo-2][yo-2] == 0){
            if(_b->board()[xo-4][yo-4] == -1)
                ecs++;
            else if(_b->board()[xo-4][yo-4] == -2)
                etcs++;
        }
    }

    // ENEMY
    // Getting all the cannon positions
    _cannon_positions(_b->board(), _b->positions(0), _ecann, 0);
    // Setting parameters
    vec = _ecann[0].size();
    hec = _ecann[1].size();
    dec = _ecann[2].size() + _ecann[3].size();
    // Getting the captured positions
    // Working with Vertical Cannons
    for(auto& _p : _ecann[0]) {
        // Setting coordinates
        xo = _p._x, yo = _p._y;
        
        // Processing all attacks
        if(!(xo+3 < 0 || xo+3 >= _row) && _b->board()[xo+2][yo] == 0){
            if(_b->board()[xo+3][yo] == 1)
                sce++;
            else if(_b->board()[xo+3][yo] == 2)
                stce++;
        }
        if(!(xo-3 < 0 || xo-3 >= _row) && _b->board()[xo-2][yo] == 0){
            if(_b->board()[xo-3][yo] == 1)
                sce++;
            else if(_b->board()[xo-3][yo] == 2)
                stce++;
        }
        if(!(xo+4 < 0 || xo+4 >= _row) && _b->board()[xo+2][yo] == 0){
            if(_b->board()[xo+4][yo] == 1)
                sce++;
            else if(_b->board()[xo+4][yo] == 2)
                stce++;
        }
        if(!(xo-4 < 0 || xo-4 >= _row) && _b->board()[xo-2][yo] == 0){
            if(_b->board()[xo-4][yo] == 1)
                sce++;
            else if(_b->board()[xo-4][yo] == 2)
                stce++;
        }
    }
    
    // Working with Horizontal Cannons
    for(auto& _p : _ecann[1]) {
        // Setting coordinates
        xo = _p._x, yo = _p._y;

        // Processing all attacks
        if(!(yo+3 < 0 || yo+3 >= _col) && _b->board()[xo][yo+2] == 0){
            if(_b->board()[xo][yo+3] == 1)
                sce++;
            else if(_b->board()[xo][yo+3] == 2)
                stce++;
        }
        if(!(yo-3 < 0 || yo-3 >= _col) && _b->board()[xo][yo-2] == 0){
            if(_b->board()[xo][yo-3] == 1)
                sce++;
            else if(_b->board()[xo][yo-3] == 2)
                stce++;
        }
        if(!(yo+4 < 0 || yo+4 >= _col) && _b->board()[xo][yo+2] == 0){
            if(_b->board()[xo][yo+4] == 1)
                sce++;
            else if(_b->board()[xo][yo+4] == 2)
                stce++;
        }
        if(!(yo-4 < 0 || yo-4 >= _col) && _b->board()[xo][yo-2] == 0){
            if(_b->board()[xo][yo-4] == 1)
                sce++;
            else if(_b->board()[xo][yo-4] == 2)
                stce++;
        }
    }

    // Working with Leftward Cannons
    for(auto& _p : _ecann[2]) {
        // Setting coordinates
        xo = _p._x, yo = _p._y;

        // Processing all attacks
        if(!(yo+3 < 0 || yo+3 >= _col || xo-3 < 0 || xo-3 >= _row) && _b->board()[xo-2][yo+2] == 0){
            if(_b->board()[xo-3][yo+3] == 1)
                sce++;
            else if(_b->board()[xo-3][yo+3] == 2)
                stce++;
        }
        if(!(yo-3 < 0 || yo-3 >= _col || xo+3 < 0 || xo+3 >= _row) && _b->board()[xo+2][yo-2] == 0){
            if(_b->board()[xo+3][yo-3] == 1)
                sce++;
            else if(_b->board()[xo+3][yo-3] == 2)
                stce++;
        }
        if(!(yo+4 < 0 || yo+4 >= _col || xo-4 < 0 || xo-4 >= _row) && _b->board()[xo-2][yo+2] == 0){
            if(_b->board()[xo-4][yo+4] == 1)
                sce++;
            else if(_b->board()[xo-4][yo+4] == 2)
                stce++;
        }
        if(!(yo-4 < 0 || yo-4 >= _col || xo+4 < 0 || xo+4 >= _row) && _b->board()[xo+2][yo-2] == 0){
            if(_b->board()[xo+4][yo-4] == 1)
                sce++;
            else if(_b->board()[xo+4][yo-4] == 2)
                stce++;
        }
    }

    // Working with Rightward Cannons
    for(auto& _p : _ecann[3]) {
        // Setting coordinates
        xo = _p._x, yo = _p._y;

        // Processing all attacks
        if(!(yo+3 < 0 || yo+3 >= _col || xo+3 < 0 || xo+3 >= _row) && _b->board()[xo+2][yo+2] == 0){
            if(_b->board()[xo+3][yo+3] == 1)
                sce++;
            else if(_b->board()[xo+3][yo+3] == 2)
                stce++;
        }
        if(!(yo-3 < 0 || yo-3 >= _col || xo-3 < 0 || xo-3 >= _row) && _b->board()[xo-2][yo-2] == 0){
            if(_b->board()[xo-3][yo-3] == 1)
                sce++;
            else if(_b->board()[xo-3][yo-3] == 2)
                stce++;
        }
        if(!(yo+4 < 0 || yo+4 >= _col || xo+4 < 0 || xo+4 >= _row) && _b->board()[xo+2][yo+2] == 0){
            if(_b->board()[xo+4][yo+4] == 1)
                sce++;
            else if(_b->board()[xo+4][yo+4] == 2)
                stce++;
        }
        if(!(yo-4 < 0 || yo-4 >= _col || xo-4 < 0 || xo-4 >= _row) && _b->board()[xo-2][yo-2] == 0){
            if(_b->board()[xo-4][yo-4] == 1)
                sce++;
            else if(_b->board()[xo-4][yo-4] == 2)
                stce++;
        }
    }

    // Assigning the features
    _features[11] = vsc;
    _features[12] = hsc;
    _features[13] = dsc;
    _features[14] = vec;
    _features[15] = hec;
    _features[16] = dec;
    _features[17] = ecs;
    _features[18] = etcs;
    _features[19] = sce;
    _features[20] = stce;
}

/*
 * Scoring Mechanism.
 * 
 * Most Basic Features.
 * F1: Self Soldier Count
 * F2: Self Town Count
 * F3: Enemy Soldier Count
 * F4: Enemy Town Count
 */
double Board::score() {

    // Checking if score is set, and returning if it is.
    if(score_set)
        return _score;
    
    // Calculating, if not set.
    double _sc = 0; short *_f = new short[21]();
    // Innate features
    _f[0] = ssc, _f[1] = stc, _f[2] = esc, _f[3] = etc;
    // Calculating the soldier features
    _soldier_interactions(this, _f);
    // Calculating the cannon features
    _cannon_interactions(this, _f);
    // Printing the board and features
    cout << "___________________________\n";
    _print();
    for(int i=0; i<21; i++) cout << "F" << i+1 << ": " << _f[i] << "\n";
    cout << "___________________________\n";
    // Setting the score
    set_score(_sc);
    // Returning the score
    return _score;
}

#endif