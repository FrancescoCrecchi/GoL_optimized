//
// Created by francesco on 04/02/16.
//

#include "Board.h"
#include <malloc.h>

int SEED = 12345;


Board::Board(int rows, int cols, bool initRandom) {
    //Saving num of rows and cols
    this->rows = rows+2;
    this->cols = cols+2;

    //Allocating game boards: stencil pattern applies (memory aligned)
	read = (CELL_TYPE *)_mm_malloc(this->rows * this->cols * sizeof(CELL_TYPE), 64);
	write = (CELL_TYPE *)_mm_malloc(this->rows * this->cols * sizeof(CELL_TYPE), 64);

    //Initialize the board randomly (if any)
    initialize_board(initRandom);
}

Board::~Board() {
    //Deallocate boards
    _mm_free(read);
    _mm_free(write);
}


void Board::initialize_board(bool initRandom) {
    //initialize each element randomly
    srand(SEED);

    for (int i = 0; i < rows; ++i) {
        int base = i * cols;
        for (int j = 0; j < cols; ++j) {
            if(((i > 0 && j > 0) && (i < (rows - 1) && j < (cols - 1))) && initRandom)
                write[base + j] = rand() % 2;
            else
                write[base + j] = 0; //dead
        }
    }
	fixBorders(write);
    swap();
}

inline int Board::get_cell(int c) {
    return read[c];
}

inline void Board::set_cell(int c, int value) {
    write[c] = value;
}

void Board::evaluate_cell(int c) {
    int alive_counter = get_cell(c - cols - 1) + get_cell(c - cols) + get_cell(c - cols +1) + get_cell(c - 1) + get_cell(c + 1) + get_cell(c + cols - 1) + get_cell(c + cols) + get_cell(c + cols +1);

    //Evaluating rules to establish if the cell is dead or alive
    set_cell(c, ((alive_counter == 3) || (get_cell(c) && alive_counter == 2)));
}

void Board::fixBorders(CELL_TYPE *cells) {
    
	int h = rows - 2;
	int w = cols;

	// cols
	for (int i = 0; i != h; ++i) {
		int cleft = cols * (i + 1);
		int cright = cols * (i + 2) - 1;
		cells[cleft] = cells[cright - 1];
		cells[cright] = cells[cleft + 1];
	}

	int lastRowOff = cols * (rows - 1);

	// rows
	for (int i = 0; i != w; ++i) {
		int rtop = i;
		int rbottom = lastRowOff + i;
		cells[rtop] = cells[rbottom - cols];
		cells[rbottom] = cells[rtop + cols];
	}
    

}

void Board::swap() {
    std::swap(read, write);
}


void Board::print_board(Board::CELL_TYPE *board) {
    for (int i = 1; i < rows - 1 ; ++i) {
        for (int j = 1; j < cols - 1 ; ++j) {
            if(board[i* cols + j] == 0)
                cout << ".";
            else
                cout << "0";
        }
        cout << endl;
    }
    cout << endl;
    // sleep(1); //sleep 1 sec
    // int res = system("clear");    //clear the console in UNIX systems
}