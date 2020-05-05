//#include "lwip/opt.h"
//#include "lwip/tcp.h"
//#include "lwip/ip.h"
//#include "lwip/dns.h"
//#include "lwip/netif.h"
//#include "lwip/pbuf.h"
//#include "lwip/timers.h"
#include "tcptest.h"
//#include "boards.h"
//#include "math.h"
#include "cmd.h"
//#include "nrf_log.h"
//
//char sId[32] = "0003_000012345645";
//static uint8_t str[512*3];
//char response_back[1024];
////#define RESPONSE_ADDR 0x20012280;
extern char iM_pulse_ID[6];

sensors_state_t1     sensors_state1;
sensors_state_t2     sensors_state2;
sensors_state_t3     sensors_state3;
sensors_state_t4     sensors_state4;

//static const uint8_t pulse_ID[] = "P97653";
//
//
////static uint8_t http_data_req[] = "POST /Pulse/Data HTTP/1.1\n"
////                                 "Host:  pulsedataapi-env.d6kdcypma3.us-east-1.elasticbeanstalk.com\n"
////                                "Content-Type: application/json\n"
////                                 "Content-Length: %i\n"
////                                "\n";
////
////
////
////static uint8_t http_data_req_check_internet[] = "POST /Pulse/checkPulseConnection HTTP/1.1\n"
////                                 "Host: pulsedataapi-env.d6kdcypma3.us-east-1.elasticbeanstalk.com\n"
////                                 "Content-Type: application/json\n"
////                                "Content-Length: %i\n"
////                                "\n";
//
//
//
////static uint8_t http_data_req[] = "POST /Pulse/Data HTTP/1.1\n"
//                  //               "Host:  beta.imanifold.com\n"
//                  //               "Content-Type: application/json\n"
//                   ////              "Content-Length: %i\n"
//                            //     "\n";
////
////static uint8_t http_data_req_check_internet[] = "POST /Pulse/checkPulseConnection HTTP/1.1\n"
//                              //   "Host: beta.imanifold.com\n"
//                              //   "Content-Type: application/json\n"
//                              //   "Content-Length: %i\n"
//                              //   "\n";
//
//
//static uint8_t sIdstr_format[] =
//    "{\n"
//    "\t\"sId\":\"%s\"\n"
//    "}\n";
//
//static uint8_t datastr_format_1[] =
//    "{\n"
//    "\t\"sId\": \"%s\",\n"
//    "\t\"data\":[{\n"
//    "\t\t\"rarh-1-0\": %0.2f,\n"
//    "\t\t\"radb-1-0\": %0.2f,\n"
//    "\t\t\"sarh-1-0\": %0.2f,\n"
//    "\t\t\"sadb-1-0\": %0.2f,\n"
//    "\t\t\"oarh-1-0\": %0.2f,\n"
//    "\t\t\"oadb-1-0\": %0.2f,\n"
//    "\t\t\"sltc-1-1\": %0.2f,\n"
//    "\t\t\"sltc-2-1\": %0.2f,\n"
//    "\t\t\"sltc-3-1\": %0.2f,\n"
//    "\t\t\"sltc-4-1\": %0.2f,\n"
//    "\t\t\"lltc-1-1\": %0.2f,\n"
//    "\t\t\"lltc-2-1\": %0.2f,\n"
//    "\t\t\"lltc-3-1\": %0.2f,\n"
//    "\t\t\"lltc-4-1\": %0.2f,\n"
//    "\t\t\"spc-1-1\": %0.2f,\n"
//    "\t\t\"spc-2-1\": %0.2f,\n"
//    "\t\t\"spc-3-1\": %0.2f,\n"
//    "\t\t\"spc-4-1\": %0.2f,\n"
//    "\t\t\"llpc-1-1\": %0.2f,\n"
//    "\t\t\"llpc-2-1\": %0.2f,\n"
//    "\t\t\"llpc-3-1\": %0.2f,\n"
//    "\t\t\"llpc-4-1\": %0.2f,\n"
//    "\t\t\"lUei\": [\n"
//    "\t\t\t\"%0.2fV\",\n"
//    "\t\t\t\"%0.2fV\",\n"
//    "\t\t\t\"%0.2fV\"\n"
//    "\t\t],\n"
//    "\t\t\"uUei\": [\n"
//    "\t\t\t\"%0.2fA\",\n"
//    "\t\t\t\"%0.2fA\",\n"
//    "\t\t\t\"%0.2fA\"\n"
//    "\t\t],\n"
//    "\t\t\"powerFactor\": %0.3f,\n"
//    "\t\t\"activeWatts\": %0.2f,\n"
//    "\t\t\"apparentWatts\": %0.2f,\n"
//    "\t\t\"reactiveWatts\": %0.2f,\n"
//    "\t\t\"atmpres\": %0.1f\n"
//    "\t}]\n"
//    "}\n";

static uint8_t datastr_format_circuit1[] =
    "{\n"
    "\t\"token\":\"%s\",\n"
    "\t\"pulseID\": \"%s\",\n"
    "\t\"data\":{\n"
    "\t\t\"rarh-1\": %0.2f,\n"
    "\t\t\"radb-1\": %0.2f,\n"
    "\t\t\"sarh-1\": %0.2f,\n"
    "\t\t\"sadb-1\": %0.2f,\n"
    "\t\t\"oarh-1\": %0.2f,\n"
    "\t\t\"oadb-1\": %0.2f,\n"
    "\t\t\"spc-1\": %0.2f,\n"
    "\t\t\"llpc-1\": %0.2f,\n"
    "\t\t\"sltc-1\": %0.2f,\n"
    "\t\t\"lltc-1\": %0.2f,\n"
    "\t\t\"to\": %0.2f,\n"
    "\t\t\"tw\": %0.2f,\n"
    "\t\t\"ty\": %0.2f,\n";

static uint8_t datastr_format_circuit2[] =

    "\t\t\"spc-2\": %0.2f,\n"
    "\t\t\"llpc-2\": %0.2f,\n"
    "\t\t\"sltc-2\": %0.2f,\n"
    "\t\t\"lltc-2\": %0.2f,\n";

static uint8_t datastr_format_circuit3[] =

    "\t\t\"spc-3\": %0.2f,\n"
    "\t\t\"llpc-3\": %0.2f,\n"
    "\t\t\"sltc-3\": %0.2f,\n"
    "\t\t\"lltc-3\": %0.2f,\n";

static uint8_t datastr_format_circuit4[] =

    "\t\t\"spc-4\": %0.2f,\n"
    "\t\t\"llpc-4\": %0.2f,\n"
    "\t\t\"sltc-4\": %0.2f,\n"
    "\t\t\"lltc-4\": %0.2f,\n";

static uint8_t datastr_format_phase1[] =

    "\t\t\"volts-1\": %0.2f,\n"
    "\t\t\"amps-1\": %0.2f,\n";

static uint8_t datastr_format_phase2[] =

    "\t\t\"volts-2\": %0.2f,\n"
    "\t\t\"amps-2\": %0.2f,\n";

static uint8_t datastr_format_phase3[] =

    "\t\t\"volts-3\": %0.2f,\n"
    "\t\t\"amps-3\": %0.2f,\n";


static uint8_t datastr_format_last[] =

    "\t\t\"pf\": %0.3f,\n"
    "\t\t\"watts\": %0.2f\n"
    "\t}\n"
    "}\n";

static uint8_t datastr_format_check_internet[] =

     "{\n"
    "\t\t\"token\":\"%s\",\n"
    "\t\t\"pulseID\":\"%s\"\n"
    "\t}\n" ;



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

//static struct ip_addr server_addr;

//err_t tcptest_start(void);
//void tcptest_write(struct tcp_pcb *tpcb);

int prepare_data_string_circuit1(char *s) {
  sensors_state_t4 *st = &sensors_state4;
  return sprintf(s, datastr_format_circuit1,
                  API_token1,
                  iM_pulse_ID, 
                  st->rarh10, 
                  st->radb10, 
                  st->sarh10, 
                  st->sadb10, 
                  st->oarh10, 
                  st->oadb10, 
                  st->spc11, 
                  st->llpc11, 
                  st->sltc11, 
                  st->lltc11, 
                  st->wire1_orange,
                  st->wire2_white,
                  st->wire3_yellow); 

}

int prepare_data_string_circuit2(char *s) {
  sensors_state_t4 *st = &sensors_state4;
  return sprintf(s, datastr_format_circuit2,

                  st->spc21, 
                  st->llpc21, 
                  st->sltc21, 
                  st->lltc21);
}

int prepare_data_string_circuit3(char *s) {
  sensors_state_t4 *st = &sensors_state4;
  return sprintf(s, datastr_format_circuit3,

                  st->spc31, 
                  st->llpc31, 
                  st->sltc31, 
                  st->lltc31);
}

int prepare_data_string_circuit4(char *s) {
  sensors_state_t4 *st = &sensors_state4;
  return sprintf(s, datastr_format_circuit4,

                  st->spc41, 
                  st->llpc41, 
                  st->sltc41, 
                  st->lltc41);
}

int prepare_data_string_phase1(char *s) {
  sensors_state_t4 *st = &sensors_state4;
  return sprintf(s, datastr_format_phase1,
                  st->uUei1, 
                  st->iUei1);
}

int prepare_data_string_phase2(char *s) {
  sensors_state_t4 *st = &sensors_state4;
  return sprintf(s, datastr_format_phase2,
                  st->uUei2, 
                  st->iUei2 );
}

int prepare_data_string_phase3(char *s) {
  sensors_state_t4 *st = &sensors_state4;
  return sprintf(s, datastr_format_phase3,
                  st->uUei3, 
                  st->iUei3 );
}

int prepare_data_string_last(char *s) {
  sensors_state_t4 *st = &sensors_state4;
  return sprintf(s, datastr_format_last,


                  st->powerFactor, 
                  st->activeWatts);
}



int prepare_data_check_internet(char *s) {

  return sprintf(s, datastr_format_check_internet,
              API_token1,
              iM_pulse_ID);
}


//
//void tcptest_close_restart(struct tcp_pcb *tpcb)
//{
//   
//  //  NRF_LOG_INFO(" CLOSE RESTART");
//    uint16_t timeout;
//    if (tpcb != NULL)
//    {
//      tcp_close(tpcb);
//    }   
//    sys_untimeout(tcptest_write, tpcb);
//    sys_untimeout(tcptest_start, NULL);  
//    timeout = 40000;
//    sys_timeout(timeout, tcptest_start, NULL);
//}
//
//
//uint32_t tcptest_generate_request(uint8_t *txbuff)
//{
////  uint16_t i = 0;
////  uint16_t i1 = 0;
////  uint16_t i2 = 0;
////  uint16_t i3 = 0;
////  uint16_t i4 = 0;
////  uint16_t i5 = 0;
////  uint16_t i6 = 0;
////  uint16_t i7 = 0;
////  uint16_t i8 = 0;
////  uint16_t i9 = 0;
////  uint16_t i10 = 0;
////  uint16_t j = 0;
////  uint16_t timeout;
////  static uint8_t http_data_req[200];
//  static uint8_t http_data_req_check_internet[200];
////  txbuff[0] = 0;
//
////  sprintf(http_data_req,"POST  /Pulse/Data HTTP/1.1\n"
////                               "Host:  %s\n"
////                                "Content-Type: application/json\n"
////                                 "Content-Length: %%i\n"
////                                "\n", "beta.imanifold.com");
//
//  sprintf(http_data_req_check_internet,"POST /Pulse/checkPulseConnection HTTP/1.1\n"
//                                 "Host: %s\n"
//                                 "Content-Type: application/json\n"
//                                "Content-Length: %%i\n"
//                                "\n", API_link1);
//
//  volatile err_t err = ERR_OK;
//
// if((API_token1[0] != 0xFF) && (API_link1[0] != 0xFF))  {
//
//
////if( strcmp(config_string1,"config[circuit:1,phase:1,voltage:120,frequen:60]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_phase1(buff_phase1);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_last);
////      }
////
////else if( strcmp(config_string1,"config[circuit:1,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:1,voltage:240,frequen:50]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_phase1(buff_phase1);
////      i2 = prepare_data_string_phase2(buff_phase2);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:1,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:3,voltage:460,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_phase1(buff_phase1);
////      i2 = prepare_data_string_phase2(buff_phase2);
////      i3 = prepare_data_string_phase3(buff_phase3);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_phase3);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:2,phase:1,voltage:120,frequen:60]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_phase1(buff_phase1);
////      i3 = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:2,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:1,voltage:240,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_phase1(buff_phase1);
////      i3 = prepare_data_string_phase2(buff_phase2);
////      i4 = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:2,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:3,voltage:460,frequen:60]") == 0|| strcmp(config_string1,"config[circuit:2,phase:3,voltage:460,frequen:50]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_phase1(buff_phase1);
////      i3 = prepare_data_string_phase2(buff_phase2);
////      i4 = prepare_data_string_phase3(buff_phase3);
////      i5 = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_phase3);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:3,phase:1,voltage:120,frequen:60]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_phase1(buff_phase1);
////      i4 = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_last);
////
////      }
////
////  else if( strcmp(config_string1,"config[circuit:3,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:1,voltage:240,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_phase1(buff_phase1);
////      i4 = prepare_data_string_phase2(buff_phase2);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_last);
////
////      }
////
////   else if( strcmp(config_string1,"config[circuit:3,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:3,voltage:460,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_phase1(buff_phase1);
////      i4 = prepare_data_string_phase2(buff_phase2);
////      i5 = prepare_data_string_phase3(buff_phase3);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_phase3);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:4,phase:1,voltage:120,frequen:60]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_circuit4(buff_circuit4);
////      i4 = prepare_data_string_phase1(buff_phase1);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_circuit4);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_last);
//// 
////      }
////
////else if( strcmp(config_string1,"config[circuit:4,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:1,voltage:240,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_circuit4(buff_circuit4);
////      i4 = prepare_data_string_phase1(buff_phase1);
////      i5 = prepare_data_string_phase2(buff_phase2);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_circuit4);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_last);
////
////      }
////
////   else if( strcmp(config_string1,"config[circuit:4,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:3,voltage:460,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_circuit4(buff_circuit4);
////      i4 = prepare_data_string_phase1(buff_phase1);
////      i5 = prepare_data_string_phase2(buff_phase2);
////      i6 = prepare_data_string_phase3(buff_phase3);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5+i6+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_circuit4);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_phase3);
////      strcat(txbuff, buff_last);
////
////      }
////   else
////   {
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_circuit4(buff_circuit4);
////      i4 = prepare_data_string_phase1(buff_phase1);
////      i5 = prepare_data_string_phase2(buff_phase2);
////      i6 = prepare_data_string_phase3(buff_phase3);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5+i6+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_circuit4);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_phase3);
////      strcat(txbuff, buff_last);    
////   }
//   }
//
//   return strlen(txbuff);
//  
//}
//
////periodic test task
//void tcptest_write(struct tcp_pcb *tpcb) {
//
// NRF_LOG_INFO(" WRITE DATA");
//  uint16_t i = 0;
////  uint16_t i1 = 0;
////  uint16_t i2 = 0;
////  uint16_t i3 = 0;
////  uint16_t i4 = 0;
////  uint16_t i5 = 0;
////  uint16_t i6 = 0;
////  uint16_t i7 = 0;
////  uint16_t i8 = 0;
////  uint16_t i9 = 0;
////  uint16_t i10 = 0;
////  uint16_t j = 0;
//  uint16_t timeout;
////  
////  static uint8_t http_data_req[200];
//  static uint8_t http_data_req_check_internet[200];
////
////  sprintf(http_data_req,"POST  /Pulse/Data HTTP/1.1\n"
////                               "Host:  %s\n"
////                                "Content-Type: application/json\n"
////                                 "Content-Length: %%i\n"
////                                "\n", API_link1);
//
// 
////  sprintf(http_data_req_check_internet,"POST /Pulse/checkPulseConnection HTTP/1.1\n"
////                                 "Host: %s\n"
////                                 "Content-Type: application/json\n"
////                                "Content-Length: %%i\n"
////                                "\n", API_link1);
//
//  sprintf(http_data_req_check_internet,"POST /Pulse/checkPulseConnection HTTP/1.1\n"
//                                 "Host: %s\n"
//                                 "Content-Type: application/json\n"
//                                "Content-Length: %%i\n"
//                                "\n", API_link1);
//
//  volatile err_t err = ERR_OK;
//  if (tpcb != NULL) {
//
// // if((strncmp(API_token1,"token",5) == 0) && strncmp(API_link1,"link",4) == 0) {
// if((API_token1[0] != 0xFF) && (API_link1[0] != 0xFF))  {
//    if (tpcb->state == ESTABLISHED) {
//
//
////      if(strcmp(check_Internet,"true")==0){
////   
////      
////      i = prepare_data_check_internet(buff_check_internet);
////      sprintf(txbuff, http_data_req_check_internet, i);
////      strcat(txbuff, buff_check_internet);
////      err = tcp_write(tpcb, txbuff, strlen(txbuff), TCP_WRITE_FLAG_COPY);
////      tcp_output(tpcb);
////     
////  
////  }
//
//  //else{
//
////if( strcmp(config_string1,"config[circuit:1,phase:1,voltage:120,frequen:60]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_phase1(buff_phase1);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_last);
////      }
////
////else if( strcmp(config_string1,"config[circuit:1,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:1,voltage:240,frequen:50]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_phase1(buff_phase1);
////      i2 = prepare_data_string_phase2(buff_phase2);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:1,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:1,phase:3,voltage:460,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_phase1(buff_phase1);
////      i2 = prepare_data_string_phase2(buff_phase2);
////      i3 = prepare_data_string_phase3(buff_phase3);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_phase3);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:2,phase:1,voltage:120,frequen:60]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_phase1(buff_phase1);
////      i3 = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:2,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:1,voltage:240,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_phase1(buff_phase1);
////      i3 = prepare_data_string_phase2(buff_phase2);
////      i4 = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:2,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:2,phase:3,voltage:460,frequen:60]") == 0|| strcmp(config_string1,"config[circuit:2,phase:3,voltage:460,frequen:50]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_phase1(buff_phase1);
////      i3 = prepare_data_string_phase2(buff_phase2);
////      i4 = prepare_data_string_phase3(buff_phase3);
////      i5 = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_phase3);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:3,phase:1,voltage:120,frequen:60]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_phase1(buff_phase1);
////      i4 = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_last);
////
////      }
////
////  else if( strcmp(config_string1,"config[circuit:3,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:1,voltage:240,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_phase1(buff_phase1);
////      i4 = prepare_data_string_phase2(buff_phase2);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_last);
////
////      }
////
////   else if( strcmp(config_string1,"config[circuit:3,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:3,phase:3,voltage:460,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_phase1(buff_phase1);
////      i4 = prepare_data_string_phase2(buff_phase2);
////      i5 = prepare_data_string_phase3(buff_phase3);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_phase3);
////      strcat(txbuff, buff_last);
////
////      }
////
////else if( strcmp(config_string1,"config[circuit:4,phase:1,voltage:120,frequen:60]") == 0){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_circuit4(buff_circuit4);
////      i4 = prepare_data_string_phase1(buff_phase1);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_circuit4);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_last);
//// 
////      }
////
////else if( strcmp(config_string1,"config[circuit:4,phase:1,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:1,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:1,voltage:240,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_circuit4(buff_circuit4);
////      i4 = prepare_data_string_phase1(buff_phase1);
////      i5 = prepare_data_string_phase2(buff_phase2);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_circuit4);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_last);
////
////      }
////
////   else if( strcmp(config_string1,"config[circuit:4,phase:3,voltage:240,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:3,voltage:460,frequen:60]") == 0 || strcmp(config_string1,"config[circuit:4,phase:3,voltage:460,frequen:50]") == 0 ){
////
////      i = prepare_data_string_circuit1(buff_circuit1);
////      i1 = prepare_data_string_circuit2(buff_circuit2);
////      i2 = prepare_data_string_circuit3(buff_circuit3);
////      i3 = prepare_data_string_circuit4(buff_circuit4);
////      i4 = prepare_data_string_phase1(buff_phase1);
////      i5 = prepare_data_string_phase2(buff_phase2);
////      i6 = prepare_data_string_phase3(buff_phase3);
////      j = prepare_data_string_last(buff_last);
////      sprintf(txbuff, http_data_req, i+i1+i2+i3+i4+i5+i6+j);
////      strcat(txbuff, buff_circuit1);
////      strcat(txbuff, buff_circuit2);
////      strcat(txbuff, buff_circuit3);
////      strcat(txbuff, buff_circuit4);
////      strcat(txbuff, buff_phase1);
////      strcat(txbuff, buff_phase2);
////      strcat(txbuff, buff_phase3);
////      strcat(txbuff, buff_last);
////
////      }
//
//   // }
//
//      i = prepare_data_check_internet(buff_check_internet);
//      sprintf(txbuff, http_data_req_check_internet, i);
//      strcat(txbuff, buff_check_internet);
//      err = tcp_write(tpcb, txbuff, strlen(txbuff), TCP_WRITE_FLAG_COPY);
//      tcp_output(tpcb);
//
//          err = tcp_write(tpcb, txbuff, strlen(txbuff), TCP_WRITE_FLAG_COPY);
//          tcp_output(tpcb);
////              int n;
////    n = sprintf(str, "written\n");
////    ble_serial_send(str, n);
//    }
//   }
//  }
//  
//
//
//
// // timeout = 30000;
// // sys_untimeout(tcptest_write, tpcb);
// // sys_timeout(timeout, tcptest_write, tpcb);
//
//  timeout= 5000;
////NRF_LOG_INFO(" TIMEOUT STARTS");
//  //sys_untimeout(tcptest_write, tpcb);
//  sys_timeout(timeout, tcptest_close_restart, tpcb);
//
//}
//
////connected callback function
//err_t tcptest_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
//NRF_LOG_INFO("tcptest_connected"); 
//  volatile err_t e = ERR_OK;
//  tcptest_write(tpcb);
//  return e;
//}
//
////error callback function
//void tcptest_err(void *arg, err_t err) {
//NRF_LOG_INFO(" tcp error");
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
//  sys_untimeout(tcptest_write, pcb);
//  sys_untimeout(tcptest_start, NULL);
//  sys_timeout(2000, tcptest_start, NULL);
//}
//
////data sent callback function
//err_t testtcp_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
////NRF_LOG_INFO("testtcp_sent"); 
//  //bsp_board_led_invert(BSP_BOARD_LED_1);
//  return ERR_OK;
//}
//
////receive callback function
//err_t tcptest_recv(void *arg, volatile struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
//NRF_LOG_INFO("tcptest_recv"); 
//  err_t e = ERR_OK;
//  if (p == NULL) // remote host sent a FIN packet
//  {
//          tcptest_close_restart(tpcb);
//          e = ERR_ABRT;
//  } else {
//   memcpy(response_back, p+(0x04), 20);
//    if(response_back[15]==0x32 && response_back[16] == 0x30 && response_back[17]==0x30){
//
//    int n;
//            n = sprintf(str, "success internet\n");
//            ble_serial_send(str, n);
//            NRF_LOG_INFO("success internet");
//    }
//    else{
//        int n;
//            n = sprintf(str, "failed internet\n");
//            ble_serial_send(str, n);
//            NRF_LOG_INFO("failed internet");
//    
//    }
//
//    pbuf_free(p);
//
//  }
//  return e;
//}
//
//err_t tcp_test_poll(void *arg, struct tcp_pcb *tpcb) {   
//NRF_LOG_INFO("tcp_test_poll"); 
////              int n;
////    n = sprintf(str, "did not write data\n");
////    ble_serial_send(str, n);
//    tcptest_close_restart(tpcb);
//    return ERR_ABRT;
//}
//
//void tcp_test_connect(struct ip_addr *addr) {
//NRF_LOG_INFO("tcp_test_connect");
//  err_t err;
//  struct tcp_pcb *pcb;    
//    pcb = tcp_new();
//    if (pcb == NULL)
//    {
//      tcptest_close_restart(NULL);
//      return;
//    }
//    tcp_err(pcb, tcptest_err);
//    tcp_recv(pcb, tcptest_recv);
//    tcp_sent(pcb, testtcp_sent);
//    tcp_poll(pcb,tcp_test_poll,20);
//    tcp_arg(pcb, pcb);
//    
//  //  if(strcmp(check_Internet,"true")==0){
//     err = tcp_connect(pcb, addr,
//       80, tcptest_connected);  
// //  }
//
//  //  else{
//
//  //  err = tcp_connect(pcb, addr,
//      //  80, tcptest_connected);
//       // }
//
//    if (err!= ERR_OK)
//    {
//      tcptest_close_restart(pcb);
//      return;
//    }
// 
//}
//
//void tcp_test_dns_callback(const char *name, struct ip_addr *ipaddr, void *arg) {
//NRF_LOG_INFO("tcp_test_dns_callback");
//  if ((ipaddr) && (ipaddr->addr)) {
//    tcp_test_connect(ipaddr);
//  } else {
//    sys_untimeout(tcptest_start, NULL);
//    sys_timeout(5000, tcptest_start, NULL);
//  }
//}
//
////initializes the connection between uC and host
//err_t tcptest_start(void) {
//
//
//  // i added delay before connecting to remote server so lwip can get dynamic IP address
//  err_t err;
//  if (netif_default != NULL)
//  {
//NRF_LOG_INFO(" netif_default != NULL");
////  if(strcmp(check_Internet,"true")==0){
// //  err = dns_gethostbyname("beta.imanifold.com", &server_addr, tcp_test_dns_callback, NULL);
////  }
//
// // else{
//  //  err = dns_gethostbyname("beta.imanifold.com", &server_addr, tcp_test_dns_callback, NULL);
//  err = dns_gethostbyname(API_link1, &server_addr, tcp_test_dns_callback, NULL);
//    if (err == ERR_OK) {
//    NRF_LOG_INFO(" err == ERR_OK");
//      tcp_test_connect(&server_addr);
//    }
//  }else{
//  NRF_LOG_INFO(" else");
//    sys_untimeout(tcptest_start, NULL);
//    sys_timeout(3000, tcptest_start, NULL);
//  }
//  return err;
//}
//
//
//err_t tcptest_init(void) 
//{
//NRF_LOG_INFO("tcptest_init");
//  tcptest_start();
//}