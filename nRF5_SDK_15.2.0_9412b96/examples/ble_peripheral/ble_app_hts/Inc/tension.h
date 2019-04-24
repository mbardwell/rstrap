#pragma once

#include "stdint.h"
#include <nrfx.h>
#include "ble_nus.h"


typedef uint16_t conn_handle_t;

enum hx711_mode
{
   INPUT_CH_A_128 = 25,
   INPUT_CH_B_32 = 26,
   INPUT_CH_A_64 = 27
};

enum hx711_evt
{
    DATA_READY,
    DATA_ERROR
};

struct hx711_setup // this should be variables in class
{
    uint32_t PD_SCK;
    uint32_t DOUT;
    uint32_t VDD;
    uint32_t TIMER_COUNTERTOP;
    uint32_t TIMER_COMPARE;
    uint32_t ADC_RES;
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

/**
 * @brief Function for initialising HX711
 * @param[in] mode Set HX711 to sample channel A/B at gain 128/32/64
 */
void Hx711Init(enum hx711_mode mode);

/**
 * @brief Function for starting HX711 sampling
 */
void Hx711Start();

/**
 * @brief Function for starting HX711 sampling
 */
void Hx711Sample();

/**
 * @brief Blocking function for executing a single ADC conversion.
 *
 * This function starts a single conversion, waits for it to finish, and returns the result.
 * This function will fail if the HX711 is busy.
 * 
 * @param[out] p_value Pointer to the location where the result should be placed.
 */
nrfx_err_t Hx711SampleConvert(uint32_t *p_value);