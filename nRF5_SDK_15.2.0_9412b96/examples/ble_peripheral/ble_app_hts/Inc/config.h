#pragma once

#include <stdbool.h>

/* Set this according to the manufacturer's specifications of the battery in your device */
#define PEAK_BATTERY_VOLTAGE_MV 3000

#ifdef DEBUG
    bool debugEnabled = true;
#else
    bool debugEnabled = false;
#endif

#ifdef SIMULATE
    bool simEnabled = true;
#else
    bool simEnabled = false;
#endif