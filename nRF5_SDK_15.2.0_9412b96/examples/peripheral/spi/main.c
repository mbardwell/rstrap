/**
 * Copyright (c) 2015 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "main.h"
#include "bma2x2.h"
#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

#define UNUSED(x) (void) x
#define BMA2x2_SPI_BUS_WRITE_CONTROL_BYTE	0x7F
#define BMA2x2_SPI_BUS_READ_CONTROL_BYTE	0x80
#define SPI_BUFFER_LEN                      5
static uint8_t       m_rx_buf[SPI_BUFFER_LEN];        /**< RX buffer. */

#define BMA_ERROR_CHECK(ERR_CODE) { NRF_LOG_INFO("BMA Error: %d", (int8_t) ERR_CODE); }


/**
 * @brief SPI user event handler.
 * @param event     
 */
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
    if (m_rx_buf[0] != 0)
    {
        NRF_LOG_INFO(" Received:");
        NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    }
}

s32 init_bma(struct bma2x2_t bma2x2)
{
    s32 com_rslt = ERROR;
    uint8_t tx[1] = {0x80};
    uint8_t rx[1] = {0};
    nrf_drv_spi_transfer(&spi, tx, 0, rx, 1);
    nrf_delay_us(50);
    nrf_drv_spi_transfer(&spi, tx, 0, rx, 1);


    // u8 data = 0;
    // bma_spi_read((u8) 0, (u8) 0x00, &data, (u8) 1);
    
    // com_rslt = bma2x2_init(&bma2x2);

    return com_rslt;
}

s8 bma_spi_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    UNUSED(dev_addr);

    s8 error = NO_ERROR;
    uint8_t tx_buf[2*SPI_BUFFER_LEN];
    uint8_t rx_buf[SPI_BUFFER_LEN];

    for (uint8_t string_pos = 0; string_pos < cnt; string_pos++)
    {   
        // place write bit + address in first byte
        tx_buf[2*string_pos] = (reg_addr++) & BMA2x2_SPI_BUS_WRITE_CONTROL_BYTE;
        // place data in second byte (hence the + 1)        
        tx_buf[2*string_pos + 1] = *(reg_data + string_pos);
    }

    error = (s8) nrf_drv_spi_transfer(&spi, tx_buf, (uint8_t) cnt, rx_buf, (uint8_t) cnt);
    NRF_LOG_INFO("write error: %d", (int8_t) error);

    return error;
}

s8 bma_spi_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    UNUSED(dev_addr);

    s8 error = NO_ERROR;
    uint8_t tx_buf[SPI_BUFFER_LEN];
    uint8_t rx_buf[SPI_BUFFER_LEN];

    tx_buf[BMA2x2_INIT_VALUE] = reg_addr | BMA2x2_SPI_BUS_READ_CONTROL_BYTE;

    error = (s8) nrf_drv_spi_transfer(&spi, tx_buf, (uint8_t) cnt, rx_buf, (uint8_t) cnt);
    NRF_LOG_INFO("read error: %d, tx_buf: %x, rx_buf: %x", (int8_t) error, tx_buf[0], rx_buf[0]);

    for (uint8_t string_pos = 0; string_pos < cnt; string_pos++)
    {   
        *(reg_data + string_pos) = rx_buf[string_pos];
    }

    return error;
}

void spi_delay(u32 millis_time)
{
    nrf_delay_ms(millis_time);
}

int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_SS_PIN; //29 -> 17
    spi_config.miso_pin = SPI_MISO_PIN; //28 -> 8
    spi_config.mosi_pin = SPI_MOSI_PIN; //4 -> 6
    spi_config.sck_pin  = SPI_SCK_PIN; //3 -> 15
    spi_config.mode = NRF_DRV_SPI_MODE_2;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    NRF_LOG_INFO("SPI hardware initialised.");

    spi_xfer_done = false;

    struct bma2x2_t bma2x2;
    bma2x2.bus_write = &bma_spi_write;
    bma2x2.bus_read = &bma_spi_read;
    bma2x2.delay_msec = &spi_delay;
    BMA_ERROR_CHECK(init_bma(bma2x2));

    NRF_LOG_INFO("BMA API initialised.");

    while (1)
    {
        while (!spi_xfer_done)
        {
            __WFE();
        }

        spi_xfer_done = false;

        NRF_LOG_FLUSH();

        nrf_delay_ms(200);
    }
}
