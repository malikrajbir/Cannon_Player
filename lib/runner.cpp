#include <iostream>

#include "board.hpp"
#include "moves.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    // Initialsing Board Static
    Board::_init(12, 12);
    // Creating a board
    Board _b = Board(false);
    // Containor vector
    vector<Board> _s = next_state(_b, false); 
    // Printing all moves
    for(auto& _d: _s) {
        _d._print();
    }

    // Exit
    exit(0);
}
