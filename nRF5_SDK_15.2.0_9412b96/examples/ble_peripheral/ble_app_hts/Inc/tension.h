#ifndef TENSION_H
#define TENSION_H

#include "stdint.h"

#define tension_measurement_send(...) ble_nus_data_send(...)

uint32_t ReadTensionLevel();

#endif