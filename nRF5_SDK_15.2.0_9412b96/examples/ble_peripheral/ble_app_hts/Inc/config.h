#pragma once

#include <stdbool.h>

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