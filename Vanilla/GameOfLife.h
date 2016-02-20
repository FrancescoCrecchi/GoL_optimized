//
// Created by francesco on 05/11/15.
//

#ifndef GAMEOFLIFE_GAME_BOARD_H
#define GAMEOFLIFE_GAME_BOARD_H

#include <vector>
#include <thread>

#include "../Board.h"
#include "spinning_barrier.h"

using namespace std;

class GameOfLife {
	vector<thread> thread_pool;
    spinning_barrier* barrier;

public:
    Board* game_board;

    GameOfLife(int rows, int cols, bool initRandom); //Create a new GameBoard and initialize it randomly
    void start(int numIterations, int nW);
};


#endif //GAMEOFLIFE_GAME_BOARD_H
