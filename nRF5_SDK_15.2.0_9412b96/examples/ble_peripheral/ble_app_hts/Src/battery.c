#include "battery.h"

uint8_t ReadBatteryLevel()
{
    return 0;
}


// #include "nrf_drv_saadc.h"

// #define ADC_REF_VOLTAGE_IN_MILLIVOLTS  600  //!< Reference voltage (in milli volts) used by ADC while doing conversion.
// #define DIODE_FWD_VOLT_DROP_MILLIVOLTS 270  //!< Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com.
// #define ADC_RES_10BIT                  1024 //!< Maximum digital value for 10-bit ADC conversion.
// #define ADC_PRE_SCALING_COMPENSATION   6    //!< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.
/* #define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE) \
//     ((((ADC_VALUE) *ADC_REF_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_10BIT) * ADC_PRE_SCALING_COMPENSATION)
*/

// static uint16_t batt_in_milli_volts;

// static void saadc_event_handler(nrf_drv_saadc_evt_t const * p_evt)
// {
//     if (p_evt->type = NRFX_SAADC_EVT_DONE)
//     {
//         nrf_saadc_value_t adc_result;

//         adc_result = p_evt->data.done.p_buffer[0];

//         batt_in_milli_volts =
//             ADC_RESULT_IN_MILLI_VOLTS(adc_result) + DIODE_FWD_VOLT_DROP_MILLIVOLTS;
//     }
// }

// void BatteryInit(void)
// {
//     ret_code_t err_code = nrf_drv_saadc_init(NULL, saadc_event_handler);
// }
