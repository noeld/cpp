//
// Created by Arnold Müller on 07.04.17.
//

#include "Threadpool.h"


std::atomic<unsigned> Task::counter {0};

std::atomic<unsigned> Threadpool::counter {0};