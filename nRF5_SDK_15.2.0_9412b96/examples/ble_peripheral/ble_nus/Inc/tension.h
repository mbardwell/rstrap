#pragma once

#include "stdint.h"
#include <nrfx.h>
#include "ble_nus.h"
#include "nrf_drv_gpiote.h"

typedef void (*hx711_evt_handler_t) (int32_t* data);

typedef void (*tension_alert_evt_handler_t) ();

enum hx711_mode
{
   INPUT_CH_A_128 = 25,
   INPUT_CH_B_32 = 26,
   INPUT_CH_A_64 = 27
};

struct hx711_setup // this should be variables in class
{
    uint32_t pd_sck;
    uint32_t dout;
    uint32_t vdd;
};

enum hx711_sample_status
{
    Unknown,
    Busy,
    Unread,
    Read
};

struct hx711_sample
{
    int32_t    value;   // ADC buffer. Output is 24-bit 2's complement
    uint8_t    count;   // number of bitshifts. Read complete when count == ADC_RES 
    enum hx711_sample_status status;
};

struct tension_threshold_t
{
    bool is_set;
    bool should_be_set;
    float safety_factor;
    int32_t value; 
    uint32_t alert_led_pin;
    uint32_t setup_led_pin;
    uint32_t okay_led_pin;
};


void gpiote_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
/**
 * @brief Function for initialising HX711
 * @param[in] mode Set HX711 to sample channel A/B at gain 128/32/64
 */
void hx711_init(enum hx711_mode, struct hx711_setup*, hx711_evt_handler_t, tension_alert_evt_handler_t);

/**
 * @brief Function for starting HX711 sampling
 */
void hx711_start();

/**
 * @brief Function for starting HX711 sampling
 */
void hx711_sample();

/**
 * @brief Function for converting HX711 sample to 32 bit signed format
 * 
 * HX711 output is 24 bits of data in 2's complement format.
 * The minimum value is 0x800000: -8388608
 *                      0xFFFFFF: -1
 *                      0x000000:  0
 * The maximum value is 0x7FFFFF:  8388607
 */
void hx711_convert_24_to_32_sbit(int32_t* sample);

void lock_in_tension_threshold(uint32_t safety_factor);