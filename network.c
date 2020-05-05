#include "network.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "semphr.h"
#include "nrf_log.h"

#include "common/include/nm_common.h"
#include "driver/include/m2m_wifi.h"
#include "driver/source/m2m_hif.h"
#include "driver/source/nmasic.h"
#include <winc1500netif.h>

#include "enc28j60.h"
#include <mchdrv.h>

#include <lwip/dhcp.h>
#include <lwip/err.h>
#include <lwip/inet.h>
#include <lwip/init.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/stats.h>
#include <lwip/tcp.h>
#include <lwip/timers.h>
#include <lwip/tcpip.h>
#include <lwip/netifapi.h>
#include <netif/etharp.h>

#include "ble.h"
#include "power_factor.h"
#include "tcptest.h"
#include "wifi.h"
#include "nrf_fstorage.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_fstorage_sd.h"
#include "cmd.h"
#include "socket_test.h"


MessageBufferHandle_t WifiConMBuff;
QueueHandle_t WifiConQ;

struct netif enc_netif;
static enc_device_t mchdrv_hw;
static struct dhcp enc_dhcp;
uint8_t mac_id[6];
uint8_t mac_id_wifi[6];
uint8_t count_led_on = 0, trying_to_connect = 0, wifi_disconnected = 0, led_on = 0;
char API[] = "192";
network_ipconfig_t T;
  network_msg_t p;
  network_ipconfig_t * ipconfig = (network_ipconfig_t*) p.buff;


void mch_net_poll(void);

//network init function
void net_add_enc_netif(void) {
  struct ip_addr mch_myip_addr, gw_addr, netmask;
  IP4_ADDR(&mch_myip_addr, 0, 0, 0, 0);
  IP4_ADDR(&gw_addr, 0, 0, 0, 0);
  IP4_ADDR(&netmask, 255, 255, 255, 255);

  enc_netif.hwaddr_len = 6;
  /* demo mac address */
  enc_netif.hwaddr[0] = 0x02;
  enc_netif.hwaddr[1] = 0x04;
  enc_netif.hwaddr[2] = 0xa3;
  if (netif_is_up(&winc_netif))
  {  
    enc_netif.hwaddr[3] = winc_netif.hwaddr[3];
    enc_netif.hwaddr[4] = winc_netif.hwaddr[4];
    enc_netif.hwaddr[5] = winc_netif.hwaddr[5];
    memcpy(&mac_id,enc_netif.hwaddr,12);
    memcpy(&mac_id_wifi,winc_netif.hwaddr,12);

  }
  else{
    enc_netif.hwaddr[3] = 0x03;
    enc_netif.hwaddr[4] = 0x03;
    enc_netif.hwaddr[5] = 0x03;
    
  }
  // Add our netif to LWIP (netif_add calls our driver initialization function)
  if (netif_add(&enc_netif, &mch_myip_addr, &netmask, &gw_addr, &mchdrv_hw,
          mchdrv_init, tcpip_input) == NULL) 
  {
    LWIP_ASSERT("mch_net_init: netif_add (mchdrv_init) failed\n", 0);
  } 
  else 
  {    
    enc_netif.flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
    netif_set_up(&enc_netif);
    //if DHCP is read from flash
    if(strncmp(dhcp_check_eth,"false",4)!=0)
    { 
      enc_netif.dhcp = &enc_dhcp;
      dhcp_start(&enc_netif); 
      bsp_board_led_on(BSP_BOARD_LED_0);
      vTaskDelay(100);
      bsp_board_led_off(BSP_BOARD_LED_0); 
    }
    //custom ip
    else
    {
       struct netif * netif_p;
       netif_p = &enc_netif;
       netif_p->ip_addr.addr = ipaddr_value[0];
       netif_p->gw.addr = gw_value[0];
       netif_p->netmask.addr = mask_value[0];
       bsp_board_led_on(BSP_BOARD_LED_0);
       vTaskDelay(100);
       bsp_board_led_off(BSP_BOARD_LED_0); 
    }
 
  }

}

void net_enc_poll(void) {
  mchdrv_poll(&enc_netif);
}

sint8 hif_isr(uint16_t * sz);

/** Wi-Fi connection state */
static uint8_t eth_buffer[2048];

static uint8_t wifi_autoconnect;
static uint8_t wifi_conn_info_resp;


static xSemaphoreHandle net_start_sem;
static int net_started = 0;

/**
 * \brief Callback to get the Data from socket.
 *
 * \param[in] sock socket handler.
 * \param[in] u8Msg socket event type. Possible values are:
 *  - SOCKET_MSG_BIND
 *  - SOCKET_MSG_LISTEN
 *  - SOCKET_MSG_ACCEPT
 *  - SOCKET_MSG_CONNECT
 *  - SOCKET_MSG_RECV
 *  - SOCKET_MSG_SEND
 *  - SOCKET_MSG_SENDTO
 *  - SOCKET_MSG_RECVFROM
 * \param[in] pvMsg is a pointer to message structure. Existing types are:
 *  - tstrSocketBindMsg
 *  - tstrSocketListenMsg
 *  - tstrSocketAcceptMsg
 *  - tstrSocketConnectMsg
 *  - tstrSocketRecvMsg
 */

static void eth_cb(uint8 u8MsgType, void *pvMsg, void *pvCtrlBuf) {
  switch (u8MsgType) {
  case M2M_WIFI_RESP_ETHERNET_RX_PACKET: //.....insert code as per app requirements
    //M2M_INFO("wifi eth packet received");
    winc_netif_rx_callback(u8MsgType, pvMsg, pvCtrlBuf);
    break;
  default:
    M2M_INFO("wifi eth cb");
    break;
  }
}

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_RESP_CURRENT_RSSI](@ref M2M_WIFI_RESP_CURRENT_RSSI)
 *  - [M2M_WIFI_RESP_CON_STATE_CHANGED](@ref M2M_WIFI_RESP_CON_STATE_CHANGED)
 *  - [M2M_WIFI_RESP_CONNTION_STATE](@ref M2M_WIFI_RESP_CONNTION_STATE)
 *  - [M2M_WIFI_RESP_SCAN_DONE](@ref M2M_WIFI_RESP_SCAN_DONE)
 *  - [M2M_WIFI_RESP_SCAN_RESULT](@ref M2M_WIFI_RESP_SCAN_RESULT)
 *  - [M2M_WIFI_REQ_WPS](@ref M2M_WIFI_REQ_WPS)
 *  - [M2M_WIFI_RESP_IP_CONFIGURED](@ref M2M_WIFI_RESP_IP_CONFIGURED)
 *  - [M2M_WIFI_RESP_IP_CONFLICT](@ref M2M_WIFI_RESP_IP_CONFLICT)
 *  - [M2M_WIFI_RESP_P2P](@ref M2M_WIFI_RESP_P2P)
 *  - [M2M_WIFI_RESP_AP](@ref M2M_WIFI_RESP_AP)
 *  - [M2M_WIFI_RESP_CLIENT_INFO](@ref M2M_WIFI_RESP_CLIENT_INFO)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type. Existing types are:
 *  - tstrM2mWifiStateChanged
 *  - tstrM2MWPSInfo
 *  - tstrM2MP2pResp
 *  - tstrM2MAPResp
 *  - tstrM2mScanDone
 *  - tstrM2mWifiscanResult
 */

//WIFI callback event function 
static void wifi_cb(uint8_t u8MsgType, void *pvMsg) {
  static uint8 u8ScanResultIdx = 0;
  int n;
  uint8_t str[256];
  switch (u8MsgType) {
  case M2M_WIFI_RESP_CON_STATE_CHANGED: {
        tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
        //printf("M2M_WIFI_RESP_CON_STATE_CHANGED state: %x\n", pstrWifiState->u8CurrState);
        if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
          M2M_INFO("wifi connected to AP");  //if wifi connected event received  
          trying_to_connect = 1;  //do further events using this variable
          wifi_disconnected = 0;  //disconnected is false
    //      n = sprintf(str, "trying to connect\n");
    //      ble_serial_send(str, n);
          //netif_set_link_up(&winc_netif);
          winc_netif.flags |= NETIF_FLAG_LINK_UP;
          m2m_wifi_get_connection_info();
          wifi_conn_info_resp = 1;
          wifi_autoconnect = 1; 
    //      if(keyflag == 1){   
    //      n = sprintf(str, "success connected\n");
    //      ble_serial_send(str, n);
    //      NRF_LOG_INFO("SUCCESS CONNECTED");
    //      keyflag = 0;
    //      }
    
        } else if ((pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) || (pstrWifiState->u8CurrState == M2M_WIFI_UNDEF)) {
          M2M_INFO("wifi disconnected from AP code: %i", pstrWifiState->u8CurrState);
          if(try_connect == 1){  //disconnect only if it was connected earlier 
          wifi_disconnected = 1;
          try_connect = 0;
          }
          n = sprintf(str, "wifi disconnected from AP\n");
          ble_serial_send(str, n);
          //netif_set_link_down(&winc_netif);
          winc_netif.flags &= ~NETIF_FLAG_LINK_UP;
        }
      } 
      break;
  case M2M_WIFI_REQ_DHCP_CONF: {
        M2M_INFO("wifi cb dhcp");
        volatile tstrM2MIPConfig *ipConfig = (tstrM2MIPConfig *)pvMsg;
        //netif_set_addr(&winc_netif, &ipConfig->u32StaticIP,&ipConfig->u32SubnetMask,&ipConfig->u32Gateway);
      } break;
  case M2M_WIFI_RESP_CONN_INFO: {
        tstrM2MConnInfo *pstrConnInfo = (tstrM2MConnInfo *)pvMsg;
        if (pstrConnInfo->acSSID[0] == '\0') {
          M2M_INFO("wifi not connected to AP");   
          if (wifi_autoconnect)
          {
    //        m2m_wifi_default_connect();
          }
        }
        else
        { 
          if (wifi_conn_info_resp)
          {
    //        n = sprintf(str, "wifi connected to AP: %s\n", pstrConnInfo->acSSID);
    //        ble_serial_send(str, n);
    //        wifi_conn_info_resp = 0;
          }
        }
      } break;

  case M2M_WIFI_RESP_SCAN_DONE: {
        tstrM2mScanDone *pstrInfo = (tstrM2mScanDone *)pvMsg;
        if (pstrInfo->s8ScanState == M2M_SUCCESS) {
          n = sprintf(str, "wifi num of AP found: %d\n", pstrInfo->u8NumofCh);
       //   ble_serial_send(str, n);
          u8ScanResultIdx = 0;
          if (pstrInfo->u8NumofCh >= 1) {
            m2m_wifi_req_scan_result(u8ScanResultIdx);
            u8ScanResultIdx++;
          }
        } else {
          n = sprintf(str, "wifi AP scan error <%d>\n", pstrInfo->s8ScanState);
          ble_serial_send(str, n);
          M2M_INFO("(ERR) Scan fail with error <%d>\n", pstrInfo->s8ScanState);
        }
      } break;

//scan the open wifi network and send the string via ble to app
  case M2M_WIFI_RESP_SCAN_RESULT: {  
        tstrM2mWifiscanResult *pstrScanResult = (tstrM2mWifiscanResult *)pvMsg;
        uint8 u8NumFoundAPs = m2m_wifi_get_num_ap_found();
        uint8_t sec_str[5][8] = {"null", "open", "wpa_psk", "wep", "802_1x"};
      n = snprintf(str, 256, "success wifi scan_%s  Name:%s\n",
            sec_str[pstrScanResult->u8AuthType],
            pstrScanResult->au8SSID);
        if (n > 0) {
          ble_serial_send(str, n);
          vTaskDelay(300);
        }
        M2M_INFO("ap index %d str %d\n", pstrScanResult->u8index, n);

        if (u8ScanResultIdx < u8NumFoundAPs) {
          // Read the next scan result
          m2m_wifi_req_scan_result(u8ScanResultIdx);
          u8ScanResultIdx++;
        }
      } break;
  case M2M_WIFI_RESP_DEFAULT_CONNECT :{
        n = sprintf(str, "wifi connect default failed\n");
       // ble_serial_send(str, n);
      } break;
  default: {
    
        M2M_INFO("wifi cb");
        break;
      }
  }
}

void net_add_winc_netif(void) {
  static struct dhcp winc_dhcp;
  struct ip_addr ip_addr;
  ip_addr.addr = 0;
  tstrWifiInitParam param;
  volatile int8_t ret;
  /* Initialize the BSP. */
  nm_bsp_init();
  /* Initialize Wi-Fi parameters structure. */
  memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));
  /* Initialize Wi-Fi driver with data and status callbacks. */
  param.pfAppWifiCb = wifi_cb;
  param.strEthInitParam.au8ethRcvBuf = eth_buffer;
  param.strEthInitParam.u16ethRcvBufSize = sizeof(eth_buffer);
  param.strEthInitParam.u8EthernetEnable = M2M_WIFI_MODE_ETHERNET;
  param.strEthInitParam.pfAppEthCb = eth_cb;
  ret = m2m_wifi_init(&param);
  if (M2M_SUCCESS != ret) {
    //printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
  } else {
    /* Connect to router. */
    //ret = m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
    //m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
    m2m_wifi_connect((char *)ssid_new, ssid_len, MAIN_WLAN_AUTH, (char *)key_new, M2M_WIFI_CH_ALL);
//    ret = m2m_wifi_default_connect();
    wifi_autoconnect = 1;
    /* Add winc1000 AP/WD interface. */
    struct ip_addr ip_addr_winc, mask_winc, gateway_winc;
    IP4_ADDR(&ip_addr_winc, 0, 0, 0, 0);
    IP4_ADDR(&gateway_winc, 0, 0, 0, 0);
    IP4_ADDR(&mask_winc, 255, 255, 255, 255);
    winc_netif.name[0] = 'w';
    winc_netif.name[1] = 'f';
    netif_add(&winc_netif, &ip_addr_winc, &mask_winc, &gateway_winc, 0, winc_netif_init, tcpip_input); //tcpip_input);
    
    winc_netif.flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    netif_set_up(&winc_netif);

    if(strncmp(dhcp_check_wifi,"true",4)==0){  //if DHCP config read from flash
        winc_netif.dhcp = &winc_dhcp;
        dhcp_start(&winc_netif);
        bsp_board_led_on(BSP_BOARD_LED_0);
        vTaskDelay(100);
        bsp_board_led_off(BSP_BOARD_LED_0);
    }
    else //custom IP
    {
        struct netif * netif_p;
        netif_p = &winc_netif;
        netif_p->ip_addr.addr = ipaddr_value[0];
        netif_p->gw.addr = gw_value[0];
        netif_p->netmask.addr = mask_value[0];
        NRF_LOG_INFO("K");
    }
  }
}


int network_msg_send(void * msg)
{
    uint32_t ipsr;
    uint32_t ret;
    __ASM volatile("MRS %0, ipsr"
                   : "=r"(ipsr));
    if (ipsr) {
      ret = xQueueSendFromISR(WifiConQ, msg, 0);
    } else {
      ret = xQueueSend(WifiConQ, msg, 0);
    }
    if (!ret) {
      return -1;
    }
    return 0;
}

int wifi_msg_connect(char *pcSsid, uint8 u8SsidLen, uint8 u8SecType, void *pvAuthInfo, uint16 u16Ch) {
    uint32_t ipsr;
    int ret;
    void *AuthInfo;
    network_msg_t network_msg;
    wifi_conn_param_t * wf_prm = (wifi_conn_param_t*)network_msg.buff;  
    memset(wf_prm, 0, sizeof(wifi_conn_param_t));
    memcpy(wf_prm->Ssid, pcSsid, u8SsidLen);

    switch (u8SecType) {
    case M2M_WIFI_SEC_OPEN:
      break;
    case M2M_WIFI_SEC_WPA_PSK:
      strncpy(wf_prm->AuthInfo, pvAuthInfo, M2M_MAX_PSK_LEN);
      break;
    case M2M_WIFI_SEC_WEP: {
      tstrM2mWifiWepParams *WepParams = (tstrM2mWifiWepParams *)wf_prm->AuthInfo;
      memcpy(AuthInfo, WepParams, sizeof(tstrM2mWifiWepParams));
    } break;
    case M2M_WIFI_SEC_802_1X: {
      tstr1xAuthCredentials *AuthCredentials = (tstr1xAuthCredentials *)wf_prm->AuthInfo;
      memcpy(AuthInfo, AuthCredentials, sizeof(tstr1xAuthCredentials));
    } break;
    default:
      return -1;
    }
    network_msg.req = REQ_CONNECT;
    wf_prm->u8SsidLen = u8SsidLen;
    wf_prm->u8SecType = u8SecType;
    wf_prm->u16Ch = u16Ch;
    return network_msg_send(&network_msg);
}

int network_msg_ipconfig (network_ipconfig_t * network_ipconfig)
{
  network_msg_t network_msg;
  network_msg.req = REQ_IPSETTINGS;
  memcpy(network_msg.buff, network_ipconfig, sizeof(network_ipconfig_t));
  return network_msg_send(&network_msg);
}

int wifi_msg_disconnect(void) {
  int ret;
  uint32_t ipsr;
  network_msg_t network_msg;
  network_msg.req = REQ_DISCONNECT;
  return network_msg_send(&network_msg);
}

int wifi_msg_scan(void) {
  int ret;
  uint32_t ipsr;
  network_msg_t network_msg;
  network_msg.req = REQ_SCAN;
  return network_msg_send(&network_msg);
}

int wifi_msg_connect_default(void){
    int ret;
  uint32_t ipsr;
  network_msg_t network_msg;
  network_msg.req = REQ_CONNECT_DEFAULT;
  return network_msg_send(&network_msg);
}

int network_msg_check(void){
    int ret;
  uint32_t ipsr;
  network_msg_t network_msg;
  network_msg.req = REQ_NETWORK_CHECK;
  return network_msg_send(&network_msg);
}

int network_msg_show_settings(void){
  int ret;
  network_msg_t network_msg;
  network_msg.req = REQ_SETTINGS_SHOW;
  return network_msg_send(&network_msg);
}

int network_msg_show_settings_wifi(void){
  int ret;
  network_msg_t network_msg;
  network_msg.req = REQ_SETTINGS_SHOW_WIFI;
 // uint32_t ipsr;
 // wifi_conn_param_t wifi_conn_param;
 // wifi_conn_param.req = REQ_SETTINGS_SHOW_WIFI;
 // __ASM volatile("MRS %0, ipsr"
 //                : "=r"(ipsr));
//  if (ipsr) {
//    ret = xQueueSendFromISR(WifiConQ, &wifi_conn_param, 0);
 // } else {
 //   ret = xQueueSend(WifiConQ, &wifi_conn_param, 0);
//  }
//  return ret;
return network_msg_send(&network_msg);
}

//sends if the network is on DHCP or custom and Ethernet IP, GW and MASK to app 
int network_show_settings(void)
{

    int ret;
    int n;
    uint8_t str[256];
    uint8_t network_mode_eth[40];

    if(strncmp(dhcp_check_eth,"true",4)==0){
      strcpy(network_mode_eth,"dhcp");
     }
    else{
      strcpy(network_mode_eth,"custom");
    }

    n = sprintf(str, "success mac_%x:%x:%x:%x:%x:%x||%u.%u.%u.%u#%u.%u.%u.%u#%u.%u.%u.%u#%s\n",
                  mac_id[0],mac_id[1],mac_id[2],mac_id[3],mac_id[4],mac_id[5],
                  ip4_addr1(&enc_netif.ip_addr),ip4_addr2(&enc_netif.ip_addr),ip4_addr3(&enc_netif.ip_addr),ip4_addr4(&enc_netif.ip_addr),
                  ip4_addr1(&enc_netif.netmask),ip4_addr2(&enc_netif.netmask),ip4_addr3(&enc_netif.netmask),ip4_addr4(&enc_netif.netmask),
                  ip4_addr1(&enc_netif.gw),     ip4_addr2(&enc_netif.gw),     ip4_addr3(&enc_netif.gw),     ip4_addr4(&enc_netif.gw),
                  network_mode_eth);
                  ble_serial_send(str, n);
}

//sends if the network is on DHCP or custom and WIFI IP, GW and MASK to app 
int network_show_settings_wifi(void)
{
  network_msg_t p;
  network_ipconfig_t * ipconfig = (network_ipconfig_t*) p.buff;
  int ret;
  int n;
  uint8_t str[256];
  uint8_t network_mode[40];
 
  if(strncmp(dhcp_check_wifi,"true",4)==0){
  strcpy(network_mode,"dhcp");
  }
  else{
  strcpy(network_mode,"custom");
  }

  n = sprintf(str, "success wifimac_%x:%x:%x:%x:%x:%x||%u.%u.%u.%u#%u.%u.%u.%u#%u.%u.%u.%u#%s\n",
                 mac_id_wifi[0],mac_id_wifi[1],mac_id_wifi[2],mac_id_wifi[3],mac_id_wifi[4],mac_id_wifi[5],
                ip4_addr1(&winc_netif.ip_addr),ip4_addr2(&winc_netif.ip_addr),ip4_addr3(&winc_netif.ip_addr),ip4_addr4(&winc_netif.ip_addr),
                ip4_addr1(&winc_netif.netmask),ip4_addr2(&winc_netif.netmask),ip4_addr3(&winc_netif.netmask),ip4_addr4(&winc_netif.netmask),
                ip4_addr1(&winc_netif.gw),     ip4_addr2(&winc_netif.gw),     ip4_addr3(&winc_netif.gw),     ip4_addr4(&winc_netif.gw),
                network_mode);
                ip4_addr4(&enc_netif.gw));
                ble_serial_send(str, n);
}


void wifi_check_connect_queue(void) {
 uint8_t str[256];
  network_msg_t p;
  volatile uint8_t ret;
  if (xQueueReceive(WifiConQ, &p, 0)) {
    switch (p.req) {
    case REQ_CONNECT:
    {
      wifi_conn_param_t * wf_prm = (wifi_conn_param_t*)p.buff;  
      M2M_INFO("wifi REQ_CONNECT");
      ret = m2m_wifi_connect(wf_prm->Ssid, wf_prm->u8SsidLen, wf_prm->u8SecType, wf_prm->AuthInfo, wf_prm->u16Ch);      
    }      
      break;
    case REQ_SCAN:
      M2M_INFO("wifi REQ_SCAN");
      ret = m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
      break;
    case REQ_DISCONNECT:
      M2M_INFO("wifi REQ_DISCONNECT");
      ret = m2m_wifi_disconnect();
      wifi_autoconnect = 0;
      break;
    case REQ_CONNECT_DEFAULT:
      M2M_INFO("wifi REQ_CONNECT_DEFAULT");
      ret = m2m_wifi_default_connect();
      wifi_autoconnect =1;
      break;
  //  case REQ_NETWORK_CHECK:
   //   M2M_INFO("REQ_NETWORK_CHECK");
   //   network_conn_check();
      //tcptest_init();
     // break;
    case REQ_SETTINGS_SHOW:
      network_show_settings();
      break;
    case REQ_SETTINGS_SHOW_WIFI:
      network_show_settings_wifi();
      break;
    case REQ_IPSETTINGS:
      {
        network_ipconfig_t * ipconfig = (network_ipconfig_t*) p.buff;
        struct netif * netif_p;
        if (ipconfig->iface == IFACE_ETH)
          netif_p = &enc_netif;
        else if (ipconfig->iface == IFACE_WLAN)
          netif_p = &winc_netif;
        else
          break;
        if (ipconfig->dhcp)
        {
          netifapi_dhcp_start(netif_p);
          int n;
          n = sprintf(str, "success ipconfig dhcp saved\n");
          ble_serial_send(str, n);
          break;
        }
        netifapi_dhcp_stop(netif_p);
        netifapi_netif_set_addr(netif_p, &ipconfig->ipaddr, &ipconfig->netmask, &ipconfig->gwaddr);
        /*netif_p->ip_addr.addr = ipconfig->ipaddr;
        netif_p->gw.addr = ipconfig->gwaddr;
        netif_p->netmask.addr = ipconfig->netmask;*/
        int n;
        n = sprintf(str, "success ipconfig saved\n");
        ble_serial_send(str, n);
      }
      break;
    default:
      break;
    }
  }
}

void net_winc_poll(void) {
  int16_t i;
  static uint16_t ap_check_timeout = 0;
  uint16_t size;
  i = 64;
  xSemaphoreTake(winc_semph, portMAX_DELAY);
  while ((hif_isr(&size) == M2M_SUCCESS) && (--i))
  {
    if (size == 0)
      break;
  }  
  if (ap_check_timeout-- == 0) {
    m2m_wifi_get_connection_info();
    ap_check_timeout = 30000;
  }
  xSemaphoreGive(winc_semph);
}

uint32_t client_init(void) ;

SemaphoreHandle_t initSemaphore = NULL;

static void tcpip_init_done(void *arg) {
  //net_tcpip_task = xTaskGetCurrentTaskHandle();
  xSemaphoreGive(net_start_sem);
}

void ethernet_init_inside_thread( void *parm)
{
  NRF_LOG_INFO("HERE INSIDE 3 ");
  net_add_winc_netif();  //adds wifi network interface
  net_add_enc_netif();   //adds ethernet network interface     
  xSemaphoreGive( initSemaphore );
}

void network_task_fn(void *arg) {

  int16_t i;
  uint32_t ret;
  uint16_t dhcp_timeout = 1;  //delay variable
  vTaskDelay(1000);
  initSemaphore = xSemaphoreCreateBinary();
  tcpip_init( ethernet_init_inside_thread, "");
  WifiConQ = xQueueCreate(1, sizeof(network_msg_t));
  xSemaphoreTake( initSemaphore, portMAX_DELAY  ) ; //continue only after ethernet_init_inside_thread is completed

 
  vSemaphoreDelete(initSemaphore);
  TaskHandle_t socket_task_handle;
   
  ret = xTaskCreate(socket_test_task, "socket_test",512*3, NULL,1, &socket_task_handle); //socket task to send data
  if (ret != pdPASS)
    {
      NRF_LOG_INFO("xTaskCreate(socket_test_task) err: %x", ret);
    }
 
  while (1) {

    if(strncmp(is_internet1,"wifi",4)==0)  
      { net_winc_poll();}  //poll for wifi events
    else if(strncmp(is_internet1,"ethernet",8)==0)
      { net_enc_poll();}  //poll for ethernet events

    wifi_check_connect_queue();  //check wifi queue for any commands sent from app
  
    if (dhcp_timeout-- == 0) {
      dhcp_timeout = 1;
      //if Ethernet link is up and running
      if (((!(enc_netif.flags & NETIF_FLAG_DHCP))||(enc_netif.dhcp->state == DHCP_BOUND)) && netif_is_link_up(&enc_netif)) {
        if (netif_default != &enc_netif) { //set default network interface 
          netifapi_netif_set_default(&enc_netif);
          bsp_board_led_on(BSP_BOARD_LED_0); //blue led ON
          led_on = 1;
        }
      }
      //if WIFI link is up and running
      else if (((!(winc_netif.flags & NETIF_FLAG_DHCP)) ||(winc_netif.dhcp->state == DHCP_BOUND)) && netif_is_link_up(&winc_netif)) {
        if (netif_default != &winc_netif) { //set default network interface
          netifapi_netif_set_default(&winc_netif);
          bsp_board_led_on(BSP_BOARD_LED_0); //blue LED ON
          led_on = 1;
          }

        if(keyflag == 1 && keyconnect == 1 && led_on ==1){ 
            count_led_on ++;
            if(count_led_on >10){
            count_led_on = 0;
            uint8_t str[20];
            uint8_t n = sprintf(str, "success connected\n");
            ble_serial_send(str, n);
            NRF_LOG_INFO("SUCCESS CONNECTED");
            keyflag = keyconnect =  trying_to_connect = 0; 
          }              
        }           
      } 
      else {
        netifapi_netif_set_default(NULL);
        bsp_board_led_off(BSP_BOARD_LED_0);
        led_on = 0;
      }
    }
    vTaskDelay(10);
  }

  return 0;
}
