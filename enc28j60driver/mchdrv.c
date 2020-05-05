#include <mchdrv.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>
#if LWIP_IPV6
#include <lwip/ethip6.h>
#endif
#include "enc28j60.h"
#include "boards.h"
#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t enc_semph;

err_t mchdrv_poll(struct netif *netif) {
	err_t result;
        xSemaphoreTake(enc_semph, portMAX_DELAY);

	struct pbuf *buf = NULL;

	uint8_t epktcnt;
	bool linkstate;
	enc_device_t *encdevice = (enc_device_t*)netif->state;
		if (encdevice->state ==0){
                xSemaphoreGive(enc_semph);
		return  ERR_IF;
	}
	
	linkstate = enc_MII_read(encdevice, ENC_PHSTAT1) & (1 << 2);

	if (linkstate)
        {
            netif->flags |= NETIF_FLAG_LINK_UP;
            //netif_set_link_up(netif);
        }else{
            netif->flags &= ~NETIF_FLAG_LINK_UP;
            //netif_set_link_down(netif);
        }
        
        epktcnt = enc_RCR(encdevice, ENC_EPKTCNT);
        while (epktcnt){
                buf = NULL;
		if (enc_read_received_pbuf(encdevice, &buf) == 0)
		{
			LWIP_DEBUGF(NETIF_DEBUG, ("incoming: %d packages, first read into %x\n", epktcnt, (unsigned int)(buf)));
			result = netif->input(buf, netif);
			LWIP_DEBUGF(NETIF_DEBUG, ("received with result %d\n", result));
		} else {
			/* FIXME: error reporting */
			LWIP_DEBUGF(NETIF_DEBUG, ("didn't receive.\n"));
                        break;
		}
                epktcnt--;
        }
        xSemaphoreGive(enc_semph);
        return  ERR_OK;
	
}

static err_t mchdrv_linkoutput(struct netif *netif, struct pbuf *p)
{
        xSemaphoreTake(enc_semph, portMAX_DELAY);
	enc_device_t *encdevice = (enc_device_t*)netif->state;
	if (encdevice->state ==0){
                xSemaphoreGive(enc_semph);
		return  ERR_IF;
	}
	enc_transmit_pbuf(encdevice, p);
	LWIP_DEBUGF(NETIF_DEBUG, ("sent %d bytes.\n", p->tot_len));
	/* FIXME: evaluate result state */
        xSemaphoreGive(enc_semph);
	return ERR_OK;
}

err_t mchdrv_init(struct netif *netif) {
	int result;
	err_t retval = ERR_OK;
	enc_device_t *encdevice = (enc_device_t*)netif->state;

	LWIP_DEBUGF(NETIF_DEBUG, ("Starting mchdrv_init.\n"));
        enc_semph = xSemaphoreCreateMutex();
	
	encdevice->state = 1;
	result = enc_setup_basic(encdevice);
	if (result != 0)
	{
		LWIP_DEBUGF(NETIF_DEBUG, ("Error %d in enc_setup, interface setup aborted.\n", result));
		encdevice->state =0;
		retval = ERR_IF;
	}else{
		result = enc_bist_manual(encdevice);
		if (result != 0)
		{
			LWIP_DEBUGF(NETIF_DEBUG, ("Error %d in enc_bist_manual, interface setup aborted.\n", result));
			encdevice->state =0;
			retval = ERR_IF;
		}else{
			enc_ethernet_setup(encdevice, 4*1024, netif->hwaddr);
			/* enabling this unconditonally: there seems not to be a generic way by
			* which protocols indicate their multicast requirements to the netif,
			* going for "always on" for now */
			//enc_set_multicast_reception(encdevice, 1);
		}
	}

	netif->output = etharp_output;
#if LWIP_IPV6
	netif->output_ip6 = ethip6_output;
#endif
	netif->linkoutput = mchdrv_linkoutput;

	netif->mtu = 1500; /** FIXME check with documentation when jumboframes can be ok */

	netif->flags |= NETIF_FLAG_ETHARP | NETIF_FLAG_BROADCAST;

	LWIP_DEBUGF(NETIF_DEBUG, ("Driver initialized.\n"));
        xSemaphoreGive(enc_semph);

	return retval;
}
