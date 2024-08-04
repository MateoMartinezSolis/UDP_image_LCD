/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "udpecho.h"

#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/api.h"
#include "lwip/sys.h"

#include "fsl_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "rtos_spi.h"
#include "lcd_driver.h"

SemaphoreHandle_t semPrint;
SemaphoreHandle_t mutex;


void printImageBMP(void *args);
void taskInit(void* args);

char buffer[4096];

/*-----------------------------------------------------------------------------------*/
static void udpecho_thread(void *arg)
{
	struct netconn *conn;
	struct netbuf *buf;
	//char buffer[4096];
	err_t err;
	LWIP_UNUSED_ARG(arg);

#if LWIP_IPV6
	conn = netconn_new(NETCONN_UDP_IPV6);
	LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);
	netconn_bind(conn, IP6_ADDR_ANY, 7);
#else /* LWIP_IPV6 */
	conn = netconn_new(NETCONN_UDP);
	LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);
	netconn_bind(conn, IP_ADDR_ANY, 7);
#endif /* LWIP_IPV6 */

	while (1) {
		err = netconn_recv(conn, &buf);
		if (err == ERR_OK) {
			/*  no need netconn_connect here, since the netbuf contains the address */
			xSemaphoreTake(mutex,	portMAX_DELAY);
			if(netbuf_copy(buf, buffer, sizeof(buffer)) != buf->p->tot_len) {
				LWIP_DEBUGF(LWIP_DBG_ON, ("netbuf_copy failed\n"));
			} else {
				buffer[buf->p->tot_len] = '\0';

				PRINTF("RX size: %d \n\r", buf->p->tot_len);
				xSemaphoreGive(semPrint);
			}
			xSemaphoreGive(mutex);
			netbuf_delete(buf);
			xSemaphoreGive(semPrint);
		}
	}
}
/*-----------------------------------------------------------------------------------*/
void udpecho_init(void)
{
	semPrint = xSemaphoreCreateBinary();
	mutex  = xSemaphoreCreateMutex();
	sys_thread_new("udpecho_thread", udpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
	xTaskCreate(printImageBMP, "printImage", 500, NULL, 2, NULL);
}

void printImageBMP(void *args){

	rtosSPI_master_config_t config;
	config.baudRate = 115200;
	config.miso_pin = 3;
	config.mode = TRANSFER_MODE_0;
	config.mosi_pin = 2;
	config.pcs_pin = 0;
	config.port_mux = 2;
	config.sclk_pin = 1;
	config.spi_number = 0;
	config.spi_port = 0;

	rtosSPI_init(&config);
	LCD_begin();
	clearLCD();
	while(1)
	{
		xSemaphoreTake(semPrint, portMAX_DELAY);
		xSemaphoreTake(mutex,	portMAX_DELAY);
		//clearLCD();
		printImage(buffer);
		GPIO_PortToggle(GPIOB, 1<<22);
		xSemaphoreGive(mutex);

	}
}
#endif /* LWIP_NETCONN */
