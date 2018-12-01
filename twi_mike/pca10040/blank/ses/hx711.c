#include "hx711.h"

NRF_TIMER_Type* TIMER0 = (NRF_TIMER_Type*) TIMER0_BASE; // NRF_TIMER_Type has register structure
NRF_TIMER_Type* TIMER1 = (NRF_TIMER_Type*) TIMER1_BASE;
NRF_TIMER_Type* TIMER2 = (NRF_TIMER_Type*) TIMER2_BASE;

static volatile sample_t sample;

void dout_init(void) {
    /*
        Setup input pin for reading ADC bits
    */

    // Timer2 to ensure 1 us between PD_SCK rising edge and DOUT read
    TIMER2 -> MODE &= TIMER_MODE_MODE_Timer; // [DEFAULT]
    TIMER2 -> CC[0] |= 1; // Wait 1 us
    TIMER2 -> PRESCALER |= 4; // [DEFAULT] Counts at 1 MHz (16MHz/2^PRESCALER)
    TIMER2 -> SHORTS |= (TIMER_SHORTS_COMPARE0_CLEAR_Enabled 
                         << TIMER_SHORTS_COMPARE0_CLEAR_Pos) |
                        (TIMER_SHORTS_COMPARE0_STOP_Enabled
                         << TIMER_SHORTS_COMPARE0_STOP_Pos);
    
    NRF_PPI -> CH[3].EEP = (uint32_t) &TIMER0 -> EVENTS_COMPARE[0]; // Event when timer0 == CC[0] (pd_sck set high)
    NRF_PPI -> CH[3].TEP = (uint32_t) &TIMER2 -> TASKS_START; // Timer2 start when PD_SCK goes high
}

void pdsck_init(NRF_GPIOTE_Type* gpiote) {
    /*
        Setup timer and gpiote peripherals interconnected by PPI for PWM
    */

    // For creating the pulse
    TIMER0 -> MODE &= TIMER_MODE_MODE_Timer; // [DEFAULT]
    TIMER0 -> CC[0] |= 10; // Triggers low to high
    TIMER0 -> CC[1] |= 20; // Triggers high to low
    TIMER0 -> PRESCALER |= 4; // [DEFAULT] Counts at 1 MHz (16MHz/2^PRESCALER)
    TIMER0 -> SHORTS |= (TIMER_SHORTS_COMPARE1_CLEAR_Enabled 
                         << TIMER_SHORTS_COMPARE1_CLEAR_Pos);

    gpiote -> CONFIG[0] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) |
                          (PD_SCK << GPIOTE_CONFIG_PSEL_Pos) |
                          (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos)|
                          (GPIOTE_CONFIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos);

    // For stopping the pulse
    TIMER1 -> MODE = TIMER_MODE_MODE_LowPowerCounter;
    TIMER1 -> CC[0] |= 25; // Default number of pulses
    TIMER1 -> SHORTS |= (TIMER_SHORTS_COMPARE0_CLEAR_Enabled
                          << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
    
    // Interconnecting
    NRF_PPI -> CH[0].EEP = (uint32_t) &TIMER0 -> EVENTS_COMPARE[0]; // Event when timer0 == CC[0]
    NRF_PPI -> CH[0].TEP = (uint32_t) &gpiote -> TASKS_SET[0]; // PD_SCK pin set high

    NRF_PPI -> CH[1].EEP = (uint32_t) &TIMER0 -> EVENTS_COMPARE[1]; // Event when timer0 == CC[1]
    NRF_PPI -> CH[1].TEP = (uint32_t) &gpiote -> TASKS_CLR[0]; // PD_SCK pin set low
    NRF_PPI -> FORK[1].TEP = (uint32_t) &TIMER1 -> TASKS_COUNT; // Counter increments with falling edge

    NRF_PPI -> CH[2].EEP = (uint32_t) &TIMER1 -> EVENTS_COMPARE[0]; // Event when timer1 == CC[0]
    NRF_PPI -> CH[2].TEP = (uint32_t) &TIMER0 -> TASKS_STOP; // Stop timer0
    NRF_PPI -> CHEN |= (1 << 0 | 1 << 1 | 1 << 2); // Enable PPI channel 0, 1 and 2
}

void sample_hx711(hx711_mode_t number_of_pulses) {
    NRF_LOG_INFO("Sampling");
    NRF_LOG_FLUSH();
    TIMER1 -> CC[0] = number_of_pulses;
    NRF_PPI -> CHEN &= ~(1 << 3); // Disable PPI channel 3
    TIMER0 -> TASKS_START = 1; // Send next_conversion signal

    while (TIMER1 -> EVENTS_COMPARE[0] == 0); // hold up until next_conversion signal is sent
    TIMER1 -> TASKS_CLEAR = 1;
    TIMER1 -> EVENTS_COMPARE[0] = 0;

//    while (read_pin(DOUT)); // hold up until dout pin goes low
    NRF_PPI -> CHEN |= (1 << 3); // Enable PPI channel 3
    sample.count = 0; // Reset count

    TIMER0 -> TASKS_START = 1; // Start conversion

    for (uint32_t i=0; i < 24; i++) {
        while(TIMER2 -> EVENTS_COMPARE[0] == 0);
        TIMER2 -> EVENTS_COMPARE[0] = 0;
        sample.value |= (nrf_gpio_pin_read(DOUT) << (23-i));
        sample.count++;
    }

    NRF_LOG_INFO("Number of bits : %d", sample.count);
    NRF_LOG_INFO("ADC val: 0x%x", sample.value);
    NRF_LOG_FLUSH();
}

void twi_init() {
   /*
       Setup two-wire interface with hx711
   */

    pdsck_init(GPIOTE);
    dout_init();
}