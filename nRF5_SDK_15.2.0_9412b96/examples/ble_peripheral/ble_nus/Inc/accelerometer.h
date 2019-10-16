#pragma once
#include "stdint.h"
#include "nrf_log.h"
#include "bma2x2.h"
#include "ble_nus.h"
#include "nrf_drv_spi.h"
#include "nrf_drv_gpiote.h"

/* for the time being we're using APP_ERROR_CHECK because it will handle the error
 * if any non-zero ERR_CODE is returned (by entering a fault handler). 
 * APP_ERROR_CHECK is only for nrf error codes though, not the BMA, so this needs to 
 * be changed to nrf_log or something in the future
 */
#define BMA_ERROR_CHECK(ERR_CODE) { APP_ERROR_CHECK((ret_code_t) ERR_CODE); }

typedef s32 bma_error_t;

struct bma_xyz_sample {
    s16 x;
    s16 y;
    s16 z;
};

void bma_spi_init(nrf_drv_spi_config_t *spi_config);

s8 bma_spi_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

s8 bma_spi_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

void spi_delay(u32 millis_time);

void bma_sample(struct bma_xyz_sample *sample);

void bma_wake();

void bma_sleep();

void bma_enable_tap_interrupt(uint32_t pin, nrf_drv_gpiote_evt_handler_t evt_handler);