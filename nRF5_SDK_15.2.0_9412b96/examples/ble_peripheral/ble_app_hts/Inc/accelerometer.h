#pragma once
#include "nrf_log.h"
#include "bma2x2.h"

#define BMA_ERROR_CHECK(ERR_CODE) { APP_ERROR_CHECK((ret_code_t) ERR_CODE); }

void bma_spi_init();

s8 bma_spi_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

s8 bma_spi_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

void spi_delay(u32 millis_time);

s32 bma2x2_data_readout();