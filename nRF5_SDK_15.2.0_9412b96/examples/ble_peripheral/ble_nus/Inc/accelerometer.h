#pragma once
#include "nrf_log.h"
#include "bma2x2.h"
#include "ble_nus.h"
#include "nrf_drv_spi.h"

#define BMA_ERROR_CHECK(ERR_CODE) { APP_ERROR_CHECK((ret_code_t) ERR_CODE); }

struct bma_xyz_sample {
    s16 x;
    s16 y;
    s16 z;
};

void bma_spi_init(nrf_drv_spi_config_t *spi_config);

s8 bma_spi_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

s8 bma_spi_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

void spi_delay(u32 millis_time);

void bma2x2_sample(struct bma_xyz_sample *sample);