//
// Created by francesco on 04/02/16.
//

#ifndef GAMEOFLIFE_BOARD_H
#define GAMEOFLIFE_BOARD_H

#include <iostream>

using namespace std;

class Board {
public:
	typedef int32_t CELL_TYPE;
    int rows, cols;
    CELL_TYPE *read, *write;


    Board(int rows, int cols, bool initRandom);

    int get_cell(int c);
    void set_cell(int c, int value);

    void initialize_board(bool initRandom);
    void evaluate_cell(int c);
    void swap();
    void fixBorders(CELL_TYPE *cells);

};


#endif //GAMEOFLIFE_BOARD_H
