#pragma once

#include "stdint.h"

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS 600  //!< Reference voltage (in milli volts) used by ADC while doing conversion.
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS 270 //!< Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com.
#define ADC_RES_10BIT 1024                 //!< Maximum digital value for 10-bit ADC conversion.
#define ADC_PRE_SCALING_COMPENSATION 6     //!< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.
#define LOWEST_ALLOWABLE_BATTERY_VOLTAGE 2120
#define NOMINAL_FRESH_BATTERY_VOLTAGE 3000

struct battery_sensor_params
{
    float slope;
    float intercept;
};

void battery_adc_init();
void battery_level_in_mv(int16_t* voltage);
int16_t battery_convert_adc_sample_to_mv(int16_t adc_value, int ref_voltage, int resolution, int prescaler);
int8_t battery_convert_mv_to_percent_charge(int16_t adc_value, float lowest_allowed_millivolt, float highest_allowed_millivolt);