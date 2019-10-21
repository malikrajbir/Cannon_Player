#include <iostream>

#include "board.hpp"
#include "moves.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    // Initialsing Board Static
    Board::_init(10, 10);
    // Creating a board
    Board _b = Board(false);
    _b._print();
    // Moving and checking...
    _b = move_player(_b, {2, 1}, {3, 1}, true);
    // Printing the new board
    _b._print();
    // Exit
    exit(0);
}
