/**
 *
 * \file
 *
 * \brief WINC1500 configuration.
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

#ifndef CONF_WINC_H_INCLUDED
#define CONF_WINC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "boards.h"


#define CONF_WINC_USE_SPI				(1)
#define ETH_MODE                              1
/*
   ---------------------------------
   ---------- PIN settings ---------
   ---------------------------------
*/

#ifdef BOARD_PCA10056

#define CONF_WINC_PIN_RESET				NRF_GPIO_PIN_MAP(1,8)
#define CONF_WINC_PIN_CHIP_ENABLE                       NRF_GPIO_PIN_MAP(1,7)
#define CONF_WINC_PIN_WAKE				//NRF_GPIO_PIN_MAP(0,26)
/** SPI pin and instance settings. */
#define CONF_WINC_SPI_CS_PIN                            NRF_GPIO_PIN_MAP(1,4)
#define CONF_WINC_SPI_MOSI				NRF_GPIO_PIN_MAP(1,3)
#define CONF_WINC_SPI_MISO				NRF_GPIO_PIN_MAP(1,2)
#define CONF_WINC_SPI_SCK				NRF_GPIO_PIN_MAP(1,1)
/** SPI interrupt pin. */
#define CONF_WINC_SPI_INT_PIN                           NRF_GPIO_PIN_MAP(1,6)		
#else  ///BOARD_PCA10056

#ifdef BOARD_CUSTOM

#define CONF_WINC_PIN_RESET				NRF_GPIO_PIN_MAP(0,26)
#define CONF_WINC_PIN_CHIP_ENABLE                        NRF_GPIO_PIN_MAP(1,9)
#define CONF_WINC_PIN_WAKE				NRF_GPIO_PIN_MAP(0,16)
/** SPI pin and instance settings. */
#define CONF_WINC_SPI_CS_PIN			NRF_GPIO_PIN_MAP(0,15)
#define CONF_WINC_SPI_MISO				NRF_GPIO_PIN_MAP(0,13)
#define CONF_WINC_SPI_MOSI				NRF_GPIO_PIN_MAP(0,17)
#define CONF_WINC_SPI_SCK				NRF_GPIO_PIN_MAP(0,12)
/** SPI interrupt pin. */
//#define CONF_WINC_SPI_INT_PIN			NRF_GPIO_PIN_MAP(2,127)		 // not used

#endif // BOARD_CUSTOM
#endif  //BOARD_PCA10056

/** SPI clock. */
#define CONF_WINC_SPI_CLOCK				(12000000)
#define CONF_WINC_SPI_INSTANCE 2
/*
   ---------------------------------
   --------- Debug Options ---------
   ---------------------------------
*/

#define CONF_WINC_DEBUG					(1)
#define CONF_WINC_PRINTF				NRF_LOG_INFO
#define CONF_M2M_LOG_LEVEL                              M2M_LOG_DBG

#ifdef __cplusplus
}
#endif

#endif /* CONF_WINC_H_INCLUDED */
