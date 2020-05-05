//#include "lwip/opt.h"
//#include "lwip/tcp.h"
//#include "lwip/ip.h"
//#include "lwip/dns.h"
//#include "lwip/netif.h"
//#include "lwip/pbuf.h"
//#include "lwip/timers.h"
//#include "boards.h"
//#include "math.h"
//#include "cmd.h"
//#include "network_conn_check.h"
//#include "nrf_log.h"
//
//static struct ip_addr server_addr;
//err_t network_conn_check_start(void);
//void netwk_check_write(struct tcp_pcb *tpcb);
//static char response_back[1024];
//static uint8_t str[512*3];
//static uint8_t buff_check_internet[1024];
//static uint8_t txbuff[2048];
//
//
//
//void netwk_check_close_restart(struct tcp_pcb *tpcb)
//{
//NRF_LOG_INFO("netwk_check_close_restart");
//    uint16_t timeout;
//    if (tpcb != NULL)
//    {
//      tcp_close(tpcb);
//    }   
//            int n;
//        n = sprintf(str, "failed internet\n");
//        ble_serial_send(str, n);
//        NRF_LOG_INFO("failed internet of restart");
//
// //   sys_untimeout(netwk_check_write, tpcb);
//  //  sys_untimeout(network_conn_check_start, NULL);  
//  //  timeout = 40000;
//  //  sys_timeout(timeout, network_conn_check_start, NULL);
////  network_conn_check_start();
//
//}
//
//void netwk_check_write(struct tcp_pcb *tpcb) {
//NRF_LOG_INFO("netwk_check_write");
//  uint16_t i = 0;
//  static uint8_t http_data_req_check_internet[200];
//
//  sprintf(http_data_req_check_internet,"POST /Pulse/checkPulseConnection HTTP/1.1\n"
//                                 "Host: %s\n"
//                                 "Content-Type: application/json\n"
//                                "Content-Length: %%i\n"
//                                "\n", API_link1);
//
//  volatile err_t err = ERR_OK;
// 
//  if (tpcb != NULL) {
//
//    if((API_token1[0] != 0xFF) && (API_link1[0] != 0xFF))  {
//    if (tpcb->state == ESTABLISHED) {
//
//      
//      i = prepare_data_check_internet(buff_check_internet);
//      sprintf(txbuff, http_data_req_check_internet, i);
//      strcat(txbuff, buff_check_internet);
//      err = tcp_write(tpcb, txbuff, strlen(txbuff), TCP_WRITE_FLAG_COPY);
//      tcp_output(tpcb);
//   
//     }
//   }
// }
//}
//
//
////receive callback function
//err_t netwk_check_recv(void *arg, volatile struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
//NRF_LOG_INFO("netwk_check_recv");
//  err_t e = ERR_OK;
//  if (p == NULL) // remote host sent a FIN packet
//  {      NRF_LOG_INFO("p == NULL");
//        // sys_untimeout(network_conn_check_start, NULL);
//       //  sys_timeout(3000, network_conn_check_start, NULL);
//    // netwk_check_close_restart(tpcb);
//     e = ERR_ABRT;
//  }  
//  else 
//  {   NRF_LOG_INFO("inside recv");
//      sys_untimeout(network_conn_check_start, NULL);
//      pbuf_free(p);
//      memcpy(response_back, p+(0x04), 20);
//
//      if(response_back[15]==0x32 && response_back[16] == 0x30 && response_back[17]==0x30){
//    
//          if(send_response_flag == 1){
//            int n;
//            n = sprintf(str, "success internet\n");
//            ble_serial_send(str, n);
//            send_response_flag = 0;
//            no_ipaddress = 0;
//            check_netif_default = 0;
//            tcp_close(tpcb);
//            }
//
//          NRF_LOG_INFO("success internet\n");
//      }
//
//    else{
//        int n;
//        n = sprintf(str, "failed internet\n");
//        ble_serial_send(str, n);
//        NRF_LOG_INFO("FAILED INTERNET OF RECV");
//        tcp_close(tpcb);
//      }
//    
//
//      strcpy(check_Internet,"false");
//   
//  }
//  return e;
//
//
//}
//
//
//
////err_t netwk_check_poll(void *arg, struct tcp_pcb *tpcb) {    
//  //  netwk_check_close_restart(tpcb);
//   // return ERR_ABRT;
////}
//
//
//
////error callback function
//void netwk_check_err(void *arg, err_t err) {
//NRF_LOG_INFO("netwk_check_err");
//  struct tcp_pcb * pcb = (struct tcp_pcb*)arg;
//  volatile err_t e;
//  switch (err) {
//  case ERR_ABRT:
//
//    break;
//  case ERR_RST:
//
//    break;
//  }
//  sys_untimeout(netwk_check_write, pcb);
//  sys_untimeout(network_conn_check_start, NULL);
//            int n;
//          n = sprintf(str, "failed internet\n");
//          ble_serial_send(str, n);
//          NRF_LOG_INFO("failed internet FROM NETWRK CHECK ERROR");
//    
////   if(send_response_flag == 1){
////          int n;
////        n = sprintf(str, "failed internet\n");
////        ble_serial_send(str, n);
//// // sys_timeout(2000, network_conn_check_start, NULL);
////}
//// network_conn_check_start();
//
//}
//
//err_t netwk_check_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
//NRF_LOG_INFO("netwk_check_sent");
//  //bsp_board_led_invert(BSP_BOARD_LED_1);
//  return ERR_OK;
//}
//
//
//err_t netwk_check_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
//NRF_LOG_INFO("netwk_check_connected");
//  volatile err_t e = ERR_OK;
//
// // sys_untimeout(network_conn_check_start, NULL);
//
//  netwk_check_write(tpcb);
//  return e;
//}
//
//void netwk_check_connect(struct ip_addr *addr) {
//
////NRF_LOG_INFO("netwk_check_connect");
//  NRF_LOG_INFO("netwk_check_connect");
//  err_t err;
//  struct tcp_pcb *pcb_check_conn;    
//    pcb_check_conn = tcp_new();
//    if (pcb_check_conn == NULL)
//    {
//      netwk_check_close_restart(NULL);
//      return;
//    }
//    tcp_err(pcb_check_conn, netwk_check_err);
//    tcp_recv(pcb_check_conn, netwk_check_recv);
//    tcp_sent(pcb_check_conn, netwk_check_sent);
//
//   // tcp_poll(pcb_check_conn,netwk_check_poll,20);
//    tcp_arg(pcb_check_conn, pcb_check_conn);
//    
//
//    tcp_arg(pcb_check_conn, pcb_check_conn);
//    
//
//
//     err = tcp_connect(pcb_check_conn, addr,
//       80, netwk_check_connected);  
//
//
//    if (err!= ERR_OK)
//    {
//      NRF_LOG_INFO("netwk_check_connect not connected");
//    //  netwk_check_close_restart(pcb_check_conn);
//        sys_untimeout(network_conn_check_start, NULL);
//       sys_timeout(3000, network_conn_check_start, NULL);
//     // return;
//    }
// 
//}
//
//void network_conn_check_dns_callback(const char *name, struct ip_addr *ipaddr, void *arg) {
//
//  if ((ipaddr) && (ipaddr->addr)) {
//  NRF_LOG_INFO("(ipaddr) && (ipaddr->addr)");
//  no_ipaddress = 0;
//  netwk_check_connect(ipaddr);
//  }
//
// else {
//    NRF_LOG_INFO("no ip address");
//    ++no_ipaddress;
//    if(no_ipaddress<5){
//    sys_untimeout(network_conn_check_start, NULL);
//    sys_timeout(5000, network_conn_check_start, NULL);
//    }
//    else{
//    NRF_LOG_INFO("else of no ipaddress");
//          sys_untimeout(network_conn_check_start, NULL);
//        //  sys_untimeout(network_conn_check_start, NULL);
//        //  sys_untimeout(network_conn_check_start, NULL);
//          no_ipaddress = 0;
//          int n;
//          n = sprintf(str, "failed internet\n");
//          ble_serial_send(str, n);
//          NRF_LOG_INFO("failed internet of no ipaddress");
//    
//    }
//
// //  network_conn_check_start();
//
//  }
//}
//
//
//
////initializes the connection between uC and host
//err_t network_conn_check_start(void) {
//  NRF_LOG_INFO("network_conn_check_start 1");
//  err_t err;
//  if (netif_default != NULL)
//
//  {
//  check_netif_default = 0;
//    NRF_LOG_INFO("netif_default != NULL");
//  err = dns_gethostbyname(API_link1, &server_addr, network_conn_check_dns_callback, NULL);
//    if (err == ERR_OK) {
//      NRF_LOG_INFO("err == ERR_OK");
//      netwk_check_connect(&server_addr);
//    }
//  
//  else{
//  NRF_LOG_INFO("err != ERR_OK");
//    sys_untimeout(network_conn_check_start, NULL);
//    sys_timeout(3000, network_conn_check_start, NULL);
//
// //  network_conn_check_start();
//
//  }
//  return err;
//}
//else{
//NRF_LOG_INFO("netif_default == NULL");
//    ++check_netif_default;
//
//    if(check_netif_default<5){
//    sys_untimeout(network_conn_check_start, NULL);
//    sys_timeout(3000, network_conn_check_start, NULL);
//    }
//
//    else{ NRF_LOG_INFO("else of netif_default == NULL");
//          sys_untimeout(network_conn_check_start, NULL);
//       //   sys_untimeout(network_conn_check_start, NULL);
//       //   sys_untimeout(network_conn_check_start, NULL);
//          check_netif_default =0;
//          int n;
//          n = sprintf(str, "failed internet\n");
//          ble_serial_send(str, n);
//          NRF_LOG_INFO("failed internet from netif default");
//
//
//    }
//}
//}
//
//
//void network_conn_check(void)
//{
//  NRF_LOG_INFO("network_conn_check");
//  network_conn_check_start();
//  send_response_flag = 1;
// 
//
//}
