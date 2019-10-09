#include "temperature.h"
#include "nrf_soc.h"
#include "app_error.h"

void ReadTemperature(ble_hts_meas_t* temperature)
{
    int32_t temp;
    uint32_t err_code = sd_temp_get(&temp); // Blocks program for 50 us
    APP_ERROR_CHECK(err_code);

    temperature->temp_in_fahr_units = false; // Set temp to report in Celcius
    temperature->time_stamp_present = false; // No timestamp
    temperature->temp_type_present  = false; // Temp type refers to a body part. We are not measuring a body part

    temperature->temp_in_celcius.exponent = -2;
    /* temp provides a number in units of 1/4 degree celcius (ie temp = 5 means the temperature is 1.25 C).
       So we multiply by 0.25. We also multiply by 100 because we send the mantissa as an integer and accomadate 
       for the 100 by setting the exponent to -2 */
    temperature->temp_in_celcius.mantissa = temp*0.25*100;
}