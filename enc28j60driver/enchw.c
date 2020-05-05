#include "enchw.h"

//#include <asf.h>

#include "boards.h"
#include "enc28j60.h"
//#include "nrf_drv_spi.h"
#include "nrfx_spim.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#define ENC_SPI_INSTANCE 1

static nrfx_spim_t spi =NRFX_SPIM_INSTANCE(ENC_SPI_INSTANCE);

void enchw_setup(enchw_device_t *dev) {
  nrf_gpio_cfg_output(ENC28J60_SEL);
  nrf_gpio_pin_set(ENC28J60_SEL);
  nrf_gpio_cfg_output(ENC28J60_RST);
  nrf_gpio_pin_clear(ENC28J60_RST);
  nrf_delay_ms(2);
  nrf_gpio_pin_set(ENC28J60_RST);
  nrf_delay_ms(2);

  //nrf_gpio_cfg_input (ENC28J60_INT, NRF_GPIO_PIN_PULLUP);

  nrfx_spim_config_t spi_config = NRFX_SPIM_DEFAULT_CONFIG;

  spi_config.frequency =  NRF_SPIM_FREQ_1M;
  #ifdef BOARD_PCA10056
  spi_config.frequency =  NRF_SPIM_FREQ_1M;
  #endif
  spi_config.ss_pin   = NRFX_SPIM_PIN_NOT_USED;
  spi_config.miso_pin = ENC28J60_MISO;
  spi_config.mosi_pin = ENC28J60_MOSI;
  spi_config.sck_pin = ENC28J60_SCK;
  APP_ERROR_CHECK(nrfx_spim_init(&spi, &spi_config, NULL, NULL));
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
  nrfx_spim_xfer_desc_t spim_xfer = NRFX_SPIM_SINGLE_XFER(&byte, 1, &ret, 0);
  nrfx_spim_xfer(&spi, &spim_xfer, 0);
  //nrf_drv_spi_transfer(&spi, &byte, 1, &ret, 0);
  return ret;
}

void enchw_sendbyte(enchw_device_t *dev, uint8_t byte) {
  nrfx_spim_xfer_desc_t spim_xfer = NRFX_SPIM_SINGLE_XFER(&byte, 1, NULL, 0);
  nrfx_spim_xfer(&spi, &spim_xfer, 0);
  //nrf_drv_spi_transfer(&spi, &byte, 1, NULL, 0);
}

uint16_t enchw_rxtx(enchw_device_t *dev, uint8_t *txbuff, uint8_t *rxbuff, uint16_t len) {
  nrfx_spim_xfer_desc_t spim_xfer = NRFX_SPIM_SINGLE_XFER(txbuff, len, rxbuff, len);
  nrfx_spim_xfer(&spi, &spim_xfer, 0);

}

uint16_t enchw_sendbuff(enchw_device_t *dev, uint8_t *pbuff, uint16_t len) {
  nrfx_spim_xfer_desc_t spim_xfer = NRFX_SPIM_SINGLE_XFER(pbuff, len, NULL, 0);
  nrfx_spim_xfer(&spi, &spim_xfer, 0);
}

uint16_t enchw_rcvbuff(enchw_device_t *dev, uint8_t *pbuff, uint16_t len) {
  nrfx_spim_xfer_desc_t spim_xfer = NRFX_SPIM_SINGLE_XFER( NULL, 0, pbuff, len);
  nrfx_spim_xfer(&spi, &spim_xfer, 0);
}