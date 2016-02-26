//
// Created by francesco on 12/02/16.
//

#include "spinning_barrier.h"

spinning_barrier::spinning_barrier (unsigned int n) : n_ (n), nwait_ (0), step_(0) {}

void spinning_barrier::wait (function<void()> callback) {

    unsigned int step = step_.load ();

    if (nwait_.fetch_add (1) < n_ - 1)
    {
        if(nwait_.load() == 1)
            start = chrono::system_clock::now();

        while (step_.load () == step) /* Run in circles and scream like a little girl.  */
            ;
    }
    else /* OK, last thread to come.  */
    {
        //Execute callback
        callback();

        nwait_.store (0); // XXX: maybe can use relaxed ordering here ??
        step_.fetch_add (1);
        end = chrono::system_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds> (end - start).count();
        total_time_barrier += elapsed;
    }
}