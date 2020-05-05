#define ONEWIRE_SUCCESS            0x00
#define ONEWIRE_ERR                0x01
#define ONEWIRE_ERR_BUSY           0x02
#define ONEWIRE_ERR_NO_RESPONSE    0x03

#include "stdint.h"
int onewire_init (void);

int onewire_reset (uint32_t pin_number);
int onewire_write(uint32_t pin_number, uint8_t * buff, uint16_t len);
int onewire_read(uint32_t pin_number, uint8_t * buff, uint16_t len);