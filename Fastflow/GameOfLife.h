//
// Created by francesco on 05/11/15.
//

#ifndef GAMEOFLIFE_GAME_BOARD_H
#define GAMEOFLIFE_GAME_BOARD_H

#include "../Board.h"

using namespace std;

class GameOfLife {

public:
    Board* game_board;

    GameOfLife(int rows, int cols, bool initRandom); //Create a new GameBoard and initialize it randomly
    ~GameOfLife();

    void start(int numIterations, int nW);
};


#endif //GAMEOFLIFE_GAME_BOARD_H
