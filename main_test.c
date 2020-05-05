#include "app_error.h"
#include "app_timer.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_hci.h"
#include "ble_nus.h"
#include "boards.h"
#include "bsp_btn_ble.h"
#include "math.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_delay.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_timer.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrfx_spim.h"
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#include "network.h"

#include "tcptest.h"
#include "cmd.h"

#include "ble.h"
#include "ble_sensors.h"
#include "temperature.h"

#include "pulse_uicr.h"


#define SAMPLES_IN_BUFFER 8
static nrf_saadc_value_t m_buffer_pool[4][SAMPLES_IN_BUFFER];

//BLE variables
uint16_t bytes_to_send = 20;
int flag = 0;

#if defined(UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined(UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_cli.h"
#include "nrf_cli_uart.h"
#include "nrf_cli_rtt.h"

#define CLI_EXAMPLE_LOG_QUEUE_SIZE  (16)

NRF_CLI_UART_DEF(m_cli_uart_transport, 0, 256, 16);
NRF_CLI_DEF(m_cli_uart,
            "uart_cli:~$ ",
            &m_cli_uart_transport.transport,
            '\r',
            CLI_EXAMPLE_LOG_QUEUE_SIZE);

NRF_CLI_RTT_DEF(m_cli_rtt_transport);
NRF_CLI_DEF(m_cli_rtt,
            "rtt_cli:~$ ",
            &m_cli_rtt_transport.transport,
            '\n',
            CLI_EXAMPLE_LOG_QUEUE_SIZE);

/**@brief Function for initializing the timer module.
 */


static void timers_init(void) {
  ret_code_t err_code = app_timer_init();
  APP_ERROR_CHECK(err_code);
}

static void cli_init(void)
{
    ret_code_t ret;

    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;
    uart_config.pseltxd = TX_PIN_NUMBER;
    uart_config.pselrxd = RX_PIN_NUMBER;
    uart_config.hwfc    = NRF_UART_HWFC_DISABLED;
    ret = nrf_cli_init(&m_cli_uart, &uart_config, true, false, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(ret);
}

static void cli_process(void)
{
    nrf_cli_process(&m_cli_uart);
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void) {
  uint32_t err_code;// = bsp_indication_set(BSP_INDICATE_IDLE);
  APP_ERROR_CHECK(err_code);

  // Prepare wakeup buttons.
  err_code = bsp_btn_ble_sleep_mode_prepare();
  APP_ERROR_CHECK(err_code);

  // Go to system-off mode (this function will not return; wakeup will cause a reset).
  err_code = sd_power_system_off();
  APP_ERROR_CHECK(err_code);
}



/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool *p_erase_bonds) {
  bsp_event_t startup_event;

  uint32_t err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
  APP_ERROR_CHECK(err_code);

  //err_code = bsp_btn_ble_init(NULL, &startup_event);
  APP_ERROR_CHECK(err_code);

  *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void) {
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for initializing power management.
 */
static void power_management_init(void) {
  ret_code_t err_code;
  err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void) {
  UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
  nrf_pwr_mgmt_run();
}


static TaskHandle_t network_task_handle;
static TaskHandle_t sensors_task_handle;
static TaskHandle_t cli_task_handle;


void sensors_task_fn(void *arg) {
  ret_code_t err_code;
  while (1) {
    vTaskDelay(1500);
    #ifdef BOARD_CUSTOM
    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
    nrf_drv_saadc_sample();
    DHT_task_callback(38);
    vTaskDelay(1);
    DHT_task_callback(39);
    vTaskDelay(1);
    DHT_task_callback(10);
    vTaskDelay(1);
    Read_Temperature(44);
    vTaskDelay(1);
    Read_Temperature(27);
    vTaskDelay(1);
    Read_Temperature(6);
    vTaskDelay(1);
    Read_Temperature(7);
    vTaskDelay(1);
    Read_Temperature(8);
    vTaskDelay(1);
    Read_Temperature(40);
    vTaskDelay(1);
    Read_Temperature(11);
    vTaskDelay(1);
    Read_Temperature(33);
    vTaskDelay(1);
    Read_Temperature(34);
    vTaskDelay(1);
    Read_Temperature(35);
    vTaskDelay(1);
    Read_Temperature(36);
    vTaskDelay(1);
    Read_Temperature(37);
    vTaskDelay(1);
    energy_measurement();
    #endif
    NRF_LOG_INFO("ble sensors update %x", ble_sensors_update());
    cmd_sensors_timer();
  }
}

void cli_task_fn(void *arg) {
  ret_code_t ret;
  ret = nrf_cli_start(&m_cli_uart);
  while(1)
  {
    cli_process();
    vTaskDelay(10);
  }
}


void randomize_session_Id(void) {
  ret_code_t ret;
  uint8_t b = 0;
  uint32_t i;
  while (b < 4) {
    ret = sd_rand_application_bytes_available_get(&b);
  }
  ret = sd_rand_application_vector_get((uint8_t *)&i, 4);
  sprintf(sId + 8, "%u", i);
}

/**@brief Application main function.
 */
int main(void) {
  volatile ret_code_t ret;
  uint32_t i;
  bool erase_bonds;

  log_init();
  
  cli_init();
  
  ble_init();
  
  timers_init();
  buttons_leds_init(&erase_bonds);
  power_management_init();
  saadc_init();
  pf_spim_init();
  
  randomize_session_Id();
  /* Create task for network*/
//  UNUSED_VARIABLE(xTaskCreate(network_task_fn, "network", configMINIMAL_STACK_SIZE + 512, NULL, 2, &network_task_handle));
//  UNUSED_VARIABLE(xTaskCreate(sensors_task_fn, "sensors", configMINIMAL_STACK_SIZE + 120, NULL, 2, &sensors_task_handle)); 
//  UNUSED_VARIABLE(xTaskCreate(cli_task_fn, "cli", configMINIMAL_STACK_SIZE + 120, NULL, 2, &cli_task_handle)); 
  /* Start FreeRTOS scheduler. */

  NRF_LOG_RAW_INFO("system started. \n\r");
  vTaskStartScheduler();  
  // Enter main loop.
  for (;;) {
    ASSERT(false);
    /* FreeRTOS should not be here... FreeRTOS goes back to the start of stack
         * in vTaskStartScheduler function. */
  }
}

