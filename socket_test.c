#include <lwip/netdb.h>
#include <lwip/netif.h>
#include <lwip/sockets.h>

#include "FreeRTOS.h"
#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_tls.h"
#include "socket_test.h"
#include "tcptest.h"
#include "task.h"
#include "cmd.h"
#include "network.h"

#define SENDER_PORT_NUM 0

uint32_t send_period = 50000;
uint8_t count_check_internet = 0, socket_conn_issue =0, count_inside_select = 0;
static uint8_t data_buffer[512 * 4];
static uint8_t buff [2048];
static nrf_tls_instance_t m_tls_instance;


static uint8_t buff_circuit1[1024];
static uint8_t buff_circuit2[1024];
static uint8_t buff_circuit3[1024];
static uint8_t buff_circuit4[1024];
static uint8_t buff_phase1[1024];
static uint8_t buff_phase2[1024];
static uint8_t buff_phase3[1024];
static uint8_t buff_last[1024];
static uint8_t buff_check_internet[1024];
static uint8_t txbuff[2048];
static uint8_t sidtxbuff[512];

static int socket_fd;

static uint8_t sample_request[] = "POST  /Pulse/Data HTTP/1.1\nHost:  pulsedataapi-env.d6kdcypma3.us-east-1.elasticbeanstalk.com\nContent-Type: application/json\nContent-Length: 578\n\n{\n\t\"token\":\"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJwdWxzZUlEIjoiUDY2NjMxIiwidmVyc2lvbiI6IjEuMC44IiwiaWF0IjoxNTY5NTA3MjY5fQ.zBVD_UzHDd22OSukS7G49V5-vlGZy7sRifuqpgTz0EU\",\n\t\"pulseID\": \"P66631\",\n\t\"data\":{\n\t\t\"rarh-1\": 0.00,\n\t\t\"radb-1\": 0.00,\n\t\t\"sarh-1\": 0.00,\n\t\t\"sadb-1\": 0.00,\n\t\t\"oarh-1\": 50.32,\n\t\t\"oadb-1\": 21.81,\n\t\t\"spc-1\": -91.83,\n\t\t\"llpc-1\": -163.75,\n\t\t\"sltc-1\": 0.00,\n\t\t\"lltc-1\": 0.00,\n\t\t\"to\": 0.00,\n\t\t\"tw\": 0.00,\n\t\t\"ty\": 0.00,\n\t\t\"volts-1\": 0.01,\n\t\t\"amps-1\": 0.00,\n\t\t\"volts-2\": 0.01,\n\t\t\"amps-2\": 0.00,\n\t\t\"volts-3\": 0.01,\n\t\t\"amps-3\": 0.00,\n\t\t\"pf\": 64.536,\n\t\t\"watts\": 0.00\n\t}\n}\n";

//ARSH CHANGES
//static uint8_t http_data_req[] = "POST /Pulse/Data HTTP/1.1\n"
//                                 "Host:  pulseapi.imanifold.com\n"
//                                "Content-Type: application/json\n"
//                                 "Content-Length: %i\n"
//                                "\n";


static uint32_t tls_output_handler(nrf_tls_instance_t const *p_instance,
    uint8_t const *p_data,
    uint32_t datalen) {
  int n = 0;
  const uint16_t transport_write_len = datalen;
  NRF_LOG_INFO("tls_output_handler: %d bytes", datalen);
  if (datalen == 0) {
    return NRF_SUCCESS;
  }
//   stats_display();
  n = send(socket_fd, p_data, datalen, 0);
  if (n != datalen)
    NRF_LOG_INFO("tls_output_handler err: %x", n);
  if (n == datalen)
    return NRF_SUCCESS;
  else
    return -1;
}


//resovlves host address and returns socket
//on error returns negative value
int connect_to_addr (uint8_t * addr)
{
  struct hostent *server_hostent;
  struct sockaddr_in sa;
  int ret_socket;

  uint8_t addrbuff[256];

  struct addrinfo hints, *servinfo, *p;
  int rv;
  int ret;
  int sockopt =1;
  int sock_timeout =10000;
  uint32_t txlen;
  uint32_t t;

  int recv_data; 

  //resolve host address
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // use AF_INET6 to force IPv6
  hints.ai_socktype = SOCK_STREAM;
  strncpy(addrbuff, addr, sizeof(addrbuff));
  if ((rv = getaddrinfo(addrbuff, "443", &hints, &servinfo)) != 0) {
    printf("getaddrinfo err: 0x%x\n", rv);
    return -1;
  }

  
  ret_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (ret_socket < 0) {
    printf("socket call failed");
    freeaddrinfo(servinfo);
    return -1;
  }
  /*rv = setsockopt(ret_socket, SOL_SOCKET, SO_KEEPALIVE,(void*)&sockopt, sizeof(sockopt));
  if (rv >= 0)
    rv = setsockopt(ret_socket, SOL_SOCKET, SO_RCVTIMEO,(void*)&sock_timeout, sizeof(sock_timeout));  
  if (rv >= 0)  
    rv = setsockopt(ret_socket, SOL_SOCKET, SO_SNDTIMEO,(void*)&sock_timeout, sizeof(sock_timeout));  
  if (rv < 0)
  {
    printf("socket opt failed");
    freeaddrinfo(servinfo);
    close(ret_socket);
    return -1;
  }  */

  memset(&sa, 0, sizeof(struct sockaddr_in));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = netif_default->ip_addr.addr;
  sa.sin_port = htons(SENDER_PORT_NUM);
  if (bind(ret_socket, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) == -1) {
    printf("Bind to Port Number %d failed\n", SENDER_PORT_NUM);
    close(ret_socket);
    freeaddrinfo(servinfo);
    return -1;
  }
  if (connect(ret_socket, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
    printf("connect failed \n");
    freeaddrinfo(servinfo);
    close(ret_socket);
    return -1;
  }
  freeaddrinfo(servinfo);
  return ret_socket;
}

void socket_test_task(void *arg) {
  uint32_t ret,txlen,rxlen,timeout;
  uint16_t restart_pulse_count = 0;
  int n;
  uint8_t str[256];
  uint8_t waiting_for_response ;
  uint8_t is_timeout, write_error, write_error_count = 0;  
  int recv_ret, rc;
  fd_set fdset;
  struct timeval tv;
  static uint8_t http_data_req[200];
  uint16_t i,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,j = 0;
   repeat:  //until socket connects successfully
  //resolve host address
  //socket_fd = connect_to_addr("pulsedataapi-env.d6kdcypma3.us-east-1.elasticbeanstalk.com");
  //socket_fd = connect_to_addr("pulseapi.imanifold.com");
    socket_fd = connect_to_addr(API_link1);
    if (socket_fd < 0) {
       NRF_LOG_INFO("tls_client_not_connected\r\n");
      check_internet = 0;
      if(trying_to_connect ==1){
      socket_conn_issue++;
      if(socket_conn_issue >12){  //if socket fails 12 consecutive times
         n = sprintf(str, "failed internet\n");   //internet problem
         ble_serial_send(str, n);
         trying_to_connect = 0;
         socket_conn_issue = 0;
      }
     else{
   // NRF_LOG_INFO("trying to connect\n");
         n = sprintf(str, "success trying to connect\n"); //trying max 12 times
         ble_serial_send(str, n);
     }
    }
    else if(wifi_disconnected == 1){  //if wifi has been disconnected for than ~3us
       count_check_internet++;
       if(count_check_internet >3){   
         count_check_internet = 0;
         n = sprintf(str, "failed internet\n"); //internet problem
         ble_serial_send(str, n); 
         wifi_disconnected = 0;
         count_check_internet = 0;
       }
       else{
        // NRF_LOG_INFO("trying to connect\n");
         n = sprintf(str, "success trying to connect\n");
         ble_serial_send(str, n);
        }
    }

    vTaskDelay(2000);
    goto repeat;
  }
 // after TLS client is connected
  check_internet = 1; //check internet connection variable
  if(write_error == 1){  //trying to solve dinas problem by restarting system
      restart_system =1;
  }

  //allocate tls client
  nrf_tls_instance_t *tls_instance = &m_tls_instance;
  nrf_tls_key_settings_t tls_keys = {};
  const nrf_tls_options_t tls_options =
      {
          .output_fn = tls_output_handler,
          .transport_type = NRF_TLS_TYPE_STREAM,
          .role = NRF_TLS_ROLE_CLIENT,
          .p_key_settings = &tls_keys};
  fcntl(socket_fd, F_SETFL, O_NONBLOCK);
  nrf_tls_alloc(tls_instance, &tls_options);
  timeout = xTaskGetTickCount() - send_period;
  tv.tv_sec = 10;
  tv.tv_usec = 0;
  is_timeout = 0;
  waiting_for_response = 0;
  while (1) {
 
 if(restart_pulse_count>1440) //restart PULSE after 24hrs
 {
    sd_nvic_SystemReset();
     
 }

//make the http POST request using the API LINK from flash
  sprintf(http_data_req,"POST  /Pulse/Data HTTP/1.1\n"
                               "Host:  %s\n"
                                "Content-Type: application/json\n"
                                 "Content-Length: %%i\n"
                                "\n", API_link1);

//check for correct config string
if( strcmp(config_string1,"config[circuit:1,phase:1,voltage:120,frequen:60]") == 0){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_phase1(buff_phase1);
      j = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+j);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_last);
      }

else if( strcmp(config_string1,"config[circuit:1,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:1,voltage:240,frequen:50]") == 0){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_phase1(buff_phase1);
      i2 = prepare_data_string_phase2(buff_phase2);
      j = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+j);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_phase2);
      strcat(txbuff, buff_last);

      }

else if( strcmp(config_string1,"config[circuit:1,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:3,voltage:460,frequen:50]") == 0 ){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_phase1(buff_phase1);
      i2 = prepare_data_string_phase2(buff_phase2);
      i3 = prepare_data_string_phase3(buff_phase3);
      j = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+i3+j);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_phase2);
      strcat(txbuff, buff_phase3);
      strcat(txbuff, buff_last);

      }

else if( strcmp(config_string1,"config[circuit:2,phase:1,voltage:120,frequen:60]") == 0){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_circuit2(buff_circuit2);
      i2 = prepare_data_string_phase1(buff_phase1);
      i3 = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+i3);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_circuit2);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_last);

      }

else if( strcmp(config_string1,"config[circuit:2,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:1,voltage:240,frequen:50]") == 0 ){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_circuit2(buff_circuit2);
      i2 = prepare_data_string_phase1(buff_phase1);
      i3 = prepare_data_string_phase2(buff_phase2);
      i4 = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_circuit2);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_phase2);
      strcat(txbuff, buff_last);

      }

else if( strcmp(config_string1,"config[circuit:2,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:3,voltage:460,frequen:60]") == 0|| strcmp(config_string1,"config[circuit:2,phase:3,voltage:460,frequen:50]") == 0){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_circuit2(buff_circuit2);
      i2 = prepare_data_string_phase1(buff_phase1);
      i3 = prepare_data_string_phase2(buff_phase2);
      i4 = prepare_data_string_phase3(buff_phase3);
      i5 = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_circuit2);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_phase2);
      strcat(txbuff, buff_phase3);
      strcat(txbuff, buff_last);

      }

else if( strcmp(config_string1,"config[circuit:3,phase:1,voltage:120,frequen:60]") == 0){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_circuit2(buff_circuit2);
      i2 = prepare_data_string_circuit3(buff_circuit3);
      i3 = prepare_data_string_phase1(buff_phase1);
      i4 = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_circuit2);
      strcat(txbuff, buff_circuit3);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_last);

      }

  else if( strcmp(config_string1,"config[circuit:3,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:1,voltage:240,frequen:50]") == 0 ){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_circuit2(buff_circuit2);
      i2 = prepare_data_string_circuit3(buff_circuit3);
      i3 = prepare_data_string_phase1(buff_phase1);
      i4 = prepare_data_string_phase2(buff_phase2);
      j = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+j);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_circuit2);
      strcat(txbuff, buff_circuit3);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_phase2);
      strcat(txbuff, buff_last);

      }

   else if( strcmp(config_string1,"config[circuit:3,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:3,voltage:460,frequen:50]") == 0 ){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_circuit2(buff_circuit2);
      i2 = prepare_data_string_circuit3(buff_circuit3);
      i3 = prepare_data_string_phase1(buff_phase1);
      i4 = prepare_data_string_phase2(buff_phase2);
      i5 = prepare_data_string_phase3(buff_phase3);
      j = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5+j);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_circuit2);
      strcat(txbuff, buff_circuit3);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_phase2);
      strcat(txbuff, buff_phase3);
      strcat(txbuff, buff_last);

      }

else if( strcmp(config_string1,"config[circuit:4,phase:1,voltage:120,frequen:60]") == 0){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_circuit2(buff_circuit2);
      i2 = prepare_data_string_circuit3(buff_circuit3);
      i3 = prepare_data_string_circuit4(buff_circuit4);
      i4 = prepare_data_string_phase1(buff_phase1);
      j = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+j);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_circuit2);
      strcat(txbuff, buff_circuit3);
      strcat(txbuff, buff_circuit4);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_last);
 
      }

else if( strcmp(config_string1,"config[circuit:4,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:1,voltage:240,frequen:50]") == 0 ){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_circuit2(buff_circuit2);
      i2 = prepare_data_string_circuit3(buff_circuit3);
      i3 = prepare_data_string_circuit4(buff_circuit4);
      i4 = prepare_data_string_phase1(buff_phase1);
      i5 = prepare_data_string_phase2(buff_phase2);
      j = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5+j);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_circuit2);
      strcat(txbuff, buff_circuit3);
      strcat(txbuff, buff_circuit4);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_phase2);
      strcat(txbuff, buff_last);

      }

   else if( strcmp(config_string1,"config[circuit:4,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:3,voltage:460,frequen:50]") == 0 ){

      i = prepare_data_string_circuit1(buff_circuit1);
      i1 = prepare_data_string_circuit2(buff_circuit2);
      i2 = prepare_data_string_circuit3(buff_circuit3);
      i3 = prepare_data_string_circuit4(buff_circuit4);
      i4 = prepare_data_string_phase1(buff_phase1);
      i5 = prepare_data_string_phase2(buff_phase2);
      i6 = prepare_data_string_phase3(buff_phase3);
      j = prepare_data_string_last(buff_last);
      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5+i6+j);
      strcat(txbuff, buff_circuit1);
      strcat(txbuff, buff_circuit2);
      strcat(txbuff, buff_circuit3);
      strcat(txbuff, buff_circuit4);
      strcat(txbuff, buff_phase1);
      strcat(txbuff, buff_phase2);
      strcat(txbuff, buff_phase3);
      strcat(txbuff, buff_last);

      }

    FD_SET(socket_fd, &fdset);
    //wait for socket to receive data
    rc = select(socket_fd + 1, &fdset, 0, 0, &tv);
  //   NRF_LOG_INFO("RC %d",rc);
    if (rc < 0)
 //   NRF_LOG_INFO("RC<0");
      break;
    if (rc == 0) {
      NRF_LOG_INFO("select() timeout after 10 sec");
      if(wifi_disconnected == 1){
      ++count_inside_select;
      if(count_inside_select>2){
                n = sprintf(str, "failed internet\n");
     ble_serial_send(str, n); 
     wifi_disconnected = 0;
     count_inside_select = 0;
      
      }
      else{
      //NRF_LOG_INFO("trying to connect\n");
           n = sprintf(str, "success trying to connect\n");
          ble_serial_send(str, n);
      }
      
      }
      is_timeout = 1;
      if (waiting_for_response)
        break;
    }
    uint8_t testingInt = 0;
    //check if there is some data received on socket
    if (FD_ISSET(socket_fd, &fdset)) {
  //  NRF_LOG_INFO("FD_ISSET");
      recv_ret = recv(socket_fd, data_buffer, sizeof(data_buffer), 0);
      if (recv_ret > 0) {
        is_timeout = 0;
        NRF_LOG_INFO("tls_input: %d bytes", recv_ret);
        ret = nrf_tls_input(tls_instance, data_buffer, recv_ret);
        if (ret != NRF_SUCCESS) {
          NRF_LOG_INFO("nrf_tls_input 0x%x", ret);
        }
        txlen = 0;
      } else {
        NRF_LOG_INFO("recv ret %i", recv_ret);
        break;
      }
    }
//    else{
//    testingInt = 2;
//    uint8_t hello123 = fdset.fd_bits[0];
//      NRF_LOG_INFO("aya FD_ISSET issue FDSET VALUE %d", hello123);
//    }
    //receive data from TLS
    rxlen = sizeof(buff);
    ret = nrf_tls_read(tls_instance, buff, &rxlen);
//    if(testingInt == 2){
//      NRF_LOG_INFO("aya FD_ISSET issue ret VALUE %d", ret);
//    }
    if (ret == NRF_SUCCESS) {
      waiting_for_response = 0;
      NRF_LOG_INFO("tls read %d", rxlen);
    }
  //  NRF_LOG_INFO("SEND PERIOD %d",send_period );
    if ((xTaskGetTickCount() - timeout) > send_period) {
//    NRF_LOG_INFO("xTaskGetTickCount(): %d", xTaskGetTickCount());
//    NRF_LOG_INFO("timeout: %d", timeout);
//    NRF_LOG_INFO("send_period: %d", send_period);
      txlen = 0;
      //write 0 bytes to tls to check if tls connection is established
      uint32_t tlswriteresponse = nrf_tls_write(tls_instance, buff, &txlen);
//      if(testingInt == 2){
//        NRF_LOG_INFO("aya FD_ISSET issue tlswriteresponse VALUE %d", tlswriteresponse);
//      }
//      if ( tlswriteresponse == NRF_SUCCESS) {
        ret = nrf_tls_write(tls_instance, buff, &txlen);
//        stats_display();
        vTaskDelay(500);
        if(ret  == NRF_SUCCESS){
//      if ((nrf_tls_write(tls_instance, buff, &txlen) == NRF_SUCCESS)) {
        NRF_LOG_INFO("tls connection established");
//        txlen = sizeof(sample_request) - 1;

      //send only if token and link are available 
      if((API_token1[0] != 0xFF) && (API_link1[0] != 0xFF)){
      txlen = strlen(txbuff);
       //stats_display();
        ret = nrf_tls_write(tls_instance, txbuff, &txlen);
        //stats_display();
        vTaskDelay(500);
        if (ret != NRF_SUCCESS) {
          NRF_LOG_INFO("nrf_tls_write error: %x", ret);
//          write_error = 1;
          break;
        } else {
        
          timeout = xTaskGetTickCount();
          NRF_LOG_INFO("nrf_tls_write written %i bytes", txlen);
          waiting_for_response = 1;
          is_timeout = 0;
          restart_pulse_count++;

        }
       }
      } else if (is_timeout){
        break;
      }
    }
    else{
    
    write_error_count++;
    if(write_error_count>200)
    {
     write_error = 1;
    }
    }
  }
  close(socket_fd);
  //NRF_LOG_INFO("CLOSING SOCKET");
  nrf_tls_free(tls_instance);
  vTaskDelay(5000);
  goto repeat;
  while (1) {
    vTaskDelay(100);
  }
}