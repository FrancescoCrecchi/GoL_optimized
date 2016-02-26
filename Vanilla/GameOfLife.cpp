//
// Created by francesco on 05/11/15.
//
#include "GameOfLife.h"
#include "task.h"
#include "../serial.h"

GameOfLife::GameOfLife(int rows, int cols, bool initRandom) {
    game_board = new Board(rows, cols, initRandom);
}

//MAIN METHOD: evaluate the board for a 'numIterations'
void GameOfLife::start(int numIterations, int nW) {

	//Barrier creation
    barrier = new spinning_barrier(nW);

    bool multithread = (nW != 0);
    
    if(multithread) {

        // Reserving space for the thread_pool
        thread_pool.reserve(nW);

        //Split work among workers in the thread_pool
        int howmuch = (game_board->rows - 2) / nW;

        //int start = 0;
        int stop = 0;

        for (int i = 0; i < nW ; ++i)
        {
            auto start = i*howmuch + 1;
            
    	    if (i  != (nW - 1)) //not last thread
                stop  = start+howmuch;
    	    else
                stop = game_board->rows - 1;

            //Set up worker[i]
            this->thread_pool.emplace_back(thread(plain_task, game_board, start, stop, numIterations, barrier));
        }

        //Thread join
        for (int i = 0; i < nW; ++i){
            this->thread_pool[i].join();
        }
        this->thread_pool.clear();

        //DEBUG!
        // cout << "Barrier time: " << barrier->total_time_barrier << endl;
    }
	else {
		best_serial_task(game_board, numIterations);
	}
}
