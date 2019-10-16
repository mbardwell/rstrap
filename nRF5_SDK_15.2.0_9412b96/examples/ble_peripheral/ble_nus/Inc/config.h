#pragma once

#include <stdbool.h>
#include "pca10040.h"


#ifdef DEVKIT
    #define BMA_INTERRUPT_1 16
    #define BMA_INTERRUPT_2 15
    #define BMA_SPI_SS_PIN 19
    #define BMA_SPI_MISO_PIN 23
    #define BMA_SPI_MOSI_PIN 22
    #define BMA_SPI_SCK_PIN 20
    #define BMA_TAP_INTERRUPT_PIN BUTTON_2 //14

    #define HX711_PIN_PD_SCK 2
    #define HX711_PIN_DOUT 26
    #define HX711_PIN_VDD 27

    #define LED1 17
    #define LED2 18
    #define LED3 19

    #define SW1 BUTTON_1 //13
#else
    #define BMA_INTERRUPT_1 16
    #define BMA_INTERRUPT_2 14
    #define BMA_SPI_SS_PIN 17
    #define BMA_SPI_MISO_PIN 8
    #define BMA_SPI_MOSI_PIN 6
    #define BMA_SPI_SCK_PIN 15
    #define BMA_TAP_INTERRUPT_PIN BMA_INTERRUPT_1

    #define HX711_PIN_PD_SCK 7
    #define HX711_PIN_DOUT 5
    #define HX711_PIN_VDD 4

    #define LED1 12 // red
    #define LED2 10 // green
    #define LED3 9  // blue

    #define SW1 13
#endif

#define NUS_ACCEL_X_TAG			0x0
#define NUS_ACCEL_Y_TAG			0x1
#define NUS_ACCEL_Z_TAG			0x2
#define NUS_BATTERY_TAG			0x3
#define NUS_TEMP_TAG            0x4
#define NUS_TENSION_TAG         0x5

#define DEVICE_NAME "rStrap"                    /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME "Route7"              /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUM "prototype-v1"                /**< Model number. Will be passed to Device Information Service. */

#define APP_BLE_OBSERVER_PRIO 3 /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG 1  /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_INTERVAL 40 /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */

#define APP_ADV_DURATION 18000 /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define ACCEL_LEVEL_MEAS_INTERVAL APP_TIMER_TICKS(2000)
#define BATTERY_LEVEL_MEAS_INTERVAL APP_TIMER_TICKS(2000)
#define TEMP_LEVEL_MEAS_INTERVAL APP_TIMER_TICKS(2000)
#define TENSION_LEVEL_MEAS_INTERVAL APP_TIMER_TICKS(2000)

#define MIN_CONN_INTERVAL MSEC_TO_UNITS(50, UNIT_1_25_MS)  /**< Minimum acceptable connection interval (0.5 seconds) */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(100, UNIT_1_25_MS) /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY 0                                     /**< Slave latency. */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS)    /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000) /**< Time from initiating event (connect or start of indication) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND 0                               /**< Perform bonding. */
#define SEC_PARAM_MITM 0                               /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC 0                               /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS 0                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES BLE_GAP_IO_CAPS_NONE /**< No I/O capabilities. */
#define SEC_PARAM_OOB 0                                /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE 7                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE 16                      /**< Maximum encryption key size. */

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */