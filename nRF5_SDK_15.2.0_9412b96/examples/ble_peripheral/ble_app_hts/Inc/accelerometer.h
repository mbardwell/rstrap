#pragma once
#include "nrf_log.h"

#define BMA_ERROR_CHECK(ERR_CODE) { APP_ERROR_CHECK((ret_code_t) ERR_CODE); }

void bma_spi_init();

void send_message();
