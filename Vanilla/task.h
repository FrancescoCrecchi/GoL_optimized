//
// Created by francesco on 10/02/16.
//

#ifndef GAMEOFLIFE_TASK_H
#define GAMEOFLIFE_TASK_H

#include "../Board.h"
#include "spinning_barrier.h"

void plain_task(Board* board, int start, int stop, int numIterations, spinning_barrier* barrier) {

	int h = board->rows, w = board->cols;
	Board::CELL_TYPE *in = board->read;
	Board::CELL_TYPE *out = board->write;

	for (int l = 0; l != numIterations; ++l) {

		for (int i = start; i < stop; ++i) { //going through the rows

			int sc = i * w + 1; 	//current row cell
			int sn = sc - w; 		//previous row cell
			int ss = sc + w;		//next row cell


			#pragma simd //enforces loop vectorization
			#pragma vector nontemporal //because the compiler do not vectorize nested loops
			for (int j = 1; j < w - 1; ++j) {

				int count = in[sn - 1] +
					in[sn + 1] +
					in[ss - 1] +
					in[ss + 1] +
					in[sc - 1] +
					in[sc + 1] +
					in[sn] +
					in[ss];

				out[sc] = (in[sc]) ? (count == 2 || count == 3) : (count == 3);

				sc++;sn++;ss++;
			}

			// fix col borders
			out[i * w] = out[i * w + w - 2];
			out[i * w + w - 1] = out[i * w + 1];

			// fix row borders
			if (i == 1 || i == h - 2) {

				Board::CELL_TYPE *rowSrc = out + w * i;
				Board::CELL_TYPE *rDst = out + w * ((i == 1) ? (h - 1) : 0);

				#pragma simd
				#pragma vector nontemporal
				for (int i = w; i != 0; --i) {
					*rDst++ = *rowSrc++;
				}
			}

		}
		
		//Barrier here
		barrier->wait([&](){
#if PRINT
			board->print_board(out);
#endif
		});

		Board::CELL_TYPE *tmp = in;
		in = out;
		out = tmp;
	}

	board->read = in;
	board->write = out;
}

#endif //GAMEOFLIFE_TASK_H
