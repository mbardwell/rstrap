#include <stdint.h>         // standard types
#include <stdbool.h>        // true & false
#include "nrf52.h"          // Definitions of all register types
#include "boards.h"         // Include board-specific defines
#include "nrf_log.h"        // NRF_LOG_INFO
#include "nrf_log_ctrl.h"   // NRF_LOG_FLUSH

// Used peripheral base addresses
#define TIMER0_BASE 0x40008000UL
#define TIMER1_BASE 0x40009000UL
#define TIMER2_BASE 0x4000A000UL

#define GPIOTE_BASE 0x40006000UL
#define GPIOTE (NRF_GPIOTE_Type*) GPIOTE_BASE

#define PD_SCK          23
#define DOUT            24
#define VDD             25

typedef enum {
    CHANNEL_A_128 = 25, // check this
    CHANNEL_B_128 = 26, // check this
    CHANNEL_A_64 = 27 // check this
} hx711_mode_t;

typedef struct {
    uint32_t value;
    uint8_t count;
} sample_t;