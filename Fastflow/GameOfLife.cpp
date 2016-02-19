//
// Created by francesco on 05/11/15.
//
#include "GameOfLife.h"
#include "task.h"

GameOfLife::GameOfLife(int rows, int cols, bool initRandom) {
    game_board = new Board(rows, cols, initRandom);
}

//MAIN METHOD: evaluate the board for a 'numIterations'
void GameOfLife::start(int numIterations, int nW) {

    bool multithread = (nW != 0);
    
    if(multithread) {

            ff_task(game_board, numIterations, nW);

    }
	else {
		serial_task(game_board, numIterations);
	}
}