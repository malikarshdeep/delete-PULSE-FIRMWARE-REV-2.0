#include "enchw.h"

//#include <asf.h>

#include "boards.h"
#include "enc28j60.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrfx_spim.h"

#define SPI_INSTANCE 0
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE); /**< SPI instance. */

void enchw_setup(enchw_device_t *dev) {
  nrf_gpio_cfg_output(ENC28J60_SEL);
  nrf_gpio_pin_set(ENC28J60_SEL);
  nrf_gpio_cfg_output(ENC28J60_RST);
  nrf_gpio_pin_clear(ENC28J60_RST);
  nrf_delay_ms(2);
  nrf_gpio_pin_set(ENC28J60_RST);
nrf_delay_ms(2);

  nrf_gpio_cfg_input (ENC28J60_INT, NRF_GPIO_PIN_PULLUP);

  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi_config.frequency = NRF_SPI_FREQ_500K;// NRF_SPI_FREQ_4M;
  //spi_config.ss_pin   = SPI_SS_PIN;
  spi_config.miso_pin = ENC28J60_MISO;
  spi_config.mosi_pin = ENC28J60_MOSI;
  spi_config.sck_pin = ENC28J60_SCK;
  APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));
  //setup_workaround_for_ftpan_58(&spi->spim->p_reg,
}

/***
 * We rely on enc28j60.c behaviour
 * that it always calls our functions in the following order:
 *
 *   1. enchw_select
 *   2. enchw_exchangebyte
 *   3. enchw_unselect
 *
 * So we do spi_lock at enchw_select, and do spi_unlock at enchw_unselect,
 * dev->pmaster in enchw_exchangebyte is locked.
 */

void enchw_select(enchw_device_t *dev) {
  nrf_gpio_pin_clear(ENC28J60_SEL);
  nrf_delay_us(1);
}

void enchw_unselect(enchw_device_t *dev) {
  nrf_gpio_pin_set(ENC28J60_SEL);
}

uint8_t enchw_exchangebyte(enchw_device_t *dev, uint8_t byte) {
  uint8_t ret;
  nrf_drv_spi_transfer(&spi, &byte, 1, &ret, 0);
  return ret;
}

void enchw_sendbyte(enchw_device_t *dev, uint8_t byte) {
  nrf_drv_spi_transfer(&spi, &byte, 1, NULL, 0);
}

uint16_t enchw_rxtx(enchw_device_t *dev, uint8_t *txbuff, uint8_t *rxbuff, uint16_t len) {
  while (len) {
    if (len > 0xff) {
      nrf_drv_spi_transfer(&spi, txbuff, 0xff, rxbuff, 0xff);
      len -= 0xff;
      txbuff += 0xff;
      rxbuff += 0xff;
    } else {
      nrf_drv_spi_transfer(&spi, txbuff, len, rxbuff, len);
      len = 0;
    }
  }
}

uint16_t enchw_sendbuff(enchw_device_t *dev, uint8_t *pbuff, uint16_t len) {
  while (len) {
    if (len > 0xff) {
      nrf_drv_spi_transfer(&spi, pbuff, 0xff, NULL, 0);
      len -= 0xff;
      pbuff += 0xff;
    } else {
      nrf_drv_spi_transfer(&spi, pbuff, len, NULL, 0);
      len = 0;
    }
  }
}

uint16_t enchw_rcvbuff(enchw_device_t *dev, uint8_t *pbuff, uint16_t len) {
  while (len) {
    if (len > 0xff) {
      nrf_drv_spi_transfer(&spi, NULL, 0, pbuff, 0xff);
      len -= 0xff;
      pbuff += 0xff;

    } else {
      nrf_drv_spi_transfer(&spi, NULL, 0, pbuff, len);
      len = 0;
    }
  }
}