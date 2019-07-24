#include "accelerometer.h"
#include "bma2x2.h" 
#include "nrf_drv_spi.h"

#define SPI_INSTANCE 0
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
static volatile bool spi_xfer_done;

#define BMA_ERROR_CHECK(ERR_CODE) { APP_ERROR_CHECK((ret_code_t) ERR_CODE); }


void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
}

void bma_spi_init()
{
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = BMA_SPI_SS_PIN; //29 -> 17
    spi_config.miso_pin = BMA_SPI_MISO_PIN; //28 -> 8
    spi_config.mosi_pin = BMA_SPI_MOSI_PIN; //4 -> 6
    spi_config.sck_pin  = BMA_SPI_SCK_PIN; //3 -> 15
    spi_config.mode = NRF_DRV_SPI_MODE_3;
    spi_config.bit_order = NRF_SPI_BIT_ORDER_MSB_FIRST;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));
}