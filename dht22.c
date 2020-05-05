//#include <lwip/pbuf.h>
//#include <lwip/raw.h>
//#include <lwip/tcp.h>
//#include <lwip/dns.h>
//#include <lwip/timers.h>
//#include <netif/etharp.h>
//
//#include "boards.h"
//#include "math.h"
//#include "tcptest.h"
//#include "dht22.h"
//#include "nrf_delay.h"
//
//
//
//
//void DHT_task_callback(int a) {
//
//  ret_code_t err_code;
//
// // NRF_LOG_INFO("get first data\n");
//  //printf("get first data\n");
//  DHT11_START_SEQUENCE(a);
//  DHT11_READ_5_BYTES_DATA(a);
//}
//
//void DHT11_START_SEQUENCE(int q) {
//  DHT11_DIR_OUTPUT(q);
//
//  nrf_gpio_pin_write(q, level_1);
//  //DHT11_ONE_WIRE_OUTPUT_PIN = 1;
//
//  nrf_gpio_pin_write(q, level_0);
//  vTaskDelay(25);
//  //ets_delay_us(22000);
//
//  nrf_gpio_pin_write(q, level_1);
//  //	Delay_micro_second(30);
//  nrf_delay_us(30);
//
//  DHT11_DIR_INPUT(q);
//
//  Count_micro_second = 0;
//  //while(DHT11_ONE_WIRE_INPUT_PIN==0)
//  while (nrf_gpio_pin_read(q) == 0) {
//
//    //	printf("waiting for make line high by dht11\n");
//    Count_micro_second++;
//    nrf_delay_us(1);
//
//    if (Count_micro_second > 100) {
//      break;
//    }
//  }
//
//  Count_micro_second = 0;
//
//  test_start_seq2 = Count_micro_second;
//
//  Count_micro_second = 0;
//  //	while(DHT11_ONE_WIRE_INPUT_PIN==1)
//  while (nrf_gpio_pin_read(q) == 1) {
//    //printf("waiting for make line low by dht11\n");
//    Count_micro_second++;
//    nrf_delay_us(1);
//
//    if (Count_micro_second > 100) {
//      break;
//    }
//  }
//
//  Count_micro_second = 0;
//
//  test_start_seq3 = Count_micro_second;
//}
//
//unsigned char DHT11_READ_1_BYTES_DATA(int h) {
//
//  unsigned int i = 0;
//  unsigned char Return_Value = 0;
//  Count_micro_second = 0;
//
//  for (i = 0; i < 8; i++) {
//    Return_Value = Return_Value << 1;
//
//    while ((nrf_gpio_pin_read(h)) == 0) {
//      Count_micro_second++;
//      nrf_delay_us(5);
//
//      if (Count_micro_second > 100) {
//        break;
//      }
//    }
//    Count_micro_second = 0;
//
//    while ((nrf_gpio_pin_read(h)) == 1) {
//      Count_micro_second++;
//      nrf_delay_us(5);
//      if (Count_micro_second > 100) {
//        break;
//      }
//    }
//
//    if (Count_micro_second < 7) {
//
//      Return_Value = Return_Value | 0;
//    } else {
//      Return_Value = Return_Value | 1;
//    }
//  }
//  //		printf("return_value=%c\n",Return_Value);
//
//  return Return_Value;
//}
//
//void DHT11_DIR_INPUT(int f) {
//  nrf_gpio_cfg_input(f, NRF_GPIO_PIN_PULLUP);
//}
//
//void DHT11_DIR_OUTPUT(int g) {
//  nrf_gpio_cfg_output(g);
//  //DHT11_ONE_WIRE_DIR = 1;
//}
//
//char DHT11_READ_5_BYTES_DATA(int u) {
//
//  Humidity_Int = DHT11_READ_1_BYTES_DATA(u);
//  Humidity_DEC = DHT11_READ_1_BYTES_DATA(u);
//  TEMPERATURE_Int = DHT11_READ_1_BYTES_DATA(u);
//  TEMPERATURE_DEC = DHT11_READ_1_BYTES_DATA(u);
//  CHECKSUM = DHT11_READ_1_BYTES_DATA(u);
//  //				printf("datas = %c %c\n",Humidity_Int,TEMPERATURE_Int);
//
//  if (((Humidity_Int + Humidity_DEC + TEMPERATURE_Int + TEMPERATURE_DEC) & 0xff) != CHECKSUM) {
//
// //   NRF_LOG_INFO("got wrong values\n");
//
// //if(u == 34){
// 
// //++wrong_rarh;
//
// //if(wrong_rarh == 20){
// //sensors_state1.rarh10 = 0;
// // sensors_state1.radb10 = 0;
// //  sensors_state2.rarh10 = 0;
// // sensors_state2.radb10 = 0;
// //  sensors_state3.rarh10 = 0;
// // sensors_state3.radb10 = 0;
// //  sensors_state4.rarh10 = 0;
// // sensors_state4.radb10 = 0;
////wrong_rarh = 0;
//// return 1;
//// 
// //}
//
// 
// //}
//
// // else if(u == 35){
// 
// //++wrong_sarh;
//
//// if(wrong_sarh == 20){
//// sensors_state1.sarh10 = 0;
//// sensors_state1.sadb10 = 0;
////  sensors_state2.sarh10 = 0;
//// sensors_state2.sadb10 = 0;
////  sensors_state3.sarh10 = 0;
//// sensors_state3.sadb10 = 0;
// // sensors_state4.sarh10 = 0;
// //sensors_state4.sadb10 = 0;
// //wrong_sarh = 0;
// //return 1;
// 
// //}
//
// 
// //}
//
//   //else if(u == 36){
// 
// //++wrong_oarh;
//
// //if(wrong_oarh == 20){
//// sensors_state1.oarh10 = 0;
//// sensors_state1.oadb10 = 0;
////  sensors_state2.oarh10 = 0;
// //sensors_state2.oadb10 = 0;
// // sensors_state3.oarh10 = 0;
// //sensors_state3.oadb10 = 0;
// // sensors_state4.oarh10 = 0;
// //sensors_state4.oadb10 = 0;
//// wrong_oarh = 0;
// return 1;
// 
// //}
//
// 
//// }
//    
//  } 
//  
//  else {
//  //  NRF_LOG_INFO("got values\n");
//    uint16_t humidity_bin = (Humidity_Int << 8) | (Humidity_DEC);
//    uint16_t temp_bin = (TEMPERATURE_Int << 8) | (TEMPERATURE_DEC);
//    //printf("got values\n");
//
//    //printf("Humidity1 %d",humidity_bin );
//    //printf("Temperature1 %d", temp_bin );
//    humi[0] = (humidity_bin & 0xFF00) >> 8;
//    humi[1] = (humidity_bin & 0x00FF);
//
//    temprature[0] = (temp_bin & 0xFF00) >> 8;
//    temprature[1] = (temp_bin & 0x00FF);
//
//   // NRF_LOG_INFO("value5: %d     ", value[5]);
//
//    float num1 = (humi[0] << 8) | (humi[1]);
//    float num2 = (temprature[0] << 8) | (temprature[1]);
//
//    //uint16_t res = humi[0] + humi[1];
//    //sensors_state.rarh10= *(humi) ;
//    //  float result = convert_hex_to_dec(humi);
//
//    if (u == 34) {
//
//    if( isnan(num1) == 0 && (num1 <1000) ){
//      sensors_state1.rarh10 = num1/10;
//      sensors_state2.rarh10 = num1/10;
//      sensors_state3.rarh10 = num1/10;
//      sensors_state4.rarh10 = num1/10;
//
//    //  sensors_state1.rarh10 = (sensors_state1.rarh10 / 10);
//    //  sensors_state2.rarh10 = (sensors_state2.rarh10 / 10);
//   //   sensors_state3.rarh10 = (sensors_state3.rarh10 / 10);
//   //   sensors_state4.rarh10 = (sensors_state4.rarh10 / 10);
//    //   NRF_LOG_INFO("rarah: %d", sensors_state1.rarh10);
//
//      }
//    
//
//      if( isnan(num2) == 0 ){
//      sensors_state1.radb10 = num2/10;
//      sensors_state2.radb10 = num2/10;
//      sensors_state3.radb10 = num2/10;
//      sensors_state4.radb10 = num2/10;
//
//   //   sensors_state1.radb10 = (sensors_state1.radb10 / 10);
//   //   sensors_state2.radb10 = (sensors_state2.radb10 / 10);
//   //   sensors_state3.radb10 = (sensors_state3.radb10 / 10);
//   //   sensors_state4.radb10 = (sensors_state4.radb10 / 10);
//
//      }
//    
//    }
//
//    else if (u == 35) {
//
//    if( isnan(num1) == 0 && (num1 <1000) ){
//      sensors_state1.sarh10 = num1/10;
//      sensors_state2.sarh10 = num1/10;
//      sensors_state3.sarh10 = num1/10;
//      sensors_state4.sarh10 = num1/10;
//
//     // sensors_state1.sarh10 = (sensors_state1.sarh10 / 10);
//    //  sensors_state2.sarh10 = (sensors_state2.sarh10 / 10);
//     // sensors_state3.sarh10 = (sensors_state3.sarh10 / 10);
//    //  sensors_state4.sarh10 = (sensors_state4.sarh10 / 10);
//    //  NRF_LOG_INFO("sarh: %d", sensors_state1.sarh10);
//
//      }
//
//      
//      if( isnan(num2) == 0 ){
//      sensors_state1.sadb10 = num2/10;
//      sensors_state2.sadb10 = num2/10;
//      sensors_state3.sadb10 = num2/10;
//      sensors_state4.sadb10 = num2/10;
//
//     // sensors_state1.sadb10 = (sensors_state1.sadb10 / 10);
//     // sensors_state2.sadb10 = (sensors_state2.sadb10 / 10);
//     // sensors_state3.sadb10 = (sensors_state3.sadb10 / 10);
//     // sensors_state4.sadb10 = (sensors_state4.sadb10 / 10);
//
//}    
//      
//    }
//
//    else if (u == 36) {
//    if( isnan(num1) == 0 && (num1 <1000)){
//      sensors_state1.oarh10 = num1/10;
//      sensors_state2.oarh10 = num1/10;
//      sensors_state3.oarh10 = num1/10;
//      sensors_state4.oarh10 = num1/10;
//
//    //  sensors_state1.oarh10 = (sensors_state1.oarh10 / 10);
//    //  sensors_state2.oarh10 = (sensors_state2.oarh10 / 10);
//    //  sensors_state3.oarh10 = (sensors_state3.oarh10 / 10);
//    //  sensors_state4.oarh10 = (sensors_state4.oarh10 / 10);
//    //  NRF_LOG_INFO("oarah: %d", sensors_state1.oarh10);
//      }
//
//      
//
//      if( isnan(num2) == 0 ){
//      sensors_state1.oadb10 = num2/10;
//      sensors_state2.oadb10 = num2/10;
//      sensors_state3.oadb10 = num2/10;
//      sensors_state4.oadb10 = num2/10;
//
//    //  sensors_state1.oadb10 = (sensors_state1.oadb10 / 10);
//    //  sensors_state2.oadb10 = (sensors_state2.oadb10 / 10);
//    //  sensors_state3.oadb10 = (sensors_state3.oadb10 / 10);
//    //  sensors_state4.oadb10 = (sensors_state4.oadb10 / 10);
//   }
//      
//    }
//
//    // printf("Humidity %d",Humidity_Int + Humidity_DEC );
//    // printf("Temperature %d",TEMPERATURE_Int + TEMPERATURE_DEC );
//    return err;
//  }
//}
//
//void Convert_Humidity_Temperature_Values(unsigned char byte1) {
//
//  unsigned char intermediate_value1 = 0; //Varible required while converting the hex value to decimal value
//                                         /*******************************************************************************
//				Procedure to convert 8bit hex value to equivalent BCD value
//				*******************************************************************************/
//  decimal_value3 = (byte1 / 100);
//  intermediate_value1 = byte1 - (100 * decimal_value3);
//  decimal_value2 = (intermediate_value1 / 10);
//  decimal_value1 = intermediate_value1 - (10 * decimal_value2);
//}
//
