#pragma once

#include "stdint.h"
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

struct hx711_sample
{
   uint32_t   value;   // buffer for ADC read
   uint8_t    count;   // number of bitshifts. Read complete when count == ADC_RES 
};

void InitHx711(enum hx711_mode mode);
void StartHx711(bool single);
void SampleHx711();