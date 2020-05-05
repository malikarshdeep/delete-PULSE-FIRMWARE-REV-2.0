// Energy measuremnt SPI mapping
#define IPF_SI NRF_GPIO_PIN_MAP(1, 14)
#define IPF_SDO NRF_GPIO_PIN_MAP(1, 11)
#define IPF_SCK NRF_GPIO_PIN_MAP(1, 10)
#define IPF_CS NRF_GPIO_PIN_MAP(1, 15)



#define PF_SPI_INSTANCE 3                                           /**< SPI instance index. */
static volatile bool spi_xfer_done;                                 /**< Flag used to indicate that SPI instance completed the transfer. */
static uint16_t m_tx_buf = 0b0100000010000000;                      //0X30H
static uint32_t m_tx_buf1 = 0b01111000010101100011000000000000;     //0X30H
static uint32_t m_tx_buf2 = 0b01111000010101100100000000000000;     //0X40H
static uint32_t m_tx_buf3 = 0b01111000010101100101000000000000;     //0X50H
static uint32_t m_tx_buf4 = 0b01111000010101100110000000000000;     //0X60H
static uint16_t m_tx_buf5 = 0b1011110010000000;                     //0XBCH
static uint16_t m_tx_buf6 = 0b1101100110000000;                     //0XBCH
static uint32_t config_start = 0b01111000010101100011000000000000;  //0X30H
static uint32_t cal_start = 0b01111000010101100100000000000000;
static uint32_t harm_start = 0b01111000010101100101000000000000; //0X50H
static uint32_t adj_start = 0b01111000010101100110000000000000;  //0X60H
//static uint32_t       mmode0 =        0b11000010000010000011001100000000;           //0X60H
static uint32_t test = 0b0011000010000000;
static uint16_t urmsA = 0b1101100110000000;                     //0XBCH
static uint16_t urmsB = 0b1101101010000000;                     //0XBCH
static uint16_t urmsC = 0b1101101110000000;                     //0XBCH
static uint16_t irmsA = 0b1101110110000000;                     //0XBCH
static uint16_t irmsB = 0b1101111010000000;                     //0XBCH
static uint16_t irmsC = 0b1101111110000000;                     //0XBCH
static uint16_t pf = 0b1011110010000000;                        //0XBCH
static uint16_t pf_a = 0b1011110110000000;
static uint16_t pf_B = 0b1011111010000000;
static uint16_t pf_C = 01011111110000000;   
static uint32_t uoffsetA = 0b00000000000000000110001100000000;  //0X60H
static uint32_t uoffsetB = 0b00000000000000000110011100000000;  //0X60H
static uint32_t uoffsetC = 0b00000000000000000110101100000000;  //0X60H
static uint32_t ioffsetA = 0b00000000000000000110100000000000;  //0X60H
static uint32_t ioffsetB = 0b00000000000000000110110000000000;  //0X60H
static uint32_t ioffsetC = 0b00000000000000000110110000000000;  //0X60H
static uint32_t igainA = {0x30, 0x75, 0x62, 0x00};              //0X60H
static uint32_t igainA_r = 0b10000001001000110110001000000000;  //0X60H
static uint32_t vGain = 0b11101110011100110110000100000000;     //0X60H
static uint32_t igainA_r1 = 0b01011010000000000110001000000000; //0X60H
static uint16_t igain_read = 0b0110001010000000;                //0XBCH
static uint32_t igainC = 0b11110101001000110110101000000000;    //0X60H
static uint32_t mmode0 = 0b10000111000100000011001100000000;    //0X60H
static uint32_t mmode0_single = 0b10000110000100000011001100000000;    //0X60H
static uint16_t active_power = 0b1011000010000000;              //0XBCH
static uint16_t active_powerA = 0b1011000110000000;  
static uint16_t active_powerB = 0b1011001010000000;  
static uint16_t active_powerC = 0b1011001110000000;  
static uint16_t reactive_power = 0b1011010010000000;            //0XBCH
static uint16_t reactive_powerA = 0b1011010110000000;   
static uint16_t reactive_powerB = 0b1011011010000000;   
static uint16_t reactive_powerC = 0b1011011110000000;   
static uint16_t apparent_power = 0b1011100010000000;
static uint16_t apparent_powerA = 0b1011100110000000;
static uint16_t apparent_powerB = 0b1011101010000000;
static uint16_t apparent_powerC = 0b1011101110000000;
static uint32_t vgainA = 0b00111100110011000110000100000000;
static uint32_t vgainC = 0b11100101110010110110100100000000; //0XBCH
static uint16_t vgain_read = 0b0110000110000000;
static uint16_t igainC_read = 0b0110101010000000;
static uint32_t igain_A_default = 0b00110000011101010110001000000000;
static uint32_t igain_C_default = 0b00110000011101010110101000000000;
static uint32_t igainA_new = 0b00010101011110000110001000000000;
static uint32_t igainB_new = 0b00010101011110000110011000000000;
static uint32_t igainC_new = 0b01110101011101110110101000000000;
static uint16_t mmode0_read = 0b0011001110000000;
static uint16_t harm_read = 0b0101000010000000;
static uint16_t mmode1_read = 0b0011010010000000;
static uint16_t PLconstH = 0b0011000110000000;
static uint16_t PLconstL = 0b0011001010000000;
static uint16_t PStartTh = 0b0011010110000000;
static uint16_t PGainAF = 0b0101010010000000;
static uint16_t UgainA =      0b0110000110000000;
static uint16_t IgainA =      0b0110001010000000;
static uint16_t UoffsetA =      0b0110001110000000;
static uint16_t IoffsetA =      0b0110010010000000;
static uint16_t UgainB =      0b0110010110000000;
static uint16_t IgainB =      0b0110011010000000;
static uint16_t UoffsetB =      0b0110011110000000;
static uint16_t IoffsetB =      0b0110100010000000;
static uint16_t UgainC =      0b0110100110000000;
static uint16_t IgainC =      0b0110101010000000;
static uint16_t UoffsetC =      0b0110101110000000;
static uint16_t IoffsetC =      0b0110110010000000;
static uint16_t IgainN =      0b0110110110000000;
static uint16_t IoffsetN =      0b0110111010000000;
static uint16_t CS0 =      0b0011101110000000;

static uint8_t m_rx_buf[4];        /**< RX buffer. */
static uint8_t m_rx_buf2[4];       /**< RX buffer. */
static const uint8_t m_length = 4; /**< Transfer length. */
extern char config_string1[];

void energy_measurement();
void pf_spim_init(void);