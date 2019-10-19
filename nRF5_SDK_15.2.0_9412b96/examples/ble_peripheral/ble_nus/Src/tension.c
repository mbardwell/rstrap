#include "stdint.h"
#include <nrfx.h>
#include "tension.h"
#include "ble_nus.h"
#include "nrf_log.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "config.h"

#define HX711_DEFAULT_TIMER_COUNTERTOP 32
#define HX711_DEFAULT_TIMER_COMPARE 16
#define HX711_DEFAULT_ADC_RES 24

static hx711_evt_handler_t hx711_callback = NULL;
static tension_alert_evt_handler_t tension_alert_callback = NULL;
static struct hx711_sample m_sample;
static enum hx711_mode m_mode;
static INITIALISE_TENSION_THRESHOLD(m_tension_threshold);
struct hx711_setup *m_setup = NULL;

void gpiote_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (m_setup != NULL)
    {
        nrf_drv_gpiote_in_event_disable(m_setup->dout);
        hx711_sample();
    }
    else
    {
        NRF_LOG_WARNING("hx711 setup has not been assigned yet");
    }
}

void hx711_init(enum hx711_mode mode, struct hx711_setup *setup, hx711_evt_handler_t send_sample_cb, tension_alert_evt_handler_t alert_cb)
{
    ret_code_t ret_code;
    hx711_callback = send_sample_cb;
    tension_alert_callback = alert_cb;
    m_setup = setup;
    m_mode = mode;

    if (m_setup != NULL)
    {
        nrf_gpio_cfg_output(m_setup->pd_sck);
        nrf_gpio_pin_set(m_setup->pd_sck);

        if (!nrf_drv_gpiote_is_init())
        {
            ret_code = nrf_drv_gpiote_init();
            APP_ERROR_CHECK(ret_code);
        }

        nrf_drv_gpiote_in_config_t gpiote_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
        nrf_gpio_cfg_input(m_setup->dout, NRF_GPIO_PIN_NOPULL);
        ret_code = nrf_drv_gpiote_in_init(m_setup->dout, &gpiote_config, gpiote_evt_handler);
        APP_ERROR_CHECK(ret_code);


        /* Set up timers, gpiote, and ppi for clock signal generation*/
        NRF_TIMER1->CC[0]     = 1;
        NRF_TIMER1->CC[1]     = HX711_DEFAULT_TIMER_COMPARE;
        NRF_TIMER1->CC[2]     = HX711_DEFAULT_TIMER_COUNTERTOP;
        NRF_TIMER1->SHORTS    = (uint32_t) (1 << 2);    //COMPARE2_CLEAR
        NRF_TIMER1->PRESCALER = 0;

        NRF_TIMER2->CC[0]     = m_mode;
        NRF_TIMER2->MODE      = 2;

        NRF_GPIOTE->CONFIG[1] = (uint32_t) (3 | (m_setup->pd_sck << 8) | (1 << 16) | (1 << 20));

        NRF_PPI->CH[0].EEP   = (uint32_t) &NRF_TIMER1->EVENTS_COMPARE[0];
        NRF_PPI->CH[0].TEP   = (uint32_t) &NRF_GPIOTE->TASKS_SET[1];
        NRF_PPI->CH[1].EEP   = (uint32_t) &NRF_TIMER1->EVENTS_COMPARE[1];
        NRF_PPI->CH[1].TEP   = (uint32_t) &NRF_GPIOTE->TASKS_CLR[1];
        NRF_PPI->FORK[1].TEP = (uint32_t) &NRF_TIMER2->TASKS_COUNT; // Increment on falling edge
        NRF_PPI->CH[2].EEP   = (uint32_t) &NRF_TIMER2->EVENTS_COMPARE[0];
        NRF_PPI->CH[2].TEP   = (uint32_t) &NRF_TIMER1->TASKS_SHUTDOWN;
        NRF_PPI->CHEN = NRF_PPI->CHEN | 7;
    }
    else
    {
        NRF_LOG_WARNING("hx711 setup has not been assigned yet");
    }
}

void hx711_start()
{
    NRF_LOG_DEBUG("start sampling");
    
    NRF_GPIOTE->TASKS_CLR[1] = 1;
    // Generates interrupt when new sampling is available. 
    nrf_drv_gpiote_in_event_enable(m_setup->dout, true);
}

void hx711_stop()
{
    NRF_LOG_DEBUG("stop sampling");
    nrf_drv_gpiote_in_event_disable(m_setup->dout);
}

void check_tension(int32_t tension_sample)
{
    if (m_tension_threshold.should_be_set)
    {
        if (m_tension_threshold.safety_factor)
        {
            m_tension_threshold.value = tension_sample * m_tension_threshold.safety_factor / 100;
        }
        else
        {
            m_tension_threshold.value = tension_sample * DEFAULT_SAFETY_FACTOR_IN_PERCENT / 100;            
        }
        m_tension_threshold.should_be_set = false;
        m_tension_threshold.is_set = true;
        NRF_LOG_DEBUG("threshold set: %d", m_tension_threshold.value);
        nrf_gpio_pin_set(m_tension_threshold.alert_led_pin);
        nrf_gpio_pin_set(m_tension_threshold.setup_led_pin);
        nrf_gpio_pin_clear(m_tension_threshold.okay_led_pin);
    }

    if (m_tension_threshold.is_set)
    {
        if (tension_sample <= m_tension_threshold.value)
        {
            if (tension_alert_callback != NULL)
            {
                tension_alert_callback();
            }
            nrf_gpio_pin_set(m_tension_threshold.okay_led_pin);
            nrf_gpio_pin_set(m_tension_threshold.setup_led_pin);
            nrf_gpio_pin_clear(m_tension_threshold.alert_led_pin);
            NRF_LOG_INFO("tension below threshold. Strap loose!");
        }
    }
}

/* Clocks out HX711 result - if readout fails consistently, try to increase the clock period and/or enable compiler optimization */
void hx711_sample()
{
    NRF_TIMER2->TASKS_CLEAR = 1;
    m_sample.count = 0;
    m_sample.value = 0;
    m_sample.status = Busy;
    NRF_TIMER1->TASKS_START = 1; // Starts clock signal on PD_SCK
    NRF_LOG_INFO("sampling hx711");

    for (uint32_t i=0; i < HX711_DEFAULT_ADC_RES; i++)
    {
        do
        {
            /* NRF_TIMER->CC[1] contains number of clock cycles.*/
            NRF_TIMER2->TASKS_CAPTURE[1] = 1;
            if (NRF_TIMER2->CC[1] >= HX711_DEFAULT_ADC_RES)
            {
                NRF_LOG_WARNING("readout not in sync");
                goto EXIT; // Readout not in sync with PD_CLK. Abort and notify error.
            }
        }
        while(NRF_TIMER1->EVENTS_COMPARE[0] == 0);
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
        m_sample.value |= (nrf_gpio_pin_read(m_setup->dout) << (23 - i));
        m_sample.count++;
        m_sample.status = Unread;
    }
    EXIT:

    m_sample.value = hx711_convert(m_sample.value);
    
    if (m_sample.value > 0x7FFFFF)
    {
        NRF_LOG_WARNING("sample returned a negative value. Check connections");
        return;
    }
    NRF_LOG_DEBUG("number of bits: %d. ADC val: 0x%x or 0d%d", 
    m_sample.count,
    m_sample.value,
    m_sample.value);

    check_tension(m_sample.value);

    if (hx711_callback != NULL)
    {
        hx711_callback(&m_sample.value);
    }
}

/**
 * @brief Function for converting HX711 sample
 */
uint32_t hx711_convert(uint32_t sample)
{
    uint32_t converted = (sample << 8) >> 8;
    if (converted > 0xFFFFFF)
    {
        NRF_LOG_WARNING("converted value greater than possible for 24-bit sample");
        // TODO: Deal with this
    }

    return converted;
}

nrfx_err_t hx711_sample_convert(uint32_t *p_value)
{
    nrfx_err_t err_code = NRFX_ERROR_INVALID_STATE;

    if (p_value == NULL)
    {
        NRF_LOG_WARNING("function does not accept null pointers");
        err_code = NRFX_ERROR_NULL;
    }
    else
    {        
        if (m_sample.status == Unread)
        {
            uint32_t converted_sample = hx711_convert(m_sample.value);
            *p_value = converted_sample;
            m_sample.status = Read;
            err_code = NRFX_SUCCESS;
        }
        hx711_start();
    }
    
    return err_code;
}

void lock_in_tension_threshold(uint32_t safety_factor)
{
    m_tension_threshold.should_be_set = true;
    m_tension_threshold.safety_factor = safety_factor;
}