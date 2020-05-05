#ifndef CMD_H
#define CMD_H

#include "stdint.h"
#include "nrf_fstorage_sd.h"

extern uint32_t number_circuit;
extern char config_string[];
extern char config_string1[];
extern char reset_pulse[]; 
extern char API_token[];
extern char API_token_first[];  
extern char API_token1[];
extern char API_link[];
extern char API_link_first[];   
extern char API_link1[240];
extern char is_internet[];
extern char is_internet_first[];   
extern char is_internet1[];
extern char API_link_inital[];
extern char pulseID[];
extern char pulseVersion[];
extern char isInternet[];
extern char check_Internet[];
extern char check_Internet_true[];
extern char ipaddress_changed[50];
extern char dhcp_check_wifi[50];
extern char dhcp_check_eth[50];
extern char dhcp_check_changed[50];
extern char iswifi[];
extern char iswifi_changed[];     
extern uint32_t ipaddr_value[40];
extern uint32_t gw_value[40]; 
extern uint32_t mask_value[40];    
extern int flagg;
extern uint32_t ipaddr, gwaddr, netmask;
extern uint8_t dhcp;
extern uint32_t ipaddr_test;
extern uint8_t check_internet;
extern uint8_t keyflag, keyconnect;
extern uint8_t link_recvd;
extern uint8_t test_internet;
extern uint8_t restart_system;
extern uint8_t is_ble_on, try_connect;
extern uint8_t ssid_changed, key_changed, ssid_len;
extern uint8_t ssid_saved_copy[80],key_copy[80];
extern char ssid_new[80], key_new[80];
//extern bool bleON;
int8_t cmd_parse (uint8_t * cmd, uint16_t len);
int8_t cmd_feed(uint8_t * buff, uint16_t len);
void cmd_sensors_timer (void);

 //void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage);
 //static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);
// static uint32_t nrf5_flash_end_addr_get();
// static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);
// static void print_flash_info(nrf_fstorage_t * p_fstorage)

#endif //CMD_H