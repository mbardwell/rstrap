#include "stdint.h"
#include "tension.h"
#include "ble_nus.h"
#include "nrf_log.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpiote.h"

static void InitTWI();
static void InitBLE();
static void SampleTension();
static void SendTensionMeasurement(uint32_t tension);
static void GpioteEventHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

static ble_nus_t *m_nus = NULL;
static conn_handle_t *m_conn_handle = NULL;

void TensionCommunicationInit(ble_nus_t *nus, conn_handle_t *conn_handle)
{
    InitTWI();
    /* Eventually, all BLE send/receive functions should go in one translation unit. Until then
       this is an easy way to pass BLE-needed information between translation units */
    InitBLE(nus, conn_handle);
}

static void InitTWI()
{
    nrf_drv_gpiote_pin_t dummy1 = 0;
    nrf_gpiote_polarity_t dummy2 = 0;
    GpioteEventHandler(dummy1, dummy2);
}

static void InitBLE(ble_nus_t *nus, conn_handle_t *conn_handle)
{
    m_nus = nus;
    m_conn_handle = conn_handle;
}

static void GpioteEventHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    SampleTension();
}

void StartTensionSample()
{

}

static void SampleTension()
{
    uint32_t tension = 10;
    if (m_nus != NULL &&
        m_conn_handle != NULL)
        {
            SendTensionMeasurement(tension);
        }
}

static void SendTensionMeasurement(uint32_t tension)
{
    ret_code_t err_code;
    uint16_t len = 0;
    uint8_t tension_bytes[4];
    
    len = uint32_encode(tension, tension_bytes);
    err_code = ble_nus_data_send(m_nus, tension_bytes, &len, *m_conn_handle);
    if (err_code != NRF_ERROR_INVALID_STATE) // TODO: remove this quick fix (used in other send functions too)
    {
        APP_ERROR_CHECK(err_code);
    }
}