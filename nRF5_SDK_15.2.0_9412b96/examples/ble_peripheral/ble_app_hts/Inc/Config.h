#include <stdbool.h>

#ifndef MAIN_H
#define MAIN_H

#define SIMULATION

#ifdef SIMULATION
    bool simEnabled = true;
#else
    bool simEnabled = false;
#endif

#endif