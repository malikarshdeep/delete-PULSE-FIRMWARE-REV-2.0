//#include <lwip/pbuf.h>
//#include <lwip/raw.h>
//#include <lwip/tcp.h>
//#include <lwip/dns.h>
//#include <lwip/timers.h>
//#include <netif/etharp.h>
//
//#include "boards.h"
//#include "math.h"
//#include "nrf_delay.h"
//#include "power_factor.h"
//#include "nrfx_spim.h"
//#include "tcptest.h"
//#include "f_storage.h"
//#include "nrf_fstorage_sd.h"
//#include "cmd.h"
//
//
//
//
//
//void storage_task_fn(void *arg);
//
//static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);
//
//NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
//{
//    /* Set a handler for fstorage events. */
//    .evt_handler = fstorage_evt_handler,
//
//    /* These below are the boundaries of the flash space assigned to this instance of fstorage.
//     * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
//     * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
//     * last page of flash available to write data. */
//    .start_addr = 0x80000,
//    .end_addr   = 0x8ffff,
//};
//
///**@brief   Helper function to obtain the last address on the last page of the on-chip flash that
// *          can be used to write user data.
// */
//static uint32_t nrf5_flash_end_addr_get()
//{
//    uint32_t const bootloader_addr = NRF_UICR->NRFFW[0];
//    uint32_t const page_sz         = NRF_FICR->CODEPAGESIZE;
//    uint32_t const code_sz         = NRF_FICR->CODESIZE;
//
//    return (bootloader_addr != 0xFFFFFFFF ?
//            bootloader_addr : (code_sz * page_sz));
//}
//
//
//
//static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
//{
//    if (p_evt->result != NRF_SUCCESS)
//    {
//        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
//        return;
//    }
//
//    switch (p_evt->id)
//    {
//        case NRF_FSTORAGE_EVT_WRITE_RESULT:
//        {
//            NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.",
//                         p_evt->len, p_evt->addr);
//        } break;
//
//        case NRF_FSTORAGE_EVT_ERASE_RESULT:
//        {
//            NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.",
//                         p_evt->len, p_evt->addr);
//        } break;
//
//        default:
//            break;
//    }
//}
//
//
//static void print_flash_info(nrf_fstorage_t * p_fstorage)
//{
//    NRF_LOG_INFO("========| flash info |========");
//    NRF_LOG_INFO("erase unit: \t%d bytes",      p_fstorage->p_flash_info->erase_unit);
//    NRF_LOG_INFO("program unit: \t%d bytes",    p_fstorage->p_flash_info->program_unit);
//    NRF_LOG_INFO("==============================");
//}
//
//
//void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
//{
//    /* While fstorage is busy, sleep and wait for an event. */
//    while (nrf_fstorage_is_busy(p_fstorage))
//    {
//          vTaskDelay(1);
//    }
//}
//
//
//uint32_t iM_pulse_ID_addr = 0x10001080;          //PULSE ID stored at this location
//uint8_t iM_pulse_ID_length = 6;
//
//
//void storage_task_fn(void *arg) {
//
//memcpy(iM_pulse_ID, (uint8_t *)iM_pulse_ID_addr, iM_pulse_ID_length);
//
//
//  //config_string[0] = 'z';
//volatile ret_code_t rc;
//nrf_fstorage_api_t * p_fs_api;
//p_fs_api = &nrf_fstorage_sd;
//
//rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);  //Flash storage initialziation
//APP_ERROR_CHECK(rc);
//
////if (rc == NRF_SUCCESS){
////  // NRF_LOG_INFO("fstorage init successful");  
////    }
////
////  else{
//// //  NRF_LOG_INFO("fstorage init failed");
////   }
//
//
//    
//#define CONFIG_ADDR  0x81000  //Config quipment stored at 81000h (config[voltage......])
//
//rc = nrf_fstorage_read(
//    &fstorage,               /* The instance to use. */
//    CONFIG_ADDR,            /* The address in flash where to read data from. */
//    config_string1,        /* A buffer to copy the data into. */
//    48                    /* Lenght of the data, in bytes. */
// );
//
//
// 
//#define FLASH_ADDR_TOKEN  0x82000   //API token stored at 82000h
//
//rc = nrf_fstorage_read(
//    &fstorage,              
//    FLASH_ADDR_TOKEN,     
//    API_token1,        
//    240  
// );
//
//
//  
//#define FLASH_ADDR_LINK  0x83000    //Read API link stored at 83000h
//
//rc = nrf_fstorage_read(
//    &fstorage,   
//    FLASH_ADDR_LINK,     
//    API_link1,        
//    240 
// );
//
//
//while(1){
//
//
//       if (strncmp(API_token_first,"token",5) == 0)  // if we received a new token
//       {
//      
//            static uint32_t pages_to_erase = 1;
//       
//            rc = nrf_fstorage_erase(  // erase the old memory
//                &fstorage,   
//                0x82000,     
//                pages_to_erase, 
//                NULL            
//            );  
//            APP_ERROR_CHECK(rc);
//
//            wait_for_flash_ready(&fstorage);
//
//       
//            if(rc == NRF_SUCCESS){     //if successuffully erased 
//       
//                 vTaskDelay(100);
//
//                 rc = nrf_fstorage_write(  //write the token 
//                    &fstorage,   
//                    0x82000,     
//                    API_token,       
//                    240, 
//                    NULL           
//                  );
//   
//                APP_ERROR_CHECK(rc);
//                wait_for_flash_ready(&fstorage);
//
//
//                strcpy(API_token1,API_token);  //API_token1 has the new value  
//                API_token[0] = 'z';            //API_token is empty again to detect if token changes again
//   
//                int n;
//                n = sprintf(str, "success token\n");
//                ble_serial_send(str, n);
//
//                API_token_first[0] = 'z';     //API_token_first is empty again to detect a new command to change token
//
//            }  
//   
//       }
//
//   
//       if (strncmp(API_link_first,"link",4) ==0)
//       {
//      
//            static uint32_t pages_to_erase = 1;
//      
//            rc = nrf_fstorage_erase(
//                &fstorage,   
//                0x83000,     
//                pages_to_erase, 
//                NULL           
//                );  
//     
//           APP_ERROR_CHECK(rc);
//           wait_for_flash_ready(&fstorage);
//  
//       
//             if(rc == NRF_SUCCESS){
//       
//                 vTaskDelay(100);
//                 rc = nrf_fstorage_write(
//                      &fstorage,  
//                      0x83000,     
//                      API_link,       
//                      240,
//                      NULL           
//                    );
//   
//                APP_ERROR_CHECK(rc);
//                wait_for_flash_ready(&fstorage);
//        
//
//                strcpy(API_link1,API_link); 
//                API_link[0] = 'z';
//    
//    
//                int n;
//                n = sprintf(str, "success link\n");
//                ble_serial_send(str, n);
//                API_link_first[0] = 'z';
//
//
//             }  
//   
//       }
//
//     
//
////      if((strcmp(config_string,"z"))==0){     //config string did not change
////
////            vTaskDelay(100);  //  NRF_LOG_INFO("do nothing");
////          }
////
////      else{ 
//           
//        if((strcmp(config_string,"z")) != 0){   //config string changed
//
//            static uint32_t pages_to_erase = 1;
//
//            rc = nrf_fstorage_erase(
//                &fstorage,   
//                0x81000,     
//                pages_to_erase, 
//                NULL            
//                );  
//          
//           APP_ERROR_CHECK(rc);
//           wait_for_flash_ready(&fstorage);
//
//
//           if(rc == NRF_SUCCESS){
//     
//               vTaskDelay(1);
//               rc = nrf_fstorage_write(
//               &fstorage,             
//               0x81000,              
//               config_string,      
//               240,             
//               NULL            
//              );
//   
//              APP_ERROR_CHECK(rc);
//
//              if(rc == NRF_SUCCESS){
//
//                  wait_for_flash_ready(&fstorage);
//                  memcpy(config_string1,config_string,48);
//                  strcpy(config_string,"z");
//
//                  int n;
//                  n = sprintf(str, "success equipment\n");
//                  ble_serial_send(str, n);
//  
//             }
//          }   
//       }
//      
//
//    
//        if(strcmp(reset_pulse,"reset_pulse!") == 0)   //if received a command to reset pulse
//        {
//  
//            static uint32_t pages_to_erase = 5;       //erase 5 pages conatining link, token and config equipment info
//
//            rc = nrf_fstorage_erase(
//                  &fstorage,   
//                  0x80000,     
//                  pages_to_erase, 
//                  NULL            
//                );
//              
//            APP_ERROR_CHECK(rc);
//            wait_for_flash_ready(&fstorage);
//
//            strcpy(reset_pulse,"x");    //reset pulse command is empty now to detect for the command again 
//            vTaskDelay(2000);
//  
//       }
//
//  
//
//
//        if(strcmp(update,"true")== 0){    // if received a command to update firmware
//
//              sd_power_gpregret_set(0,BOOTLOADER_DFU_START);
//              sd_nvic_SystemReset();
//            }
//    
//  }
//}  