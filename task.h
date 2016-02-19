//
// Created by francesco on 10/02/16.
//

#ifndef GAMEOFLIFE_TASK_H
#define GAMEOFLIFE_TASK_H

#include "Board.h"

void serial_task(Board* board, int numIterations) {

	int h = board->rows, w = board->cols;
	Board::CELL_TYPE *in = board->read;
	Board::CELL_TYPE *out = board->write;

	for (int l = 0; l != numIterations; ++l) {

		for (int i = 1; i != h - 1; ++i) {

			int sc = i * w + 1;
			int sn = sc - w;
			int ss = sc + w;

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

		}


		// fix row borders
		int lastRowOff = w * (h - 1);
		int lastRowOff0 = w * (h - 2);
		int firstRowOff0 = w;

		Board::CELL_TYPE *rtopDst = out;
		Board::CELL_TYPE *rbottomDst = out + lastRowOff;
		Board::CELL_TYPE *rbottomSrc = out + lastRowOff0;
		Board::CELL_TYPE *rtopSrc = out + firstRowOff0;

		// rows
		for (int i = w; i != 0; --i) {
			*rtopDst++ = *rbottomSrc++;
			*rbottomDst++ = *rtopSrc++;
		}

		Board::CELL_TYPE *tmp = in;
		in = out;
		out = tmp;
	}

	board->read = in;
	board->write = out;
}

void omp_task(Board* board, int numIterations) {

	int h = board->rows, w = board->cols;
	Board::CELL_TYPE *in = board->read;
	Board::CELL_TYPE *out = board->write;

	for (int l = 0; l < numIterations; ++l) {

		#pragma omp for schedule(static)
		for (int i = 1; i < h - 1; ++i) {

			int sc = i * w + 1;
			int sn = sc - w;
			int ss = sc + w;

			#pragma simd
			#pragma vector nontemporal
			for (int j = 1; j < w - 1; ++j) {

				int count = in[sn-1] +
					in[sn+1] +
					in[ss-1] +
					in[ss+1] +
					in[sc-1] +
					in[sc+1] +
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

		Board::CELL_TYPE *tmp = in;
		in = out;
		out = tmp;
	}


#pragma omp single 
	{
		board->read = in;
		board->write = out;
	}

}


#endif //GAMEOFLIFE_TASK_H
