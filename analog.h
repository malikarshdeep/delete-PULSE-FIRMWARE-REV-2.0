#define SAADC_SAMPLES_IN_BUFFER 8
#define SAMPLES_IN_BUFFER 8

float C1 = 0.00100924;
float C2 = 0.0002378;
float C3 = 0.000000201920;
float A1, A2;
float P1, P2, P3, P4, P5, P6, P7, P8;
static uint8_t value[8];

static int m_buffer_pool1[SAADC_SAMPLES_IN_BUFFER];


static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE(1);
static nrf_saadc_value_t m_buffer_pool[4][SAMPLES_IN_BUFFER];
static nrf_ppi_channel_t m_ppi_channel;
static uint32_t m_adc_evt_counter;

void saadc_sampling_event_init(void);
static void timers_init(void);

