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