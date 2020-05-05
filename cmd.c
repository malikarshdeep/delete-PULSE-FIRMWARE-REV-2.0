#include "cmd.h"
#include "ble.h"
#include "driver/include/m2m_wifi.h"
#include "FreeRTOS.h"
#include "network.h"
#include "message_buffer.h"
#include "semphr.h"
#include "stdint.h"
#include "string.h"
#include "tcptest.h"
#include "timers.h"
#include "nrf_fstorage.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_fstorage_sd.h"
#include "nrf_delay.h"



/*command line interface
commands ends with '\n' or '\r' symbol
commands:
  wifi ssid <ssid>
    stores name of the access point in buffer
  wifi key <key>
    stores key in buffer
  wifi keyind <number from 0 to 3>
    stores key index for WEP security access points
  wifi login <login>
    stores login for 802.1x security access points
  wifi scan
    starts scanning for access points
  wifi connect default
    connects to default AP stored in winc1500 module
  wifi connect <security type>
    connects to access point with ssid saved in buffer
    avaliable security types are:
    wpa - WPA/PSK
    wep - WEP
    802_1x - 802.1x
    open - open AP with no security

  sensors
    returns sensors state in JSON format
*/
#define CMD_BUFF_LEN 256
#define SSID_MAX_LEN 32
#define KEY_MAX_LEN  64





uint32_t times_count = 2;
char config_string[240] = "z"; 
char config_string1[240];
char reset_pulse[30];
char API_token[240];
char API_token_first[240];
char API_token1[240];
char is_internet[240];
char is_internet_first[240];
char is_internet1[240];
char API_link[240];
char API_link_first[240];
char API_link1[240];
char API_link_inital[240];
char pulseID[240];
char pulseVersion[240];
char isInternet[240] = "false";
char check_Internet[240];
char check_Internet_true[240];
static char pulse_Version[] = "success version_1.0.8";
uint32_t ipaddr, gwaddr, netmask;
uint8_t dhcp = 0;
extern char update[];
extern uint8_t mac_id[6];
extern uint8_t mac_id_wifi[6];
char mac_address[50];
char mac_address_wifi[50];
char ipaddress_changed[50];
char dhcp_check_wifi[] = "true";
char dhcp_check_eth[] = "true";
char iswifi[];
char iswifi_changed[] = "false";
uint32_t ipaddr_value[40];
uint32_t gw_value[40];
uint32_t mask_value[40];
char dhcp_check_changed[] = "false";
uint32_t ipaddr_test = 0xBADC0FFE;
uint8_t check_internet;
uint8_t test_internet;
uint8_t keyflag =0, keyconnect = 0, try_connect = 0;
uint8_t link_recvd = 0;
uint8_t restart_system = 0;
uint8_t is_ble_on;
char ssid_new[80], key_new[80];
//char *ssid_new;
uint8_t ssid_changed =0, key_changed = 0, ssid_len;
uint8_t ssid_saved_copy[80], key_copy[80];
//bool bleON = 1;






static int8_t cmd_ipconfig(uint8_t *str, uint16_t len);
static int8_t cmd_wifi (uint8_t *str, uint16_t len);
static int8_t cmd_wifi_connect (uint8_t *str, uint16_t len);
int8_t cmd_sensors(uint8_t *str, uint16_t len);
static uint8_t str[512*3];


static volatile uint8_t sensors_timer_active = 0;
static volatile int32_t sensors_timer_tick;
#define CMD_SENSORS_TIMEOUT 30000

static struct {
  uint8_t ssid_saved[SSID_MAX_LEN +1];
  uint8_t ssid_saved_len;
  uint8_t key   [KEY_MAX_LEN +1];
  uint8_t key_ind;
  uint8_t login[M2M_1X_USR_NAME_MAX ];
  uint8_t ssid_curr[SSID_MAX_LEN +1];
  uint8_t ssid_curr_len;
}wifi_param;

static struct {
    uint8_t p[CMD_BUFF_LEN];
    uint16_t len;
}cmd_buff;



//***flash storagr functions
void fstorage_evt_handler_network(nrf_fstorage_evt_t * p_evt);


NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage_network) =
{
    /* Set a handler for fstorage events. */
    .evt_handler = fstorage_evt_handler_network,

    /* These below are the boundaries of the flash space assigned to this instance of fstorage.
     * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
     * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
     * last page of flash available to write data. */
    .start_addr = 0x90000,
    .end_addr   = 0x9ffff,
};

void fstorage_evt_handler_network(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        default:
            break;
    }
}

void wait_for_flash_ready_network(nrf_fstorage_t const * p_fstorage)
{
   NRF_LOG_INFO("WAITING");
    /* While fstorage is busy, sleep and wait for an event. */
    while (nrf_fstorage_is_busy(p_fstorage))
    {
        //  vTaskDelay(1);
        nrf_delay_ms(100);
    }
    NRF_LOG_INFO("WAIT DONE");
}
//***end flash storage functions




int8_t cmd_feed(uint8_t * buff, uint16_t len)
{
// NRF_LOG_INFO("2 TIMES IN FEED");
  uint16_t i = 0;
  uint8_t *p;
  while (i < len)
  {
    p = buff + i;
    if ((*p == '\r') || (*p == '\n'))
    {
  //  NRF_LOG_INFO("2 TIMES IN PARSE IF");
      cmd_parse(cmd_buff.p,cmd_buff.len);
      cmd_buff.len = 0;
    }else
    {
      if (cmd_buff.len < CMD_BUFF_LEN)
      {
        *(cmd_buff.p+cmd_buff.len) = *p;
        cmd_buff.len++;
      }else{
        cmd_buff.len = 0;
        return -1;
      }
    }
    i++;
  }
  return 0;
}

//compares input string to command and returns argument and argument length
//input:
//uint8_t * str     - input string
//uint16_t len      - input string length
//uint8_t * cmd     - command to compare
//                    in case if there are any arguments after command in input string, command ends with space
//output:
//uint8_t ** arg    - argument that follows after cmd in str
//uint16_t * arglen - argument length
//retval            - 1 on successfull comparison, 0 otherwise
static int cmd_cmp(uint8_t * str, uint16_t len, uint8_t * cmd, uint8_t ** arg, uint16_t * arglen)
{
    uint16_t l = strlen(cmd);
    if ((l>0) && (0 == strncmp(str,cmd, l)))
    {
      *arg = str+l;
      *arglen = len -l;                  
      if (cmd[l-1] == ' ')
      {
        if (len > l)
        {
          return 1;
        }else{          
          return 0;
        }
      }
      else
      {
        if (len == l)
        {           
          return 1;
        }else{
          return 0;
        }
      }
    }        
    return 0;    
}

int8_t cmd_parse (uint8_t * cmd, uint16_t len)
{ 
  uint8_t * ptr;
  uint16_t arglen;
    int i;

    ++times_count;

  NRF_LOG_INFO("cmd parse %i symbols\n", len);
  if (cmd_cmp(cmd,len,"wifi ",&ptr,&arglen))
  {
    return cmd_wifi (ptr, arglen);
  } else if (cmd_cmp(cmd,len,"sensors ",&ptr,&arglen))
  {    
    i = prepare_data_string_circuit1(str);
    if (i>0)
    {
     ble_serial_send(str, i);
    }
    return cmd_sensors (ptr, arglen);
  }

  else if (cmd_cmp(cmd,6,"config",&ptr,&arglen)) 
  {

   
    memcpy(config_string,cmd,240);

  }

  
//    else if (cmd_cmp(cmd,8,"pulse_id",&ptr,&arglen)) 
//  {
//   
//
//    int n = sizeof(pulseID);
//    ble_serial_send(pulseID, n);
//
//  }

      else if (cmd_cmp(cmd,5,"token",&ptr,&arglen)) 
  {

    memcpy(API_token_first,cmd,5);
    memcpy(API_token,cmd+6,240);
    //NRF_LOG_INFO("COPIED");


  }

        else if (cmd_cmp(cmd,8,"internet",&ptr,&arglen)) 
  {
    memcpy(is_internet_first,cmd,240);
    memcpy(is_internet,cmd+9,240);
    if(strncmp(is_internet,"wifi",4)==0){
    strcpy(dhcp_check_eth,"true");}
    else{strcpy(dhcp_check_wifi,"true");}
    strcpy(dhcp_check_changed,"true");

  }

      else if (cmd_cmp(cmd,12,"reset_pulse!",&ptr,&arglen)) 
  {
    memcpy(reset_pulse,cmd,12);
  }

      else if (cmd_cmp(cmd,4,"link",&ptr,&arglen)) 
  {

    memcpy(API_link_first,cmd,4);
  //  memcpy(API_link_inital,cmd+12,len-8);
    memcpy(API_link,cmd+5,len-5);
    //  memcpy(API_link,cmd+5,240);
  //  link_recvd = 1;
     // NRF_LOG_INFO("COPIED");

  }

  else if (cmd_cmp(cmd, 13, "test internet", &ptr, &arglen)) {
         // NRF_LOG_INFO("test internet");
          test_internet =1;
//network_msg_check();
//if(check_internet == 1){
//            int n;
//            n = sprintf(str, "success internet\n");
//            ble_serial_send(str, n);
//            NRF_LOG_INFO("SUCCESS INTERNET");
//
//}
//
//else{
//            int n;
//            n = sprintf(str, "failed internet\n");
//            ble_serial_send(str, n);
//            NRF_LOG_INFO("FAILED INTERNET");
//
//}

  }

          else if (cmd_cmp(cmd,13,"test ethernet",&ptr,&arglen)) 
  {
if((times_count%2) == 0)
{
//NRF_LOG_INFO("DO NOTHING");
}
 else{ 
    int n;
    n = sprintf(str, "success ethernet\n");
    ble_serial_send(str, n);
  }

  }

          else if (cmd_cmp(cmd,11,"get version",&ptr,&arglen)) 
  {

    int n;
    n = sprintf(str, "success version_1.1.4\n");
    ble_serial_send(str, n);

  }

        else if (cmd_cmp(cmd,16,"activate dfutarg",&ptr,&arglen)) 
  {

  strcpy(update,"true");

  }



    else if (cmd_cmp(cmd, 13, "restart pulse", &ptr, &arglen)) {

   sd_nvic_SystemReset();

  }

          else if (cmd_cmp(cmd,7,"get mac",&ptr,&arglen)) 
  {

    //int n;
    //n = sprintf(mac_address, "success mac_%x:%x:%x:%x:%x:%x\n", mac_id[0],mac_id[1],mac_id[2],mac_id[3],mac_id[4],mac_id[5]);
    //ble_serial_send(mac_address, n);
    network_msg_show_settings();

  }


            else if (cmd_cmp(cmd,11,"get wifimac",&ptr,&arglen)) 
  {

  //  int n;
  //  n = sprintf(mac_address_wifi, "success wifimac_%x:%x:%x:%x:%x:%x\n", mac_id_wifi[0],mac_id_wifi[1],mac_id_wifi[2],mac_id_wifi[3],mac_id_wifi[4],mac_id_wifi[5]);
  //  ble_serial_send(mac_address_wifi, n);
network_msg_show_settings_wifi();
  }

  else if (cmd_cmp(cmd, len, "ipconfig ", &ptr, &arglen)) {
      cmd_ipconfig(ptr, arglen);
  }
  
}

#define INVALID 0

uint32_t ip_to_int(const uint8_t **ip) {
  /* The return value. */
  uint32_t v = 0;
  /* The count of the number of bytes processed. */
  uint16_t i;
  /* A pointer to the next digit to process. */
  const uint8_t *start;

  start = *ip;
  for (i = 0; i < 4; i++) {
    /* The digit being processed. */
    uint8_t c;
    /* The value of this byte. */
    int16_t n = 0;
    uint8_t l = 0;
    while (1) {
      c = *start;
      start++;
      if (c >= '0' && c <= '9') {
        n *= 10;
        n += c - '0';
        l = 1;
      }
      /* We insist on stopping at "." if we are still parsing
               the first, second, or third numbers. If we have reached
               the end of the numbers, we will allow any character. */
      else if (((i < 3 && c == '.') || i == 3) && l) {
        break;
      } else {
        return INVALID;
      }
    }
    if (n >= 256) {
      return INVALID;
    }
    v *= 256;
    v += n;
  }
  *ip = start;
  v = ((v & 0xff) << 24) | ((v & 0xff00) << 8) | ((v & 0xff0000) >> 8) | ((v & 0xff000000) >> 24);
  return v;
}


static int8_t cmd_ipconfig(uint8_t *str, uint16_t len) {

  nrf_fstorage_api_t * p_fs_api;
  p_fs_api = &nrf_fstorage_sd;
  volatile ret_code_t rc; 
  network_ipconfig_t ipconfig;

  uint8_t *ptr;
  uint16_t arglen;
  uint8_t iface = 0;

  memset(&ipconfig, 0, sizeof(ipconfig));
  if (cmd_cmp(str, len, "wlan ", &ptr, &arglen))
    ipconfig.iface = IFACE_WLAN;
  else if (cmd_cmp(str, len, "eth ", &ptr, &arglen))
    ipconfig.iface = IFACE_ETH;
  else
    return -1;
  str = ptr;
  len = arglen;
  if (cmd_cmp(str, len, "dhcp", &ptr, &arglen)) {
    ipconfig.dhcp = 1;
   
    if(ipconfig.iface == IFACE_WLAN){
    strcpy(dhcp_check_wifi,"true");
    strcpy(dhcp_check_eth,"false");
    strcpy(dhcp_check_changed,"true");}

    else if(ipconfig.iface == IFACE_ETH){
        strcpy(dhcp_check_eth,"true");
        strcpy(dhcp_check_wifi,"false");
        strcpy(dhcp_check_changed,"true");
    }

    return network_msg_ipconfig(&ipconfig);
  }

  ipaddr = ip_to_int(&ptr);
  if (ipaddr == INVALID)
    return -1;
  gwaddr = ip_to_int(&ptr);
  if (gwaddr == INVALID)
    return -1;
  netmask = ip_to_int(&ptr);
  if (netmask == INVALID)
    return -1;
  ipconfig.ipaddr = ipaddr;
  ipconfig.gwaddr = gwaddr;
  ipconfig.netmask = netmask;

  if(ipconfig.iface == IFACE_WLAN){
  strcpy(dhcp_check_wifi,"false");
  strcpy(dhcp_check_eth,"true");
  strcpy(dhcp_check_changed,"true");}

  else if(ipconfig.iface == IFACE_ETH){
  strcpy(dhcp_check_eth,"false");
  strcpy(dhcp_check_wifi,"true");
  strcpy(dhcp_check_changed,"true");}

  strcpy(ipaddress_changed,"changed");
  return network_msg_ipconfig(&ipconfig);
}


int8_t cmd_sensors(uint8_t *str, uint16_t len)
{
    uint8_t * ptr;
  uint16_t arglen;    
    if (cmd_cmp(str,len,"cont",&ptr,&arglen))
    {
      sensors_timer_active = 1;
      sensors_timer_tick =xTaskGetTickCount();
    }
    else if (cmd_cmp(str,len,"stop",&ptr,&arglen))
    {
          sensors_timer_active = 0;          
    }
    return 0;
}

void cmd_sensors_timer (void)
{
  int i;
  uint32_t tick;
  if (sensors_timer_active)
  {
    tick = xTaskGetTickCount();
    if ( (tick-sensors_timer_tick) > CMD_SENSORS_TIMEOUT)
    {
      sensors_timer_tick =xTaskGetTickCount();
      i = prepare_data_string_circuit1(str);
      if (i>0)
      {
        ble_serial_send(str, i);
      }
    }
  }
}


int8_t cmd_wifi (uint8_t *str, uint16_t len)
{
  uint16_t arglen;
  uint8_t * ptr;
  if (cmd_cmp(str,len,"ssid ",&ptr,&arglen))
  {
    if (arglen <= SSID_MAX_LEN)
    {      
      memcpy(wifi_param.ssid_saved, ptr, arglen);
      wifi_param.ssid_saved[arglen] = 0;
      wifi_param.ssid_saved_len = arglen;
      strcpy(ssid_saved_copy,wifi_param.ssid_saved );
      ssid_changed = 1;
        int n;
        n = sprintf(str, "success ssid\n");
        ble_serial_send(str, n);
    }


  }
  else if (cmd_cmp(str,len,"key ",&ptr,&arglen))
  {
    if (arglen <= KEY_MAX_LEN)
    {
      memcpy(wifi_param.key, ptr, arglen);
      wifi_param.key[arglen] = 0;
             strcpy(key_copy,wifi_param.key);
      key_changed = 1;
        keyflag = 1;
        int n;
        n = sprintf(str, "success key\n");
        ble_serial_send(str, n);
    }

      
      
    
  }
  else if (cmd_cmp(str,len,"keyind ",&ptr,&arglen))
  {
     uint8_t ind;
     if (arglen == 1)
     {      
      ind = *ptr - '0';
      if (ind < 4)
      {
        wifi_param.key_ind = ind;
      }
     }
  }
  else if (cmd_cmp(str,len,"login ",&ptr,&arglen))
  {      
    if (arglen < (M2M_1X_USR_NAME_MAX))
    {
      memcpy(wifi_param.login, ptr, arglen);
      ptr[arglen] = 0;
    }
  }
  else if (cmd_cmp(str,len,"connect ",&ptr,&arglen))
  {
    keyconnect = 1;
    return cmd_wifi_connect(ptr,arglen);
      //  int n;
     //   n = sprintf(str, "success connect\n");
     //   ble_serial_send(str, n);

  }
  else if (cmd_cmp(str,len,"disconnect",&ptr,&arglen))
  {
    return wifi_msg_disconnect();
  }
  else if (cmd_cmp(str,len,"scan",&ptr,&arglen))
  {
    wifi_msg_scan();
  }
  else
  {
    return -1;
  }
}

void check_internet1(){

//NRF_LOG_INFO("CHECKING INTERNET");
sys_timeout(10000, check_internet1, NULL);

}

int8_t cmd_wifi_connect (uint8_t *str, uint16_t len)
{
    uint8_t * ptr;
    uint16_t arglen;
    uint8_t sectype = 0;
    uint8_t auth_buff[128];
    if (cmd_cmp(str,len,"wpa",&ptr,&arglen))
    {
      sectype = M2M_WIFI_SEC_WPA_PSK; 
         
    }
    else if (cmd_cmp(str,len,"wep",&ptr,&arglen))
    {
      sectype = M2M_WIFI_SEC_WEP;    
    }
    else if (cmd_cmp(str,len,"802_1x",&ptr,&arglen))
    {
      sectype = M2M_WIFI_SEC_802_1X;
    }
    else if (cmd_cmp(str,len,"open",&ptr,&arglen))
    {
      sectype = M2M_WIFI_SEC_OPEN;
    }
    else if (cmd_cmp(str,len,"default",&ptr,&arglen))
    {
      return wifi_msg_connect_default();      
    }
    else
    {
      return -1;
    }
    memset(auth_buff,0,sizeof(auth_buff));
    switch (sectype)
    {     
      case M2M_WIFI_SEC_OPEN:        
        break;
      case M2M_WIFI_SEC_WPA_PSK:
        strncpy (auth_buff, wifi_param.key, sizeof(wifi_param.key));
        break;
      case M2M_WIFI_SEC_WEP:
      {
        tstrM2mWifiWepParams * WepParams = (tstrM2mWifiWepParams*)auth_buff;
        WepParams->u8KeyIndx = wifi_param.key_ind;
        WepParams->u8KeySz = strlen(wifi_param.key);
        strncpy(WepParams->au8WepKey, wifi_param.key, sizeof(wifi_param.key));
      }

        break;
      case M2M_WIFI_SEC_802_1X:
      {
        tstr1xAuthCredentials * AuthCredentials = auth_buff;
        strncpy(AuthCredentials->au8UserName, wifi_param.key, sizeof(AuthCredentials->au8UserName));
      }
        break;
      
    }

        try_connect = 1;
    wifi_msg_connect(wifi_param.ssid_saved,wifi_param.ssid_saved_len, M2M_WIFI_SEC_WPA_PSK, auth_buff, M2M_WIFI_CH_ALL);
}