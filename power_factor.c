#include <lwip/pbuf.h>
#include <lwip/raw.h>
#include <lwip/tcp.h>
#include <lwip/dns.h>
#include <lwip/timers.h>
#include <netif/etharp.h>

#include "boards.h"
#include "math.h"
#include "nrf_delay.h"
#include "power_factor.h"
#include "nrfx_spim.h"
#include "tcptest.h"
#include "nrf_log.h"

static const nrfx_spim_t spi = NRFX_SPIM_INSTANCE(PF_SPI_INSTANCE); /**< SPI instance 3 */

void energy_measurement() {

  volatile ret_code_t ret;
  nrfx_spim_xfer_desc_t spim_xfer = NRFX_SPIM_SINGLE_XFER(&config_start, m_length, m_rx_buf, m_length);

  spim_xfer.p_tx_buffer = &config_start;  //config address of chip
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0)); //sending required config to chip
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length); //setting rx buff to 0

  //send these 4 commands as part of configuration 
  spim_xfer.p_tx_buffer = &cal_start;  
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);

  spim_xfer.p_tx_buffer = &adj_start;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);

  spim_xfer.p_tx_buffer = &harm_start;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);

//Setting up mode for chip(Single phase/Three phase)
if(strcmp(config_string1[23],0x31) == 0 ){
  spim_xfer.p_tx_buffer = &mmode0_single;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);
}

else{
  spim_xfer.p_tx_buffer = &mmode0;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);

}
//setting up volatge 1 gain
  spim_xfer.p_tx_buffer = &vgainA;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);
//setting up volatge 3 gain
  spim_xfer.p_tx_buffer = &vgainC;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);
//setting up current 1 gain
  spim_xfer.p_tx_buffer = &igainA_new;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);
//setting up current 2 gain
  spim_xfer.p_tx_buffer = &igainB_new;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);
//setting up current 3 gain
  spim_xfer.p_tx_buffer = &igainC_new;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);
//Receive volatge 1 value
  spim_xfer.p_tx_buffer = &urmsA;
  spim_xfer.p_rx_buffer = m_rx_buf2;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  float num6 = (m_rx_buf2[2] << 8) | (m_rx_buf2[3]);
  sensors_state1.uUei1 = num6 / 100;
  sensors_state2.uUei1 = num6 / 100;
  sensors_state3.uUei1 = num6 / 100;
  sensors_state4.uUei1 = num6 / 100;
  memset(m_rx_buf2, 0, m_length);
//Receive volatge 2 value
  spim_xfer.p_tx_buffer = &urmsB;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  float num14 = (m_rx_buf[2] << 8) | (m_rx_buf[3]);
  sensors_state1.uUei2 = num14 / 100;
  sensors_state2.uUei2 = num14 / 100;
  sensors_state3.uUei2 = num14 / 100;
  sensors_state4.uUei2 = num14 / 100;
  memset(m_rx_buf, 0, m_length);
//Receive volatge 3 value
  spim_xfer.p_tx_buffer = &urmsC;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  float num7 = (m_rx_buf[2] << 8) | (m_rx_buf[3]);
  sensors_state1.uUei3 = num7 / 100;
  sensors_state2.uUei3 = num7 / 100;
  sensors_state3.uUei3 = num7 / 100;
  sensors_state4.uUei3 = num7 / 100;
  memset(m_rx_buf, 0, m_length);
//Receive current 1 value
  spim_xfer.p_tx_buffer = &irmsA;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  float num8 = (m_rx_buf[2] << 8) | (m_rx_buf[3]);
  sensors_state1.iUei1 = (num8 / 1000) * 3;
  sensors_state2.iUei1 = (num8 / 1000) * 3;
  sensors_state3.iUei1 = (num8 / 1000) * 3;
  sensors_state4.iUei1 = (num8 / 1000) * 3;
  memset(m_rx_buf, 0, m_length);
//Receive current 2 value
  spim_xfer.p_tx_buffer = &irmsB;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
 vTaskDelay(20);
 float num15 = (m_rx_buf[2] << 8) | (m_rx_buf[3]);
  sensors_state1.iUei2 = (num15 / 1000) * 3;
  sensors_state2.iUei2 = (num15 / 1000) * 3;
  sensors_state3.iUei2 = (num15 / 1000) * 3;
  sensors_state4.iUei2 = (num15 / 1000) * 3;
  memset(m_rx_buf, 0, m_length);
//Receive current 3 value
  spim_xfer.p_tx_buffer = &irmsC;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  float num9 = (m_rx_buf[2] << 8) | (m_rx_buf[3]);
  sensors_state1.iUei3 = (num9 / 1000) * 3;
  sensors_state2.iUei3 = (num9 / 1000) * 3;
  sensors_state3.iUei3 = (num9 / 1000) * 3;
  sensors_state4.iUei3 = (num9 / 1000) * 3;
  memset(m_rx_buf, 0, m_length);
//Receive power factor value
  spim_xfer.p_tx_buffer = &pf;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  float num10 = (m_rx_buf[2] << 8) | (m_rx_buf[3]);
  sensors_state1.powerFactor = num10 / 1000;
  sensors_state2.powerFactor = num10 / 1000;
  sensors_state3.powerFactor = num10 / 1000;
  sensors_state4.powerFactor = num10 / 1000;
  memset(m_rx_buf, 0, m_length);
//Receive  watts
  spim_xfer.p_tx_buffer = &active_power;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  float num13 = (m_rx_buf[2] << 8) | (m_rx_buf[3]);
  sensors_state1.activeWatts = num13 * 4 * 3;
  sensors_state2.activeWatts = num13 * 4 * 3;
  sensors_state3.activeWatts = num13 * 4 * 3;
  sensors_state4.activeWatts = num13 * 4 * 3;
  memset(m_rx_buf, 0, m_length);
//Setting offset value of current
  spim_xfer.p_tx_buffer = &IoffsetN;
  spim_xfer.p_rx_buffer = m_rx_buf;
  APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &spim_xfer, 0));
  vTaskDelay(20);
  memset(m_rx_buf, 0, m_length);

}

void pf_spim_init(void) {
  nrfx_spim_config_t spi_config = NRFX_SPIM_DEFAULT_CONFIG;
  spi_config.frequency = NRF_SPIM_FREQ_500K;
  spi_config.ss_pin = IPF_CS;
  spi_config.miso_pin = IPF_SDO;
  spi_config.mosi_pin = IPF_SI;
  spi_config.sck_pin = IPF_SCK;
  APP_ERROR_CHECK(nrfx_spim_init(&spi, &spi_config, NULL, NULL));
}


init_keys(){

//voltage1
sensors_state1.key0 = 0;
sensors_state2.key0 = 0;
sensors_state3.key0 = 0;
sensors_state4.key0 = 0;
//voltage2
sensors_state1.key1 = 1;
sensors_state2.key1 = 1;
sensors_state3.key1 = 1;
sensors_state4.key1 = 1;
//voltage3
sensors_state1.key2 = 2;
sensors_state2.key2 = 2;
sensors_state3.key2 = 2;
sensors_state4.key2 = 2;
//current1
sensors_state1.key3 = 3;
sensors_state2.key3 = 3;
sensors_state3.key3 = 3;
sensors_state4.key3 = 3;
//current2
sensors_state1.key4 = 4;
sensors_state2.key4 = 4;
sensors_state3.key4 = 4;
sensors_state4.key4 = 4;
//current3
sensors_state1.key5 = 5;
sensors_state2.key5 = 5;
sensors_state3.key5 = 5;
sensors_state4.key5 = 5;
//power factor
sensors_state1.key6 = 6;
sensors_state2.key6 = 6;
sensors_state3.key6 = 6;
sensors_state4.key6 = 6;
//power
sensors_state1.key7 = 7;
sensors_state2.key7 = 7;
sensors_state3.key7 = 7;
sensors_state4.key7 = 7;
//oarh
sensors_state1.key8 = 8;
sensors_state2.key8 = 8;
sensors_state3.key8 = 8;
sensors_state4.key8 = 8;
//oadb
sensors_state1.key9 = 9;
sensors_state2.key9 = 9;
sensors_state3.key9 = 9;
sensors_state4.key9 = 9;
//sarh
sensors_state1.key10 = 10;
sensors_state2.key10 = 10;
sensors_state3.key10 = 10;
sensors_state4.key10 = 10;
//sadb
sensors_state1.key11 = 11;
sensors_state2.key11 = 11;
sensors_state3.key11 = 11;
sensors_state4.key11 = 11;
//rarh
sensors_state1.key12 = 12;
sensors_state2.key12 = 12;
sensors_state3.key12 = 12;
sensors_state4.key12 = 12;
//radb
sensors_state1.key13 = 13;
sensors_state2.key13 = 13;
sensors_state3.key13 = 13;
sensors_state4.key13 = 13;
//sp1
sensors_state1.key14 = 14;
sensors_state2.key14 = 14;
sensors_state3.key14 = 14;
sensors_state4.key14 = 14;
//hp1
sensors_state1.key15 = 15;
sensors_state2.key15 = 15;
sensors_state3.key15 = 15;
sensors_state4.key15 = 15;
//slt1
sensors_state1.key16 = 16;
sensors_state2.key16 = 16;
sensors_state3.key16 = 16;
sensors_state4.key16 = 16;
//llt1
sensors_state1.key17 = 17;
sensors_state2.key17 = 17;
sensors_state3.key17 = 17;
sensors_state4.key17 = 17;
//sp2
//sensors_state1.key19 = 19;
sensors_state2.key19 = 19;
sensors_state3.key19 = 19;
sensors_state4.key19 = 19;
//hp2
sensors_state2.key20 = 20;
sensors_state3.key20 = 20;
sensors_state4.key20 = 20;
//slt2
sensors_state2.key21 = 21;
sensors_state3.key21 = 21;
sensors_state4.key21 = 21;
//llt2
sensors_state2.key22 = 22;
sensors_state3.key22 = 22;
sensors_state4.key22 = 22;
//sp3
//sensors_state2.key24 = 24;
sensors_state3.key24 = 24;
sensors_state4.key24 = 24;
//hp3
sensors_state3.key25 = 25;
sensors_state4.key25 = 25;
//slt3
sensors_state3.key26 = 26;
sensors_state4.key26 = 26;
//llt3
//sensors_state1.key27 = 27;
//sensors_state2.key27 = 27;
sensors_state3.key27 = 27;
sensors_state4.key27 = 27;
//sp4
sensors_state4.key29 = 29;
//hp4
sensors_state4.key30 = 30;
//slt4
sensors_state4.key31 = 31;
//llt4
sensors_state4.key32 = 32;
//t_stat_ckt1
sensors_state1.key34 = 34;
sensors_state1.key35 = 35;
sensors_state1.key36 = 36;
//t_stat_ckt2
sensors_state2.key34 = 34;
sensors_state2.key35 = 35;
sensors_state2.key36 = 36;
//t_stat_ckt3
sensors_state3.key34 = 34;
sensors_state3.key35 = 35;
sensors_state3.key36 = 36;
//t_stat_ckt4
sensors_state4.key34 = 34;
sensors_state4.key35 = 35;
sensors_state4.key36 = 36;

}


//thermostat signals 

thermostat_signals(){


  nrf_gpio_cfg_input(10, NRF_GPIO_PIN_NOPULL);
  nrf_gpio_cfg_input(39, NRF_GPIO_PIN_NOPULL);
  nrf_gpio_cfg_input(25, NRF_GPIO_PIN_PULLUP);

//Orange wire
  if(nrf_gpio_pin_read(25) == 0){   //pin is high by default, will be pulled low if 24AC supplied by wire
    sensors_state1.wire1_orange = 1;
    sensors_state2.wire1_orange = 1;
    sensors_state3.wire1_orange = 1;
    sensors_state4.wire1_orange = 1;
  }

  else{     //if not active, wait for 1ms to confirm 
      vTaskDelay(1);
      if(nrf_gpio_pin_read(25) == 1){
      sensors_state1.wire1_orange = 0;
      sensors_state2.wire1_orange = 0;
      sensors_state3.wire1_orange = 0;
      sensors_state4.wire1_orange = 0;
    }
    else{ //if changed withinh 1ms, its ON
        sensors_state1.wire1_orange = 1;
        sensors_state2.wire1_orange = 1;
        sensors_state3.wire1_orange = 1;
        sensors_state4.wire1_orange = 1;
    }
 }

//Same for White wire
 if(nrf_gpio_pin_read(10) == 0){
  sensors_state1.wire2_white = 1;
  sensors_state2.wire2_white = 1;
  sensors_state3.wire2_white = 1;
  sensors_state4.wire2_white = 1;

}
else{  
      vTaskDelay(1);
      if(nrf_gpio_pin_read(10) == 1){
      sensors_state1.wire2_white = 0;
      sensors_state2.wire2_white = 0;
      sensors_state3.wire2_white = 0;
      sensors_state4.wire2_white = 0;
      }
      else{
        sensors_state1.wire2_white = 1;
        sensors_state2.wire2_white = 1;
        sensors_state3.wire2_white = 1;
        sensors_state4.wire2_white = 1;
       }
  }

//Same for yellow wire
if(nrf_gpio_pin_read(39) == 0){
  sensors_state1.wire3_yellow = 1;
  sensors_state2.wire3_yellow = 1;
  sensors_state3.wire3_yellow = 1;
  sensors_state4.wire3_yellow = 1;
}

else{
    vTaskDelay(1);
    if(nrf_gpio_pin_read(39) == 1){
      sensors_state1.wire3_yellow = 0;
      sensors_state2.wire3_yellow = 0;
      sensors_state3.wire3_yellow = 0;
      sensors_state4.wire3_yellow = 0;
    }
    else{
      sensors_state1.wire3_yellow = 1;
      sensors_state2.wire3_yellow = 1;
      sensors_state3.wire3_yellow = 1;
      sensors_state4.wire3_yellow = 1;

    }

  }
  
}