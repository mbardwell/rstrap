#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

// #define SIMULATION
/* Set this according to the manufacturer's specifications of the battery in your device */
#define PEAK_BATTERY_VOLTAGE_MV 3000

#ifdef SIMULATION
    bool simEnabled = true;
#else
    bool simEnabled = false;
#endif

#endif