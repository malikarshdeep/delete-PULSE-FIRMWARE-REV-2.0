/**
 * Copyright (c) 2017, Zaowubang, Inc
 *
 * All rights reserved.
 *
 */
#ifndef CUSTOM_BOARD_H
#define CUSTOM_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"
	
// LEDs definitions for pulse board
#define LEDS_NUMBER    4

#define LED_1          NRF_GPIO_PIN_MAP(0,9)
#define LED_2          NRF_GPIO_PIN_MAP(0,9)
#define LED_3          NRF_GPIO_PIN_MAP(0,9)
#define LED_4          NRF_GPIO_PIN_MAP(0,9)
#define LED_START      LED_1
#define LED_STOP       LED_4

#define LEDS_ACTIVE_STATE 1

#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }

#define LEDS_INV_MASK  LEDS_MASK

#define BSP_LED_0      9
#define BSP_LED_1      9
#define BSP_LED_2      9
#define BSP_LED_3      9

#define BUTTONS_NUMBER 0
#define BUTTON_PULL    NRF_GPIO_PIN_NOPULL

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1}

#define BSP_BUTTON_0   BUTTON_1

#define HWFC           false

#define RX_PIN_NUMBER  NRF_GPIO_PIN_MAP(0,10)
#define TX_PIN_NUMBER  NRF_GPIO_PIN_MAP(0,25)
#define CTS_PIN_NUMBER  NRF_GPIO_PIN_MAP(3,32)
#define RTS_PIN_NUMBER  NRF_GPIO_PIN_MAP(3,32)


#ifdef __cplusplus
}
#endif

#endif // CUSTOM_BOARD_H
