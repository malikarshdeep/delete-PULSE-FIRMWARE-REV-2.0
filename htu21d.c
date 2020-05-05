#include "htu21d.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"
#include "app_error.h"
#include "nrf.h"
#include "bsp.h"
#include "app_util_platform.h"
#include "tcptest.h"
#include "nrf_log.h"


float tempTemperature;
float humHumidity;
float rawTemperature,rawHumidity  ;
static volatile bool m_xfer_done = false;



/**
 * @brief TWI master instance.
 *
 * Instance of TWI master driver that will be used for communication with simulated
 * EEPROM memory.
 */



static const nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(0);


char checkCRC(uint16_t message_from_sensor, uint8_t check_value_from_sensor)
{
  //Test cases from datasheet:
  //message = 0xDC, checkvalue is 0x79
  //message = 0x683A, checkvalue is 0x7C
  //message = 0x4E85, checkvalue is 0x6B

  uint32_t remainder = (uint32_t)message_from_sensor << 8; //Pad with 8 bits because we have to add in the check value
  remainder |= check_value_from_sensor; //Add on the check value

  uint32_t divsor = (uint32_t)SHIFTED_DIVISOR;

  for (int i = 0 ; i < 16 ; i++) //Operate on only 16 positions of max 24. The remaining 8 are our remainder and should be zero when we're done.
  {
    //Serial.print("remainder: ");
    //Serial.println(remainder, BIN);
    //Serial.print("divsor:    ");
    //Serial.println(divsor, BIN);
    //Serial.println();

    if ( remainder & (uint32_t)1 << (23 - i) ) //Check if there is a one in the left position
      remainder ^= divsor;

    divsor >>= 1; //Rotate the divsor max 16 times so that we have 8 bits left of a remainder
  }

  return (char)remainder;
}




ret_code_t tcaselect(uint8_t port)
{
    uint8_t count = 0;
    ret_code_t err_code;
    if (port > 7) return;
    uint8_t command_address = 1<<port;
    err_code = nrf_drv_twi_tx(&m_twi_master, TCAADDR, &command_address, 1, false);
   // APP_ERROR_CHECK(err_code);
    if(err_code != 0){
       return;}
       // port_sensor = port;

        while (m_xfer_done == false && count<200){
    
    count++;
    vTaskDelay(1);
 // nrf_delay_ms(1);
  
    }

}

void tca_read()
{
    uint8_t count = 0;
    m_xfer_done = false;   
    /* Read 1 byte from the specified address - skip 3 bits dedicated for fractional part of temperature. */
    ret_code_t err_code = nrf_drv_twi_rx(&m_twi_master, TCAADDR, returned2_over_I2C, 1);
    vTaskDelay(1);
  // nrf_delay_ms(1);

   if(err_code != 0){
     return;}

        while (m_xfer_done == false && count<200){
    
    count++;
    vTaskDelay(1);
 // nrf_delay_ms(1);
    }
}



__STATIC_INLINE void data_handler(uint8_t temp)
{
   // NRF_LOG_INFO("Temperature: %d Celsius degrees.", temp);
}


void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
            {
                data_handler(m_sample);
            }
            m_xfer_done = true;
            break;
        default:
            break;
    }
}


ret_code_t humRead_command(void)
{
    ret_code_t err_code;
    uint8_t count = 0;
    /* Writing to LM75B_REG_CONF "0" set temperature sensor in NORMAL mode. */
    uint8_t command_address = HUMIDITY_NO_HOLD_MASTER_ADDRESS;
    err_code = nrf_drv_twi_tx(&m_twi_master, HTU21D_ADDRESS, &command_address, 1, false);
   // APP_ERROR_CHECK(err_code);

     if(err_code != 0){
     return;}

        while (m_xfer_done == false && count<200){
    
    count++;
    vTaskDelay(1);
 // nrf_delay_ms(1);
    }
}

/**
 * @brief Function for reading data from temperature sensor.
 */
ret_code_t read_sensor_data_temp(uint8_t port_sensor)
{
    m_xfer_done = false;
     uint8_t count = 0;
   //command to receive the temp inside returned_over_I2C buffer
     ret_code_t err_code = nrf_drv_twi_rx(&m_twi_master, HTU21D_ADDRESS, returned_over_I2C, 3);
     if(err_code != 0){
        return;}

     while (m_xfer_done == false && count<200){
        count++;
        vTaskDelay(1);
      }
//Byte 0 + Byte 1
     rawTemperature = returned_over_I2C[0] << 8 | returned_over_I2C[1];
//If CRC did not match or the buffer is 0: BAD VALUE  
      if ((checkCRC(rawTemperature, returned_over_I2C[2]) != 0)|| ((returned_over_I2C[0] == 0) && (returned_over_I2C[1] == 0) )) {
           //if its outdoor sensor 
           if(port_sensor == 2 ){
            bad_oadb_count++;
           if(bad_oadb_count > 500){
               sensors_state1.oadb10 = -32.125;
               sensors_state2.oadb10 = -32.125;
               sensors_state3.oadb10 = -32.125;
               sensors_state4.oadb10 = -32.125;
               bad_oadb_count = 0;
          }
      }
     //if its return
     else if(port_sensor == 4){
     bad_radb_count++;
  //   NRF_LOG_INFO("BAD VALUE RADB NO %d",bad_radb_count );
           if(bad_radb_count>500){
               sensors_state1.radb10 = -32.125;
               sensors_state2.radb10 = -32.125;
               sensors_state3.radb10 = -32.125;
               sensors_state4.radb10 = -32.125;
               bad_radb_count = 0;
           }
     
     }
     //if its supply
     else if(port_sensor == 6){
     bad_sadb_count++;
   //  NRF_LOG_INFO("BAD VALUE SADB NO %d",bad_sadb_count );
            if(bad_sadb_count>500){
                 sensors_state1.sadb10 = -32.125;
                 sensors_state2.sadb10 = -32.125;
                 sensors_state3.sadb10 = -32.125;
                 sensors_state4.sadb10 = -32.125;
                 bad_sadb_count = 0;
            }
     }
     
    return (ERROR_BAD_CRC);
   }
    //if good readings, calculate temp Formula: 
     tempTemperature = ((rawTemperature)/65536)*175.72;
     tempTemperature = tempTemperature - 46.85;

     if(port_sensor == 2 ){
     sensors_state1.oadb10 = tempTemperature;
     sensors_state2.oadb10 = tempTemperature;
     sensors_state3.oadb10 = tempTemperature;
     sensors_state4.oadb10 = tempTemperature;
     bad_oadb_count = 0;
     }

     else if(port_sensor == 4){
     sensors_state1.radb10 = tempTemperature;
     sensors_state2.radb10 = tempTemperature;
     sensors_state3.radb10 = tempTemperature;
     sensors_state4.radb10 = tempTemperature;
     bad_radb_count = 0;
     
     }
     
     else if(port_sensor == 6){
     sensors_state1.sadb10 = tempTemperature;
     sensors_state2.sadb10 = tempTemperature;
     sensors_state3.sadb10 = tempTemperature;
     sensors_state4.sadb10 = tempTemperature;
     bad_sadb_count = 0;
     }

     returned_over_I2C[0] = returned_over_I2C[1] = 0;
}

//Command to read humidity 
ret_code_t read_sensor_data_HUM(uint8_t port_sensor)
{
    m_xfer_done = false;
    uint8_t count = 0;
    //Receive values inside returned1_over_I2C buffer
    ret_code_t err_code = nrf_drv_twi_rx(&m_twi_master, HTU21D_ADDRESS, returned1_over_I2C, 3);

    if(err_code != 0){
      return;}

    while (m_xfer_done == false && count<200){
        count++;
        vTaskDelay(1);
      }
    //Byte0+Byt1
    rawHumidity = returned1_over_I2C[0] << 8 | returned1_over_I2C[1];
    //If CRC failed or buffer is 0: BAD VALUES  
    if ((checkCRC(rawHumidity, returned1_over_I2C[2]) != 0)|| ((returned1_over_I2C[0] == 0) && (returned1_over_I2C[1] == 0) )) {
         //if bad value from outdoor
          if(port_sensor == 2 ){
            bad_oarh_count++;
            if(bad_oarh_count>500){
               sensors_state1.oarh10 = -32.125;
               sensors_state2.oarh10 = -32.125;
               sensors_state3.oarh10 = -32.125;
               sensors_state4.oarh10 = -32.125;
               bad_oarh_count = 0;
            }
         }
      //if bad value from return
       else if(port_sensor == 4){
           bad_rarh_count++;
           if(bad_rarh_count>500){
             sensors_state1.rarh10 = -32.125;
             sensors_state2.rarh10 = -32.125;
             sensors_state3.rarh10 = -32.125;
             sensors_state4.rarh10 = -32.125;
             bad_rarh_count = 0;
          }
      }
     
      else if(port_sensor == 6){
          bad_sarh_count++;
          if(bad_sarh_count>500){
            sensors_state1.sarh10 = -32.125;
            sensors_state2.sarh10 = -32.125;
            sensors_state3.sarh10 = -32.125;
            sensors_state4.sarh10 = -32.125;
            bad_sarh_count = 0;
          }
      }

     return (ERROR_BAD_CRC);
    }
//If received good values: Formula
     humHumidity = ((rawHumidity)/65536)*125;
     humHumidity = humHumidity - 6;

     if(port_sensor == 2){
       sensors_state1.oarh10 = humHumidity;
       sensors_state2.oarh10 = humHumidity;
       sensors_state3.oarh10 = humHumidity;
       sensors_state4.oarh10 = humHumidity;
       bad_oarh_count = 0;
     }

     else if(port_sensor == 4){     
       sensors_state1.rarh10 = humHumidity;
       sensors_state2.rarh10 = humHumidity;
       sensors_state3.rarh10 = humHumidity;
       sensors_state4.rarh10 = humHumidity;
       bad_rarh_count = 0;
     }

    else if(port_sensor == 6){    
       sensors_state1.sarh10 = humHumidity;
       sensors_state2.sarh10 = humHumidity;
       sensors_state3.sarh10 = humHumidity;
       sensors_state4.sarh10 = humHumidity;
       bad_sarh_count = 0;
     }

     returned1_over_I2C[0] = returned1_over_I2C[1] = 0;
}


/**
 * @brief Initialize the master TWI.
 *
 * Function used to initialize the master TWI interface that would communicate with simulated EEPROM.
 *
 * @return NRF_SUCCESS or the reason of failure.
 */
 //I2C configuration
static ret_code_t twi_master_init(void)
{
    ret_code_t ret;
    const nrf_drv_twi_config_t config =
    {
       .scl                = TWI_SCL_M,
       .sda                = TWI_SDA_M,
       .frequency          = TWI_FREQUENCY_FREQUENCY_K50, 
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    ret = nrf_drv_twi_init(&m_twi_master, &config, twi_handler, NULL);

    if (NRF_SUCCESS == ret)
    {
        nrf_drv_twi_enable(&m_twi_master);
    }

    return ret;
}

//initialize the i2c lib
ret_code_t htu21d_hw_init (void)
{
  return twi_master_init();
}

//uninitialize the i2c library
void htu21d_hw_uninit(void)
{
 nrf_drv_twi_uninit(&m_twi_master);
}

//ret_code_t i2c_test (void)
//{
//  ret_code_t ret;
//  uint8_t txbuff[1];
//  ret = nrf_drv_twi_tx(&m_twi_master, 0xaa, txbuff, sizeof(txbuff), false);
//  return ret;
//}
//Send the command to read temperature from sensor
ret_code_t tempRead_command(void)
{
    ret_code_t ret;
    uint8_t count = 0;
    uint8_t command_address = TEMPERATURE_NO_HOLD_MASTER_ADDRESS;
    ret = nrf_drv_twi_tx(&m_twi_master, HTU21D_ADDRESS, &command_address, 1, false);
        if(ret !=0){ 
            return;
            }
    //wait while interrupt flags the transfer complete, count to avoid getting stuck 
    while (m_xfer_done == false && count<200){
    count++;
    vTaskDelay(1);
    }

}
//Command to software reset the sensor
ret_code_t soft_reset_htu21(void)
{
    ret_code_t ret;
    uint8_t count = 0;
    uint8_t command_address = SOFT_RESET_HTU21D;
    ret = nrf_drv_twi_tx(&m_twi_master, HTU21D_ADDRESS, &command_address, 1, false);
        if(ret !=0){
            return;
            }
//wait while interrupt flags the transfer complete, count to avoid getting stuck
    while (m_xfer_done == false && count<200){
        count++;
        vTaskDelay(1);
        }

}