//
// Created by francesco on 12/02/16.
//

#ifndef GAMEOFLIFE_SPINNING_BARRIER_H
#define GAMEOFLIFE_SPINNING_BARRIER_H

#include <atomic>
#include <functional>
#include <chrono>
#include <iostream>

using namespace std;

class spinning_barrier {
protected:
    /* Number of synchronized threads. */
    const unsigned int n_;

    /* Number of threads currently spinning.  */
    std::atomic<unsigned int> nwait_;

    /* Number of barrier syncronizations completed so far */
    std::atomic<unsigned int> step_;
public:
    spinning_barrier (unsigned int n);
    
    void wait (function<void()> callback);
};


#endif //GAMEOFLIFE_SPINNING_BARRIER_H
