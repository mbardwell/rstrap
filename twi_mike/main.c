/** @file
* @brief rStrap TWI library
*
* NOTE: To get build to work, I right clicked nRF_micro-ecc folder and excluded it from build
*/

#include "main.h"

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void button_init(NRF_GPIOTE_Type* gpiote) {
    /*
        Setup button 1 (pin 13)
    */

    NRF_P0 -> PIN_CNF[BUTTON_1] = (3 << 2); // Pull up pin
    gpiote -> CONFIG[1] = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
                          (DOUT << GPIOTE_CONFIG_PSEL_Pos) |
                          (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos);
}

int main(void)
{
    log_init();
    twi_init();

    hx711_mode_t mode = CHANNEL_A_128;
    sample_hx711(mode);

    while(true) {}
}
/** @} */
