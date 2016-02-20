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

	//Barrier creation
    barrier = new spinning_barrier(nW);

    multithread = (nW != 0);
    
    if(multithread) {

        //Reserving space for the thread_pool
        thread_pool.reserve(nW);

        //Split work among workers in the thread_pool
        int howmuch = (game_board->rows - 2) / nW;

        int start = 0;
        int stop = 1;

        for (int i = 0; i < nW - 1; ++i)
        {
            start = i*howmuch + 1;
            stop  = start+howmuch;

            //Set up worker[i]
            this->thread_pool.emplace_back(thread(plain_task, game_board, start, stop, numIterations, barrier));
        }
        //Using also the main thread
        plain_task(game_board, stop , game_board->rows - 1,  numIterations, barrier);

        //Thread join and swap
        for (int i = 0; i < nW - 1; ++i){
            this->thread_pool[i].join();
        }
        this->thread_pool.clear();

    }
	else {
		plain_task(game_board, 1, game_board->rows - 1, numIterations, NULL);
	}
}
