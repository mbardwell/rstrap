#include "hx711.h"
#include "boards.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_default_backends.c"
#include "nrf_log_backend_uart.c"
#include "nrf_log_backend_serial.c"

// Prototypes
void twi_init(void);
void sample_hx711(hx711_mode_t number_of_pulses);