#include "ble.h"
#include "app_error.h"
#include "app_timer.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_hci.h"
#include "ble_nus.h"
#include "ble_radio_notification.h"
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
#include "task.h"
#include "semphr.h"
#include "cmd.h"
#include "ble_sensors.h"

#include "nrf_sdm.h"

SemaphoreHandle_t ble_serial_semp;
SemaphoreHandle_t ble_serial_buff_semp;
SemaphoreHandle_t ble_radio_semp;
static TaskHandle_t ble_serial_task_handle;


static void nus_data_handler(ble_nus_evt_t *p_evt);

#define APP_BLE_CONN_CFG_TAG 1 /**< A tag identifying the SoftDevice BLE configuration. */

#define DEVICE_NAME "iM-Pulse : P66630"                        /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE BLE_UUID_TYPE_VENDOR_BEGIN /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_BLE_OBSERVER_PRIO 3 /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define APP_ADV_INTERVAL 64 /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */

#define APP_ADV_DURATION 18000 /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL MSEC_TO_UNITS(20, UNIT_1_25_MS)    /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(75, UNIT_1_25_MS)    /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY 0                                      /**< Slave latency. */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS)     /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define UART_TX_BUF_SIZE 256 /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256 /**< UART RX buffer size. */
#define SAADC_SAMPLES_IN_BUFFER 8
static int m_buffer_pool1[SAADC_SAMPLES_IN_BUFFER];
static uint8_t value[8];
char adv_name[12];
uint8_t adv_name_length = 6;
uint32_t adv_name_address = 0x10001080;

static struct
{
  uint16_t len;
  uint8_t buff[2048];
} ble_serial_buff;

BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT); /**< BLE NUS service instance. */
NRF_BLE_GATT_DEF(m_gatt);                         /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                           /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);               /**< Advertising module instance. */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;               /**< Handle of the current connection. */
static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3; /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
static ble_uuid_t m_adv_uuids[] =                                      /**< Universally unique service identifier. */
    {
        {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};


/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {
  app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void) {
  uint32_t err_code;
  ble_gap_conn_params_t gap_conn_params;
  ble_gap_conn_sec_mode_t sec_mode;
  static char ble_adv_string[] = "iM-Pulse : ";

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    memcpy(adv_name, (uint8_t *)adv_name_address, adv_name_length);
    strcat(ble_adv_string,adv_name);

 // err_code = sd_ble_gap_device_name_set(&sec_mode,
    //  (const uint8_t *)DEVICE_NAME,
    //  strlen(DEVICE_NAME));

            err_code = sd_ble_gap_device_name_set(&sec_mode,
                                      ble_adv_string,
                                     17);
  APP_ERROR_CHECK(err_code);

  memset(&gap_conn_params, 0, sizeof(gap_conn_params));

  gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
  gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
  gap_conn_params.slave_latency = SLAVE_LATENCY;
  gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

  err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
  APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error) {
  APP_ERROR_HANDLER(nrf_error);
}


/**@snippet [Handling the data received over BLE] */

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void) {
  uint32_t err_code;
  ble_nus_init_t nus_init;
  nrf_ble_qwr_init_t qwr_init = {0};

  // Initialize Queued Write Module.
  qwr_init.error_handler = nrf_qwr_error_handler;

  err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
  APP_ERROR_CHECK(err_code);

  // Initialize NUS.
  memset(&nus_init, 0, sizeof(nus_init));

  nus_init.data_handler = nus_data_handler;

  err_code = ble_nus_init(&m_nus, &nus_init);
  APP_ERROR_CHECK(err_code);

  err_code = ble_sensors_init();
  APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt) {
  uint32_t err_code;

  if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
    err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
    APP_ERROR_CHECK(err_code);
  }
}

/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error) {
  APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void) {
  uint32_t err_code;
  ble_conn_params_init_t cp_init;

  memset(&cp_init, 0, sizeof(cp_init));

  cp_init.p_conn_params = NULL;
  cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
  cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail = false;
  cp_init.evt_handler = on_conn_params_evt;
  cp_init.error_handler = conn_params_error_handler;

  err_code = ble_conn_params_init(&cp_init);
  APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt) {
  uint32_t err_code;

  switch (ble_adv_evt) {
  case BLE_ADV_EVT_FAST:
    //err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
    //APP_ERROR_CHECK(err_code);
    break;
  case BLE_ADV_EVT_IDLE:
    //            sleep_mode_enter();
    break;
  default:
    break;
  }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
  uint32_t err_code;

  switch (p_ble_evt->header.evt_id) {
  case BLE_GAP_EVT_CONNECTED:
    //NRF_LOG_INFO("Connected");
    is_ble_on =1;
    //err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
    //APP_ERROR_CHECK(err_code);
    m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
    APP_ERROR_CHECK(err_code);
    break;

  case BLE_GAP_EVT_DISCONNECTED:
    //NRF_LOG_INFO("Disconnected");
    is_ble_on =0;
    // LED indication will be changed when advertising starts.    
    m_conn_handle = BLE_CONN_HANDLE_INVALID;
    break;

  case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
    NRF_LOG_DEBUG("PHY update request.");
    ble_gap_phys_t const phys =
        {
            .rx_phys = BLE_GAP_PHY_AUTO,
            .tx_phys = BLE_GAP_PHY_AUTO,
        };
    err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
    APP_ERROR_CHECK(err_code);
  } break;

  case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
    // Pairing not supported
    err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    break;

  case BLE_GATTS_EVT_SYS_ATTR_MISSING:
    // No system attributes have been stored.
    err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
    APP_ERROR_CHECK(err_code);
    break;

  case BLE_GATTC_EVT_TIMEOUT:
    // Disconnect on GATT Client timeout event.
    err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("BLE_GATTC_EVT_TIMEOUT");
    break;

  case BLE_GATTS_EVT_TIMEOUT:
    // Disconnect on GATT Server timeout event.
    err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("BLE_GATTC_EVT_TIMEOUT");
    break;

  default:
    // No implementation needed.
    break;
  }
  //ble_nus_on_ble_evt(p_ble_evt, &m_nus);
}

/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void) {
  ret_code_t err_code;

  err_code = nrf_sdh_enable_request();
  APP_ERROR_CHECK(err_code);

  // Configure the BLE stack using the default settings.
  // Fetch the start address of the application RAM.
  uint32_t ram_start = 0;// 0;
  err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
  APP_ERROR_CHECK(err_code);

  // Enable BLE stack.
  err_code = nrf_sdh_ble_enable(&ram_start);
  APP_ERROR_CHECK(err_code);

  // Register a handler for BLE events.
  NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

static void ble_stack_deinit(void) {
  ret_code_t err_code;

  err_code = nrf_sdh_disable_request();
  APP_ERROR_CHECK(err_code);
  sd_softdevice_disable();
}

/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t *p_gatt, nrf_ble_gatt_evt_t const *p_evt) {
  if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)) {
    m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
    NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
  }
  NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
      p_gatt->att_mtu_desired_central,
      p_gatt->att_mtu_desired_periph);
}

/**@brief Function for initializing the GATT library. */
void gatt_init(void) {
  ret_code_t err_code;

  err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
  APP_ERROR_CHECK(err_code);
}
/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void) {
  uint32_t err_code;
  ble_advertising_init_t init;

  memset(&init, 0, sizeof(init));

  init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
  init.advdata.include_appearance = false;
  init.advdata.flags =0;// BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

  init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  init.srdata.uuids_complete.p_uuids = m_adv_uuids;

  init.config.ble_adv_fast_enabled = true;
  init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
  init.config.ble_adv_fast_timeout = 0;//APP_ADV_DURATION;
  init.evt_handler = on_adv_evt;

  err_code = ble_advertising_init(&m_advertising, &init);
  APP_ERROR_CHECK(err_code);

  ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void) {
  uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
  APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event) {
  uint32_t err_code;
  switch (event) {
  case BSP_EVENT_SLEEP:
    //            sleep_mode_enter();
    break;

  case BSP_EVENT_DISCONNECT:
    err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_ERROR_INVALID_STATE) {
      APP_ERROR_CHECK(err_code);
    }
    break;

  case BSP_EVENT_WHITELIST_OFF:
    if (m_conn_handle == BLE_CONN_HANDLE_INVALID) {
      err_code = ble_advertising_restart_without_whitelist(&m_advertising);
      if (err_code != NRF_ERROR_INVALID_STATE) {
        APP_ERROR_CHECK(err_code);
      }
    }
    break;

  default:
    break;
  }
}


void ble_task_fn(void *arg) {
  while (1) {
    //ble_nus_data_send(&m_nus, value, &bytes_to_send, m_conn_handle);
    vTaskDelay(10000);
  }
}

/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_evt       Nordic UART Service event.
 */
/**@snippet [Handling the data received over BLE] */
static void nus_data_handler(ble_nus_evt_t *p_evt) {

  if (p_evt->type == BLE_NUS_EVT_RX_DATA) {
    uint32_t err_code;
    cmd_feed(p_evt->params.rx_data.p_data,  p_evt->params.rx_data.length);
  } else if (p_evt->type == BLE_NUS_EVT_TX_RDY){  /**< Service is ready to accept new data to be transmitted. */
    xSemaphoreGiveFromISR(ble_serial_semp, pdFALSE);
  } else if(p_evt->type == BLE_NUS_EVT_COMM_STARTED){ /**< Notification has been enabled. */
    ble_serial_buff.len = 0;
    xSemaphoreGiveFromISR(ble_serial_semp, pdFALSE);
  }
  
}

uint32_t ble_serial_send(uint8_t * data, uint16_t len)
{
  volatile uint32_t ret;
  uint16_t l;
  uint16_t space;
  uint32_t ipsr;

  if (len>BLE_NUS_MAX_DATA_LEN)
  {    
    l = BLE_NUS_MAX_DATA_LEN;
  }else{
    l = len;
  }

  __ASM volatile("MRS %0, ipsr"
                 : "=r"(ipsr));
  if (ipsr) {
    ret = xSemaphoreTakeFromISR(ble_serial_semp, 0);
  } else {
    ret = xSemaphoreTake(ble_serial_semp, 0);
  }

  if (pdTRUE == ret)
  {
      ret = ble_nus_data_send(&m_nus, data, &l, m_conn_handle);
      if (NRF_SUCCESS == ret)
      {
        len-= l;
        data = data+l;
      }else{
        if (ipsr){
          xSemaphoreGiveFromISR(ble_serial_semp, pdFALSE);
        }else{
          xSemaphoreGive(ble_serial_semp);
        }        
        return -1;
      }
  }
  if  (len>0 )
  {
    space = sizeof(ble_serial_buff.buff) - ble_serial_buff.len;
    if (space > len)
    {
      memcpy(ble_serial_buff.buff + ble_serial_buff.len, data, len);
      ble_serial_buff.len += len;
    }
    else
    {
      return -1;
    }
  }
  return ret;

}
void ble_serial_task_fn(void * arg){
  uint16_t len;
  int ret;
  memset (&ble_serial_buff, 0, sizeof(ble_serial_buff));  
  ret = xSemaphoreGive(ble_serial_semp);
  while (1)
  { 
    vTaskDelay(25);
    if (pdTRUE == xSemaphoreTake(ble_serial_semp,portMAX_DELAY))
    {
      if (ble_serial_buff.len)
      {
        if (ble_serial_buff.len < BLE_NUS_MAX_DATA_LEN)
        {  
        NRF_LOG_INFO("SENDING DATA INSIDE BLE SERIAL TASK");
          ret = ble_nus_data_send(&m_nus, ble_serial_buff.buff, &ble_serial_buff.len, m_conn_handle);
          ble_serial_buff.len = 0;
        }else{
          len = BLE_NUS_MAX_DATA_LEN;
          ret = ble_nus_data_send(&m_nus, ble_serial_buff.buff, &len, m_conn_handle);
          memcpy(ble_serial_buff.buff, ble_serial_buff.buff+len, ble_serial_buff.len-len);
          ble_serial_buff.len -= len;
        }
        if (ret != NRF_SUCCESS)
        {
            xSemaphoreGive(ble_serial_semp);
        }
      }else{
        xSemaphoreGive(ble_serial_semp);
      }
    }
  }
} 

static void radio_notification_handler (bool radio_active)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(radio_active)
  {
    xSemaphoreTakeFromISR(ble_radio_semp, &xHigherPriorityTaskWoken);
   // bsp_board_led_on(BSP_BOARD_LED_2);
  }
  else
  {
    xSemaphoreGiveFromISR(ble_radio_semp, &xHigherPriorityTaskWoken);
   // bsp_board_led_off(BSP_BOARD_LED_2);
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void ble_wait_radio_free (void)
{  
    if (xSemaphoreTake(ble_radio_semp, (TickType_t)10) == pdTRUE)
    {
      xSemaphoreGive(ble_radio_semp);
    }  
}

void ble_init(void)
{
  ble_stack_init();
  APP_ERROR_CHECK(ble_radio_notification_init(2, NRF_RADIO_NOTIFICATION_DISTANCE_3620US, radio_notification_handler));    
  gap_params_init();
  gatt_init();
  services_init();
  ble_serial_semp = xSemaphoreCreateBinary();  
  ble_serial_buff_semp = xSemaphoreCreateBinary();
  ble_radio_semp = xSemaphoreCreateBinary();
  if (ble_radio_semp == NULL)
  {
    APP_ERROR_CHECK(1);
  }
  UNUSED_VARIABLE(xTaskCreate(ble_serial_task_fn, "ble_serial", configMINIMAL_STACK_SIZE + 128, NULL, 2, &ble_serial_task_handle));

  advertising_init();
  conn_params_init();
  advertising_start();
}

void ble_deinit(void)
{
  ble_stack_deinit();
  vTaskDelete(ble_serial_task_handle);
  vSemaphoreDelete(ble_serial_semp);
  vSemaphoreDelete(ble_serial_buff_semp);
}