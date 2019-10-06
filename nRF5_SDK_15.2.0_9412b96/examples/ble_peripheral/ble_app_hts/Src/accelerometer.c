#include "accelerometer.h"
#include "bma2x2.h" 
#include "nrf_drv_spi.h"
#include "nrf_delay.h"

#define SPI_INSTANCE 0

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
struct bma2x2_t bma2x2;
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
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    bma2x2.chip_id = BMA2x2_INIT_VALUE;
    bma2x2.bus_write = &bma_spi_write;
    bma2x2.bus_read = &bma_spi_read;
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




void send_accel_over_uart(s16 x, s16 y, s16 z, ble_nus_t* m_nus, uint16_t* m_conn_handle)
{
    ret_code_t err_code;
    static uint16_t max_n_digits = 3; // accel is 16-bit measurement. Add byte for EOL char
    uint8_t accel_x[max_n_digits];
    uint8_t accel_y[max_n_digits];
    uint8_t accel_z[max_n_digits];

    uint16_big_encode((u16) x, accel_x);
    uint16_big_encode((u16) y, accel_y);
    uint16_big_encode((u16) z, accel_z);

    err_code = ble_nus_data_send(m_nus, accel_x, &max_n_digits, *m_conn_handle);
    err_code = ble_nus_data_send(m_nus, accel_y, &max_n_digits, *m_conn_handle);
    err_code = ble_nus_data_send(m_nus, accel_z, &max_n_digits, *m_conn_handle);

    if (err_code != NRF_ERROR_INVALID_STATE) // TODO: remove this quick fix (used in other send functions too)
    {
        APP_ERROR_CHECK(err_code);
    }
}




s32 bma2x2_data_readout(ble_nus_t* nus, uint16_t* handler)
{
	/*Local variables for reading accel x, y and z data*/
	s16	accel_x_s16, accel_y_s16, accel_z_s16 = BMA2x2_INIT_VALUE;
	/* Local variable used to assign the bandwidth value*/
	u8 bw_value_u8 = BMA2x2_INIT_VALUE;
	/* Local variable used to set the bandwidth value*/
	u8 banwid = BMA2x2_INIT_VALUE;
	/* status of communication*/
	s32 com_rslt = NO_ERROR;

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

    BMA_ERROR_CHECK(com_rslt);

    send_accel_over_uart(accel_x_s16, accel_y_s16, accel_z_s16, nus, handler);
        
    return com_rslt;
}