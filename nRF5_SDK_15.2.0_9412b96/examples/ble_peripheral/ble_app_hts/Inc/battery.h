#ifndef BATTERY_H
#define BATTERY_H

#include "stdint.h"

struct battery_sensor_params
{
    float slope;
    float intercept;
};

int16_t AdcResultInMillivolts(int16_t adc_value, int ref_voltage, int resolution, int prescaler);
int8_t MillivoltsToPercentCharge(int16_t adc_value, float lowest_allowed_millivolt, float highest_allowed_millivolt);
void BatteryADCInit();
void StartBatteryADC();

#endif