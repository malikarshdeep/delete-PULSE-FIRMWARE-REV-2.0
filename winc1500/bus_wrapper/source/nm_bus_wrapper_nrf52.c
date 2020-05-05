/**
 *
 * \file
 *
 * \brief This module contains NMC1000 bus wrapper APIs implementation.
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

#include <stdio.h>
#include "bsp/include/nm_bsp.h"
#include "common/include/nm_common.h"
#include "bus_wrapper/include/nm_bus_wrapper.h"
#include "nrfx_spim.h"
#include "conf_winc.h"

#include "nrf_delay.h"

#define NM_BUS_MAX_TRX_SZ	256

tstrNmBusCapabilities egstrNmBusCapabilities =
{
	NM_BUS_MAX_TRX_SZ
};

#ifdef CONF_WINC_USE_I2C

struct i2c_master_module i2c_master_instance;
#define SLAVE_ADDRESS 0x60

/** Number of times to try to send packet if failed. */
#define I2C_TIMEOUT 100

static sint8 nm_i2c_write(uint8 *b, uint16 sz)
{
	return -1;
}

static sint8 nm_i2c_read(uint8 *rb, uint16 sz)
{

	return -1;
}

static sint8 nm_i2c_write_special(uint8 *wb1, uint16 sz1, uint8 *wb2, uint16 sz2)
{
	return -1;
}
#endif

#ifdef CONF_WINC_USE_SPI

static nrfx_spim_t winc_spim =NRFX_SPIM_INSTANCE(CONF_WINC_SPI_INSTANCE);


sint8 spi_rw(uint8* pu8Mosi, uint8* pu8Miso, uint16 u16Sz)
{
        NRF_SPIM_Type * p_spim = (NRF_SPIM_Type *)winc_spim.p_reg;
	int32_t err;        
        size_t sz_tx = u16Sz, sz_rx = u16Sz;
        if(((pu8Miso == NULL) && (pu8Mosi == NULL)) ||(u16Sz == 0)) {
		return M2M_ERR_INVALID_ARG;
	}
        if (pu8Mosi == NULL){
          sz_tx = 0;
        }
        if (pu8Miso == NULL){
          sz_rx = 0;
        }
        nrfx_spim_xfer_desc_t spim_xfer = NRFX_SPIM_SINGLE_XFER(pu8Mosi, sz_tx, pu8Miso, sz_rx);

        nrfx_spim_xfer(&winc_spim, &spim_xfer, 0);

        return M2M_SUCCESS;
}
#endif

/*
*	@fn		nm_bus_init
*	@brief	Initialize the bus wrapper
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
sint8 nm_bus_init(void *pvinit)
{
  NRF_SPIM_Type * p_spim = (NRF_SPIM_Type *)winc_spim.p_reg;
  uint8_t mosi = 0xff;
	sint8 result = M2M_SUCCESS;
#ifdef CONF_WINC_USE_I2C
        result = -1;
#elif defined CONF_WINC_USE_SPI

  nrfx_spim_config_t spi_config = NRFX_SPIM_DEFAULT_CONFIG;
  spi_config.mode = NRF_SPIM_MODE_0;
  spi_config.frequency =  NRF_SPIM_FREQ_32M;
  #ifdef BOARD_PCA10056
  spi_config.frequency =  NRF_SPIM_FREQ_2M;
  #endif
  spi_config.ss_pin   = CONF_WINC_SPI_CS_PIN;
  spi_config.miso_pin = CONF_WINC_SPI_MISO;
  spi_config.mosi_pin = CONF_WINC_SPI_MOSI;
  spi_config.sck_pin = CONF_WINC_SPI_SCK;
  spi_config.orc            = 0x00;
  APP_ERROR_CHECK(result = nrfx_spim_init(&winc_spim, &spi_config, NULL, NULL));
 

#endif
  	nm_bsp_reset();
	nm_bsp_sleep(1);
	return result;
}

/*
*	@fn		nm_bus_ioctl
*	@brief	send/receive from the bus
*	@param[IN]	u8Cmd
*					IOCTL command for the operation
*	@param[IN]	pvParameter
*					Arbitrary parameter depenging on IOCTL
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*	@note	For SPI only, it's important to be able to send/receive at the same time
*/
sint8 nm_bus_ioctl(uint8 u8Cmd, void* pvParameter)
{
	sint8 s8Ret = 0;
	switch(u8Cmd)
	{
#ifdef CONF_WINC_USE_I2C
		case NM_BUS_IOCTL_R: {
			tstrNmI2cDefault *pstrParam = (tstrNmI2cDefault *)pvParameter;
			s8Ret = -1;
		}
		break;
		case NM_BUS_IOCTL_W: {
			tstrNmI2cDefault *pstrParam = (tstrNmI2cDefault *)pvParameter;
			s8Ret = -1;
		}
		break;
		case NM_BUS_IOCTL_W_SPECIAL: {
			tstrNmI2cSpecial *pstrParam = (tstrNmI2cSpecial *)pvParameter;
			s8Ret = -1;
		}
		break;
#elif defined CONF_WINC_USE_SPI
		case NM_BUS_IOCTL_RW: {
			tstrNmSpiRw *pstrParam = (tstrNmSpiRw *)pvParameter;
			s8Ret = spi_rw(pstrParam->pu8InBuf, pstrParam->pu8OutBuf, pstrParam->u16Sz);
		}
		break;
#endif
		default:
			s8Ret = -1;
			M2M_ERR("invalide ioclt cmd\n");
			break;
	}

	return s8Ret;
}

/*
*	@fn		nm_bus_deinit
*	@brief	De-initialize the bus wrapper
*/
sint8 nm_bus_deinit(void)
{
	sint8 result = M2M_SUCCESS;

#ifdef CONF_WINC_USE_I2C

#endif /* CONF_WINC_USE_I2C */
#ifdef CONF_WINC_USE_SPI
        nrfx_spim_uninit(&winc_spim);
#endif /* CONF_WINC_USE_SPI */
	return result;
}

/*
*	@fn			nm_bus_reinit
*	@brief		re-initialize the bus wrapper
*	@param [in]	void *config
*					re-init configuration data
*	@return		M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
sint8 nm_bus_reinit(void* config)
{
	return M2M_SUCCESS;
}

