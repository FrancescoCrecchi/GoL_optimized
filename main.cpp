//
// Created by francesco on 05/11/15.
//

#include <chrono>
#include "GameOfLife.h"

using namespace std;

//#define TEST 1

#ifdef TEST
#include "DavideTestImpl.h"
void checkResults(Davide::Board &dboard, Board &fboard, int dim) {

	for (int y = 0; y != dim; ++y) {
		for (int x = 0; x != dim; ++x) {
			if ((dboard.cells[(y+1)*(dim+2) + (x+1)]) != fboard.read[(y + 1)*(dim+2) + x + 1]) {
				std::cout << "Different results" << std::endl;
				return;
			}
		}
	}

	std::cout << "Same results" << std::endl;
}
#endif


int main(int argc, char* argv[]) {

    //Game board variables
    bool initRandom = true;
    int dim ,numIterations = -1;
    int nW = 0;

    switch(argc)
    {
        case 3:
            dim = atoi(argv[1]);
            numIterations = atoi(argv[2]);
            break;
        case 4:
            dim = atoi(argv[1]);
            numIterations = atoi(argv[2]);
            nW = atoi(argv[3]);
            break;
        default:
            cerr << "Usage: ./GameOfLife dim nIter [nWorkers]" << endl;
            exit(-1);
    }

    GameOfLife *GoL = new GameOfLife(dim, dim, initRandom); //By now we are considering only squared matrices


	#ifdef TEST
	static int8_t initialConfig[4096*4096];
	for (int i = 0; i != dim*dim; ++i) {
		initialConfig[i] = rand() % 2; //(rand() % 4) == 0 ? 1 : 0;
	}
	Davide::Board dboard(dim, dim);
	dboard.init(true, initialConfig);
	GoL->game_board->initialize_board(true, initialConfig);
	checkResults(dboard, *GoL->game_board, dim);
	#endif


	auto start = chrono::system_clock::now();
    GoL->start(numIterations, nW);
    auto end = chrono::system_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds> (end - start).count();
    cout << elapsed << endl;

#ifdef TEST
	cout << endl;
	start = chrono::system_clock::now();
	dboard.run(numIterations);
	end = chrono::system_clock::now();
	elapsed = chrono::duration_cast<chrono::milliseconds> (end - start).count();
	cout << elapsed << endl;
	checkResults(dboard, *GoL->game_board, dim);
#endif

    return 0;
}
