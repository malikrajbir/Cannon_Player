// Library for basic scoring mechanism of the boards.

#include <iostream>
#include <vector>
#include <map>
#include <iomanip>

#include "board.hpp"

#ifndef __SCORE__
#define __SCORE__

using namespace std;

#define _x first
#define _y second
#define pss pair<short, short>
#define loop(_var, _start, _end) for(int _var=_start; _var<_end; _var++)
#define loopx(_var, _start, _end, _step) for(int _var=_start; _var<_end; _var+=_step)


/*
 * THE MOST BASIC/INNATE SCORING
 */
double Board::score() {
    return ssc*100+stc*10000-esc*100-etc*10000;
}

#endif