#pragma once

#include "stdint.h"
#include <nrfx.h>
#include "ble_nus.h"
#include "nrf_drv_gpiote.h"

typedef void (*hx711_evt_handler_t) (uint32_t* data);

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
    uint32_t   value;   // buffer for ADC read
    uint8_t    count;   // number of bitshifts. Read complete when count == ADC_RES 
    enum hx711_sample_status status;
};

struct tension_threshold_t
{
    bool is_set;
    bool should_be_set;
    float safety_factor;
    uint32_t value;
    uint32_t alert_led_pin;
    uint32_t setup_led_pin;
    uint32_t okay_led_pin;
};


void gpiote_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
/**
 * @brief Function for initialising HX711
 * @param[in] mode Set HX711 to sample channel A/B at gain 128/32/64
 */
void hx711_init(enum hx711_mode mode, struct hx711_setup *setup, hx711_evt_handler_t callback);

/**
 * @brief Function for starting HX711 sampling
 */
void hx711_start();

/**
 * @brief Function for starting HX711 sampling
 */
void hx711_sample();


uint32_t hx711_convert(uint32_t sample);
/**
 * @brief Blocking function for executing a single ADC conversion.
 *
 * This function starts a single conversion, waits for it to finish, and returns the result.
 * This function will fail if the HX711 is busy.
 * 
 * @param[out] p_value Pointer to the location where the result should be placed.
 */
nrfx_err_t hx711_sample_convert(uint32_t *p_value);

void lock_in_tension_threshold(uint32_t safety_factor);