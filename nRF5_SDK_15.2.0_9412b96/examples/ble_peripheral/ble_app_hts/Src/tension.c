#include "stdint.h"
#include "tension.h"
#include "ble_nus.h"
#include "nrf_log.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpiote.h"


/******** HX711 class *********/

static volatile struct hx711_sample m_sample;
static enum hx711_mode m_mode;
static struct hx711_setup setup = {23, 24, 25, 32, 16, 24};
static bool m_continous_sampling = false;

void gpiote_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    nrf_drv_gpiote_in_event_disable(setup.DOUT);
    SampleHx711(false);
}

void InitHx711(enum hx711_mode mode)
{
    ret_code_t ret_code;

    m_mode = mode;

    nrf_gpio_cfg_output(setup.PD_SCK);
    nrf_gpio_pin_set(setup.PD_SCK);

    if (!nrf_drv_gpiote_is_init())
    {
        ret_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(ret_code);
    }

    nrf_drv_gpiote_in_config_t gpiote_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    nrf_gpio_cfg_input(setup.DOUT, NRF_GPIO_PIN_NOPULL);
    ret_code = nrf_drv_gpiote_in_init(setup.DOUT, &gpiote_config, gpiote_evt_handler);
    APP_ERROR_CHECK(ret_code);


    /* Set up timers, gpiote, and ppi for clock signal generation*/
    NRF_TIMER1->CC[0]     = 1;
    NRF_TIMER1->CC[1]     = setup.TIMER_COMPARE;
    NRF_TIMER1->CC[2]     = setup.TIMER_COUNTERTOP;
    NRF_TIMER1->SHORTS    = (uint32_t) (1 << 2);    //COMPARE2_CLEAR
    NRF_TIMER1->PRESCALER = 0;

    NRF_TIMER2->CC[0]     = m_mode;
    NRF_TIMER2->MODE      = 2;

    NRF_GPIOTE->CONFIG[1] = (uint32_t) (3 | (setup.PD_SCK << 8) | (1 << 16) | (1 << 20));

    NRF_PPI->CH[0].EEP   = (uint32_t) &NRF_TIMER1->EVENTS_COMPARE[0];
    NRF_PPI->CH[0].TEP   = (uint32_t) &NRF_GPIOTE->TASKS_SET[1];
    NRF_PPI->CH[1].EEP   = (uint32_t) &NRF_TIMER1->EVENTS_COMPARE[1];
    NRF_PPI->CH[1].TEP   = (uint32_t) &NRF_GPIOTE->TASKS_CLR[1];
    NRF_PPI->FORK[1].TEP = (uint32_t) &NRF_TIMER2->TASKS_COUNT; // Increment on falling edge
    NRF_PPI->CH[2].EEP   = (uint32_t) &NRF_TIMER2->EVENTS_COMPARE[0];
    NRF_PPI->CH[2].TEP   = (uint32_t) &NRF_TIMER1->TASKS_SHUTDOWN;
    NRF_PPI->CHEN = 7;
}

void StartHx711(bool single)
{
    m_continous_sampling = !single;

    NRF_LOG_DEBUG("Start sampling");
    
    NRF_GPIOTE->TASKS_CLR[1] = 1;
    // Generates interrupt when new sampling is available. 
    nrf_drv_gpiote_in_event_enable(setup.DOUT, true);
}

void StopHx711()
{
    NRF_LOG_DEBUG("Stop sampling");
    nrf_drv_gpiote_in_event_disable(setup.DOUT);
}

/* Clocks out HX711 result - if readout fails consistently, try to increase the clock period and/or enable compiler optimization */
void SampleHx711()
{
    NRF_TIMER2->TASKS_CLEAR = 1;
    m_sample.count = 0;
    m_sample.value = 0;
    NRF_TIMER1->TASKS_START = 1; // Starts clock signal on PD_SCK

    for (uint32_t i=0; i < setup.ADC_RES; i++)
    {
        do
        {
            /* NRF_TIMER->CC[1] contains number of clock cycles.*/
            NRF_TIMER2->TASKS_CAPTURE[1] = 1;
            if (NRF_TIMER2->CC[1] >= setup.ADC_RES) goto EXIT; // Readout not in sync with PD_CLK. Abort and notify error.
        }
        while(NRF_TIMER1->EVENTS_COMPARE[0] == 0);
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
        m_sample.value |= (nrf_gpio_pin_read(setup.DOUT) << (23 - i));
        m_sample.count++;
    }
    EXIT:

    NRF_LOG_INFO("Number of bits: %d. ADC val: 0x%x out of 0xFFFFFF", 
    m_sample.count,
    m_sample.value,
    16777216);
}

/******** HX711 class end *********/


// static void SendTensionMeasurement(uint32_t tension)
// {
//     ret_code_t err_code;
//     uint16_t len = 0;
//     uint8_t tension_bytes[4];
    
//     len = uint32_encode(tension, tension_bytes);
//     err_code = ble_nus_data_send(m_nus, tension_bytes, &len, *m_conn_handle);
//     if (err_code != NRF_ERROR_INVALID_STATE) // TODO: remove this quick fix (used in other send functions too)
//     {
//         APP_ERROR_CHECK(err_code);
//     }
// }

// void InitBLE(ble_nus_t *nus, conn_handle_t *conn_handle)
// {
//     m_nus = nus;
//     m_conn_handle = conn_handle;
// }