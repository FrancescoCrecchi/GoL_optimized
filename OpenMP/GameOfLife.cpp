//
// Created by francesco on 05/11/15.
//

#include "GameOfLife.h"
#include "task.h"
#include "../serial.h"
#include <omp.h>

GameOfLife::GameOfLife(int rows, int cols, bool initRandom) {
    game_board = new Board(rows, cols, initRandom);
}

//MAIN METHOD: evaluate the board for a 'numIterations'
void GameOfLife::start(int numIterations, int nW) {

    auto multithread = (nW != 0);
    
    if(multithread) {
		//kmp_set_defaults("KMP_AFFINITY=compact");

// #pragma omp parallel num_threads(nW)
        // {
            omp_task(game_board, numIterations, nW);
        // }
    }
	else {
		/*best_*/serial_task(game_board, numIterations);
	}
}
