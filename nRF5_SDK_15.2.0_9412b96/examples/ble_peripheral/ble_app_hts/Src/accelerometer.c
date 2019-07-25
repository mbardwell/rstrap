#include "accelerometer.h"
#include "bma2x2.h" 
#include "nrf_drv_spi.h"
#include "nrf_delay.h"

#define SPI_INSTANCE 0

static const nrf_drv_spi_t m_spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
static volatile bool spi_xfer_done;


#define BMA2x2_SPI_BUS_WRITE_CONTROL_BYTE	0x7F
#define BMA2x2_SPI_BUS_READ_CONTROL_BYTE	0x80

#ifdef DEVKIT
    #define BMA_SPI_SS_PIN 19
    #define BMA_SPI_MISO_PIN 23
    #define BMA_SPI_MOSI_PIN 22
    #define BMA_SPI_SCK_PIN 20
#else
    #define BMA_SPI_SS_PIN 17
    #define BMA_SPI_MISO_PIN 8
    #define BMA_SPI_MOSI_PIN 6
    #define BMA_SPI_SCK_PIN 15
#endif

void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
}

void bma_spi_init()
{
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = BMA_SPI_SS_PIN;
    spi_config.miso_pin = BMA_SPI_MISO_PIN;
    spi_config.mosi_pin = BMA_SPI_MOSI_PIN;
    spi_config.sck_pin  = BMA_SPI_SCK_PIN;
    spi_config.mode = NRF_DRV_SPI_MODE_3;
    spi_config.bit_order = NRF_SPI_BIT_ORDER_MSB_FIRST;
    APP_ERROR_CHECK(nrf_drv_spi_init(&m_spi, &spi_config, spi_event_handler, NULL));
    NRF_LOG_INFO("spi initialised");
}

void send_message()
{
    s8 error = NO_ERROR;
    u8 tx_buf[] = {0x1A};

    spi_xfer_done = false;
    error = (s8) nrf_drv_spi_transfer(&m_spi, tx_buf, sizeof(tx_buf), NULL, 0);

    nrf_delay_ms(1000);
}