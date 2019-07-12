/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
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
/** @file
 * @defgroup tw_sensor_example main.c
 * @{
 * @ingroup nrf_twi_example
 * @brief TWI Sensor Example main file.
 *
 * This file contains the source code for a sample application using TWI.
 *
 */

#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "bma2x2.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* I2C defines */
#define	I2C_BUFFER_LEN      8
#define BMA2x2_BUS_READ_WRITE_ARRAY_INDEX	1

#define ADDR_LSB            22
#define SDA_PIN             20
#define SCL_PIN             19
/* Common addresses definition for temperature sensor. */
#define LM75B_ADDR          (0x90U >> 1)

#define LM75B_REG_TEMP      0x00U
#define LM75B_REG_CONF      0x01U
#define LM75B_REG_THYST     0x02U
#define LM75B_REG_TOS       0x03U

/* Mode for LM75B. */
#define NORMAL_MODE 0U

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read from temperature sensor. */
static uint8_t m_sample;

/**
 * @brief Function for setting active mode on MMA7660 accelerometer.
 */
void LM75B_set_mode(void)
{
    ret_code_t err_code;

    /* Writing to LM75B_REG_CONF "0" set temperature sensor in NORMAL mode. */
    uint8_t reg[2] = {LM75B_REG_CONF, NORMAL_MODE};
    err_code = nrf_drv_twi_tx(&m_twi, BMA2x2_I2C_ADDR1, reg, sizeof(reg), false);
    APP_ERROR_CHECK(err_code);
    while (m_xfer_done == false);

    /* Writing to pointer byte. */
    reg[0] = LM75B_REG_TEMP;
    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, BMA2x2_I2C_ADDR1, reg, 1, false);
    APP_ERROR_CHECK(err_code);
    while (m_xfer_done == false);
}

/**
 * @brief Function for handling data from temperature sensor.
 *
 * @param[in] temp          Temperature in Celsius degrees read from sensor.
 */
__STATIC_INLINE void data_handler(uint8_t temp)
{
    NRF_LOG_INFO("Temperature: %d Celsius degrees.", temp);
}

/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
            {
                data_handler(m_sample);
            }
            m_xfer_done = true;
            break;
        default:
            break;
    }
}

/**
 * @brief UART initialization.
 */
void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_lm75b_config = {
       .scl                = SCL_PIN,
       .sda                = SDA_PIN,
       .frequency          = NRF_DRV_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_lm75b_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);

    // config output pin to zero
    nrf_gpio_cfg_output(ADDR_LSB);
    nrf_gpio_pin_clear(ADDR_LSB);
}

// /**
//  * @brief Function for reading data from temperature sensor.
//  */
// static void read_sensor_data()
// {
//     m_xfer_done = false;

//     /* Read 1 byte from the specified address - skip 3 bits dedicated for fractional part of temperature. */
//     ret_code_t err_code = nrf_drv_twi_rx(&m_twi, BMA2x2_I2C_ADDR1, &m_sample, sizeof(m_sample));
//     APP_ERROR_CHECK(err_code);
// }

s32 init_bma(struct bma2x2_t bma2x2)
{
    s32 com_rslt = ERROR;

    com_rslt = bma2x2_init(&bma2x2);
    APP_ERROR_CHECK(com_rslt);
    com_rslt += bma2x2_set_power_mode(BMA2x2_MODE_NORMAL);

    u8 bw_value_u8 = BMA2x2_INIT_VALUE;
    bw_value_u8 = 0x08;/* set bandwidth of 7.81Hz*/
	com_rslt += bma2x2_set_bw(bw_value_u8);

    u8 banwid = BMA2x2_INIT_VALUE;
	/* This API used to read back the written value of bandwidth*/
	com_rslt += bma2x2_get_bw(&banwid);

    return com_rslt;
}

s8 bma_i2c_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BMA2x2_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN];
	u8 stringpos = BMA2x2_INIT_VALUE;

    array[BMA2x2_INIT_VALUE] = reg_addr;
	for (stringpos = BMA2x2_INIT_VALUE; stringpos < cnt; stringpos++) {
		array[stringpos + BMA2x2_BUS_READ_WRITE_ARRAY_INDEX] =
		*(reg_data + stringpos);
	}

    m_xfer_done = false;
    iError = nrf_drv_twi_tx(&m_twi, dev_addr, array, sizeof(array), false);
    NRF_LOG_INFO("error: %d, array: %d", (ret_code_t) iError, array[0]);
    NRF_LOG_FLUSH();
    APP_ERROR_CHECK((ret_code_t) iError);
    while (m_xfer_done == false);


    return (s8) iError;
}

s8 bma_i2c_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BMA2x2_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN] = {BMA2x2_INIT_VALUE};
	u8 stringpos = BMA2x2_INIT_VALUE;

    bma_i2c_write(dev_addr, reg_addr, NULL, 0); // write control byte

    nrf_delay_us(100);

	array[BMA2x2_INIT_VALUE] = reg_addr;

    m_xfer_done = false;
    iError = nrf_drv_twi_rx(&m_twi, BMA2x2_I2C_ADDR1, array, cnt);
    APP_ERROR_CHECK((ret_code_t) iError);

    for (stringpos = BMA2x2_INIT_VALUE; stringpos < cnt; stringpos++) {
		*(reg_data + stringpos) = array[stringpos];
    }

	return (s8)iError;
}

void i2c_delay(u32 millis_time)
{
    nrf_delay_ms(millis_time);
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("\r\n starting twi example");

    twi_init();
    LM75B_set_mode();
    NRF_LOG_INFO("\r\n initialised twi");

    struct bma2x2_t bma2x2;
    bma2x2.bus_write = &bma_i2c_write;
    bma2x2.bus_read = &bma_i2c_read;
    bma2x2.dev_addr = BMA2x2_I2C_ADDR1;
    bma2x2.delay_msec = &i2c_delay;
    init_bma(bma2x2);
    NRF_LOG_INFO("\r\n initialised BMA2x2");

    s32 com_rslt = ERROR;
    /* bma2x2acc_data structure used to read accel xyz data*/
	struct bma2x2_accel_data sample_xyz;

    while (true)
    {
        nrf_delay_ms(500);

        do
        {
            __WFE();
        }while (m_xfer_done == false);

        /* accessing the bma2x2acc_data parameter by using sample_xyz*/
        /* Read the accel XYZ data*/
        com_rslt += bma2x2_read_accel_xyz(&sample_xyz);
        APP_ERROR_CHECK((ret_code_t) com_rslt);

        NRF_LOG_INFO("X: %d, Y: %d, Z: %d", sample_xyz.x, sample_xyz.y, sample_xyz.z);
        NRF_LOG_FLUSH();
    }
}

/** @} */
