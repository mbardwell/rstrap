#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

// #define SIMULATION

#ifdef SIMULATION
    bool simEnabled = true;
#else
    bool simEnabled = false;
#endif

#endif