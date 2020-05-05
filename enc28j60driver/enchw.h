#include <stdint.h>
//#include <asf.h>

typedef struct {
	uint8_t i;
//	struct spi_module* pmaster;  // pointer to master SPI, can be shared with other SPI slaves
	//struct spi_slave_inst slave; // slave ENC28J60 SPI device, 
} enchw_device_t;

#define DEBUG(...)

#ifdef  BOARD_CUSTOM
#define ENC28J60_SEL 21
#define ENC28J60_SCK 23
#define ENC28J60_MISO 22
#define ENC28J60_MOSI 20
#define ENC28J60_RST 19
#define ENC28J60_INT 24
#else
#ifdef BOARD_PCA10056
#define ENC28J60_SEL 4
#define ENC28J60_SCK 30
#define ENC28J60_MISO 31
#define ENC28J60_MOSI 29
#define ENC28J60_RST 28
//#define ENC28J60_INT 
#endif //BOARD_PCA10056
#endif //BOARD_CUSTOM

void enchw_setup(enchw_device_t *dev);
void enchw_select(enchw_device_t *dev);
void enchw_unselect(enchw_device_t *dev);
uint8_t enchw_exchangebyte(enchw_device_t *dev, uint8_t byte);
uint16_t enchw_rxtx (enchw_device_t *dev, uint8_t *txbuff, uint8_t *rxbuff, uint16_t len);

void enchw_sendbyte(enchw_device_t *dev, uint8_t byte);
uint16_t enchw_sendbuff (enchw_device_t *dev, uint8_t *pbuff, uint16_t len);
uint16_t enchw_rcvbuff (enchw_device_t *dev, uint8_t *pbuff, uint16_t len);
