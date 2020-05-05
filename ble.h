#ifndef BLE_H
#define BLE_H
#include "bsp.h"

void bsp_event_handler(bsp_event_t event);
void ble_init(void);
void ble_deinit(void);
uint32_t ble_serial_send(uint8_t * data, uint16_t len);
void ble_wait_radio_free (void);

#endif