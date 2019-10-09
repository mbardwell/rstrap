#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "stdint.h"
#include "ble_hts_custom.h" // For ble_hts_meas_t

void ReadTemperature(ble_hts_meas_t*);

#endif