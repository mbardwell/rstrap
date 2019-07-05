#include "stdint.h"
#include <nrfx.h>
#include "tension.h"
#include "ble_nus.h"
#include "nrf_log.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpiote.h"

static volatile struct hx711_sample m_sample;
static enum hx711_mode m_mode;
#ifdef DEVKIT
    #define PIN_PD_SCK 23
    #define PIN_DOUT 24
    #define PIN_VDD 25
#else
    #define PIN_PD_SCK 7
    #define PIN_DOUT 5
    #define PIN_VDD 4
#endif
#define DEFAULT_TIMER_COUNTERTOP 32
#define DEFAULT_TIMER_COMPARE 16
#define DEFAULT_ADC_RES 24
static struct hx711_setup setup = {
    PIN_PD_SCK, 
    PIN_DOUT,
    BT832_VDD,
    PIN_VDD,
    DEFAULT_TIMER_COUNTERTOP, 
    DEFAULT_TIMER_COMPARE,
    DEFAULT_ADC_RES
    };

/**
 * @brief Function for converting HX711 sample
 */
static uint32_t Hx711Convert(uint32_t sample)
{
    uint32_t converted = (sample << 8) >> 8;
    if (converted > 0xFFFFFF)
    {
        NRF_LOG_INFO("ERROR: converted value greater than possible for 24-bit sample");
        // TODO: Deal with this
    }

    return converted;
}

void gpiote_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    nrf_drv_gpiote_in_event_disable(setup.dout_pin);
    Hx711Sample();
}

void Hx711Init(enum hx711_mode mode)
{
    ret_code_t ret_code;

    m_mode = mode;

    nrf_gpio_cfg_output(setup.pd_sck_pin);
    nrf_gpio_pin_set(setup.pd_sck_pin);

    if (!nrf_drv_gpiote_is_init())
    {
        ret_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(ret_code);
    }

    nrf_drv_gpiote_in_config_t gpiote_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    nrf_gpio_cfg_input(setup.dout_pin, NRF_GPIO_PIN_NOPULL);
    ret_code = nrf_drv_gpiote_in_init(setup.dout_pin, &gpiote_config, gpiote_evt_handler);
    APP_ERROR_CHECK(ret_code);


    /* Set up timers, gpiote, and ppi for clock signal generation*/
    NRF_TIMER1->CC[0]     = 1;
    NRF_TIMER1->CC[1]     = setup.timer_compare;
    NRF_TIMER1->CC[2]     = setup.timer_countertop;
    NRF_TIMER1->SHORTS    = (uint32_t) (1 << 2);    //COMPARE2_CLEAR
    NRF_TIMER1->PRESCALER = 0;

    NRF_TIMER2->CC[0]     = m_mode;
    NRF_TIMER2->MODE      = 2;

    NRF_GPIOTE->CONFIG[1] = (uint32_t) (3 | (setup.pd_sck_pin << 8) | (1 << 16) | (1 << 20));

    NRF_PPI->CH[0].EEP   = (uint32_t) &NRF_TIMER1->EVENTS_COMPARE[0];
    NRF_PPI->CH[0].TEP   = (uint32_t) &NRF_GPIOTE->TASKS_SET[1];
    NRF_PPI->CH[1].EEP   = (uint32_t) &NRF_TIMER1->EVENTS_COMPARE[1];
    NRF_PPI->CH[1].TEP   = (uint32_t) &NRF_GPIOTE->TASKS_CLR[1];
    NRF_PPI->FORK[1].TEP = (uint32_t) &NRF_TIMER2->TASKS_COUNT; // Increment on falling edge
    NRF_PPI->CH[2].EEP   = (uint32_t) &NRF_TIMER2->EVENTS_COMPARE[0];
    NRF_PPI->CH[2].TEP   = (uint32_t) &NRF_TIMER1->TASKS_SHUTDOWN;
    NRF_PPI->CHEN = NRF_PPI->CHEN | 7;
}

void Hx711Start()
{
    NRF_LOG_DEBUG("Start sampling");
    
    NRF_GPIOTE->TASKS_CLR[1] = 1;
    // Generates interrupt when new sampling is available. 
    nrf_drv_gpiote_in_event_enable(setup.dout_pin, true);
}

void Hx711Stop()
{
    NRF_LOG_DEBUG("Stop sampling");
    nrf_drv_gpiote_in_event_disable(setup.dout_pin);
}

/* Clocks out HX711 result - if readout fails consistently, try to increase the clock period and/or enable compiler optimization */
void Hx711Sample()
{
    uint32_t converted_sample;

    NRF_TIMER2->TASKS_CLEAR = 1;
    m_sample.count = 0;
    m_sample.value = 0;
    m_sample.status = Busy;
    NRF_TIMER1->TASKS_START = 1; // Starts clock signal on PD_SCK

    for (uint32_t i=0; i < setup.adc_res; i++)
    {
        do
        {
            /* NRF_TIMER->CC[1] contains number of clock cycles.*/
            NRF_TIMER2->TASKS_CAPTURE[1] = 1;
            if (NRF_TIMER2->CC[1] >= setup.adc_res)
            {
                goto EXIT; // Readout not in sync with PD_CLK. Abort and notify error.
            }
        }
        while(NRF_TIMER1->EVENTS_COMPARE[0] == 0);
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
        m_sample.value |= (nrf_gpio_pin_read(setup.dout_pin) << (23 - i));
        m_sample.count++;
        m_sample.status = Unread;
    }
    EXIT:

    converted_sample = Hx711Convert(m_sample.value);
    
    if (converted_sample > 0x7FFFFF)
    {
        NRF_LOG_DEBUG("Sample returned a negative value. Check connections");
    }
    NRF_LOG_DEBUG("Number of bits: %d. ADC val: 0x%x or 0d%d", 
    m_sample.count,
    converted_sample,
    converted_sample);
}

nrfx_err_t Hx711SampleConvert(uint32_t *p_value)
{
    nrfx_err_t err_code = NRFX_ERROR_INVALID_STATE;

    if (p_value == NULL)
    {
        NRF_LOG_INFO("Hx711SampleConvert does not accept null pointers");
        err_code = NRFX_ERROR_NULL;
    }
    else
    {        
        if (m_sample.status == Unread)
        {
            uint32_t converted_sample = Hx711Convert(m_sample.value);
            *p_value = converted_sample;
            m_sample.status = Read;
            err_code = NRFX_SUCCESS;
        }
        Hx711Start();
    }
    
    return err_code;
}