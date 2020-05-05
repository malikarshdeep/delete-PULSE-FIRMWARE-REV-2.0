/**
 *
 * \file
 *
 * \brief This module contains SAMD21 BSP APIs implementation.
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#include "FreeRTOS.h"
#include "bsp/include/nm_bsp.h"
#include "bsp/include/nm_bsp_internal.h"
#include "common/include/nm_common.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#include "conf_winc.h"

static tpfNmBspIsr gpfIsr;

static void chip_isr(void) {
  if (gpfIsr) {
    gpfIsr();
  }
}

/*
 *	@fn		init_chip_pins
 *	@brief	Initialize reset, chip enable and wake pin
 */
static void init_chip_pins(void) {
  /* Configure control pins as output. */
  nrf_gpio_cfg_output(CONF_WINC_PIN_RESET);
  nrf_gpio_cfg_output(CONF_WINC_PIN_CHIP_ENABLE);
  nrf_gpio_pin_set(CONF_WINC_PIN_CHIP_ENABLE);
  nrf_gpio_pin_set(CONF_WINC_PIN_RESET);
}

/*
 *	@fn		nm_bsp_init
 *	@brief	Initialize BSP
 *	@return	0 in case of success and -1 in case of failure
 */
sint8 nm_bsp_init(void) {
  gpfIsr = NULL;
  /* Initialize chip IOs. */
  init_chip_pins();
  nm_bsp_reset();
  return M2M_SUCCESS;
}

/**
 *	@fn		nm_bsp_deinit
 *	@brief	De-iInitialize BSP
 *	@return	0 in case of success and -1 in case of failure
 */
sint8 nm_bsp_deinit(void) {
  nrf_gpio_cfg_default(CONF_WINC_PIN_RESET);
  nrf_gpio_cfg_default(CONF_WINC_PIN_CHIP_ENABLE);
  return M2M_SUCCESS;
}

/**
 *	@fn		nm_bsp_reset
 *	@brief	Reset NMC1500 SoC by setting CHIP_EN and RESET_N signals low,
 *           CHIP_EN high then RESET_N high
 */
void nm_bsp_reset(void) {
  nrf_gpio_pin_clear(CONF_WINC_PIN_CHIP_ENABLE);
  nrf_gpio_pin_clear(CONF_WINC_PIN_RESET);
  nm_bsp_sleep(1);
  nrf_gpio_pin_set(CONF_WINC_PIN_CHIP_ENABLE);
  nm_bsp_sleep(10);
  nrf_gpio_pin_set(CONF_WINC_PIN_RESET);
  nm_bsp_sleep(30);

}

/*
 *	@fn		nm_bsp_sleep
 *	@brief	Sleep in units of mSec
 *	@param[IN]	u32TimeMsec
 *				Time in milliseconds
 */
void nm_bsp_sleep(uint32 u32TimeMsec) {
  while (u32TimeMsec--) {
    vTaskDelay(1);
  }
}

/*
 *	@fn		nm_bsp_register_isr
 *	@brief	Register interrupt service routine
 *	@param[IN]	pfIsr
 *				Pointer to ISR handler
 */
void nm_bsp_register_isr(tpfNmBspIsr pfIsr) {
  int32_t err_code;
  gpfIsr = pfIsr;

  nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
  in_config.pull = NRF_GPIO_PIN_PULLUP;
  
  //err_code = nrfx_gpiote_in_init(CONF_WINC_SPI_INT_PIN, &in_config, gpfIsr);
  //APP_ERROR_CHECK(err_code);
  //nrfx_gpiote_in_event_enable(CONF_WINC_SPI_INT_PIN, true);
}

/*
 *	@fn		nm_bsp_interrupt_ctrl
 *	@brief	Enable/Disable interrupts
 *	@param[IN]	u8Enable
 *				'0' disable interrupts. '1' enable interrupts
 */
void nm_bsp_interrupt_ctrl(uint8 u8Enable) {
  if (u8Enable) {
		//nrfx_gpiote_in_event_enable(CONF_WINC_SPI_INT_PIN, true);
	} else {
		//nrfx_gpiote_in_event_disable(CONF_WINC_SPI_INT_PIN);
	}
}