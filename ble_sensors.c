#include "sdk_common.h"
#include "ble_sensors.h"
#include "nrf_ble_gatt.h"
#include "ble_link_ctx_manager.h"
#include "ble_srv_common.h"
#include "nrf_fstorage_sd.h"
#include "tcptest.h"
#include "cmd.h"
#include "nrf_log.h"

//0407b7e3-25f1-407b-8721-db9d0568cb31
#define BLE_SENSORS_BASE_UUID {{0x31, 0xcb, 0x68, 0x05, 0x9d, 0xdb, 0x21, 0x87, 0x7b, 0x40, 0xf1, 0x25, 0xe3, 0xb7, 0x07, 0x04}}
#define BLE_UUID_SENSORS_SERVICE 0x0011 /**< The UUID of the Sensors Service. */
#define BLE_UUID_SENSORS_CHARACTERISTIC 0x0012 /**< The UUID of the Sensors Service. */

#define BLE_SENSORS_CHAR_LEN  32



static uint32_t sensors_char_add (void);

struct ble_sensors_s
{
  uint8_t                  uuid_type;
  uint16_t                 service_handle;
  ble_gatts_char_handles_t sensors_handles;
  blcm_link_ctx_storage_t * const p_link_ctx_storage; /**< Pointer to link context storage with handles of all current connections and its context. */
} ble_sensors;


uint32_t ble_sensors_init(void)
{
    ret_code_t    err_code;
    ble_uuid_t    ble_uuid;
    ble_uuid128_t sensors_base_uuid = BLE_SENSORS_BASE_UUID;

    err_code = sd_ble_uuid_vs_add(&sensors_base_uuid, &ble_sensors.uuid_type);

    ble_uuid.type = ble_sensors.uuid_type;
    ble_uuid.uuid = BLE_UUID_SENSORS_SERVICE;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &ble_sensors.service_handle);
    VERIFY_SUCCESS(err_code);

    err_code = sensors_char_add();
    VERIFY_SUCCESS(err_code);

  
    return err_code;
}

static uint32_t sensors_char_add (void)
{
    volatile ret_code_t    err_code;
    /**@snippet [Adding proprietary characteristic to the SoftDevice] */
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    
    uint8_t str[] = BLE_SENSORS_DESCRIPTOR_STR;
    ble_add_descr_params_t descr_params;
    uint16_t descr_handle;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = ble_sensors.uuid_type;
    ble_uuid.uuid = BLE_UUID_SENSORS_CHARACTERISTIC;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

  

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(sensors_state4);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(sensors_state4);
    





    err_code = sd_ble_gatts_characteristic_add(ble_sensors.service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &ble_sensors.sensors_handles);
    VERIFY_SUCCESS(err_code);
    /**@snippet [Adding proprietary characteristic to the SoftDevice] */

    memset(&descr_params,0,sizeof(descr_params));

    descr_params.uuid = BLE_UUID_DESCRIPTOR_CHAR_USER_DESC;
    descr_params.uuid_type = ble_sensors.uuid_type;
    
    descr_params.read_access = SEC_OPEN;

    descr_params.init_len  = sizeof(str);
    descr_params.init_offs = 0;
    descr_params.max_len   = sizeof(str);
    descr_params.p_value =  str;

    err_code = descriptor_add(ble_sensors.sensors_handles.value_handle, &descr_params,&descr_handle);
    return err_code;
}

uint32_t ble_sensors_update(void)
{
  ble_gatts_hvx_params_t hvx_params;
  uint16_t len;
  memset(&hvx_params,0 , sizeof(hvx_params));



  if(strcmp(reset_pulse,"reset_pulse!")!=0){

 
        if( strcmp(config_string1,"config[circuit:1,phase:1,voltage:120,frequen:60]") == 0) {
            sensors_state1.uUei2 = -32.125;  //phase 2 set to NULL
            sensors_state1.uUei3 = -32.125;  //phase 2 set to NULL
            sensors_state1.iUei2 = -32.125;  //phase 3 set to NULL
            sensors_state1.iUei3 = -32.125;  //phase 3 set to NULL

            len = sizeof(sensors_state1);
            hvx_params.handle = ble_sensors.sensors_handles.value_handle;
            hvx_params.p_data = &sensors_state1;
            hvx_params.p_len = &len;
            hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }

        else if( strcmp(config_string1,"config[circuit:1,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:1,voltage:240,frequen:50]") == 0 ){

            sensors_state1.uUei3 = -32.125;  //phase 3 set to NULL
            sensors_state1.iUei3 = -32.125;  //phase 3 set to NULL

            len = sizeof(sensors_state1);
            hvx_params.handle = ble_sensors.sensors_handles.value_handle;
            hvx_params.p_data = &sensors_state1;
            hvx_params.p_len = &len;
            hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }

        else if( strcmp(config_string1,"config[circuit:1,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:3,voltage:460,frequen:50]") == 0 ){

            len = sizeof(sensors_state1);
            hvx_params.handle = ble_sensors.sensors_handles.value_handle;
            hvx_params.p_data = &sensors_state1;
            hvx_params.p_len = &len;
            hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }

        else if( strcmp(config_string1,"config[circuit:2,phase:1,voltage:120,frequen:60]") == 0){
            sensors_state2.uUei2 = -32.125;
            sensors_state2.uUei3 = -32.125;
            sensors_state2.iUei2 = -32.125;
            sensors_state2.iUei3 = -32.125;

            len = sizeof(sensors_state2);
            hvx_params.handle = ble_sensors.sensors_handles.value_handle;
            hvx_params.p_data = &sensors_state2;
            hvx_params.p_len = &len;
            hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }

        else if( strcmp(config_string1,"config[circuit:2,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:1,voltage:240,frequen:50]") == 0 ){

            sensors_state2.uUei3 = -32.125;
            sensors_state2.iUei3 = -32.125;

            len = sizeof(sensors_state2);
            hvx_params.handle = ble_sensors.sensors_handles.value_handle;
            hvx_params.p_data = &sensors_state2;
            hvx_params.p_len = &len;
            hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
 
       }

        else if( strcmp(config_string1,"config[circuit:2,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:3,voltage:460,frequen:60]") == 0|| strcmp(config_string1,"config[circuit:2,phase:3,voltage:460,frequen:50]") == 0){

            len = sizeof(sensors_state2);
            hvx_params.handle = ble_sensors.sensors_handles.value_handle;
            hvx_params.p_data = &sensors_state2;
            hvx_params.p_len = &len;
            hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }

        else if( strcmp(config_string1,"config[circuit:3,phase:1,voltage:120,frequen:60]") == 0){
         
           sensors_state3.uUei2 = -32.125;
           sensors_state3.uUei3 = -32.125;
           sensors_state3.iUei2 = -32.125;
           sensors_state3.iUei3 = -32.125;

           len = sizeof(sensors_state3);
           hvx_params.handle = ble_sensors.sensors_handles.value_handle;
           hvx_params.p_data = &sensors_state3;
           hvx_params.p_len = &len;
           hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }

        else if( strcmp(config_string1,"config[circuit:3,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:1,voltage:240,frequen:50]") == 0 ){

           sensors_state3.uUei3 = -32.125;
           sensors_state3.iUei3 = -32.125;

           len = sizeof(sensors_state3);
           hvx_params.handle = ble_sensors.sensors_handles.value_handle;
           hvx_params.p_data = &sensors_state3;
           hvx_params.p_len = &len;
           hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }

        else if( strcmp(config_string1,"config[circuit:3,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:3,voltage:460,frequen:50]") == 0 ){


            len = sizeof(sensors_state3);
            hvx_params.handle = ble_sensors.sensors_handles.value_handle;
            hvx_params.p_data = &sensors_state3;
            hvx_params.p_len = &len;
            hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }

        else if( strcmp(config_string1,"config[circuit:4,phase:1,voltage:120,frequen:60]") == 0){

            sensors_state4.uUei2 = -32.125;
            sensors_state4.uUei3 = -32.125;
            sensors_state4.iUei2 = -32.125;
            sensors_state4.iUei3 = -32.125;

            len = sizeof(sensors_state4);
            hvx_params.handle = ble_sensors.sensors_handles.value_handle;
            hvx_params.p_data = &sensors_state4;
            hvx_params.p_len = &len;
            hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }

        else if( strcmp(config_string1,"config[circuit:4,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:1,voltage:240,frequen:50]") == 0 ){

           sensors_state4.uUei3 = -32.125;
           sensors_state4.iUei3 = -32.125;

           len = sizeof(sensors_state4);
           hvx_params.handle = ble_sensors.sensors_handles.value_handle;
           hvx_params.p_data = &sensors_state4;
           hvx_params.p_len = &len;
           hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }

        else if( strcmp(config_string1,"config[circuit:4,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:3,voltage:460,frequen:50]") == 0 ){

            len = sizeof(sensors_state4);
            hvx_params.handle = ble_sensors.sensors_handles.value_handle;
            hvx_params.p_data = &sensors_state4;
            hvx_params.p_len = &len;
            hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
       }
  }

  // else if(number_circuit == 2){
 // len = sizeof(sensors_state2);
 // hvx_params.handle = ble_sensors.sensors_handles.value_handle;
 // hvx_params.p_data = &sensors_state2;
 // hvx_params.p_len = &len;
 // hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
 // }
 NRF_LOG_INFO("SENDING DATA INSIDE BLE SERAIL UPDATE");
  return sd_ble_gatts_hvx(0, &hvx_params);
  //sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID, ble_sensors.sensors_handles.value_handle, &ble_gatts_value);
  //sd_ble_gatts_service_changed(BLE_CONN_HANDLE_ALL,ble_sensors.sensors_handles.value_handle,ble_sensors.sensors_handles.value_handle);
}