#include "temperature.h"
#include "nrf_soc.h"
#include "app_error.h"

void temperature_sample(int32_t* temperature)
{
    uint32_t err_code = sd_temp_get(temperature); // Blocks program for 50 us
    APP_ERROR_CHECK(err_code);

    *temperature = *temperature*0.25;
}