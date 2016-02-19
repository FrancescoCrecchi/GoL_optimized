//
// Created by francesco on 05/11/15.
//

#include <chrono>
#include "GameOfLife.h"

using namespace std;


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

	auto start = chrono::system_clock::now();
    GoL->start(numIterations, nW);
    auto end = chrono::system_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds> (end - start).count();
    cout << elapsed << endl;

    return 0;
}
