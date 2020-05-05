#include "app_error.h"
#include "bsp.h"
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "nrf_gpio.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_gpiote.h"
#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "onewire.h"

#include "boards.h"


const nrf_drv_timer_t TIMER_onewire = NRF_DRV_TIMER_INSTANCE(1);
static nrf_ppi_channel_t m_ppi_0;
static nrf_ppi_channel_t m_ppi_1;

static SemaphoreHandle_t xSemaphore;
typedef struct onewire_t {
  uint32_t pin_number;
  uint32_t state;
  uint8_t presence;
  uint8_t ibit; //number of current bit
  uint8_t nbytes; //number of bits left to to send/receive
  uint8_t *buff;
} onewire_t;

volatile onewire_t onewire;

static enum {
  READY =0,
  RESET,
  WRITE,
  READ
} state;

static void timer_onewire_event_handler(nrf_timer_event_t event_type, void *p_context) {
  uint32_t time_ticks;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  switch (event_type) {
  case NRF_TIMER_EVENT_COMPARE0:
    
  case NRF_TIMER_EVENT_COMPARE1:
    if (WRITE == onewire.state)
    {  
      if (0==onewire.nbytes)
      {
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);          
        break;
      }        
      if ((*onewire.buff) & (1 << onewire.ibit))        
         time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 5);
      else
         time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 100); 
      nrf_drv_timer_compare(&TIMER_onewire,NRF_TIMER_CC_CHANNEL0, time_ticks,false);
      if ((++onewire.ibit)>7)
      {
        onewire.ibit = 0;
        onewire.buff++;
        onewire.nbytes--; 
      }
      time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 120);
      nrf_drv_timer_extended_compare(&TIMER_onewire, NRF_TIMER_CC_CHANNEL1, time_ticks, NRF_TIMER_SHORT_COMPARE1_STOP_MASK, true);
      //if (0==onewire.nbytes)
      //  nrf_drv_ppi_channel_disable(m_ppi_1);  
      nrfx_gpiote_clr_task_trigger(onewire.pin_number);
      nrf_drv_timer_clear(&TIMER_onewire);
      nrf_drv_timer_resume(&TIMER_onewire);         
      break;
    }
    if (READ == onewire.state)
    {
      if (nrf_gpio_pin_read(onewire.pin_number))      
        *onewire.buff |= 1<<onewire.ibit;      
      if ((++onewire.ibit) > 7)
      {
        onewire.ibit = 0;
        onewire.buff++;
        onewire.nbytes--; 
        if (0 == onewire.nbytes)        
          nrf_drv_ppi_channel_disable(m_ppi_0);                  
      }      
    }

    break;
  case NRF_TIMER_EVENT_COMPARE2:
    if (RESET == onewire.state)
      if (!nrf_gpio_pin_read(onewire.pin_number))
        onewire.presence =1;   
    if (READ == onewire.state)
    {
      if (0 == onewire.nbytes)        
      {
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);           
      }
      else
      {
        nrf_drv_timer_clear(&TIMER_onewire);
        nrfx_gpiote_out_task_trigger(onewire.pin_number);
        nrf_drv_timer_resume(&TIMER_onewire);  
      }
    }            
    break;
  case NRF_TIMER_EVENT_COMPARE3:
      if (RESET == onewire.state)
      {
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);  
      }            
    break;

  default:
    //Do nothing.
    break;
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int onewire_init(void) {
  uint32_t err_code = NRF_SUCCESS;
  uint32_t time_ticks;
  APP_ERROR_CHECK(err_code);
  err_code = nrf_drv_ppi_channel_alloc(&m_ppi_0);
  APP_ERROR_CHECK(err_code);
  err_code = nrf_drv_ppi_channel_alloc(&m_ppi_1);
  APP_ERROR_CHECK(err_code);
  xSemaphore = xSemaphoreCreateBinary();
  if (xSemaphore == NULL) {
    APP_ERROR_CHECK(NRF_ERROR_NO_MEM);
  }
  return NRF_SUCCESS;
}

int onewire_reset(uint32_t pin_number) {
  uint32_t time_ticks;
  uint32_t ret;
      uint32_t compare_evt_addr;
    uint32_t gpiote_task_addr;
  if (onewire.state != READY)
    return ONEWIRE_ERR_BUSY;
  

  onewire.pin_number = pin_number;
  onewire.state = RESET;
  onewire.presence =0;
  nrf_gpio_cfg(onewire.pin_number, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_pin_set(onewire.pin_number);
  //nrf_gpio_pin_set(onewire.pin_number);

  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
  ret = nrf_drv_timer_init(&TIMER_onewire, &timer_cfg, timer_onewire_event_handler);
  APP_ERROR_CHECK(ret);
  time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 10);
  nrf_drv_timer_compare(&TIMER_onewire,NRF_TIMER_CC_CHANNEL0, time_ticks,false);
  time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 480);
  nrf_drv_timer_compare(&TIMER_onewire,NRF_TIMER_CC_CHANNEL1, time_ticks,false);
  time_ticks += nrf_drv_timer_us_to_ticks(&TIMER_onewire, 70);
  nrf_drv_timer_compare(&TIMER_onewire,NRF_TIMER_CC_CHANNEL2, time_ticks,true);
  time_ticks += nrf_drv_timer_us_to_ticks(&TIMER_onewire, 480-70);
  nrf_drv_timer_extended_compare(&TIMER_onewire, NRF_TIMER_CC_CHANNEL3, time_ticks, NRF_TIMER_SHORT_COMPARE3_STOP_MASK, true);

  nrf_drv_gpiote_out_config_t gpiote_out_config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(true);
  ret = nrf_drv_gpiote_out_init(onewire.pin_number, &gpiote_out_config);
  APP_ERROR_CHECK(ret);
  nrf_gpio_cfg(onewire.pin_number, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1, NRF_GPIO_PIN_NOSENSE);
  gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(onewire.pin_number);
  compare_evt_addr = nrf_drv_timer_event_address_get(&TIMER_onewire, NRF_TIMER_EVENT_COMPARE0);
  ret = nrf_drv_ppi_channel_assign(m_ppi_0, compare_evt_addr, gpiote_task_addr);
  APP_ERROR_CHECK(ret);
  compare_evt_addr = nrf_drv_timer_event_address_get(&TIMER_onewire, NRF_TIMER_EVENT_COMPARE1);
  ret = nrf_drv_ppi_channel_assign(m_ppi_1, compare_evt_addr, gpiote_task_addr);
  APP_ERROR_CHECK(ret);
  ret = nrf_drv_ppi_channel_enable(m_ppi_0);
  APP_ERROR_CHECK(ret);
  ret = nrf_drv_ppi_channel_enable(m_ppi_1);
  APP_ERROR_CHECK(ret);


  nrf_drv_gpiote_out_task_enable(onewire.pin_number);
  nrf_drv_timer_enable(&TIMER_onewire);
  if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdFALSE) {
    ret = ONEWIRE_ERR;
    nrf_drv_timer_disable(&TIMER_onewire);
  }
  nrf_drv_timer_uninit(&TIMER_onewire);
  nrf_drv_gpiote_out_uninit(onewire.pin_number);   
  nrf_gpio_pin_set(onewire.pin_number);
  nrf_gpio_cfg(onewire.pin_number, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1, NRF_GPIO_PIN_NOSENSE);
  nrf_drv_ppi_channel_disable(m_ppi_0);
  nrf_drv_ppi_channel_disable(m_ppi_1);
  onewire.state = READY;
  if ((0==ret)&&(0==onewire.presence))
  {
    ret = ONEWIRE_ERR_NO_RESPONSE;
  }
  return ret;
}

int onewire_write(uint32_t pin_number, uint8_t * buff, uint16_t len)
{
  uint32_t time_ticks;
  uint32_t ret;
  uint32_t compare_evt_addr;
  uint32_t gpiote_task_addr;
  if (onewire.state != READY)
  {
    return ONEWIRE_ERR_BUSY;
  }
  if (0 == len)
    return 0;
  onewire.state = WRITE;
  onewire.nbytes = len;
  onewire.ibit = 0;
  onewire.buff = buff;
  onewire.pin_number = pin_number;

  nrf_gpio_cfg(onewire.pin_number, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_pin_set(onewire.pin_number);
  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
  ret = nrf_drv_timer_init(&TIMER_onewire, &timer_cfg, timer_onewire_event_handler);
  APP_ERROR_CHECK(ret);
  time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 120);
  nrf_drv_timer_extended_compare(&TIMER_onewire, NRF_TIMER_CC_CHANNEL1, time_ticks, NRF_TIMER_SHORT_COMPARE1_STOP_MASK, true);
  if (*onewire.buff & 1)
    time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 5);
  else
    time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 100);
  nrf_drv_timer_compare(&TIMER_onewire,NRF_TIMER_CC_CHANNEL0, time_ticks,false);
  onewire.ibit++;

  nrf_drv_gpiote_out_config_t gpiote_out_config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);
  ret = nrf_drv_gpiote_out_init(onewire.pin_number, &gpiote_out_config);
  APP_ERROR_CHECK(ret);
  nrf_gpio_cfg(onewire.pin_number, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1, NRF_GPIO_PIN_NOSENSE);

  gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(onewire.pin_number);
  compare_evt_addr = nrf_drv_timer_event_address_get(&TIMER_onewire, NRF_TIMER_EVENT_COMPARE0);
  ret = nrf_drv_ppi_channel_assign(m_ppi_0, compare_evt_addr, gpiote_task_addr);
  APP_ERROR_CHECK(ret);
  compare_evt_addr = nrf_drv_timer_event_address_get(&TIMER_onewire, NRF_TIMER_EVENT_COMPARE1);
  //ret = nrf_drv_ppi_channel_assign(m_ppi_1, compare_evt_addr, gpiote_task_addr);
  APP_ERROR_CHECK(ret);
  ret = nrf_drv_ppi_channel_enable(m_ppi_0);
  APP_ERROR_CHECK(ret);
  //ret = nrf_drv_ppi_channel_enable(m_ppi_1);
  APP_ERROR_CHECK(ret);
  
  nrf_drv_gpiote_out_task_enable(onewire.pin_number);
  nrf_drv_timer_enable(&TIMER_onewire);
  
  if (xSemaphoreTake(xSemaphore, (TickType_t)300) == pdFALSE) {
    ret = ONEWIRE_ERR;
    nrf_drv_timer_disable(&TIMER_onewire);
  }
  nrf_drv_timer_uninit(&TIMER_onewire);
  nrf_gpio_cfg_default(onewire.pin_number);
  nrf_drv_gpiote_in_uninit(onewire.pin_number);   
  nrf_gpio_pin_set(onewire.pin_number);
  nrf_gpio_cfg(onewire.pin_number, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1, NRF_GPIO_PIN_NOSENSE);  
  nrf_drv_ppi_channel_disable(m_ppi_0);
  nrf_drv_ppi_channel_disable(m_ppi_1);  
  onewire.state = READY;
  return ret;
}

int onewire_read(uint32_t pin_number, uint8_t * buff, uint16_t len)
{
  uint32_t time_ticks;
  uint32_t ret;
  uint32_t compare_evt_addr;
  uint32_t gpiote_task_addr;
  if (onewire.state != READY)
  {
    return ONEWIRE_ERR_BUSY;
  }
  if (0 == len)
    return 0;
  onewire.state = READ;
  onewire.nbytes = len;
  onewire.ibit = 0;
  onewire.buff = buff;
  onewire.pin_number = pin_number;
  memset(buff,0,len);

  nrf_gpio_cfg(onewire.pin_number, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_pin_set(onewire.pin_number);
  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
  ret = nrf_drv_timer_init(&TIMER_onewire, &timer_cfg, timer_onewire_event_handler);
  APP_ERROR_CHECK(ret);
  time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 2);
  nrf_drv_timer_compare(&TIMER_onewire,NRF_TIMER_CC_CHANNEL0, time_ticks,false);
  time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 5);
  nrf_drv_timer_compare(&TIMER_onewire,NRF_TIMER_CC_CHANNEL1, time_ticks,true);
  time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_onewire, 120);
  nrf_drv_timer_extended_compare(&TIMER_onewire, NRF_TIMER_CC_CHANNEL2, time_ticks, NRF_TIMER_SHORT_COMPARE2_STOP_MASK, true);  

  nrf_drv_gpiote_out_config_t gpiote_out_config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);
  ret = nrf_drv_gpiote_out_init(onewire.pin_number, &gpiote_out_config);
  APP_ERROR_CHECK(ret);
  nrf_gpio_cfg(onewire.pin_number, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1, NRF_GPIO_PIN_NOSENSE);

  gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(onewire.pin_number);
  compare_evt_addr = nrf_drv_timer_event_address_get(&TIMER_onewire, NRF_TIMER_EVENT_COMPARE1);
  ret = nrf_drv_ppi_channel_assign(m_ppi_0, compare_evt_addr, gpiote_task_addr);
  APP_ERROR_CHECK(ret);
  compare_evt_addr = nrf_drv_timer_event_address_get(&TIMER_onewire, NRF_TIMER_EVENT_COMPARE2);
  ret = nrf_drv_ppi_channel_assign(m_ppi_1, compare_evt_addr, gpiote_task_addr);
  APP_ERROR_CHECK(ret);
  ret = nrf_drv_ppi_channel_enable(m_ppi_0);
  APP_ERROR_CHECK(ret);
  //ret = nrf_drv_ppi_channel_enable(m_ppi_1);
  APP_ERROR_CHECK(ret);
  
  nrf_drv_gpiote_out_task_enable(onewire.pin_number);
  nrf_drv_timer_enable(&TIMER_onewire);
  
  if (xSemaphoreTake(xSemaphore, (TickType_t)300) == pdFALSE) {
    ret = ONEWIRE_ERR;
    nrf_drv_timer_disable(&TIMER_onewire);
  }
  nrf_drv_timer_uninit(&TIMER_onewire);
  nrf_gpio_cfg_default(onewire.pin_number);
  nrf_drv_gpiote_in_uninit(onewire.pin_number);   
  nrf_gpio_pin_set(onewire.pin_number);
  nrf_gpio_cfg(onewire.pin_number, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1, NRF_GPIO_PIN_NOSENSE);  
  nrf_drv_ppi_channel_disable(m_ppi_0);
  nrf_drv_ppi_channel_disable(m_ppi_1);  
  onewire.state = READY;
  return ret;
  
}
