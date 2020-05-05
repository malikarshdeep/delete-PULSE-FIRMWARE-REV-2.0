#ifndef  _HTU21D_H
#define _HTU21D_H
#include "app_error.h"
#include "nrf.h"


//static uint8_t tempTemperature;
//static uint8_t humHumidity;

uint8_t port;
uint8_t port_sensor;
uint16_t bad_oadb_count =0;
uint16_t bad_radb_count =0;
uint16_t bad_sadb_count =0;
uint16_t bad_oarh_count =0;
uint16_t bad_rarh_count =0;
uint16_t bad_sarh_count =0;

static uint8_t returned_over_I2C[3], returned1_over_I2C[3];
//float rawTemperature,rawHumidity  ;
static uint8_t m_sample;
ret_code_t htu21d_hw_init (void);
void htu21d_hw_uninit (void);
//ret_code_t i2c_test (void);
ret_code_t read_sensor_data_temp(uint8_t port_sensor);
ret_code_t tempRead_command(void);
ret_code_t read_sensor_data_HUM(uint8_t port_sensor);
ret_code_t humRead_command(void);
ret_code_t tcaselect(uint8_t);
char checkCRC(uint16_t message_from_sensor, uint8_t check_value_from_sensor);

#define TCAADDR 0x70
#define TWI_INSTANCE_ID     0
#define HTU21D_ADDRESS 0x40
#define HUMIDITY_NO_HOLD_MASTER_ADDRESS 0xF5
#define TEMPERATURE_NO_HOLD_MASTER_ADDRESS 0xF3
#define SOFT_RESET_HTU21D 0xFE
#define TWI_SDA_M NRF_GPIO_PIN_MAP(1,06)
#define TWI_SCL_M NRF_GPIO_PIN_MAP(1,05)
#define TWI_FREQUENCY_FREQUENCY_K50 (0xCC0000UL)
#define TWI_FREQUENCY_FREQUENCY_K25 (0x660000UL)
#define SHIFTED_DIVISOR 0x988000
#define ERROR_BAD_CRC		999

static uint8_t returned2_over_I2C[1] ;

#endif //_HTU21D_H

