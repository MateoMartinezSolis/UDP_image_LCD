
#ifndef RTOS_SPI_H_
#define RTOS_SPI_H_

#include <stdint.h>
#include "fsl_dspi.h"

#define BITS_PER_FRAME	8U
#define _1_NSEC			1000000000U

#define SPI_BAUDRATE	500000U


typedef enum
{
	USE_SPI0,
	USE_SPI1,
	USE_SPI2

} spi_ports_t;

typedef enum
{
	TRANSFER_MODE_0,	 /* SPI Mode 0, CPOL = 0, CPHA = 0: CLK idle state = low, data sampled on rising edge and shifted on falling edge */
	TRANSFER_MODE_1,	 /* SPI Mode 1, CPOL = 0, CPHA = 1: CLK idle state = low, data sampled on the falling edge and shifted on the rising edge */
	TRANSFER_MODE_2,	 /* SPI Mode 2, CPOL = 1, CPHA = 0: CLK idle state = high, data sampled on the falling edge and shifted on the rising edge */
	TRANSFER_MODE_3		 /* SPI Mode 3, CPOL = 1, CPHA = 1: CLK idle state = high, data sampled on the rising edge and shifted on the falling edge */
} ctar_modes_t;


// parametros de inicializacion
typedef struct
{
	uint32_t baudRate;
	uint8_t	spi_number;
	uint8_t	spi_port;	/* This selects the spi port to use */
	uint8_t port_mux; /* For port enabling clocks */
	uint8_t miso_pin;
	uint8_t	mosi_pin;
	uint8_t	sclk_pin;
	uint8_t pcs_pin;
	ctar_modes_t mode;
} rtosSPI_master_config_t;

// init
void rtosSPI_init(rtosSPI_master_config_t *config);

// TX
void rtosSPI_send(uint8_t spi, uint8_t * data, uint32_t size);

// RX
//void rtosSPI_receive(uint8_t spi, uint8_t * data, uint32_t size);

#endif /* RTOS_SPI_H */
