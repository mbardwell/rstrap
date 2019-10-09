#include "battery.h"
#include "nrf_drv_saadc.h"
#include "app_error.h"
#include "nrf_log.h"

static void saadc_event_handler(nrf_drv_saadc_evt_t const * p_evt) {}

void BatteryADCInit()
{
    ret_code_t err_code;
    /* since we are currently using blocking code from the HAL, we do not use the event */
    err_code = nrf_drv_saadc_init(NULL, saadc_event_handler);
    APP_ERROR_CHECK(err_code);

    nrf_saadc_channel_config_t channel_config =
    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_VDD);
    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for converting battery ADC results to a percentage from 0-100
 */
int16_t AdcResultInMillivolts(int16_t adc_value, int ref_voltage, int resolution, int prescaler)
{
    return (((adc_value*ref_voltage) / resolution) * prescaler);
}

/**@brief Function for converting AdcResultInMillivolts to a percentage from 0-100
 */
int8_t MillivoltsToPercentCharge(int16_t adc_value_in_mv, float lowest_allowed_mv, float highest_allowed_mv)
{
    // Battery sensor params are calculated by running voltage-to-adc-value experiments. This
    // slope/intercept combo is set such that a fresh 3V battery shows 100% charge, which drops
    // to 0% charge when the battery reaches 2.1V (just above when the device will turn off)
    const struct battery_sensor_params m_battery_sensor_params = {114, -242};
    if (adc_value_in_mv < lowest_allowed_mv)
    {
        return 0;
    }
    else if (adc_value_in_mv > highest_allowed_mv)
    {
        return 100;
    }
    else
    {
        return ((float) adc_value_in_mv/1000)*m_battery_sensor_params.slope + m_battery_sensor_params.intercept;
    }
}