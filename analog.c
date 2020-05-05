#include "nrf_drv_ppi.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_timer.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "analog.h"
#include <lwip/pbuf.h>
#include <lwip/raw.h>
#include <lwip/tcp.h>
#include <lwip/dns.h>
#include <lwip/timers.h>
#include <netif/etharp.h>
#include "boards.h"
#include "math.h"
#include "tcptest.h"





void timer_handler(nrf_timer_event_t event_type, void *p_context) {
 }

void saadc_sampling_event_init(void) {
  ret_code_t err_code;

  err_code = nrf_drv_ppi_init();
  APP_ERROR_CHECK(err_code);

  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
  timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
  err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handler);
  APP_ERROR_CHECK(err_code);

  /* setup m_timer for compare event every 400ms */
  uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 400);
  nrf_drv_timer_extended_compare(&m_timer,
      NRF_TIMER_CC_CHANNEL0,
      ticks,
      NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
      false);
  nrf_drv_timer_enable(&m_timer);

  uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer,
      NRF_TIMER_CC_CHANNEL0);
  uint32_t saadc_sample_task_addr = nrf_drv_saadc_sample_task_get();

  /* setup ppi channel so that timer compare event is triggering sample task in SAADC */
  err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_assign(m_ppi_channel,
      timer_compare_event_addr,
      saadc_sample_task_addr);
  APP_ERROR_CHECK(err_code);
}

void saadc_sampling_event_enable(void) {
  ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);

  APP_ERROR_CHECK(err_code);
}

void saadc_callback(nrf_drv_saadc_evt_t const *p_event) {
  if (p_event->type == NRF_DRV_SAADC_EVT_DONE) {
    //ret_code_t err_code;
    float adc_value[8];
    //float T, T1, T2;
    int i;
    //loop for all the analog sesnor channels
    for (i = 0; i < SAADC_SAMPLES_IN_BUFFER; i++) {
      adc_value[i] = p_event->data.done.p_buffer[i];
    }
    //calculation: 
    P1 = (adc_value[0]) / 1252.59;
    P1 = (P1 - 0.8) * 10;
    P1 = P1*14.5038 + 2.2;

    if (isnan(P1) == 0) {
      sensors_state1.spc11 = P1;   //Ckt 1 Suction Pressure1
      sensors_state2.spc11 = P1;   //Ckt 2 Suction Pressure1
      sensors_state3.spc11 = P1;   //Ckt 3 Suction Pressure1
      sensors_state4.spc11 = P1;   //Ckt 4 Suction Pressure1
    }
   
    P2 = (adc_value[1]) / 1252.59;
    P2 = (P2 - 0.8) * 20;
    P2 = P2*14.5038 + 3.7;

    if (isnan(P2) == 0) {
      sensors_state1.llpc11 = P2;  //Ckt 1 Liquid Pressure1
      sensors_state2.llpc11 = P2;  //Ckt 2 Liquid Pressure1
      sensors_state3.llpc11 = P2;  //Ckt 3 Liquid Pressure1
      sensors_state4.llpc11 = P2;  //Ckt 4 Liquid Pressure1
    }


    P3 = (adc_value[2]) / 1252.59;
    P3 = (P3 - 0.8) * 10;
    P3 = P3*14.5038 + 2.2;

    if (isnan(P3) == 0) {
      sensors_state2.spc21 = P3;  //Ckt 2 Suction Pressure2
      sensors_state3.spc21 = P3;  //Ckt 3 Suction Pressure2
      sensors_state4.spc21 = P3;  //Ckt 4 Suction Pressure2
    }


    P4 = (adc_value[3]) / 1252.59;
    P4 = (P4 - 0.8) * 20;
    P4 = P4*14.5038 + 3.7;

    if (isnan(P4) == 0) {
      sensors_state2.llpc21 = P4; //Ckt 2 Liquid Pressure2
      sensors_state3.llpc21 = P4; //Ckt 3 Liquid Pressure2 
      sensors_state4.llpc21 = P4; //Ckt 4 Liquid Pressure2
    }

    P5 = (adc_value[4]) / 1252.59;
    P5 = (P5 - 0.8) * 10;
    P5 = P5*14.5038 + 2.2;

    if (isnan(P5) == 0) {
      sensors_state3.spc31 = P5; //Ckt 3 Suction Pressure3
      sensors_state4.spc31 = P5; //Ckt 4 Suction Pressure3
    }

    P6 = (adc_value[5]) / 1252.59;
    P6 = (P6 - 0.8) * 20;
    P6 = P6*14.5038 + 3.7;

    if (isnan(P6) == 0) {
      sensors_state3.llpc31 = P6;  //Ckt 3 Liquid Pressure3
      sensors_state4.llpc31 = P6;  //Ckt 4 Liquid Pressure3
    }

    P7 = (adc_value[6]) / 1252.59;
    P7 = (P7 - 0.8) * 10;
    P7 = P7*14.5038 + 2.2;

    if (isnan(P7) == 0) {
        sensors_state4.spc41 = P7;  //Ckt 4 Suction Pressure4
    }

    P8 = (adc_value[7]) / 1252.59;
    P8 = (P8 - 0.8) * 20;
    P8 = P8*14.5038 + 3.7;

    if (isnan(P8) == 0) {
      sensors_state4.llpc41 = P8;  //Ckt 4 Liquid Pressure4

    }

  }
}

void saadc_init(void) {

  ret_code_t err_code;
  nrf_saadc_channel_config_t channel_1_config =
    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN3);
  nrf_saadc_channel_config_t channel_2_config =
    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN2);
  nrf_saadc_channel_config_t channel_3_config =
    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN7);
  nrf_saadc_channel_config_t channel_4_config =
    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN6);
  nrf_saadc_channel_config_t channel_5_config =
    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN5);
  nrf_saadc_channel_config_t channel_6_config =
    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN4);
  nrf_saadc_channel_config_t channel_7_config =
    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);
  nrf_saadc_channel_config_t channel_8_config =
    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN1);

  err_code = nrf_drv_saadc_init(NULL, saadc_callback);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_saadc_channel_init(0, &channel_1_config);
    APP_ERROR_CHECK(err_code);
  err_code = nrf_drv_saadc_channel_init(1, &channel_2_config);
    APP_ERROR_CHECK(err_code);
  err_code = nrf_drv_saadc_channel_init(2, &channel_3_config);
    APP_ERROR_CHECK(err_code);
  err_code = nrf_drv_saadc_channel_init(3, &channel_4_config);
    APP_ERROR_CHECK(err_code);
  err_code = nrf_drv_saadc_channel_init(4, &channel_5_config);
    APP_ERROR_CHECK(err_code);
  err_code = nrf_drv_saadc_channel_init(5, &channel_6_config);
    APP_ERROR_CHECK(err_code);
  err_code = nrf_drv_saadc_channel_init(6, &channel_7_config);
    APP_ERROR_CHECK(err_code);
  err_code = nrf_drv_saadc_channel_init(7, &channel_8_config);
    APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
  APP_ERROR_CHECK(err_code);

 }