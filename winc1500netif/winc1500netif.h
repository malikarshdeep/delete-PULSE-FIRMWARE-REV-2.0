#ifndef ETHERNETIF_H_INCLUDED
#define ETHERNETIF_H_INCLUDED

#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "netif/etharp.h"
#include <driver/include/m2m_wifi.h>
#include "FreeRTOS.h"
#include <semphr.h>

extern SemaphoreHandle_t winc_semph;

extern struct netif winc_netif;



typedef void (*wifi_task_cb)(void *arg);
typedef struct {
	uint32_t id;
	void *pbuf;
	void * payload;
	uint32_t payload_size;
	wifi_task_cb	handler;
	void * priv;
} hif_msg_t ;

void winc_netif_tx_from_queue(hif_msg_t *msg);
void winc_netif_rx_callback(uint8 msg_type, void * msg, void *ctrl_buf);
err_t winc_netif_init(struct netif *netif);
void winc_fill_callback_info(tstrEthInitParam *info);

#endif