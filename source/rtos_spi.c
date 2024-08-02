#include "rtos_spi.h"
#include "MK64F12.h"
#include "fsl_clock.h" // CLOCK_GetFreq()
#include "fsl_port.h"

#include "FreeRTOS.h"
#include "semphr.h"

typedef struct
{
	SemaphoreHandle_t semBinTX;
	SemaphoreHandle_t semBinRX;
	SemaphoreHandle_t mutexTX;
	SemaphoreHandle_t mutexRX;
	dspi_master_handle_t spiHandle;
} rtosSPI_handle_t;

rtosSPI_handle_t handlers [3];

static int8_t rtosSPI_clockConfig(uint8_t port);

static int8_t rtosSPI_portConfig(uint8_t port, uint8_t pin, uint8_t mux);

static int8_t rtosSPI_spiMode(uint32_t baudRate, uint8_t mode, dspi_master_ctar_config_t * ctar);

static int8_t rtosSPI_spiConfig(uint8_t spi, uint32_t baudRate, uint8_t mode);

static void rtosSPI_callback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData);

// init
void rtosSPI_init(rtosSPI_master_config_t *config)
{
	/* Enable clock */
	rtosSPI_clockConfig(config->spi_port);

	/* Configure the SPI pins to use */
	rtosSPI_portConfig(config->spi_port, config->miso_pin, config->port_mux); // MISO
	rtosSPI_portConfig(config->spi_port, config->mosi_pin, config->port_mux); // MOSI
	rtosSPI_portConfig(config->spi_port, config->pcs_pin,  config->port_mux); // Peropherical Chip Select
	rtosSPI_portConfig(config->spi_port, config->sclk_pin, config->port_mux); // SPI CLK

	/* Configure the SPI */
	rtosSPI_spiConfig(config->spi_number, config->baudRate, config->mode);

	/* Semaphores and Mutexes */
	handlers[config->spi_number].semBinTX = xSemaphoreCreateBinary();
	handlers[config->spi_number].semBinRX = xSemaphoreCreateBinary();
	/* MUTEX */
	handlers[config->spi_number].mutexTX = xSemaphoreCreateMutex();
	handlers[config->spi_number].mutexRX = xSemaphoreCreateMutex();

}

// TRANSFER
void rtosSPI_send(uint8_t spi, uint8_t * data, uint32_t size)
{
	//static volatile dspi_transfer_t xfer;
	dspi_transfer_t xfer;

	xfer.txData = data;
	xfer.rxData = NULL;
	xfer.dataSize = size;
	xfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

	if(USE_SPI2 == spi)
	{
		xSemaphoreTake(handlers[2].mutexTX, portMAX_DELAY);

		DSPI_MasterTransferNonBlocking(SPI2, &handlers[2].spiHandle, &xfer);

		xSemaphoreTake(handlers[2].semBinTX, portMAX_DELAY);

		xSemaphoreGive(handlers[2].mutexTX);
	}
	else if(USE_SPI1 == spi)
	{
		xSemaphoreTake(handlers[1].mutexTX, portMAX_DELAY);

		DSPI_MasterTransferNonBlocking(SPI1, &handlers[1].spiHandle, &xfer);

		xSemaphoreTake(handlers[1].semBinTX, portMAX_DELAY);

		xSemaphoreGive(handlers[1].mutexTX);
	}
	else
	{
		xSemaphoreTake(handlers[0].mutexTX, portMAX_DELAY);

		DSPI_MasterTransferNonBlocking(SPI0, &handlers[0].spiHandle, &xfer);

		xSemaphoreTake(handlers[0].semBinTX, portMAX_DELAY);

		xSemaphoreGive(handlers[0].mutexTX);
	}

}

// clock configuration
static int8_t rtosSPI_clockConfig(uint8_t port)
{
	int8_t error = 0;

	switch(port){
		case 0:
			CLOCK_EnableClock(kCLOCK_PortD);
			break;
	/* Add more clocks... */
		default:
			error = 1;
			break;
	}

	return error;
}

// port configuration
static int8_t rtosSPI_portConfig(uint8_t port, uint8_t pin, uint8_t mux)
{
	int8_t error = 0;

	switch(port){
		case 0:
			PORT_SetPinMux(PORTD, pin, mux);
			break;
	/* Add more port muxes... */
		default:
			error = 1;
			break;
	}

	return error;
}


// configure the clock and transfer mode
static int8_t rtosSPI_spiMode(uint32_t baudRate, uint8_t mode, dspi_master_ctar_config_t * ctar)
{
	uint8_t error = 0;

	ctar->baudRate = baudRate;
	ctar->bitsPerFrame                  = BITS_PER_FRAME;

	switch(mode)
	{
		case TRANSFER_MODE_0: /* SPI Mode 0, CPOL = 0, CPHA = 0: CLK idle state = low, data sampled on rising edge and shifted on falling edge */

			ctar->cpol                          = kDSPI_ClockPolarityActiveHigh;
			ctar->cpha                          = kDSPI_ClockPhaseFirstEdge;
			break;

		case TRANSFER_MODE_1: /* SPI Mode 1, CPOL = 0, CPHA = 1: CLK idle state = low, data sampled on the falling edge and shifted on the rising edge */

			ctar->cpol                          = kDSPI_ClockPolarityActiveHigh;
			ctar->cpha                          = kDSPI_ClockPhaseSecondEdge;
			break;

		case TRANSFER_MODE_2: /* SPI Mode 2, CPOL = 1, CPHA = 0: CLK idle state = high, data sampled on the falling edge and shifted on the rising edge */

			ctar->cpol                          = kDSPI_ClockPolarityActiveLow;
			ctar->cpha                          = kDSPI_ClockPhaseFirstEdge;
			break;

		case TRANSFER_MODE_3: /* SPI Mode 3, CPOL = 1, CPHA = 1: CLK idle state = high, data sampled on the rising edge and shifted on the falling edge */

			ctar->cpol                          = kDSPI_ClockPolarityActiveLow;
			ctar->cpha                          = kDSPI_ClockPhaseSecondEdge;
			break;

		default:
			error = 1;
			break;
	}

	ctar->direction                     = kDSPI_MsbFirst;
	ctar->pcsToSckDelayInNanoSec        = _1_NSEC / baudRate;
	ctar->lastSckToPcsDelayInNanoSec    = _1_NSEC / baudRate;
	ctar->betweenTransferDelayInNanoSec = _1_NSEC / baudRate;

	return error;
}

// spi config
static int8_t rtosSPI_spiConfig(uint8_t spi, uint32_t baudRate, uint8_t mode)
{
	uint8_t error = 0;
	dspi_master_config_t config;

	DSPI_MasterGetDefaultConfig(&config);
	config.whichCtar = kDSPI_Ctar0;

	/* configures the clock and the spi master transfer mode */
	rtosSPI_spiMode(baudRate, mode, &config.ctarConfig);

	/* Initializes the SPI module */
	switch(spi)
	{
		case USE_SPI0:  /* SPI 0 */
			DSPI_MasterInit(SPI0, &config, CLOCK_GetFreq(BUS_CLK));
			DSPI_MasterTransferCreateHandle(SPI0, &handlers[0].spiHandle, rtosSPI_callback, NULL);
			break;

		case USE_SPI1:	 /* SPI 1 */
			DSPI_MasterInit(SPI1, &config, CLOCK_GetFreq(BUS_CLK));
			DSPI_MasterTransferCreateHandle(SPI1, &handlers[1].spiHandle, rtosSPI_callback, NULL);
			break;

		case USE_SPI2:  /* SPI 2 */
			DSPI_MasterInit(SPI2, &config, CLOCK_GetFreq(BUS_CLK));
			DSPI_MasterTransferCreateHandle(SPI2, &handlers[2].spiHandle, rtosSPI_callback, NULL);
			break;
	}

	return error;
}
// callback for SPI
static void rtosSPI_callback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData)
{
	BaseType_t higerPriorityTaksWoken = pdFALSE;
	//kStatus_Success
	if (kStatus_Success == status)
	{
		if (SPI0 == base)
		{
			xSemaphoreGiveFromISR(handlers[0].semBinTX, &higerPriorityTaksWoken);
			xSemaphoreGiveFromISR(handlers[0].semBinRX, &higerPriorityTaksWoken);
		}
		else if(SPI1 == base)
		{
			xSemaphoreGiveFromISR(handlers[1].semBinTX, &higerPriorityTaksWoken);
			xSemaphoreGiveFromISR(handlers[1].semBinRX, &higerPriorityTaksWoken);
		}
		else
		{
			xSemaphoreGiveFromISR(handlers[2].semBinTX, &higerPriorityTaksWoken);
			xSemaphoreGiveFromISR(handlers[2].semBinRX, &higerPriorityTaksWoken);
		}
	}


	portYIELD_FROM_ISR(higerPriorityTaksWoken);
}
