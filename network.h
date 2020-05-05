#ifndef NETWORK_H
#define NETWORK_H
#include "stdint.h"
#include "driver/include/m2m_wifi.h"


void network_task_fn(void *arg);
int wifi_msg_connect(char *pcSsid, uint8 u8SsidLen, uint8 u8SecType, void *pvAuthInfo, uint16 u16Ch);
int wifi_msg_connect_default(void);
int network_msg_check(void);
int network_msg_show_settings(void);
int network_show_settings_wifi(void);

#define REQ_CONNECT 1
#define REQ_SCAN    2
#define REQ_DISCONNECT    3
#define REQ_CONNECT_DEFAULT    4
#define REQ_NETWORK_CHECK    5
#define REQ_SETTINGS_SHOW    6
#define REQ_SETTINGS_SHOW_WIFI    7
#define REQ_IPSETTINGS       8


extern char API[240];
extern uint8_t count_led_on, led_on;
extern uint8_t trying_to_connect, wifi_disconnected;
typedef struct network_msg_t
{
  uint8_t req;
  uint8_t buff[160];
}network_msg_t;

typedef struct wifi_conn_param_t
{    
    uint8_t u8SsidLen; 
    uint8_t u8SecType;
    uint16_t u16Ch;   
    uint8_t Ssid[M2M_MAX_PSK_LEN];
    uint8_t AuthInfo[M2M_MAX_PSK_LEN];     
} wifi_conn_param_t;

typedef enum {
  IFACE_ETH = 1,
  IFACE_WLAN
} iface_type_t;
typedef struct network_ipconfig_t
{
  uint8_t dhcp;
  iface_type_t iface;
  uint32_t ipaddr;
  uint32_t gwaddr;
  uint32_t netmask;
} network_ipconfig_t;

#endif //NETWORK_H