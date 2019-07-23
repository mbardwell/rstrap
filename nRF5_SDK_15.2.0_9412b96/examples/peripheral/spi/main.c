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


#define BMA2x2_SPI_BUS_WRITE_CONTROL_BYTE	0x7F
#define BMA2x2_SPI_BUS_READ_CONTROL_BYTE	0x80
#define SPI_BUFFER_LEN                      5
static uint8_t       m_rx_buf[SPI_BUFFER_LEN];        /**< RX buffer. */

#define BMA_ERROR_CHECK(ERR_CODE) { APP_ERROR_CHECK((ret_code_t) ERR_CODE); }


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
        NRF_LOG_INFO(" Received: %x", m_rx_buf[0]);
    }
}

s8 bma_spi_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    s8 error = NO_ERROR;
    uint8_t tx_buf[cnt+1];

    // place write bit + address in first byte
    tx_buf[0] = (reg_addr++) & BMA2x2_SPI_BUS_WRITE_CONTROL_BYTE;

    for (uint8_t string_pos = 1; string_pos < cnt+1; string_pos++)
    {   
        // place data in second byte        
        tx_buf[string_pos] = *(reg_data + string_pos - 1);
    }

    spi_xfer_done = false;
    error = (s8) nrf_drv_spi_transfer(&spi, tx_buf, cnt+1, NULL, 0);
    BMA_ERROR_CHECK(error);

    nrf_delay_us(100);

    return error;
}

s8 bma_spi_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    s8 error = NO_ERROR;
    uint8_t tx_buf[1] = {reg_addr|BMA2x2_SPI_BUS_READ_CONTROL_BYTE};
    u8 array[cnt+1];

    spi_xfer_done = false;
    error = (s8) nrf_drv_spi_transfer(&spi, tx_buf, 1, array, cnt+1);
    BMA_ERROR_CHECK(error);

    nrf_delay_us(100);

    for (uint32_t i=0; i < cnt; i++)
    {
        *(reg_data + i) = array[i+1];
    }

    return error;
}

void spi_delay(u32 millis_time)
{
    nrf_delay_ms(millis_time);
}

s32 bma2x2_data_readout(struct bma2x2_t *bma2x2)
{
	/*Local variables for reading accel x, y and z data*/
	s16	accel_x_s16, accel_y_s16, accel_z_s16 = BMA2x2_INIT_VALUE;
	/* Local variable used to assign the bandwidth value*/
	u8 bw_value_u8 = BMA2x2_INIT_VALUE;
	/* Local variable used to set the bandwidth value*/
	u8 banwid = BMA2x2_INIT_VALUE;
	/* status of communication*/
	s32 com_rslt = ERROR;


    com_rslt = bma2x2_init(bma2x2);
    com_rslt += bma2x2_set_power_mode(BMA2x2_MODE_NORMAL);


    bw_value_u8 = 0x08;/* set bandwidth of 7.81Hz*/
	com_rslt += bma2x2_set_bw(bw_value_u8);

	/* This API used to read back the written value of bandwidth*/
	com_rslt += bma2x2_get_bw(&banwid);

   	/* Read the accel X data*/
	com_rslt += bma2x2_read_accel_x(&accel_x_s16);
	/* Read the accel Y data*/
	com_rslt += bma2x2_read_accel_y(&accel_y_s16);
	/* Read the accel Z data*/
	com_rslt += bma2x2_read_accel_z(&accel_z_s16);
    NRF_LOG_INFO("accel -- x: %d, y: %d, z: %d", accel_x_s16, accel_y_s16, accel_y_s16);

    com_rslt += bma2x2_set_power_mode(BMA2x2_MODE_DEEP_SUSPEND);
        
    return com_rslt;
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
    spi_config.mode = NRF_DRV_SPI_MODE_3;
    spi_config.bit_order = NRF_SPI_BIT_ORDER_MSB_FIRST;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    NRF_LOG_INFO("SPI hardware initialised.");

    struct bma2x2_t bma2x2;
    bma2x2.chip_id = BMA2x2_INIT_VALUE;
    bma2x2.bus_write = &bma_spi_write;
    bma2x2.bus_read = &bma_spi_read;
    bma2x2.dev_addr = 102; // Random value. No chip select mechanism in place
    bma2x2.delay_msec = &spi_delay;
    BMA_ERROR_CHECK(bma2x2_init(&bma2x2));

    if (bma2x2.chip_id == 0xFA)
    {
        NRF_LOG_INFO("BMA initialised.");
    }
    else
    {
        NRF_LOG_INFO("BMA initialisation failed. Received chip id: %x", bma2x2.chip_id);
    }

    while (1)
    {
        bma2x2_data_readout(&bma2x2);
        while (!spi_xfer_done)
        {
            __WFE();
        }
        NRF_LOG_FLUSH();

        nrf_delay_ms(1000);
    }
}
